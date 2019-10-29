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

#include <nil/mtl/downstream_manager.hpp>

#include <functional>
#include <limits>

#include <nil/mtl/actor_addr.hpp>
#include <nil/mtl/actor_cast.hpp>
#include <nil/mtl/scheduled_actor.hpp>
#include <nil/mtl/logger.hpp>
#include <nil/mtl/outbound_path.hpp>

namespace nil {
    namespace mtl {

        // -- constructors, destructors, and assignment operators ----------------------

        downstream_manager::downstream_manager::path_visitor::~path_visitor() {
            // nop
        }

        downstream_manager::downstream_manager::path_predicate::~path_predicate() {
            // nop
        }

        downstream_manager::downstream_manager(stream_manager *parent) : parent_(parent) {
            // nop
        }

        downstream_manager::~downstream_manager() {
            // nop
        }

        // -- properties ---------------------------------------------------------------

        scheduled_actor *downstream_manager::self() const noexcept {
            return parent_->self();
        }

        stream_manager *downstream_manager::parent() const noexcept {
            return parent_;
        }

        bool downstream_manager::terminal() const noexcept {
            return true;
        }

        // -- path management ----------------------------------------------------------

        std::vector<stream_slot> downstream_manager::path_slots() {
            std::vector<stream_slot> xs;
            xs.reserve(num_paths());
            for_each_path([&](outbound_path &x) { xs.emplace_back(x.slots.sender); });
            return xs;
        }

        std::vector<stream_slot> downstream_manager::open_path_slots() {
            std::vector<stream_slot> xs;
            xs.reserve(num_paths());
            for_each_path([&](outbound_path &x) {
                if (!x.closing)
                    xs.emplace_back(x.slots.sender);
            });
            return xs;
        }

        size_t downstream_manager::num_paths() const noexcept {
            return 0;
        }

        downstream_manager::path_ptr downstream_manager::add_path(stream_slot slot, strong_actor_ptr target) {
            MTL_LOG_TRACE(MTL_ARG(slot) << MTL_ARG(target));
            unique_path_ptr ptr {new outbound_path(slot, std::move(target))};
            auto result = ptr.get();
            return insert_path(std::move(ptr)) ? result : nullptr;
        }

        bool downstream_manager::remove_path(stream_slot, error, bool) noexcept {
            return false;
        }

        downstream_manager::const_path_ptr downstream_manager::path(stream_slot slot) const noexcept {
            // The `const` is restored when returning the pointer.
            return const_cast<downstream_manager &>(*this).path(slot);
        }

        auto downstream_manager::path(stream_slot) noexcept -> path_ptr {
            return nullptr;
        }

        bool downstream_manager::clean() const noexcept {
            auto pred = [](const outbound_path &x) { return x.clean(); };
            return buffered() == 0 && all_paths(pred);
        }

        bool downstream_manager::clean(stream_slot slot) const noexcept {
            auto ptr = path(slot);
            return ptr != nullptr ? buffered(slot) == 0 && ptr->clean() : false;
        }

        void downstream_manager::close() {
            MTL_LOG_TRACE("");
            auto open_slots = open_path_slots();
            for (auto slot : open_slots)
                close(slot);
        }

        void downstream_manager::close(stream_slot slot) {
            MTL_LOG_TRACE(MTL_ARG(slot));
            auto ptr = path(slot);
            if (ptr == nullptr) {
                MTL_LOG_DEBUG("cannot close unknown slot:" << slot);
                return;
            }
            if (buffered(slot) == 0 && ptr->clean()) {
                MTL_LOG_DEBUG("path clean, remove immediately;" << MTL_ARG(slot));
                remove_path(slot, none, false);
                return;
            }
            MTL_LOG_DEBUG("path not clean, set to closing;" << MTL_ARG(slot));
            ptr->closing = true;
        }

        void downstream_manager::abort(error reason) {
            MTL_LOG_TRACE(MTL_ARG(reason));
            for_each_path([&](outbound_path &x) {
                auto tmp = reason;
                about_to_erase(&x, false, &tmp);
            });
            clear_paths();
        }

        size_t downstream_manager::min_credit() const {
            if (empty())
                return 0u;
            auto result = std::numeric_limits<size_t>::max();
            const_cast<downstream_manager *>(this)->for_each_path(
                [&](outbound_path &x) { result = std::min(result, static_cast<size_t>(x.open_credit)); });
            return result;
        }

        size_t downstream_manager::max_credit() const {
            size_t result = 0;
            const_cast<downstream_manager *>(this)->for_each_path(
                [&](outbound_path &x) { result = std::max(result, static_cast<size_t>(x.open_credit)); });
            return result;
        }

        size_t downstream_manager::total_credit() const {
            size_t result = 0;
            const_cast<downstream_manager *>(this)->for_each_path(
                [&](outbound_path &x) { result += static_cast<size_t>(x.open_credit); });
            return result;
        }

        void downstream_manager::emit_batches() {
            // nop
        }

        void downstream_manager::force_emit_batches() {
            // nop
        }

        size_t downstream_manager::capacity() const noexcept {
            return std::numeric_limits<size_t>::max();
        }

        size_t downstream_manager::buffered() const noexcept {
            return 0;
        }

        size_t downstream_manager::buffered(stream_slot) const noexcept {
            return 0;
        }

        int32_t downstream_manager::max_capacity() const noexcept {
            return std::numeric_limits<int32_t>::max();
        }

        bool downstream_manager::stalled() const noexcept {
            auto no_credit = [](const outbound_path &x) { return x.open_credit == 0; };
            return capacity() == 0 && all_paths(no_credit);
        }

        void downstream_manager::clear_paths() {
            // nop
        }

        bool downstream_manager::insert_path(unique_path_ptr) {
            return false;
        }

        void downstream_manager::for_each_path_impl(path_visitor &) {
            // nop
        }

        bool downstream_manager::check_paths_impl(path_algorithm algo, path_predicate &) const noexcept {
            // Return the result for empty ranges as specified by the C++ standard.
            switch (algo) {
                default:
                    MTL_ASSERT(algo == path_algorithm::all_of);
                    return true;
                case path_algorithm::any_of:
                    return false;
                case path_algorithm::none_of:
                    return true;
            }
        }

        void downstream_manager::about_to_erase(outbound_path *ptr, bool silent, error *reason) {
            MTL_LOG_TRACE(MTL_ARG(ptr) << MTL_ARG(silent) << MTL_ARG(reason));
            if (!silent) {
                if (reason == nullptr)
                    ptr->emit_regular_shutdown(self());
                else
                    ptr->emit_irregular_shutdown(self(), std::move(*reason));
            }
        }

    }    // namespace mtl
}    // namespace nil