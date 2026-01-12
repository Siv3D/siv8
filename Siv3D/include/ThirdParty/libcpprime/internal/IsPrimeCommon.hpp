/**
 *
 * libcpprime https://github.com/Rac75116/libcpprime
 *
 * Copyright (c) 2025 Rac75116
 * SPDX-License-Identifier: MIT
 *
 **/
/**
 *
 * The algorithm in this library is based on Bradley Berg's method.
 * See this page for more information:
 *https://www.techneon.com/download/is.prime.32.base.data
 *
 * Copyright 2018 Bradley Berg   < (My last name) @ t e c h n e o n . c o m >
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This algorithm is deliberately unpatented. The license above also
 * lets you even freely use it in commercial code.
 *
 * Primality testing using a hash table of bases originated with Steven Worley.
 *
 **/

#ifndef CPPR_INTERNAL_INCLUDED_IS_PRIME_COMMON
#define CPPR_INTERNAL_INCLUDED_IS_PRIME_COMMON

#include <cstdint>

#include "Utils.hpp"

namespace cppr {

namespace internal {

template <bool Strict = false>
class MontgomeryModint64Impl {
    std::uint64_t mod_ = 0, rs = 0, nr = 0, np = 0;

    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t reduce(const std::uint64_t n) const noexcept {
        // Montgomery reduction of a 128-bit value with implicit low half `n`.
        std::uint64_t q = n * nr;
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            std::uint64_t m = Mulu128High(q, mod_);
            return m == 0 ? 0 : mod_ - m;
        } else {
            std::uint64_t m = Mulu128High(q, mod_);
            return mod_ - m;
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t reduce(const std::uint64_t a, const std::uint64_t b) const noexcept {
        // Montgomery reduction of the product a*b.
        auto tmp = Mulu128(a, b);
        std::uint64_t d = tmp.high;
        std::uint64_t c = tmp.low;
        std::uint64_t q = c * nr;
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            std::uint64_t m = Mulu128High(q, mod_);
            std::uint64_t t = d - m;
            return d < m ? t + mod_ : t;
        } else {
            std::uint64_t m = Mulu128High(q, mod_);
            return d + mod_ - m;
        }
    }

   public:
    CPPR_INTERNAL_CONSTEXPR MontgomeryModint64Impl(std::uint64_t n) noexcept {
        // Precondition: n is an odd modulus > 2.
        // Internals:
        // - rs: R^2 mod n (with R = 2^64) for Montgomery domain conversion
        // - nr: -n^{-1} mod R (Newton iteration)
        // - np: representation of 1 in Montgomery domain
        Assume(n > 2 && n % 2 != 0);
        mod_ = n;
        rs = Modu128(0xffffffffffffffff % n, 0 - n, n);
        nr = n;
        for (std::uint32_t i = 0; i != 5; ++i) nr *= 2 - n * nr;
        np = reduce(rs);
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t build(std::uint32_t x) const noexcept { return reduce(x % mod_, rs); }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t build(std::uint64_t x) const noexcept { return reduce(x % mod_, rs); }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t raw(std::uint64_t x) const noexcept {
        Assume(x < mod_);
        return reduce(x, rs);
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t val(std::uint64_t x) const noexcept {
        // Converts from Montgomery domain back to the standard residue.
        // Non-strict mode permits values in [0, 2*mod) for faster operations.
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_);
            return reduce(x);
        } else {
            Assume(x < 2 * mod_);
            std::uint64_t tmp = reduce(x);
            return tmp - mod_ * (tmp >= mod_);
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t one() const noexcept {
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(np < mod_);
            return np;
        } else {
            Assume(np < 2 * mod_);
            return np;
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t neg(std::uint64_t x) const noexcept {
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_);
            return (mod_ - x) * (x != 0);
        } else {
            Assume(x < 2 * mod_);
            return (2 * mod_ - x) * (x != 0);
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t mul(std::uint64_t x, std::uint64_t y) const noexcept {
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_ && y < mod_);
            return reduce(x, y);
        } else {
            Assume(x < 2 * mod_ && y < 2 * mod_);
            return reduce(x, y);
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE bool same(std::uint64_t x, std::uint64_t y) const noexcept {
        // Equality check that tolerates the relaxed range in non-strict mode.
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_ && y < mod_);
            return x == y;
        } else {
            Assume(x < 2 * mod_ && y < 2 * mod_);
            std::uint64_t tmp = x - y;
            return (tmp == 0) || (tmp == mod_) || (tmp == 0 - mod_);
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE bool is_zero(std::uint64_t x) const noexcept {
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_);
            return x == 0;
        } else {
            Assume(x < 2 * mod_);
            return x == 0 || x == mod_;
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t add(std::uint64_t x, std::uint64_t y) const noexcept {
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_ && y < mod_);
            return x + y - (x >= mod_ - y) * mod_;
        } else {
            Assume(x < 2 * mod_ && y < 2 * mod_);
            return x + y - (x >= 2 * mod_ - y) * (2 * mod_);
        }
    }
    CPPR_INTERNAL_CONSTEXPR_INLINE std::uint64_t sub(std::uint64_t x, std::uint64_t y) const noexcept {
        if CPPR_INTERNAL_IF_CONSTEXPR (Strict) {
            Assume(x < mod_ && y < mod_);
            return x - y + (x < y) * mod_;
        } else {
            Assume(x < 2 * mod_ && y < 2 * mod_);
            return x - y + (x < y) * (2 * mod_);
        }
    }
};

CPPR_INTERNAL_CONSTEXPR_INLINE bool TrialDivision32(const std::uint32_t n) noexcept {
    // Branchless screening against a fixed set of small primes.
    return (n & 1) == 0 || 1431655766u > (0u - 1431655765u) * n || 858993460u > (0u - 858993459u) * n || 613566757u > (0u - 1227133513u) * n || 390451573u > (0u - 1171354717u) * n ||
           330382100u > (0u - 991146299u) * n || 252645136u > (0u - 252645135u) * n || 226050911u > 678152731u * n || 186737709u > (0u - 373475417u) * n;
}

constexpr std::uint16_t Bases32[256] = {
#include "IsPrimeBases32.txt"
};
CPPR_INTERNAL_CONSTEXPR bool IsPrime32(const std::uint32_t x) noexcept {
    const std::uint32_t h = x * 0xad625b89;
    std::uint32_t d = x - 1;
    std::uint32_t pw = static_cast<std::uint32_t>(Bases32[h >> 24]);
    std::int32_t s = CountrZero(d);
    d >>= s;
    if (x < (1u << 21)) {
        std::uint64_t m = 0xffffffffffffffff / x + 1;
        auto mul = [m, x](std::uint32_t a, std::uint32_t b) -> std::uint32_t { return static_cast<std::uint32_t>(Mulu128High(static_cast<std::uint64_t>(a) * b * m, x)); };
        std::uint32_t cur = pw;
        if (d != 1) {
            pw = mul(pw, pw);
            d >>= 1;
            while (d != 1) {
                std::uint32_t tmp = mul(pw, pw);
                if (d & 1) cur = mul(cur, pw);
                pw = tmp;
                d >>= 1;
            }
            cur = mul(cur, pw);
        }
        bool flag = cur == 1 || cur == x - 1;
        if (x % 4 == 3) return flag;
        if (flag) return true;
        while (--s) {
            cur = mul(cur, cur);
            if (cur == x - 1) return true;
        }
        return false;
    } else {
        std::uint32_t cur = pw;
        if (d != 1) {
            pw = static_cast<std::uint32_t>(std::uint64_t(pw) * pw % x);
            d >>= 1;
            while (d != 1) {
                std::uint32_t tmp = static_cast<std::uint32_t>(std::uint64_t(pw) * pw % x);
                if (d & 1) cur = static_cast<std::uint32_t>(std::uint64_t(cur) * pw % x);
                pw = tmp;
                d >>= 1;
            }
            cur = static_cast<std::uint32_t>(std::uint64_t(cur) * pw % x);
        }
        bool flag = cur == 1 || cur == x - 1;
        if (x % 4 == 3) return flag;
        if (flag) return true;
        while (--s) {
            cur = static_cast<std::uint32_t>(std::uint64_t(cur) * cur % x);
            if (cur == x - 1) return true;
        }
        return false;
    }
}

CPPR_INTERNAL_CONSTEXPR_INLINE bool TrialDivision64(const std::uint64_t n) noexcept {
    // Branchless screening against a fixed set of small primes.
    return (n & 1) == 0 || 6148914691236517205u >= 12297829382473034411u * n || 3689348814741910323u >= 14757395258967641293u * n || 2635249153387078802u >= 7905747460161236407u * n ||
           1676976733973595601u >= 3353953467947191203u * n || 1418980313362273201u >= 5675921253449092805u * n || 1085102592571150095u >= 17361641481138401521u * n;
}

}  // namespace internal

}  // namespace cppr

#endif
