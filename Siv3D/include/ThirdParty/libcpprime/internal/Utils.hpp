/**
 *
 * libcpprime https://github.com/Rac75116/libcpprime
 *
 * Copyright (c) 2025 Rac75116
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

#ifndef CPPR_INTERNAL_INCLUDED_INTERNAL_UTILS
#define CPPR_INTERNAL_INCLUDED_INTERNAL_UTILS

#include <cstdint>
#include <type_traits>

#if defined(__has_include) && __has_include(<bit>) && (!defined(_MSVC_LANG) || _MSVC_LANG >= 202002L)
#include <bit>
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
#include <intrin.h>
#define CPPR_INTERNAL_VC_MUL_INTRINSICS
#pragma intrinsic(_umul128)
#pragma intrinsic(__umulh)
#endif
#if !defined(__clang__) && _MSC_VER >= 1920
#include <immintrin.h>
#define CPPR_INTERNAL_VC_DIV_INTRINSICS
#pragma intrinsic(_udiv128)
#endif
#endif

#if !defined(__cpp_lib_bitops) && defined(_MSC_VER) && !(CPPR_INTERNAL_HAS_BUILTIN(__builtin_ctzll) && CPPR_INTERNAL_HAS_BUILTIN(__builtin_clzll))
#include <intrin.h>
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

namespace cppr {

namespace internal {

#ifdef CPPR_INTERNAL_CONSTEXPR_ENABLED
constexpr bool IsConstantEvaluated() noexcept { return std::is_constant_evaluated(); }
#else
constexpr bool IsConstantEvaluated() noexcept { return std::false_type::value; }
#endif

#if defined(CPPR_INTERNAL_HAS_INT128_T)
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
using UInt128 = unsigned __int128;
#pragma GCC diagnostic pop
#else
using UInt128 = unsigned __int128;
#endif
#endif

struct Int64Pair {
    std::uint64_t high, low;
};

CPPR_INTERNAL_CONSTEXPR_INLINE void Assume(const bool cond) noexcept {
    // Hint for the optimizer; ignored during constant evaluation.
    if (IsConstantEvaluated()) return;
#if CPPR_INTERNAL_HAS_BUILTIN(__builtin_assume)
    __builtin_assume(cond);
#elif CPPR_INTERNAL_HAS_BUILTIN(__builtin_unreachable)
    if (!cond) __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(cond);
#else
    (void)cond;
#endif
}

CPPR_INTERNAL_CONSTEXPR_INLINE std::int32_t CountrZero(std::uint32_t n) noexcept {
    // Precondition: n != 0 (matches std::countr_zero requirements).
    Assume(n != 0);
#ifdef __cpp_lib_bitops
    return std::countr_zero(n);
#else
#if CPPR_INTERNAL_HAS_BUILTIN(__builtin_ctz)
    if (!IsConstantEvaluated()) return static_cast<std::int32_t>(__builtin_ctz(n));
#elif defined(_MSC_VER)
    if (!IsConstantEvaluated()) {
        unsigned long r;
        _BitScanForward(&r, n);
        return static_cast<std::int32_t>(r);
    }
#endif
    std::int32_t count = 0;
    if ((n << 16) == 0) {
        count += 16;
        n >>= 16;
    }
    if ((n << 24) == 0) {
        count += 8;
        n >>= 8;
    }
    if ((n << 28) == 0) {
        count += 4;
        n >>= 4;
    }
    if ((n << 30) == 0) {
        count += 2;
        n >>= 2;
    }
    if ((n << 31) == 0) {
        ++count;
    }
    return count;
#endif
}

CPPR_INTERNAL_CONSTEXPR_INLINE std::int32_t CountrZero(std::uint64_t n) noexcept {
    // Precondition: n != 0 (matches std::countr_zero requirements).
    Assume(n != 0);
#ifdef __cpp_lib_bitops
    return std::countr_zero(n);
#else
#if CPPR_INTERNAL_HAS_BUILTIN(__builtin_ctzll)
    if (!IsConstantEvaluated()) return static_cast<std::int32_t>(__builtin_ctzll(n));
#elif defined(_MSC_VER)
    if (!IsConstantEvaluated()) {
        unsigned long r;
        _BitScanForward64(&r, n);
        return static_cast<std::int32_t>(r);
    }
#endif
    bool f = (n & 0xffffffff) == 0;
    return (f ? 32 : 0) + CountrZero(static_cast<std::uint32_t>(n >> (f ? 32 : 0)));
#endif
}

CPPR_INTERNAL_CONSTEXPR_INLINE std::int32_t CountlZero(std::uint32_t n) noexcept {
    // Precondition: n != 0 (matches std::countl_zero requirements).
    Assume(n != 0);
#ifdef __cpp_lib_bitops
    return std::countl_zero(n);
#else
#if CPPR_INTERNAL_HAS_BUILTIN(__builtin_clz)
    if (!IsConstantEvaluated()) return static_cast<std::int32_t>(__builtin_clz(n));
#elif defined(_MSC_VER)
    if (!IsConstantEvaluated()) {
        unsigned long r;
        _BitScanReverse(&r, n);
        return static_cast<std::int32_t>(r ^ 31);
    }
#endif
    std::int32_t count = 0;
    if ((n >> 16) == 0) {
        count += 16;
        n <<= 16;
    }
    if ((n >> 24) == 0) {
        count += 8;
        n <<= 8;
    }
    if ((n >> 28) == 0) {
        count += 4;
        n <<= 4;
    }
    if ((n >> 30) == 0) {
        count += 2;
        n <<= 2;
    }
    if ((n >> 31) == 0) {
        ++count;
    }
    return count;
#endif
}

CPPR_INTERNAL_CONSTEXPR_INLINE std::int32_t CountlZero(std::uint64_t n) noexcept {
    // Precondition: n != 0 (matches std::countl_zero requirements).
    Assume(n != 0);
#ifdef __cpp_lib_bitops
    return std::countl_zero(n);
#else
#if CPPR_INTERNAL_HAS_BUILTIN(__builtin_clzll)
    if (!IsConstantEvaluated()) return static_cast<std::int32_t>(__builtin_clzll(n));
#elif defined(_MSC_VER)
    if (!IsConstantEvaluated()) {
        unsigned long r;
        _BitScanReverse64(&r, n);
        return static_cast<std::int32_t>(r ^ 63);
    }
#endif
    bool f = (n >> 32) == 0;
    return (f ? 32 : 0) + CountlZero(static_cast<std::uint32_t>(n >> (f ? 0 : 32)));
#endif
}

CPPR_INTERNAL_CONSTEXPR_INLINE Int64Pair Mulu128(std::uint64_t muler, std::uint64_t mulnd) noexcept {
    // Full 128-bit product split into {high, low}.
#if defined(CPPR_INTERNAL_HAS_INT128_T)
    UInt128 tmp = static_cast<UInt128>(muler) * mulnd;
    return {static_cast<std::uint64_t>(tmp >> 64), static_cast<std::uint64_t>(tmp)};
#else
#if defined(CPPR_INTERNAL_VC_MUL_INTRINSICS)
    if (!IsConstantEvaluated()) {
        std::uint64_t high;
        std::uint64_t low = _umul128(muler, mulnd, &high);
        return {high, low};
    }
#endif
    std::uint32_t lhigh = muler >> 32;
    std::uint32_t llow = muler & 0xffffffff;
    std::uint32_t rhigh = mulnd >> 32;
    std::uint32_t rlow = mulnd & 0xffffffff;
    std::uint64_t mlh = static_cast<std::uint64_t>(llow) * rhigh;
    std::uint64_t mhl = static_cast<std::uint64_t>(lhigh) * rlow;
    std::uint64_t ma = mlh + mhl;
    std::uint64_t mll = static_cast<std::uint64_t>(llow) * rlow;
    std::uint64_t mhh = static_cast<std::uint64_t>(lhigh) * rhigh;
    std::uint64_t carry1 = static_cast<std::uint64_t>(ma < mlh) << 32;
    std::uint64_t rl = mll + (ma << 32);
    std::uint64_t carry2 = rl < mll;
    return {mhh + carry1 + carry2 + (ma >> 32), rl};
#endif
}

CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t Mulu128High(std::uint64_t muler, std::uint64_t mulnd) noexcept {
    // High 64 bits of the 128-bit product.
#if defined(CPPR_INTERNAL_HAS_INT128_T)
    return static_cast<std::uint64_t>((static_cast<UInt128>(muler) * mulnd) >> 64);
#else
#if defined(CPPR_INTERNAL_VC_MUL_INTRINSICS)
    if (!IsConstantEvaluated()) return __umulh(muler, mulnd);
#endif
    return Mulu128(muler, mulnd).high;
#endif
}

CPPR_INTERNAL_CONSTEXPR std::uint64_t Modu128(std::uint64_t numhi, std::uint64_t numlo, std::uint64_t den) {
    // Computes ((numhi << 64) | numlo) % den.
    // Preconditions: den != 0 and numhi < den (so the quotient fits in 64 bits).
    Assume(den != 0);
    Assume(numhi < den);

#if defined(CPPR_INTERNAL_X86_64) && defined(CPPR_INTERNAL_GCC_STYLE_ASM)
    if (!IsConstantEvaluated()) {
        std::uint64_t quot, rem;
        __asm__("div %[v]" : "=a"(quot), "=d"(rem) : [v] "r"(den), "a"(numlo), "d"(numhi));
        return rem;
    }
#elif defined(CPPR_INTERNAL_VC_DIV_INTRINSICS)
    if (!IsConstantEvaluated()) {
        std::uint64_t rem = 0;
        _udiv128(numhi, numlo, den, &rem);
        return rem;
    }
#endif
#if defined(CPPR_INTERNAL_HAS_INT128_RUNTIME_DIV)
    UInt128 tmp = (static_cast<UInt128>(numhi) << 64) | numlo;
    return static_cast<std::uint64_t>(tmp % den);
#else
#if defined(CPPR_INTERNAL_HAS_INT128_T)
    if (IsConstantEvaluated()) {
        UInt128 tmp = (static_cast<UInt128>(numhi) << 64) | numlo;
        return static_cast<std::uint64_t>(tmp % den);
    }
#endif
    constexpr std::uint64_t b = (static_cast<std::uint64_t>(1) << 32);
    std::uint32_t q1, q0;
    std::int32_t shift;
    std::uint32_t den1, den0, num1, num0;
    std::uint64_t rem, qhat, rhat, c1, c2;
    shift = CountlZero(den);
    den <<= shift;
    numhi <<= shift;
    numhi |= (numlo >> (-shift & 63)) & static_cast<std::uint64_t>(-static_cast<std::int64_t>(shift) >> 63);
    numlo <<= shift;
    num1 = static_cast<std::uint32_t>(numlo >> 32);
    num0 = static_cast<std::uint32_t>(numlo & 0xFFFFFFFFu);
    den1 = static_cast<std::uint32_t>(den >> 32);
    den0 = static_cast<std::uint32_t>(den & 0xFFFFFFFFu);
    qhat = numhi / den1;
    rhat = numhi % den1;
    c1 = qhat * den0;
    c2 = rhat * b + num1;
    if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
    q1 = static_cast<std::uint32_t>(qhat);
    rem = numhi * b + num1 - q1 * den;
    qhat = rem / den1;
    rhat = rem % den1;
    c1 = qhat * den0;
    c2 = rhat * b + num0;
    if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
    q0 = static_cast<std::uint32_t>(qhat);
    return (rem * b + num0 - q0 * den) >> shift;
#endif
}

}  // namespace internal

}  // namespace cppr

#endif
