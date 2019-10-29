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

#include <string>
#include <utility>
#include <functional>

#include <nil/mtl/fwd.hpp>
#include <nil/mtl/none.hpp>
#include <nil/mtl/group_module.hpp>
#include <nil/mtl/intrusive_ptr.hpp>
#include <nil/mtl/abstract_group.hpp>

#include <nil/mtl/detail/comparable.hpp>
#include <nil/mtl/detail/type_traits.hpp>

namespace nil {
    namespace mtl {

        struct invalid_group_t {
            constexpr invalid_group_t() = default;
        };

        /// Identifies an invalid {@link group}.
        /// @relates group
        constexpr invalid_group_t invalid_group = invalid_group_t {};

        class group : detail::comparable<group>, detail::comparable<group, invalid_group_t> {
        public:
            using signatures = none_t;

            group() = default;

            group(group &&) = default;

            group(const group &) = default;

            group(const invalid_group_t &);

            group &operator=(group &&) = default;

            group &operator=(const group &) = default;

            group &operator=(const invalid_group_t &);

            group(abstract_group *);

            group(intrusive_ptr<abstract_group> gptr);

            inline explicit operator bool() const noexcept {
                return static_cast<bool>(ptr_);
            }

            inline bool operator!() const noexcept {
                return !ptr_;
            }

            static intptr_t compare(const abstract_group *lhs, const abstract_group *rhs);

            intptr_t compare(const group &other) const noexcept;

            inline intptr_t compare(const invalid_group_t &) const noexcept {
                return ptr_ ? 1 : 0;
            }

            template<class Inspector>
            friend typename Inspector::result_type inspect(Inspector &f, group &x) {
                std::string x_id;
                std::string x_mod;
                auto ptr = x.get();
                if (ptr) {
                    x_id = ptr->identifier();
                    x_mod = ptr->module().name();
                }
                return f(meta::type_name("group"), meta::omittable_if_empty(), x_id, meta::omittable_if_empty(), x_mod);
            }

            friend error inspect(serializer &, group &);

            friend error inspect(deserializer &, group &);

            inline abstract_group *get() const noexcept {
                return ptr_.get();
            }

            /// @cond PRIVATE

            template<class... Ts>
            void eq_impl(message_id mid, strong_actor_ptr sender, execution_unit *ctx, Ts &&... xs) const {
                MTL_ASSERT(!mid.is_request());
                if (ptr_)
                    ptr_->enqueue(std::move(sender), mid, make_message(std::forward<Ts>(xs)...), ctx);
            }

            inline bool subscribe(strong_actor_ptr who) const {
                if (!ptr_)
                    return false;
                return ptr_->subscribe(std::move(who));
            }

            inline void unsubscribe(const actor_control_block *who) const {
                if (ptr_)
                    ptr_->unsubscribe(who);
            }

            /// MTL's messaging primitives assume a non-null guarantee. A group
            /// object indirects pointer-like access to a group to prevent UB.
            inline const group *operator->() const noexcept {
                return this;
            }

            /// @endcond

        private:
            inline abstract_group *release() noexcept {
                return ptr_.detach();
            }

            group(abstract_group *, bool);

            abstract_group_ptr ptr_;
        };

        /// @relates group
        std::string to_string(const group &x);

    }    // namespace mtl
}    // namespace nil

namespace std {
    template<>
    struct hash<nil::mtl::group> {
        inline size_t operator()(const nil::mtl::group &x) const {
            // groups are singleton objects, the address is thus the best possible hash
            return !x ? 0 : reinterpret_cast<size_t>(x.get());
        }
    };
}    // namespace std