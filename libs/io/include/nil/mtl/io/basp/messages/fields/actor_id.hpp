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

                template<typename FieldBaseType>
                using dflt_actor_id_field =
                    marshalling::field::int_value<FieldBaseType, actor_id, marshalling::option::default_num_value<0>>;

                template<typename FieldBaseType>
                using actor_id_field = marshalling::field::int_value<FieldBaseType, actor_id>;

                /// @}
            }    // namespace basp
        }        // namespace io
    }            // namespace mtl
}    // namespace nil