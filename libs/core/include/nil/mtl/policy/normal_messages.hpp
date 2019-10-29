//---------------------------------------------------------------------------//
// Copyright (c) 2011-2017 Dominik Charousset
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#pragma once

#include <nil/mtl/fwd.hpp>
#include <nil/mtl/mailbox_element.hpp>
#include <nil/mtl/unit.hpp>

namespace nil {
    namespace mtl {
        namespace policy {

            /// Configures a cached DRR queue for holding asynchronous messages with
            /// default priority.
            class normal_messages {
            public:
                // -- member types -----------------------------------------------------------

                using mapped_type = mailbox_element;

                using task_size_type = size_t;

                using deficit_type = size_t;

                using unique_pointer = mailbox_element_ptr;

                // -- constructors, destructors, and assignment operators --------------------

                normal_messages() = default;

                normal_messages(const normal_messages &) = default;

                normal_messages &operator=(const normal_messages &) = default;

                constexpr normal_messages(unit_t) {
                    // nop
                }

                // -- interface required by drr_queue ----------------------------------------

                static inline task_size_type task_size(const mailbox_element &) noexcept {
                    return 1;
                }
            };

        }    // namespace policy
    }        // namespace mtl
}    // namespace nil