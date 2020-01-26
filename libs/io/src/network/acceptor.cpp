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

#include <nil/mtl/io/network/acceptor.hpp>

#include <nil/mtl/logger.hpp>

namespace nil {
    namespace mtl {
        namespace io {
            namespace network {

                acceptor::acceptor(default_multiplexer &backend_ref, native_socket sockfd) :
                    event_handler(backend_ref, sockfd), sock_(invalid_native_socket) {
                    // nop
                }

                void acceptor::start(acceptor_manager *mgr) {
                    MTL_LOG_TRACE(MTL_ARG2("fd", fd_));
                    MTL_ASSERT(mgr != nullptr);
                    activate(mgr);
                }

                void acceptor::activate(acceptor_manager *mgr) {
                    if (!mgr_) {
                        mgr_.reset(mgr);
                        event_handler::activate();
                    }
                }

                void acceptor::removed_from_loop(operation op) {
                    MTL_LOG_TRACE(MTL_ARG2("fd", fd_) << MTL_ARG(op));
                    if (op == operation::read)
                        mgr_.reset();
                }

                void acceptor::graceful_shutdown() {
                    MTL_LOG_TRACE(MTL_ARG2("fd", fd_));
                    // Ignore repeated calls.
                    if (state_.shutting_down)
                        return;
                    state_.shutting_down = true;
                    // Shutdown socket activity.
                    shutdown_both(fd_);
                }

            }    // namespace network
        }        // namespace io
    }            // namespace mtl
}    // namespace nil