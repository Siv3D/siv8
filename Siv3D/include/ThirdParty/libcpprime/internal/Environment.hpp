/**
 *
 * libcpprime https://github.com/sortA0329/libcpprime
 *
 * Copyright (c) 2026 sortA
 * SPDX-License-Identifier: MIT
 *
 **/
// This file contains code derived from libdivide
// https://libdivide.com
//
// Original work:
// Copyright (C) 2010 - 2022 ridiculous_fish <libdivide@ridiculousfish.com>
// Copyright (C) 2016 - 2022 Kim Walisch <kim.walisch@gmail.com>
//
// libdivide is dual-licensed under the Boost Software License 1.0
// and the zlib License. This project uses the Boost Software License 1.0.
//
// The original code has been modified and integrated into this library.
// Modifications include refactoring and API replacement.
//
// Except for the portions derived from libdivide, the remainder of this
// library is licensed under the MIT License.

#ifndef CPPR_INTERNAL_INCLUDED_INTERNAL_ENVIRONMENT
#define CPPR_INTERNAL_INCLUDED_INTERNAL_ENVIRONMENT

#include <type_traits>  // IWYU pragma: export

#if defined(__has_include) && __has_include(<bit>) && (!defined(_MSVC_LANG) || _MSVC_LANG >= 202002L)
#include <bit>  // IWYU pragma: export
#endif

#if defined(__cpp_lib_is_constant_evaluated) && defined(__cpp_constexpr) && __cpp_constexpr >= 201907L
#define CPPR_INTERNAL_CONSTEXPR_ENABLED
#define CPPR_INTERNAL_CONSTEXPR constexpr
#else
#define CPPR_INTERNAL_CONSTEXPR inline
#endif

#ifdef __cpp_if_constexpr
#define CPPR_INTERNAL_IF_CONSTEXPR constexpr
#else
#define CPPR_INTERNAL_IF_CONSTEXPR
#endif

#ifdef __has_builtin
#define CPPR_INTERNAL_HAS_BUILTIN(x) __has_builtin(x)
#else
#define CPPR_INTERNAL_HAS_BUILTIN(x) 0
#endif

#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC))
#if !defined(__clang__) || _MSC_VER > 1930
#include <intrin.h>  // IWYU pragma: export
#define CPPR_INTERNAL_VC_MUL_INTRINSICS
#pragma intrinsic(_umul128)
#pragma intrinsic(__umulh)
#endif
#if !defined(__clang__) && _MSC_VER >= 1920
#include <immintrin.h>  // IWYU pragma: export
#define CPPR_INTERNAL_VC_DIV_INTRINSICS
#pragma intrinsic(_udiv128)
#endif
#endif

#if !defined(__cpp_lib_bitops) && defined(_MSC_VER) && !(CPPR_INTERNAL_HAS_BUILTIN(__builtin_ctzll) && CPPR_INTERNAL_HAS_BUILTIN(__builtin_clzll))
#include <intrin.h>  // IWYU pragma: export
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#endif

#ifdef __SIZEOF_INT128__
#define CPPR_INTERNAL_HAS_INT128_T
#if !(defined(__clang__) && defined(_MSC_VER))
#define CPPR_INTERNAL_HAS_INT128_RUNTIME_DIV
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define CPPR_INTERNAL_X86_64
#endif

#if defined(__GNUC__) || defined(__clang__)
#define CPPR_INTERNAL_GCC_STYLE_ASM
#endif

#ifdef __has_attribute
#if __has_attribute(always_inline)
#define CPPR_INTERNAL_INLINE_LAMBDA __attribute__((always_inline))
#endif
#endif
#ifndef CPPR_INTERNAL_INLINE_LAMBDA
#define CPPR_INTERNAL_INLINE_LAMBDA
#endif

#ifdef __has_attribute
#if __has_attribute(always_inline)
#define CPPR_INTERNAL_CONSTEXPR_INLINE CPPR_INTERNAL_CONSTEXPR __attribute__((always_inline))
#endif
#endif
#ifndef CPPR_INTERNAL_CONSTEXPR_INLINE
#ifdef _MSC_VER
#define CPPR_INTERNAL_CONSTEXPR_INLINE CPPR_INTERNAL_CONSTEXPR __forceinline
#else
#define CPPR_INTERNAL_CONSTEXPR_INLINE CPPR_INTERNAL_CONSTEXPR
#endif
#endif

#endif
