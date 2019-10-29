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

#pragma once

namespace nil {
    namespace mtl {
        namespace detail {

            template<class First, typename Second>
            struct type_pair {
                using first = First;
                using second = Second;
            };

            template<class First, typename Second>
            struct to_type_pair {
                using type = type_pair<First, Second>;
            };

            template<class What>
            struct is_type_pair {
                static constexpr bool value = false;
            };

            template<class First, typename Second>
            struct is_type_pair<type_pair<First, Second>> {
                static constexpr bool value = true;
            };

        }    // namespace detail
    }        // namespace mtl
}    // namespace nil