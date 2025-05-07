/**
 * libcpprime IsPrimeNoTable.hpp https://github.com/Rac75116/libcpprime
 * 
 * MIT License
 *
 * Copyright (c) 2025 Rac75116
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#include "./internal/IsPrimeCommon.hpp"

#ifndef LIBCPPRIME_INCLUDED_IS_PRIME_NO_TABLE
#define LIBCPPRIME_INCLUDED_IS_PRIME_NO_TABLE

namespace cppr {

namespace internal {

    constexpr std::uint32_t FlagTable10[32] = { 0xa08a28acu, 0x28208a20u, 0x2088288u, 0x800228a2u, 0x20a00a08u, 0x80282088u, 0x800800a2u, 0x8028228u, 0xa20a082u, 0x22880020u, 0x28020800u, 0x88208082u, 0x2022020u, 0x8828028u, 0x8008a202u, 0x20880880u, 0x20000a00u, 0xa082008u, 0x82820802u, 0x800a20u, 0x28208au, 0x20080822u, 0x20808020u, 0x2208088u, 0x20080022u, 0x28a00a00u, 0x8a200080u, 0x8a2000u, 0x808800u, 0x2082202u, 0x80820880u, 0x28220020u };
    LIBCPPRIME_CONSTEXPR bool IsPrime10(const std::uint64_t n) noexcept {
        return (FlagTable10[n / 32] >> (n % 32)) & 1;
    }

    LIBCPPRIME_CONSTEXPR std::uint64_t GetLucasBase(const std::uint64_t x) noexcept {
        std::uint32_t tmp = x % 5;
        if (tmp == 2 || tmp == 3) return 5;
        tmp = x % 13;
        if (tmp == 2 || tmp == 5 || tmp == 6 || tmp == 7 || tmp == 8 || tmp == 11) return 13;
        tmp = x % 17;
        if (tmp == 3 || tmp == 5 || tmp == 6 || tmp == 7 || tmp == 10 || tmp == 11 || tmp == 12 || tmp == 14) return 17;
        tmp = x % 21;
        if (tmp == 2 || tmp == 8 || tmp == 10 || tmp == 11 || tmp == 13 || tmp == 19) return 21;
        tmp = x % 29;
        if (tmp == 0) return 0;
        if (tmp == 2 || tmp == 3 || tmp == 8 || tmp == 10 || tmp == 11 || tmp == 12 || tmp == 14 || tmp == 15 || tmp == 17 || tmp == 18 || tmp == 19 || tmp == 21 || tmp == 26 || tmp == 27) return 29;
        if (0x02030213u >> (x & 31) & 1) {
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
                res ^= ((s & 1) & ((n & 0b111) == 3 || (n & 0b111) == 5));
                res ^= ((a & 0b11) == 3 && (n & 0b11) == 3);
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

    LIBCPPRIME_CONSTEXPR bool IsPrime64MillerRabin(const std::uint64_t x) noexcept {
        const MontgomeryModint64Impl<false> mint(x);
        const std::int32_t S = CountrZero(x - 1);
        const std::uint64_t D = (x - 1) >> S;
        const auto one = mint.one(), mone = mint.neg(one);
        auto test2 = [=](std::uint64_t base1, std::uint64_t base2) -> bool {
            auto a = one, b = one;
            auto c = mint.build(base1), d = mint.build(base2);
            std::uint64_t ex = D;
            while (ex != 1) {
                auto e = mint.mul(c, c), f = mint.mul(d, d);
                if (ex & 1) a = mint.mul(a, c), b = mint.mul(b, d);
                c = e, d = f;
                ex >>= 1;
            }
            a = mint.mul(a, c), b = mint.mul(b, d);
            bool res1 = mint.same(a, one) || mint.same(a, mone);
            bool res2 = mint.same(b, one) || mint.same(b, mone);
            if (!(res1 && res2)) {
                for (std::int32_t i = 0; i != S - 1; ++i) {
                    a = mint.mul(a, a), b = mint.mul(b, b);
                    res1 |= mint.same(a, mone), res2 |= mint.same(b, mone);
                }
                if (!res1 || !res2) return false;
            }
            return true;
        };
        auto test3 = [=](std::uint64_t base1, std::uint64_t base2, std::uint64_t base3) -> bool {
            auto a = one, b = one, c = one;
            auto d = mint.build(base1), e = mint.build(base2), f = mint.build(base3);
            std::uint64_t ex = D;
            while (ex != 1) {
                const auto g = mint.mul(d, d), h = mint.mul(e, e), i = mint.mul(f, f);
                if (ex & 1) a = mint.mul(a, d), b = mint.mul(b, e), c = mint.mul(c, f);
                d = g, e = h, f = i;
                ex >>= 1;
            }
            a = mint.mul(a, d), b = mint.mul(b, e), c = mint.mul(c, f);
            bool res1 = mint.same(a, one) || mint.same(a, mone);
            bool res2 = mint.same(b, one) || mint.same(b, mone);
            bool res3 = mint.same(c, one) || mint.same(c, mone);
            if (!(res1 && res2 && res3)) {
                for (std::int32_t i = 0; i != S - 1; ++i) {
                    a = mint.mul(a, a), b = mint.mul(b, b), c = mint.mul(c, c);
                    res1 |= mint.same(a, mone), res2 |= mint.same(b, mone), res3 |= mint.same(c, mone);
                }
                if (!res1 || !res2 || !res3) return false;
            }
            return true;
        };
        auto test4 = [=](std::uint64_t base1, std::uint64_t base2, std::uint64_t base3, std::uint64_t base4) -> bool {
            auto a = one, b = one, c = one, d = one;
            auto e = mint.build(base1), f = mint.build(base2), g = mint.build(base3), h = mint.build(base4);
            std::uint64_t ex = D;
            while (ex != 1) {
                auto i = mint.mul(e, e), j = mint.mul(f, f), k = mint.mul(g, g), l = mint.mul(h, h);
                if (ex & 1) a = mint.mul(a, e), b = mint.mul(b, f), c = mint.mul(c, g), d = mint.mul(d, h);
                e = i, f = j, g = k, h = l;
                ex >>= 1;
            }
            a = mint.mul(a, e), b = mint.mul(b, f), c = mint.mul(c, g), d = mint.mul(d, h);
            bool res1 = mint.same(a, one) || mint.same(a, mone);
            bool res2 = mint.same(b, one) || mint.same(b, mone);
            bool res3 = mint.same(c, one) || mint.same(c, mone);
            bool res4 = mint.same(d, one) || mint.same(d, mone);
            if (!(res1 && res2 && res3 && res4)) {
                for (std::int32_t i = 0; i != S - 1; ++i) {
                    a = mint.mul(a, a), b = mint.mul(b, b), c = mint.mul(c, c), d = mint.mul(d, d);
                    res1 |= mint.same(a, mone), res2 |= mint.same(b, mone), res3 |= mint.same(c, mone), res4 |= mint.same(d, mone);
                }
                if (!res1 || !res2 || !res3 || !res4) return false;
            }
            return true;
        };
        // These bases were discovered by Steve Worley and Jim Sinclair.
        if (x < 585226005592931977ull) {
            if (x < 7999252175582851ull) {
                if (x < 350269456337ull) return test3(4230279247111683200ull, 14694767155120705706ull, 16641139526367750375ull);
                else if (x < 55245642489451ull) return test2(2ull, 141889084524735ull) && test2(1199124725622454117ull, 11096072698276303650ull);
                else return test2(2ull, 4130806001517ull) && test3(149795463772692060ull, 186635894390467037ull, 3967304179347715805ull);
            } else return test3(2ull, 123635709730000ull, 9233062284813009ull) && test3(43835965440333360ull, 761179012939631437ull, 1263739024124850375ull);
        } else return test3(2ull, 325ull, 9375ull) && test4(28178ull, 450775ull, 9780504ull, 1795265022ull);
    }

    LIBCPPRIME_CONSTEXPR bool IsPrime64BailliePSW(const std::uint64_t x) noexcept {
        const MontgomeryModint64Impl<true> mint(x);
        const auto one = mint.one(), mone = mint.neg(one);
        auto miller_rabin_test = [&]() -> bool {
            const std::int32_t S = CountrZero(x - 1);
            const std::uint64_t D = (x - 1) >> S;
            auto a = one, b = mint.raw(2);
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
        const std::uint64_t Q = mint.raw(x - (D - 1) / 4);
        std::uint64_t u = one, v = one, Qn = Q;
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
        std::uint64_t f = (x + 1) & ~x;
        for (f >>= 1; f; f >>= 1) {
            u = mint.mul(u, v);
            v = mint.sub(mint.mul(v, v), mint.add(Qn, Qn));
            if (mint.is_zero(v)) return true;
            Qn = mint.mul(Qn, Qn);
        }
        return false;
    }

}  // namespace internal

LIBCPPRIME_CONSTEXPR bool IsPrimeNoTable(std::uint64_t n) noexcept {
    if (n < 1024) return internal::IsPrime10(n);
    else {
        if ((n & 1) == 0 || 6148914691236517205u >= 12297829382473034411u * n || 3689348814741910323u >= 14757395258967641293u * n || 2635249153387078802u >= 7905747460161236407u * n || 1676976733973595601u >= 3353953467947191203u * n || 1418980313362273201u >= 5675921253449092805u * n || 1085102592571150095u >= 17361641481138401521u * n) return false;
        if (n <= 0xffffffff) return internal::IsPrime32(static_cast<std::uint32_t>(n));
        else if (n < (std::uint64_t(1) << 62)) return internal::IsPrime64MillerRabin(n);
        else return internal::IsPrime64BailliePSW(n);
    }
}

}  // namespace cppr

#endif
