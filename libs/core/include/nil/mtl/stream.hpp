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

#include <nil/mtl/fwd.hpp>
#include <nil/mtl/invalid_stream.hpp>
#include <nil/mtl/stream_manager.hpp>
#include <nil/mtl/stream_slot.hpp>

#include <nil/mtl/meta/type_name.hpp>

namespace nil {
    namespace mtl {

        /// Empty marker type for streaming handshakes.
        template<class T>
        class stream {
        public:
            using value_type = T;
        };

    }    // namespace mtl
}    // namespace nil
