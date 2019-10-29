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

#include <utility>

#include <nil/mtl/detail/int_list.hpp>
#include <nil/mtl/detail/type_list.hpp>

namespace nil {
    namespace mtl {
        namespace detail {

            // this utterly useless function works around a bug in Clang that causes
            // the compiler to reject the trailing return type of apply_args because
            // "get" is not defined (it's found during ADL)
            template<long Pos, class... Ts>
            typename tl_at<type_list<Ts...>, Pos>::type get(const type_list<Ts...> &);

            template<class F, long... Is, class Tuple>
            auto apply_args(F &f, detail::int_list<Is...>, Tuple &tup) -> decltype(f(get<Is>(tup)...)) {
                return f(get<Is>(tup)...);
            }

            template<class F, long... Is, class Tuple>
            auto apply_moved_args(F &f, detail::int_list<Is...>, Tuple &tup)
                -> decltype(f(std::move(get<Is>(tup))...)) {
                return f(std::move(get<Is>(tup))...);
            }

            template<class F, class Tuple, class... Ts>
            auto apply_args_prefixed(F &f, detail::int_list<>, Tuple &, Ts &&... xs)
                -> decltype(f(std::forward<Ts>(xs)...)) {
                return f(std::forward<Ts>(xs)...);
            }

            template<class F, long... Is, class Tuple, class... Ts>
            auto apply_args_prefixed(F &f, detail::int_list<Is...>, Tuple &tup, Ts &&... xs)
                -> decltype(f(std::forward<Ts>(xs)..., get<Is>(tup)...)) {
                return f(std::forward<Ts>(xs)..., get<Is>(tup)...);
            }

            template<class F, class Tuple, class... Ts>
            auto apply_moved_args_prefixed(F &f, detail::int_list<>, Tuple &, Ts &&... xs)
                -> decltype(f(std::forward<Ts>(xs)...)) {
                return f(std::forward<Ts>(xs)...);
            }

            template<class F, long... Is, class Tuple, class... Ts>
            auto apply_moved_args_prefixed(F &f, detail::int_list<Is...>, Tuple &tup, Ts &&... xs)
                -> decltype(f(std::forward<Ts>(xs)..., std::move(get<Is>(tup))...)) {
                return f(std::forward<Ts>(xs)..., std::move(get<Is>(tup))...);
            }

            template<class F, long... Is, class Tuple, class... Ts>
            auto apply_args_suffxied(F &f, detail::int_list<Is...>, Tuple &tup, Ts &&... xs)
                -> decltype(f(get<Is>(tup)..., std::forward<Ts>(xs)...)) {
                return f(get<Is>(tup)..., std::forward<Ts>(xs)...);
            }

        }    // namespace detail
    }        // namespace mtl
}