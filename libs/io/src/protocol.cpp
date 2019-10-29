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

#include <nil/mtl/io/network/protocol.hpp>

namespace nil {
    namespace mtl {
        namespace io {
            namespace network {

                std::string to_string(const protocol &x) {
                    std::string result;
                    result += to_string(x.trans);
                    result += "/";
                    result += to_string(x.net);
                    return result;
                }

            }    // namespace network
        }        // namespace io
    }            // namespace mtl
}    // namespace nil
