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

#include <nil/mtl/outbound_path.hpp>

#include <nil/mtl/local_actor.hpp>
#include <nil/mtl/logger.hpp>
#include <nil/mtl/no_stages.hpp>
#include <nil/mtl/send.hpp>

namespace nil {
    namespace mtl {

        namespace {

            // TODO: consider making this parameter configurable
            constexpr int32_t max_batch_size = 128 * 1024;

        }    // namespace

        outbound_path::outbound_path(stream_slot sender_slot, strong_actor_ptr receiver_hdl) :
            slots(sender_slot, invalid_stream_slot), hdl(std::move(receiver_hdl)), next_batch_id(1), open_credit(0),
            desired_batch_size(50), next_ack_id(1), max_capacity(0), closing(false) {
            // nop
        }

        outbound_path::~outbound_path() {
            // nop
        }

        void outbound_path::emit_open(local_actor *self, stream_slot slot, strong_actor_ptr to, message handshake_data,
                                      stream_priority prio) {
            MTL_LOG_TRACE(MTL_ARG(slot) << MTL_ARG(to) << MTL_ARG(handshake_data) << MTL_ARG(prio));
            MTL_ASSERT(self != nullptr);
            MTL_ASSERT(to != nullptr);
            // Make sure we receive errors from this point on.
            stream_aborter::add(to, self->address(), slot, stream_aborter::sink_aborter);
            // Send message.
            unsafe_send_as(self, to, open_stream_msg {slot, std::move(handshake_data), self->ctrl(), nullptr, prio});
        }

        void outbound_path::emit_batch(local_actor *self, int32_t xs_size, message xs) {
            MTL_LOG_TRACE(MTL_ARG(slots) << MTL_ARG(xs_size) << MTL_ARG(xs));
            MTL_ASSERT(xs_size > 0);
            MTL_ASSERT(xs_size <= std::numeric_limits<int32_t>::max());
            MTL_ASSERT(open_credit >= xs_size);
            open_credit -= xs_size;
            MTL_ASSERT(open_credit >= 0);
            auto bid = next_batch_id++;
            downstream_msg::batch batch {static_cast<int32_t>(xs_size), std::move(xs), bid};
            unsafe_send_as(self, hdl, downstream_msg {slots, self->address(), std::move(batch)});
        }

        void outbound_path::emit_regular_shutdown(local_actor *self) {
            MTL_LOG_TRACE(MTL_ARG(slots));
            unsafe_send_as(self, hdl, make<downstream_msg::close>(slots, self->address()));
        }

        void outbound_path::emit_irregular_shutdown(local_actor *self, error reason) {
            MTL_LOG_TRACE(MTL_ARG(slots) << MTL_ARG(reason));
            /// Note that we always send abort messages anonymous. They can get send
            /// after `self` already terminated and we must not form strong references
            /// after that point. Since downstream messages contain the sender address
            /// anyway, we only omit redundant information.
            anon_send(actor_cast<actor>(hdl),
                      make<downstream_msg::forced_close>(slots, self->address(), std::move(reason)));
        }

        void outbound_path::emit_irregular_shutdown(local_actor *self,
                                                    stream_slots slots,
                                                    const strong_actor_ptr &hdl,
                                                    error reason) {
            MTL_LOG_TRACE(MTL_ARG(slots) << MTL_ARG(hdl) << MTL_ARG(reason));
            /// Note that we always send abort messages anonymous. See reasoning in first
            /// function overload.
            anon_send(actor_cast<actor>(hdl),
                      make<downstream_msg::forced_close>(slots, self->address(), std::move(reason)));
        }

        void outbound_path::set_desired_batch_size(int32_t value) noexcept {
            if (value == desired_batch_size)
                return;
            desired_batch_size = value < 0 || value > max_batch_size ? max_batch_size : value;
        }

    }    // namespace mtl
}    // namespace nil