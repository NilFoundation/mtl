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

// this header must be generated by the build system (may be empty)
#include <nil/mtl/detail/build_config.hpp>

// Platform-specific adjustments.
#define MTL_CACHE_LINE_SIZE 64

// Config pararameters defined by the build system (usually CMake):
//
// MTL_ENABLE_RUNTIME_CHECKS:
//   - check requirements at runtime
//
// MTL_LOG_LEVEL:
//   - denotes the amount of logging, ranging from error messages only (0)
//     to complete traces (4)

/// Denotes version of MTL in the format {MAJOR}{MINOR}{PATCH},
/// whereas each number is a two-digit decimal number without
/// leading zeros (e.g. 900 is version 0.9.0).
#define MTL_VERSION 1701

/// Defined to the major version number of MTL.
#define MTL_MAJOR_VERSION (MTL_VERSION / 10000)

/// Defined to the minor version number of MTL.
#define MTL_MINOR_VERSION ((MTL_VERSION / 100) % 100)

/// Defined to the patch version number of MTL.
#define MTL_PATCH_VERSION (MTL_VERSION % 100)

// This compiler-specific block defines:
// - MTL_DEPRECATED to annotate deprecated functions
// - MTL_PUSH_WARNINGS/MTL_POP_WARNINGS to surround "noisy" header includes
// - MTL_ANNOTATE_FALLTHROUGH to suppress warnings in switch/case statements
// - MTL_COMPILER_VERSION to retrieve the compiler version in MTL_VERSION format
// - One of the following:
//   + MTL_CLANG
//   + MTL_GCC
//   + MTL_MSVC

// sets MTL_DEPRECATED, MTL_ANNOTATE_FALLTHROUGH,
// MTL_PUSH_WARNINGS and MTL_POP_WARNINGS
#if defined(__clang__)
#define MTL_CLANG
#define MTL_LIKELY(x) __builtin_expect((x), 1)
#define MTL_UNLIKELY(x) __builtin_expect((x), 0)
#define MTL_DEPRECATED __attribute__((deprecated))
#define MTL_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#define MTL_PUSH_WARNINGS                                                                                              \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wall\"") _Pragma(                            \
        "clang diagnostic ignored \"-Wextra\"") _Pragma("clang diagnostic ignored \"-Wundef\"")                        \
        _Pragma("clang diagnostic ignored \"-Wshadow\"") _Pragma("clang diagnostic ignored \"-Wdeprecated\"") _Pragma( \
            "clang diagnostic ignored \"-Wextra-semi\"") _Pragma("clang diagnostic ignored \"-Wconversion\"")          \
            _Pragma("clang diagnostic ignored \"-Wcast-align\"") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"") \
                _Pragma("clang diagnostic ignored \"-Wswitch-enum\"")                                                  \
                    _Pragma("clang diagnostic ignored \"-Wweak-vtables\"") _Pragma(                                    \
                        "clang diagnostic ignored \"-Wdocumentation\"")                                                \
                        _Pragma("clang diagnostic ignored \"-Wold-style-cast\"") _Pragma(                              \
                            "clang diagnostic ignored \"-Wsign-conversion\"")                                          \
                            _Pragma("clang diagnostic ignored \"-Wunused-template\"") _Pragma(                         \
                                "clang diagnostic ignored \"-Wshorten-64-to-32\"")                                     \
                                _Pragma("clang diagnostic ignored \"-Wunreachable-code\"") _Pragma(                    \
                                    "clang diagnostic ignored \"-Wdouble-promotion\"")                                 \
                                    _Pragma("clang diagnostic ignored \"-Wc++14-extensions\"") _Pragma(                \
                                        "clang diagnostic ignored \"-Wunused-parameter\"")                             \
                                        _Pragma("clang diagnostic ignored \"-Wnested-anon-types\"") _Pragma(           \
                                            "clang diagnostic ignored \"-Wreserved-id-macro\"")                        \
                                            _Pragma("clang diagnostic ignored \"-Wconstant-conversion\"") _Pragma(     \
                                                "clang diagnostic ignored \"-Wimplicit-fallthrough\"")                 \
                                                _Pragma("clang diagnostic ignored \"-Wused-but-marked-unused\"")       \
                                                    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"")
#define MTL_PUSH_UNUSED_LABEL_WARNING \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wunused-label\"")
#define MTL_PUSH_NON_VIRTUAL_DTOR_WARNING \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wnon-virtual-dtor\"")
#define MTL_PUSH_DEPRECATED_WARNING \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#define MTL_POP_WARNINGS _Pragma("clang diagnostic pop")
#define MTL_ANNOTATE_FALLTHROUGH [[clang::fallthrough]]
#define MTL_COMPILER_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#if !__has_feature(cxx_thread_local)
#define MTL_NO_THREAD_LOCAL
#endif
#elif defined(__GNUC__)
#define MTL_GCC
#define MTL_LIKELY(x) __builtin_expect((x), 1)
#define MTL_UNLIKELY(x) __builtin_expect((x), 0)
#define MTL_DEPRECATED __attribute__((deprecated))
#define MTL_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#define MTL_PUSH_WARNINGS                                                                                        \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wshadow\"")                                \
        _Pragma("GCC diagnostic ignored \"-Wpragmas\"") _Pragma("GCC diagnostic ignored \"-Wpedantic\"")         \
            _Pragma("GCC diagnostic ignored \"-Wcast-qual\"") _Pragma("GCC diagnostic ignored \"-Wconversion\"") \
                _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")                                              \
                    _Pragma("GCC diagnostic ignored \"-Wc++14-extensions\"")
#define MTL_PUSH_UNUSED_LABEL_WARNING \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-label\"")
#define MTL_PUSH_NON_VIRTUAL_DTOR_WARNING \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wnon-virtual-dtor\"")
#define MTL_PUSH_DEPRECATED_WARNING \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#define MTL_POP_WARNINGS _Pragma("GCC diagnostic pop")
#if __GNUC__ >= 7
#define MTL_ANNOTATE_FALLTHROUGH __attribute__((fallthrough))
#else
#define MTL_ANNOTATE_FALLTHROUGH static_cast<void>(0)
#endif
#define MTL_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
// disable thread_local on GCC/macOS due to heap-use-after-free bug:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67135
#ifdef __APPLE__
#define MTL_NO_THREAD_LOCAL
#endif
#elif defined(_MSC_VER)
#define MTL_MSVC
#define MTL_LIKELY(x) x
#define MTL_UNLIKELY(x) x
#define MTL_DEPRECATED
#define MTL_DEPRECATED_MSG(msg)
#define MTL_PUSH_WARNINGS __pragma(warning(push))
#define MTL_PUSH_UNUSED_LABEL_WARNING __pragma(warning(push)) __pragma(warning(disable : 4102))
#define MTL_PUSH_DEPRECATED_WARNING __pragma(warning(push))
#define MTL_PUSH_NON_VIRTUAL_DTOR_WARNING __pragma(warning(push))
#define MTL_POP_WARNINGS __pragma(warning(pop))
#define MTL_ANNOTATE_FALLTHROUGH static_cast<void>(0)
#define MTL_COMPILER_VERSION _MSC_FULL_VER
#pragma warning(disable : 4624)
#pragma warning(disable : 4800)
#pragma warning(disable : 4503)
#ifndef NOMINMAX
#define NOMINMAX
#endif    // NOMINMAX
#else
#define MTL_LIKELY(x) x
#define MTL_UNLIKELY(x) x
#define MTL_DEPRECATED
#define MTL_PUSH_WARNINGS
#define MTL_PUSH_NON_VIRTUAL_DTOR_WARNING
#define MTL_PUSH_DEPRECATED_WARNING
#define MTL_POP_WARNINGS
#define MTL_ANNOTATE_FALLTHROUGH static_cast<void>(0)
#endif

// This OS-specific block defines one of the following:
// - MTL_MACOS
// - MTL_LINUX
// - MTL_BSD
// - MTL_WINDOWS
// It also defines MTL_POSIX for POSIX-compatible systems
#if defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#define MTL_IOS
#else
#define MTL_MACOS
#if defined(MTL_GCC) && !defined(_GLIBCXX_HAS_GTHREADS)
#define _GLIBCXX_HAS_GTHREADS
#endif
#endif
#elif defined(__ANDROID__)
#define MTL_ANDROID
#elif defined(__linux__)
#define MTL_LINUX
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 16)
#define MTL_POLL_IMPL
#endif
#elif defined(__FreeBSD__)
#define MTL_BSD
#elif defined(__CYGWIN__)
#define MTL_CYGWIN
#elif defined(WIN32) || defined(_WIN32)
#define MTL_WINDOWS
#else
#error Platform and/or compiler not supported
#endif
#if defined(MTL_MACOS) || defined(MTL_LINUX) || defined(MTL_BSD) || defined(MTL_CYGWIN)
#define MTL_POSIX
#endif

#if defined(MTL_WINDOWS) && defined(MTL_CLANG)
// Fix for issue with static_cast<> in objbase.h.
// See: https://github.com/philsquared/Catch/issues/690.
struct IUnknown;
#endif

#include <cstdio>
#include <cstdlib>

// Optionally enable MTL_ASSERT
#ifndef MTL_ENABLE_RUNTIME_CHECKS
#define MTL_ASSERT(unused) static_cast<void>(0)
#elif defined(MTL_WINDOWS) || defined(MTL_BSD)
#define MTL_ASSERT(stmt)                                                       \
    if (static_cast<bool>(stmt) == false) {                                    \
        printf("%s:%u: requirement failed '%s'\n", __FILE__, __LINE__, #stmt); \
        ::abort();                                                             \
    }                                                                          \
    static_cast<void>(0)
#else    // defined(MTL_LINUX) || defined(MTL_MACOS)
#include <execinfo.h>
#define MTL_ASSERT(stmt)                                                       \
    if (static_cast<bool>(stmt) == false) {                                    \
        printf("%s:%u: requirement failed '%s'\n", __FILE__, __LINE__, #stmt); \
        void *array[20];                                                       \
        auto mtl_bt_size = ::backtrace(array, 20);                             \
        ::backtrace_symbols_fd(array, mtl_bt_size, 2);                         \
        ::abort();                                                             \
    }                                                                          \
    static_cast<void>(0)
#endif

// Convenience macros.
#define MTL_IGNORE_UNUSED(x) static_cast<void>(x)

#define MTL_CRITICAL(error)                                                                  \
    do {                                                                                     \
        fprintf(stderr, "[FATAL] %s:%u: critical error: '%s'\n", __FILE__, __LINE__, error); \
        ::abort();                                                                           \
    } while (false)
