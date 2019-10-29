//---------------------------------------------------------------------------//
// Copyright (c) 2011-2018 Dominik Charousset
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying files LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt.
//---------------------------------------------------------------------------//

#pragma once

#include <map>
#include <vector>

#include <nil/mtl/fwd.hpp>
#include <nil/mtl/extend.hpp>
#include <nil/mtl/local_actor.hpp>
#include <nil/mtl/stateful_actor.hpp>

#include <nil/mtl/io/scribe.hpp>
#include <nil/mtl/io/doorman.hpp>
#include <nil/mtl/io/abstract_broker.hpp>
#include <nil/mtl/io/datagram_servant.hpp>

#include <nil/mtl/mixin/sender.hpp>
#include <nil/mtl/mixin/requester.hpp>
#include <nil/mtl/mixin/behavior_changer.hpp>

namespace nil {
    namespace mtl {

        template<>
        class behavior_type_of<io::broker> {
        public:
            using type = behavior;
        };

        namespace io {

            /// Describes a dynamically typed broker.
            /// @extends abstract_broker
            /// @ingroup Broker
            class broker : public extend<abstract_broker, broker>::with<mixin::sender, mixin::requester,
                                                                        mixin::behavior_changer>,
                           public dynamically_typed_actor_base {
            public:
                using super =
                    extend<abstract_broker, broker>::with<mixin::sender, mixin::requester, mixin::behavior_changer>;

                using signatures = none_t;

                template<class F, class... Ts>
                typename infer_handle_from_fun<F>::type fork(F fun, connection_handle hdl, Ts &&... xs) {
                    MTL_ASSERT(context() != nullptr);
                    auto sptr = this->take(hdl);
                    MTL_ASSERT(sptr->hdl() == hdl);
                    using impl = typename infer_handle_from_fun<F>::impl;
                    actor_config cfg {context()};
                    detail::init_fun_factory<impl, F> fac;
                    cfg.init_fun = fac(std::move(fun), hdl, std::forward<Ts>(xs)...);
                    auto res = this->system().spawn_class<impl, no_spawn_options>(cfg);
                    auto forked = static_cast<impl *>(actor_cast<abstract_actor *>(res));
                    forked->move_scribe(std::move(sptr));
                    return res;
                }

                void initialize() override;

                explicit broker(actor_config &cfg);

            protected:
                virtual behavior make_behavior();
            };

            /// Convenience template alias for declaring state-based brokers.
            template<class State>
            using stateful_broker = stateful_actor<State, broker>;

        }    // namespace io
    }        // namespace mtl
}    // namespace nil