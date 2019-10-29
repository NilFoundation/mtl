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

#include <nil/mtl/io/scribe.hpp>

#include <nil/mtl/logger.hpp>

namespace nil {
    namespace mtl {
        namespace io {

            scribe::scribe(connection_handle conn_hdl) : scribe_base(conn_hdl) {
                // nop
            }

            scribe::~scribe() {
                MTL_LOG_TRACE("");
            }

            message scribe::detach_message() {
                return make_message(connection_closed_msg {hdl()});
            }

            bool scribe::consume(execution_unit *ctx, const void *, size_t num_bytes) {
                MTL_ASSERT(ctx != nullptr);
                MTL_LOG_TRACE(MTL_ARG(num_bytes));
                if (detached())
                    // we are already disconnected from the broker while the multiplexer
                    // did not yet remove the socket, this can happen if an I/O event causes
                    // the broker to call close_all() while the pollset contained
                    // further activities for the broker
                    return false;
                // keep a strong reference to our parent until we leave scope
                // to avoid UB when becoming detached during invocation
                auto guard = parent_;
                auto &buf = rd_buf();
                MTL_ASSERT(buf.size() >= num_bytes);
                // make sure size is correct, swap into message, and then call client
                buf.resize(num_bytes);
                auto &msg_buf = msg().buf;
                msg_buf.swap(buf);
                auto result = invoke_mailbox_element(ctx);
                // swap buffer back to stream and implicitly flush wr_buf()
                msg_buf.swap(buf);
                flush();
                return result;
            }

            void scribe::data_transferred(execution_unit *ctx, size_t written, size_t remaining) {
                MTL_LOG_TRACE(MTL_ARG(written) << MTL_ARG(remaining));
                if (detached())
                    return;
                using transferred_t = data_transferred_msg;
                using tmp_t = mailbox_element_vals<data_transferred_msg>;
                tmp_t tmp {strong_actor_ptr {}, make_message_id(), mailbox_element::forwarding_stack {},
                           transferred_t {hdl(), written, remaining}};
                invoke_mailbox_element_impl(ctx, tmp);
                // data_transferred_msg tmp{hdl(), written, remaining};
                // auto ptr = make_mailbox_element(nullptr, invalid_message_id, {}, tmp);
                // parent()->context(ctx);
                // parent()->consume(std::move(ptr));
            }

        }    // namespace io
    }        // namespace mtl
}    // namespace nil