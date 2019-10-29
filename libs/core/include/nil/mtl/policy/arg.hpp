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
        namespace policy {

            /// Provides a wrapper to pass policy types as values to functions.
            template<class... Ts>
            struct arg {
            public:
                static const arg value;
            };

            template<class... Ts>
            const arg<Ts...> arg<Ts...>::value = arg<Ts...> {};

        }    // namespace policy
    }        // namespace mtl
}    // namespace nil