/**
 *
 * libcpprime https://github.com/Rac75116/libcpprime
 *
 * Copyright (c) 2025 Rac75116
 * SPDX-License-Identifier: MIT
 *
 **/

#ifndef CPPR_INTERNAL_INCLUDED_IS_PRIME_NO_TABLE
#define CPPR_INTERNAL_INCLUDED_IS_PRIME_NO_TABLE

#include <cstdint>

#include "internal/IsPrimeCommon.hpp"
#include "internal/Utils.hpp"

namespace cppr {

namespace internal {

constexpr std::uint32_t FlagTable10[32] = {
#include "internal/IsPrimeTable10.txt"
};
// Bitset for small n < 1024.
CPPR_INTERNAL_CONSTEXPR_INLINE bool IsPrime10(const std::uint64_t n) noexcept { return (FlagTable10[n / 32] >> (n % 32)) & 1; }

CPPR_INTERNAL_CONSTEXPR_INLINE bool GCDFilter(const std::uint32_t n) noexcept {
    auto GCD = [](std::uint32_t x, std::uint32_t y) -> std::uint32_t {
        // Binary GCD (Stein's algorithm). Assumes y != 0 when x != 0.
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

    // Very small range: fast GCD-based filters with precomputed constants.
    const std::uint32_t a = static_cast<std::uint32_t>(Modu128(272518712866683587u % n, 10755835586592736005u, n));
    if (n < 11881) return GCD(a, n) == 1;
    const std::uint32_t b = static_cast<std::uint32_t>(Modu128(827936745744686818u % n, 10132550402535125089u, n));
    return GCD((a * b) % n, n) == 1;
}

CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t GetLucasBase(const std::uint64_t x) noexcept {
    // Chooses a Lucas parameter D for the strong Lucas probable prime test.
    // Returns:
    // - 0: definitely composite (quick checks found a factor or perfect square)
    // - 1: no suitable D found in the search range (treated as pass by caller)
    // - otherwise: selected D
    std::uint64_t tmp = x % 5;
    if (tmp == 2 || tmp == 3) {
        return 5;
    }
    tmp = x % 13;
    if (tmp == 2 || tmp == 5 || tmp == 6 || tmp == 7 || tmp == 8 || tmp == 11) {
        return 13;
    }
    tmp = x % 17;
    if (tmp == 3 || tmp == 5 || tmp == 6 || tmp == 7 || tmp == 10 || tmp == 11 || tmp == 12 || tmp == 14) {
        return 17;
    }
    tmp = x % 21;
    if (tmp == 2 || tmp == 8 || tmp == 10 || tmp == 11 || tmp == 13 || tmp == 19) {
        return 21;
    }
    tmp = x % 29;
    if (tmp == 0) return 0;
    if (tmp == 2 || tmp == 3 || tmp == 8 || tmp == 10 || tmp == 11 || tmp == 12 || tmp == 14 || tmp == 15 || tmp == 17 || tmp == 18 || tmp == 19 || tmp == 21 || tmp == 26 || tmp == 27) {
        return 29;
    }
    if (0x02030213u >> (x & 31) & 1) {
        // Fast perfect-square check for candidates in specific residue classes.
        std::int32_t k = 32 - (CountlZero(x - 1) >> 1);
        std::uint64_t s = 1ull << k, t = (s + (x >> k)) >> 1;
        while (t < s) {
            s = t;
            t = (s + x / s) >> 1;
        }
        if (s * s == x) return 0;
    }
    std::uint64_t Z = 33;
    while (Z < x) {
        std::uint64_t a = Z, n = x;
        bool res = false;
        while (a != 0) {
            std::int32_t s = CountrZero(a);
            a >>= s;
            res ^= ((s & 1) & ((n & 7) == 3 || (n & 7) == 5));
            res ^= ((a & 3) == 3 && (n & 3) == 3);
            std::uint64_t t = n;
            n = a;
            a = t % n;
        }
        if (n == 1 && res) break;
        Z += 4;
    }
    if (Z >= x) return 1;
    return Z;
}

CPPR_INTERNAL_CONSTEXPR_INLINE bool IsPrime64MillerRabin(const std::uint64_t x) noexcept {
    const MontgomeryModint64Impl<false> mint(x);
    const std::int32_t S = CountrZero(x - 1);
    const std::uint64_t D = (x - 1) >> S;
    const auto one = mint.one();
    const auto mone = mint.neg(one);
    auto test2 = [=](std::uint64_t base1, std::uint64_t base2) -> bool {
        // Two-base Miller-Rabin using Montgomery arithmetic.
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
        if (!(res1 && res2)) {
            for (std::int32_t i = 0; i != S - 1; ++i) {
                a = mint.mul(a, a);
                b = mint.mul(b, b);
                res1 |= mint.same(a, mone);
                res2 |= mint.same(b, mone);
            }
            if (!res1 || !res2) return false;
        }
        return true;
    };
    auto test3 = [=](std::uint64_t base1, std::uint64_t base2, std::uint64_t base3) -> bool {
        // Three-base Miller-Rabin using Montgomery arithmetic.
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
        if (!(res1 && res2 && res3)) {
            for (std::int32_t i = 0; i != S - 1; ++i) {
                a = mint.mul(a, a);
                b = mint.mul(b, b);
                c = mint.mul(c, c);
                res1 |= mint.same(a, mone);
                res2 |= mint.same(b, mone);
                res3 |= mint.same(c, mone);
            }
            if (!res1 || !res2 || !res3) return false;
        }
        return true;
    };
    auto test4 = [=](std::uint64_t base1, std::uint64_t base2, std::uint64_t base3, std::uint64_t base4) -> bool {
        // Four-base Miller-Rabin using Montgomery arithmetic.
        auto a = one;
        auto b = one;
        auto c = one;
        auto d = one;
        auto e = mint.build(base1);
        auto f = mint.build(base2);
        auto g = mint.build(base3);
        auto h = mint.build(base4);
        std::uint64_t ex = D;
        while (ex != 1) {
            auto i = mint.mul(e, e);
            auto j = mint.mul(f, f);
            auto k = mint.mul(g, g);
            auto l = mint.mul(h, h);
            if (ex & 1) {
                a = mint.mul(a, e);
                b = mint.mul(b, f);
                c = mint.mul(c, g);
                d = mint.mul(d, h);
            }
            e = i;
            f = j;
            g = k;
            h = l;
            ex >>= 1;
        }
        a = mint.mul(a, e);
        b = mint.mul(b, f);
        c = mint.mul(c, g);
        d = mint.mul(d, h);
        bool res1 = mint.same(a, one) || mint.same(a, mone);
        bool res2 = mint.same(b, one) || mint.same(b, mone);
        bool res3 = mint.same(c, one) || mint.same(c, mone);
        bool res4 = mint.same(d, one) || mint.same(d, mone);
        if (!(res1 && res2 && res3 && res4)) {
            for (std::int32_t i = 0; i != S - 1; ++i) {
                a = mint.mul(a, a);
                b = mint.mul(b, b);
                c = mint.mul(c, c);
                d = mint.mul(d, d);
                res1 |= mint.same(a, mone);
                res2 |= mint.same(b, mone);
                res3 |= mint.same(c, mone);
                res4 |= mint.same(d, mone);
            }
            if (!res1 || !res2 || !res3 || !res4) return false;
        }
        return true;
    };
    // These bases were discovered by Steve Worley and Jim Sinclair.
    if (x < 585226005592931977ull) {
        if (x < 7999252175582851ull) {
            if (x < 350269456337ull) {
                return test3(4230279247111683200ull, 14694767155120705706ull, 16641139526367750375ull);
            } else if (x < 55245642489451ull) {
                return test2(2ull, 141889084524735ull) && test2(1199124725622454117ull, 11096072698276303650ull);
            } else {
                return test2(2ull, 4130806001517ull) && test3(149795463772692060ull, 186635894390467037ull, 3967304179347715805ull);
            }
        } else {
            return test3(2ull, 123635709730000ull, 9233062284813009ull) && test3(43835965440333360ull, 761179012939631437ull, 1263739024124850375ull);
        }
    } else {
        return test3(2ull, 325ull, 9375ull) && test4(28178ull, 450775ull, 9780504ull, 1795265022ull);
    }
}

CPPR_INTERNAL_CONSTEXPR_INLINE bool IsPrime64BailliePSW(const std::uint64_t x) noexcept {
    const MontgomeryModint64Impl<true> mint(x);
    const auto one = mint.one();
    const auto mone = mint.neg(one);
    auto miller_rabin_test = [&]() -> bool {
        // Baillie-PSW starts with a base-2 Miller-Rabin test.
        const std::int32_t S = CountrZero(x - 1);
        const std::uint64_t D = (x - 1) >> S;
        auto a = one;
        auto b = mint.raw(2);
        std::uint64_t ex = D;
        while (ex != 1) {
            auto c = mint.mul(b, b);
            if (ex & 1) a = mint.mul(a, b);
            b = c;
            ex >>= 1;
        }
        a = mint.mul(a, b);
        bool flag = mint.same(a, one) || mint.same(a, mone);
        if (x % 4 == 3) return flag;
        if (flag) return true;
        for (std::int32_t i = 0; i != S - 1; ++i) {
            a = mint.mul(a, a);
            if (mint.same(a, mone)) return true;
        }
        return false;
    };
    if (!miller_rabin_test()) return false;
    std::uint64_t D = GetLucasBase(x);
    if (D <= 1) return D == 1;
    // Strong Lucas probable prime test (implemented via Lucas sequences in Montgomery domain).
    const std::uint64_t Q = mint.raw(x - (D - 1) / 4);
    std::uint64_t u = one;
    std::uint64_t v = one;
    std::uint64_t Qn = Q;
    // Iterate Lucas sequences according to bits of (x + 1).
    std::uint64_t k = (x + 1) << CountlZero(x + 1);
    D = mint.raw(D);
    std::uint64_t t = (x >> 1) + 1;
    for (k <<= 1; k; k <<= 1) {
        std::uint64_t Qt = mint.add(Qn, Qn);
        Qn = mint.mul(Qn, Qn);
        u = mint.mul(u, v);
        v = mint.sub(mint.mul(v, v), Qt);
        if (k >> 63) {
            Qn = mint.mul(Qn, Q);
            std::uint64_t uu = u;
            u = mint.add(u, v);
            u = (u >> 1) + ((u & 1) ? t : 0);
            v = mint.add(mint.mul(D, uu), v);
            v = (v >> 1) + ((v & 1) ? t : 0);
        }
    }
    if (mint.is_zero(u) || mint.is_zero(v)) return true;
    // Extra check over factors of (x + 1) as required by the strong Lucas condition.
    std::uint64_t f = (x + 1) & ~x;
    for (f >>= 1; f; f >>= 1) {
        v = mint.sub(mint.mul(v, v), mint.add(Qn, Qn));
        if (mint.is_zero(v)) return true;
        Qn = mint.mul(Qn, Qn);
    }
    return false;
}

}  // namespace internal

CPPR_INTERNAL_CONSTEXPR bool IsPrimeNoTable(std::uint64_t n) noexcept {
    if (n < 1024) {
        return internal::IsPrime10(n);
    } else if (n <= 0xffffffff) {
        if (internal::TrialDivision32(static_cast<std::uint32_t>(n))) return false;
        if (n < 39601) {
            return internal::GCDFilter(static_cast<std::uint32_t>(n));
        }
        return internal::IsPrime32(static_cast<std::uint32_t>(n));
    } else {
        if (internal::TrialDivision64(n)) {
            return false;
        }
        if (n < (std::uint64_t(1) << 62)) {
            return internal::IsPrime64MillerRabin(n);
        } else {
            return internal::IsPrime64BailliePSW(n);
        }
    }
}

}  // namespace cppr

#endif
