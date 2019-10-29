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

#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

#include <nil/mtl/fwd.hpp>
#include <nil/mtl/abstract_actor.hpp>
#include <nil/mtl/actor_control_block.hpp>

#include <nil/mtl/detail/comparable.hpp>

namespace nil {
    namespace mtl {

        /// Stores the address of typed as well as untyped actors.
        class actor_addr : detail::comparable<actor_addr>,
                           detail::comparable<actor_addr, weak_actor_ptr>,
                           detail::comparable<actor_addr, strong_actor_ptr>,
                           detail::comparable<actor_addr, abstract_actor *>,
                           detail::comparable<actor_addr, actor_control_block *> {
        public:
            // -- friend types that need access to private ctors

            friend class abstract_actor;

            // allow conversion via actor_cast
            template<class, class, int>
            friend class actor_cast_access;

            actor_addr() = default;
            actor_addr(actor_addr &&) = default;
            actor_addr(const actor_addr &) = default;
            actor_addr &operator=(actor_addr &&) = default;
            actor_addr &operator=(const actor_addr &) = default;

            actor_addr(std::nullptr_t);

            actor_addr &operator=(std::nullptr_t);

            /// Returns the ID of this actor.
            inline actor_id id() const noexcept {
                return ptr_->id();
            }

            /// Returns the origin node of this actor.
            inline node_id node() const noexcept {
                return ptr_->node();
            }

            /// Returns the hosting actor system.
            inline actor_system &home_system() const noexcept {
                return *ptr_->home_system;
            }

            /// Exchange content of `*this` and `other`.
            void swap(actor_addr &other) noexcept;

            inline explicit operator bool() const {
                return static_cast<bool>(ptr_);
            }

            /// @cond PRIVATE

            static intptr_t compare(const actor_control_block *lhs, const actor_control_block *rhs);

            intptr_t compare(const actor_addr &other) const noexcept;

            intptr_t compare(const abstract_actor *other) const noexcept;

            intptr_t compare(const actor_control_block *other) const noexcept;

            inline intptr_t compare(const weak_actor_ptr &other) const noexcept {
                return compare(other.get());
            }

            inline intptr_t compare(const strong_actor_ptr &other) const noexcept {
                return compare(other.get());
            }

            friend inline std::string to_string(const actor_addr &x) {
                return to_string(x.ptr_);
            }

            friend inline void append_to_string(std::string &x, const actor_addr &y) {
                return append_to_string(x, y.ptr_);
            }

            template<class Inspector>
            friend typename Inspector::result_type inspect(Inspector &f, actor_addr &x) {
                return inspect(f, x.ptr_);
            }

            /// Releases the reference held by handle `x`. Using the
            /// handle after invalidating it is undefined behavior.
            friend void destroy(actor_addr &x) {
                x.ptr_.reset();
            }

            actor_addr(actor_control_block *, bool);

            inline actor_control_block *get() const noexcept {
                return ptr_.get();
            }

            /// @endcond

        private:
            inline actor_control_block *release() noexcept {
                return ptr_.release();
            }

            inline actor_control_block *get_locked() const noexcept {
                return ptr_.get_locked();
            }

            actor_addr(actor_control_block *);

            weak_actor_ptr ptr_;
        };

        /*!
         * @brief tell actor_cast which semantic this type uses
         */
        template<>
        struct has_weak_ptr_semantics<actor_addr> {
            constexpr static const bool value = true;
        };

        inline bool operator==(const actor_addr &x, std::nullptr_t) {
            return x.get() == nullptr;
        }

        inline bool operator==(std::nullptr_t, const actor_addr &x) {
            return x.get() == nullptr;
        }

        inline bool operator!=(const actor_addr &x, std::nullptr_t) {
            return x.get() != nullptr;
        }

        inline bool operator!=(std::nullptr_t, const actor_addr &x) {
            return x.get() != nullptr;
        }

    }    // namespace mtl
}    // namespace nil

// allow actor_addr to be used in hash maps
namespace std {
    template<>
    struct hash<nil::mtl::actor_addr> {
        inline size_t operator()(const nil::mtl::actor_addr &ref) const {
            return static_cast<size_t>(ref.id());
        }
    };
}    // namespace std