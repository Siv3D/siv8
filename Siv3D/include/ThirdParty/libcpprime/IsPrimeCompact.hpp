/**
 *
 * libcpprime https://github.com/sortA0329/libcpprime
 *
 * Copyright (c) 2026 sortA
 * SPDX-License-Identifier: MIT
 *
 **/

#ifndef CPPR_INTERNAL_INCLUDED_IS_PRIME_COMPACT
#define CPPR_INTERNAL_INCLUDED_IS_PRIME_COMPACT

#include <cstdint>

#include "internal/Environment.hpp"
#include "internal/IsPrimeCommon.hpp"
#include "internal/Utils.hpp"

namespace cppr {

namespace internal {

constexpr std::uint32_t FlagTable10[32] = {
#include "internal/IsPrimeTable10.txt"
};
CPPR_INTERNAL_CONSTEXPR_INLINE bool IsPrime10(const std::uint64_t n) noexcept { return (FlagTable10[n / 32] >> (n % 32)) & 1; }

CPPR_INTERNAL_CONSTEXPR_INLINE bool GCDFilter(const std::uint32_t n) noexcept {
    auto GCD = [](std::uint32_t x, std::uint32_t y) CPPR_INTERNAL_INLINE_LAMBDA -> std::uint32_t {
        // Binary GCD (Stein's algorithm).
        if (x == 0) return 0;
        Assume(y != 0);
        const std::int32_t n = CountrZero(x);
        const std::int32_t m = CountrZero(y);
        const std::int32_t l = n < m ? n : m;
        x >>= n;
        y >>= m;
        while (x != y) {
            const std::uint32_t a = y - x;
            const std::uint32_t b = x - y;
            const std::int32_t p = CountrZero(a);
            const std::int32_t q = CountrZero(b);
            Assume(p == q);
            const std::uint32_t s = y < x ? b : a;
            const std::uint32_t t = x < y ? x : y;
            x = s >> p;
            y = t;
        }
        return x << l;
    };

    const std::uint32_t a = static_cast<std::uint32_t>(Modu128(272518712866683587u % n, 10755835586592736005u, n));
    if (n < 11881) return GCD(a, n) == 1;
    const std::uint32_t b = static_cast<std::uint32_t>(Modu128(827936745744686818u % n, 10132550402535125089u, n));
    return GCD((a * b) % n, n) == 1;
}

constexpr std::uint16_t BasesCompact[256] = {
#include "internal/IsPrimeBases64Compact.txt"
};

template <bool Strict>
CPPR_INTERNAL_CONSTEXPR_INLINE bool IsPrime64Compact(const std::uint64_t x) noexcept {
    const MontgomeryModint64Impl<Strict> mint(x);
    const std::int32_t S = CountrZero(x - 1);
    const std::uint64_t D = (x - 1) >> S;
    const auto one = mint.one();
    const auto mone = mint.mone();
    auto test2 = [=](std::uint64_t base1, std::uint64_t base2) -> bool {
        auto a = one;
        auto b = one;
        auto c = mint.build(base1);
        auto d = mint.build(base2);
        std::uint64_t ex = D;
        while (ex != 1) {
            const auto e = mint.mul(c, c);
            const auto f = mint.mul(d, d);
            if (ex & 1) {
                a = mint.mul(a, c);
                b = mint.mul(b, d);
            }
            c = e;
            d = f;
            ex >>= 1;
        }
        a = mint.mul(a, c);
        b = mint.mul(b, d);
        bool res1 = mint.same(a, one) || mint.same(a, mone);
        bool res2 = mint.same(b, one) || mint.same(b, mone);
        if (x % 4 == 1 && !(res1 && res2)) {
            for (std::int32_t i = 0; i != S - 1; ++i) {
                a = mint.mul(a, a);
                b = mint.mul(b, b);
                res1 |= mint.same(a, mone);
                res2 |= mint.same(b, mone);
            }
        }
        return res1 && res2;
    };
    auto test3 = [=](std::uint64_t base1, std::uint64_t base2, std::uint64_t base3) -> bool {
        auto a = one;
        auto b = one;
        auto c = one;
        auto d = mint.build(base1);
        auto e = mint.build(base2);
        auto f = mint.build(base3);
        std::uint64_t ex = D;
        while (ex != 1) {
            const auto g = mint.mul(d, d);
            const auto h = mint.mul(e, e);
            const auto i = mint.mul(f, f);
            if (ex & 1) {
                a = mint.mul(a, d);
                b = mint.mul(b, e);
                c = mint.mul(c, f);
            }
            d = g;
            e = h;
            f = i;
            ex >>= 1;
        }
        a = mint.mul(a, d);
        b = mint.mul(b, e);
        c = mint.mul(c, f);
        bool res1 = mint.same(a, one) || mint.same(a, mone);
        bool res2 = mint.same(b, one) || mint.same(b, mone);
        bool res3 = mint.same(c, one) || mint.same(c, mone);
        if (x % 4 == 1 && !(res1 && res2 && res3)) {
            for (std::int32_t i = 0; i != S - 1; ++i) {
                a = mint.mul(a, a);
                b = mint.mul(b, b);
                c = mint.mul(c, c);
                res1 |= mint.same(a, mone);
                res2 |= mint.same(b, mone);
                res3 |= mint.same(c, mone);
            }
        }
        return res1 && res2 && res3;
    };

    // These bases were discovered by Steve Worley and Jim Sinclair.
    if (x < 55245642489451ull) {
        if (x < 350269456337ull) {
            return test3(4230279247111683200ull, 14694767155120705706ull, 16641139526367750375ull);
        } else {
            return test2(2ull, 141889084524735ull) && test2(1199124725622454117ull, 11096072698276303650ull);
        }
    }

    if (!test2(2ull, 9375ull)) return false;
    const std::uint16_t pair = BasesCompact[(2298633409u * static_cast<std::uint32_t>(x)) >> 24];
    const std::uint64_t base1 = pair >> 8, base2 = pair & 0xff;
    return test3(13ull, base1, base2);
}

}  // namespace internal

CPPR_INTERNAL_CONSTEXPR bool IsPrimeCompact(std::uint64_t n) noexcept {
    if (n < (1ull << 10)) {
        return internal::IsPrime10(n);
    } else if (n <= 0xffffffff) {
        if (internal::TrialDivision32(static_cast<std::uint32_t>(n))) return false;
        if (n < 39601) return internal::GCDFilter(static_cast<std::uint32_t>(n));
        return internal::IsPrime32(static_cast<std::uint32_t>(n));
    } else {
        if (internal::TrialDivision64(n)) return false;
        if (n < (std::uint64_t(1) << 62)) {
            return internal::IsPrime64Compact<false>(n);
        } else {
            return internal::IsPrime64Compact<true>(n);
        }
    }
}

}  // namespace cppr

#endif
