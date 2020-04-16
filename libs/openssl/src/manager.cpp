//---------------------------------------------------------------------------//
// Copyright (c) 2011-2018 Dominik Charousset
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt for Boost License or
// http://opensource.org/licenses/BSD-3-Clause for BSD 3-Clause License
//---------------------------------------------------------------------------//

#include <nil/actor/openssl/manager.hpp>

ACTOR_PUSH_WARNINGS
#include <openssl/err.h>
#include <openssl/ssl.h>
ACTOR_POP_WARNINGS

#include <vector>
#include <mutex>

#include <nil/actor/actor_control_block.hpp>
#include <nil/actor/spawner.hpp>
#include <nil/actor/spawner_config.hpp>
#include <nil/actor/expected.hpp>
#include <nil/actor/raise_error.hpp>
#include <nil/actor/scoped_actor.hpp>

#include <nil/actor/io/middleman.hpp>
#include <nil/actor/io/basp_broker.hpp>
#include <nil/actor/io/network/default_multiplexer.hpp>

#include <nil/actor/openssl/middleman_actor.hpp>

#if OPENSSL_VERSION_NUMBER < 0x10100000L
struct CRYPTO_dynlock_value {
    std::mutex mtx;
};

namespace {

    int init_count = 0;

    std::mutex init_mutex;

    std::vector<std::mutex> mutexes;

    void locking_function(int mode, int n, const char *, int) {
        if (mode & CRYPTO_LOCK)
            mutexes[static_cast<size_t>(n)].lock();
        else
            mutexes[static_cast<size_t>(n)].unlock();
    }

    CRYPTO_dynlock_value *dynlock_create(const char *, int) {
        return new CRYPTO_dynlock_value;
    }

    void dynlock_lock(int mode, CRYPTO_dynlock_value *dynlock, const char *, int) {
        if (mode & CRYPTO_LOCK)
            dynlock->mtx.lock();
        else
            dynlock->mtx.unlock();
    }

    void dynlock_destroy(CRYPTO_dynlock_value *dynlock, const char *, int) {
        delete dynlock;
    }

}    // namespace

#endif

namespace nil {
    namespace actor {
        namespace openssl {

            manager::~manager() {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
                std::lock_guard<std::mutex> lock {init_mutex};
                --init_count;
                if (init_count == 0) {
                    CRYPTO_set_locking_callback(nullptr);
                    CRYPTO_set_dynlock_create_callback(nullptr);
                    CRYPTO_set_dynlock_lock_callback(nullptr);
                    CRYPTO_set_dynlock_destroy_callback(nullptr);
                    mutexes = std::vector<std::mutex>(0);
                }
#endif
            }

            void manager::start() {
                ACTOR_LOG_TRACE("");
                manager_ =
                    make_middleman_actor(system(), system().middleman().named_broker<io::basp_broker>(atom("BASP")));
            }

            void manager::stop() {
                ACTOR_LOG_TRACE("");
                scoped_actor self {system(), true};
                self->send_exit(manager_, exit_reason::kill);
                if (!config().middleman_attach_utility_actors)
                    self->wait_for(manager_);
                manager_ = nullptr;
            }

            void manager::init(spawner_config &) {
                ACTOR_LOG_TRACE("");
                ERR_load_crypto_strings();
                OPENSSL_add_all_algorithms_conf();
                SSL_library_init();
                SSL_load_error_strings();
                if (authentication_enabled()) {
                    if (system().config().openssl_certificate.size() == 0)
                        ACTOR_RAISE_ERROR("No certificate configured for SSL endpoint");
                    if (system().config().openssl_key.size() == 0)
                        ACTOR_RAISE_ERROR("No private key configured for SSL endpoint");
                }
#if OPENSSL_VERSION_NUMBER < 0x10100000L
                std::lock_guard<std::mutex> lock {init_mutex};
                ++init_count;
                if (init_count == 1) {
                    mutexes = std::vector<std::mutex> {static_cast<size_t>(CRYPTO_num_locks())};
                    CRYPTO_set_locking_callback(locking_function);
                    CRYPTO_set_dynlock_create_callback(dynlock_create);
                    CRYPTO_set_dynlock_lock_callback(dynlock_lock);
                    CRYPTO_set_dynlock_destroy_callback(dynlock_destroy);
                    // OpenSSL's default thread ID callback should work, so don't set our own.
                }
#endif
            }

            spawner::module::id_t manager::id() const {
                return openssl_manager;
            }

            void *manager::subtype_ptr() {
                return this;
            }

            bool manager::authentication_enabled() {
                auto &cfg = system().config();
                return cfg.openssl_certificate.size() > 0 || cfg.openssl_key.size() > 0 ||
                       cfg.openssl_passphrase.size() > 0 || cfg.openssl_capath.size() > 0 ||
                       cfg.openssl_cafile.size() > 0;
            }

            spawner::module *manager::make(spawner &sys, detail::type_list<>) {
                if (!sys.has_middleman())
                    ACTOR_RAISE_ERROR("Cannot start OpenSSL module without middleman.");
                auto ptr = &sys.middleman().backend();
                if (dynamic_cast<io::network::default_multiplexer *>(ptr) == nullptr)
                    ACTOR_RAISE_ERROR("Cannot start OpenSSL module without default backend.");
                return new manager(sys);
            }

            manager::manager(spawner &sys) : system_(sys) {
                // nop
            }

        }    // namespace openssl
    }        // namespace actor
}    // namespace nil
