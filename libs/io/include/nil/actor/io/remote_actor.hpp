//---------------------------------------------------------------------------//
// Copyright (c) 2011-2020 Dominik Charousset
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying files LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt.
//---------------------------------------------------------------------------//

#pragma once

#include <set>
#include <string>
#include <cstdint>

#include <nil/actor/spawner.hpp>

#include <nil/actor/io/middleman.hpp>

namespace nil {
    namespace actor {
        namespace io {

            /// Establish a new connection to the actor at `host` on given `port`.
            /// @param host Valid hostname or IP address.
            /// @param port TCP port.
            /// @returns An `actor` to the proxy instance representing
            ///          a remote actor or an `error`.
            template<class ActorHandle = actor>
            expected<ActorHandle> remote_actor(spawner &sys, std::string host, uint16_t port) {
                return sys.middleman().remote_actor<ActorHandle>(std::move(host), port);
            }

        }    // namespace io
    }        // namespace actor
}    // namespace nil
