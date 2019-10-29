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

#include <nil/mtl/config.hpp>

#include <chrono>
#include <thread>
#include <atomic>
#include <cassert>

// GCC hack
#if defined(MTL_GCC) && !defined(_GLIBCXX_USE_SCHED_YIELD)
#include <time.h>
namespace std {
    namespace this_thread {
        namespace {
            inline void yield() noexcept {
                timespec req;
                req.tv_sec = 0;
                req.tv_nsec = 1;
                nanosleep(&req, nullptr);
            }
        }    // namespace
    }        // namespace this_thread
}    // namespace std
#endif

// another GCC hack
#if defined(MTL_GCC) && !defined(_GLIBCXX_USE_NANOSLEEP)
#include <time.h>
namespace std {
    namespace this_thread {
        namespace {
            template<class Rep, typename Period>
            inline void sleep_for(const chrono::duration<Rep, Period> &rt) {
                auto sec = chrono::duration_cast<chrono::seconds>(rt);
                auto nsec = chrono::duration_cast<chrono::nanoseconds>(rt - sec);
                timespec req;
                req.tv_sec = sec.count();
                req.tv_nsec = nsec.count();
                nanosleep(&req, nullptr);
            }
        }    // namespace
    }        // namespace this_thread
}    // namespace std
#endif

namespace nil {
    namespace mtl {
        namespace detail {

            /*
             * A thread-safe double-ended queue based on http://drdobbs.com/cpp/211601363.
             * This implementation is optimized for FIFO, i.e., it supports fast insertion
             * at the end and fast removal from the beginning. As long as the queue is
             * only used for FIFO operations, readers do not block writers and vice versa.
             */
            template<class T>
            class double_ended_queue {
            public:
                using value_type = T;
                using size_type = size_t;
                using difference_type = ptrdiff_t;
                using reference = value_type &;
                using const_reference = const value_type &;
                using pointer = value_type *;
                using const_pointer = const value_type *;

                class node {
                public:
                    pointer value;
                    std::atomic<node *> next;
                    explicit node(pointer val) : value(val), next(nullptr) {
                        // nop
                    }

                private:
                    static constexpr size_type payload_size = sizeof(pointer) + sizeof(std::atomic<node *>);
                    static constexpr size_type cline_size = MTL_CACHE_LINE_SIZE;
                    static constexpr size_type pad_size =
                        (cline_size * ((payload_size / cline_size) + 1)) - payload_size;
                    // avoid false sharing
                    static_assert(pad_size > 0, "invalid padding size calculated");
                    char pad[pad_size];
                };

                using unique_node_ptr = std::unique_ptr<node>;

                static_assert(sizeof(node *) < MTL_CACHE_LINE_SIZE, "sizeof(node*) >= MTL_CACHE_LINE_SIZE");

                double_ended_queue() {
                    head_lock_.clear();
                    tail_lock_.clear();
                    auto ptr = new node(nullptr);
                    head_ = ptr;
                    tail_ = ptr;
                }

                ~double_ended_queue() {
                    auto ptr = head_.load();
                    while (ptr) {
                        unique_node_ptr tmp {ptr};
                        ptr = tmp->next.load();
                    }
                }

                // acquires only one lock
                void append(pointer value) {
                    MTL_ASSERT(value != nullptr);
                    auto *tmp = new node(value);
                    lock_guard guard(tail_lock_);
                    // publish & swing last forward
                    tail_.load()->next = tmp;
                    tail_ = tmp;
                }

                // acquires both locks
                void prepend(pointer value) {
                    MTL_ASSERT(value != nullptr);
                    auto *tmp = new node(value);
                    node *first = nullptr;
                    // acquire both locks since we might touch last_ too
                    lock_guard guard1(head_lock_);
                    lock_guard guard2(tail_lock_);
                    first = head_.load();
                    MTL_ASSERT(first != nullptr);
                    auto next = first->next.load();
                    // first_ always points to a dummy with no value,
                    // hence we put the new element second
                    if (next) {
                        MTL_ASSERT(first != tail_);
                        tmp->next = next;
                    } else {
                        // queue is empty
                        MTL_ASSERT(first == tail_);
                        tail_ = tmp;
                    }
                    first->next = tmp;
                }

                // acquires only one lock, returns nullptr on failure
                pointer take_head() {
                    unique_node_ptr first;
                    pointer result = nullptr;
                    {    // lifetime scope of guard
                        lock_guard guard(head_lock_);
                        first.reset(head_.load());
                        node *next = first->next;
                        if (next == nullptr) {
                            // queue is empty
                            first.release();
                            return nullptr;
                        }
                        // take it out of the node & swing first forward
                        result = next->value;
                        next->value = nullptr;
                        head_ = next;
                    }
                    return result;
                }

                // acquires both locks, returns nullptr on failure
                pointer take_tail() {
                    pointer result = nullptr;
                    unique_node_ptr last;
                    {    // lifetime scope of guards
                        lock_guard guard1(head_lock_);
                        lock_guard guard2(tail_lock_);
                        MTL_ASSERT(head_ != nullptr);
                        last.reset(tail_.load());
                        if (last.get() == head_.load()) {
                            last.release();
                            return nullptr;
                        }
                        result = last->value;
                        tail_ = find_predecessor(last.get());
                        MTL_ASSERT(tail_ != nullptr);
                        tail_.load()->next = nullptr;
                    }
                    return result;
                }

                // does not lock
                bool empty() const {
                    // atomically compares first and last pointer without locks
                    return head_.load() == tail_.load();
                }

            private:
                // precondition: *both* locks acquired
                node *find_predecessor(node *what) {
                    for (auto i = head_.load(); i != nullptr; i = i->next) {
                        if (i->next == what) {
                            return i;
                        }
                    }
                    return nullptr;
                }

                // guarded by head_lock_
                std::atomic<node *> head_;
                char pad1_[MTL_CACHE_LINE_SIZE - sizeof(node *)];
                // guarded by tail_lock_
                std::atomic<node *> tail_;
                char pad2_[MTL_CACHE_LINE_SIZE - sizeof(node *)];
                // enforce exclusive access
                std::atomic_flag head_lock_;
                std::atomic_flag tail_lock_;

                class lock_guard {
                public:
                    explicit lock_guard(std::atomic_flag &lock) : lock_(lock) {
                        while (lock.test_and_set(std::memory_order_acquire)) {
                            std::this_thread::yield();
                        }
                    }
                    ~lock_guard() {
                        lock_.clear(std::memory_order_release);
                    }

                private:
                    std::atomic_flag &lock_;
                };
            };

        }    // namespace detail
    }        // namespace mtl
}    // namespace nil