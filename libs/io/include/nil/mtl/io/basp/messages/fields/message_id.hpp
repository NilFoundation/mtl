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

#include <nil/mtl/io/basp/version.hpp>

#include <nil/marshalling/marshalling.hpp>

namespace nil {
    namespace mtl {
        namespace io {
            namespace basp {

                /// @addtogroup BASP

                /*!
                 * @brief Field containing current BASP version information.
                 * @note BASP is not backwards compatible
                 */
                template<typename TFieldBase>
                using message_id_field =
                    marshalling::field::int_value<TFieldBase, std::uint64_t,
                                                  marshalling::option::default_num_value<version>,
                                                  marshalling::option::valid_num_value_range<0, version>>;

                /// @}
            }    // namespace basp
        }        // namespace io
    }            // namespace mtl
}    // namespace nil