//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
    namespace ImagePixel
    {
        namespace detail
        {
            [[nodiscard]]
            constexpr uint8 Div255Round8(uint32 x) noexcept
            {
                // Precondition: 0 <= x <= 255 * 255.
                return static_cast<uint8>(((x + 128u) * 257u) >> 16);
            }

            [[nodiscard]]
            constexpr uint8 RoundDivToUint8(uint32 numerator, uint32 denominator) noexcept
            {
                // Preconditions:
                // - denominator != 0
                // - numerator <= 255 * denominator
                return static_cast<uint8>(
                    (static_cast<uint64>(numerator) + (denominator / 2u)) / denominator
                    );
            }

            [[nodiscard]]
            constexpr Color ScaleAlphaByCoverage(Color src, uint8 coverage) noexcept
            {
                src.a = Div255Round8(static_cast<uint32>(src.a) * coverage);
                return src;
            }

            struct SourceOverPrecomputed
            {
                uint32 srcA;
                uint32 invSrcA;
                uint32 srcA255;

                uint32 srcR;
                uint32 srcG;
                uint32 srcB;

                uint32 srcR255;
                uint32 srcG255;
                uint32 srcB255;
            };

            [[nodiscard]]
            constexpr SourceOverPrecomputed MakeSourceOverPrecomputed(Color src) noexcept
            {
                const uint32 srcA = src.a;

                return SourceOverPrecomputed{
                    srcA,
                    255u - srcA,
                    srcA * 255u,

                    static_cast<uint32>(src.r) * srcA,
                    static_cast<uint32>(src.g) * srcA,
                    static_cast<uint32>(src.b) * srcA,

                    static_cast<uint32>(src.r) * srcA * 255u,
                    static_cast<uint32>(src.g) * srcA * 255u,
                    static_cast<uint32>(src.b) * srcA * 255u,
                };
            }

            [[nodiscard]]
            constexpr uint8 LerpChannel(uint8 src, uint8 dst, uint8 t) noexcept
            {
                const uint32 a = t;
                const uint32 ia = 255u - a;

                return Div255Round8(
                    static_cast<uint32>(dst) * ia
                    + static_cast<uint32>(src) * a
                );
            }

            [[nodiscard]]
            constexpr Color SourceOverAssumeOpaqueDstFromPrecomputed(const SourceOverPrecomputed& p, Color dst) noexcept
            {
                return Color{
                    Div255Round8(p.srcR + static_cast<uint32>(dst.r) * p.invSrcA),
                    Div255Round8(p.srcG + static_cast<uint32>(dst.g) * p.invSrcA),
                    Div255Round8(p.srcB + static_cast<uint32>(dst.b) * p.invSrcA),
                    255
                };
            }

            [[nodiscard]]
            constexpr Color SourceOverGeneralFromPrecomputed(const SourceOverPrecomputed& p, Color dst) noexcept
            {
                const uint32 dstA = dst.a;
                const uint32 denominator = p.srcA255 + (dstA * p.invSrcA);

                return Color{
                    RoundDivToUint8(
                        p.srcR255 + static_cast<uint32>(dst.r) * dstA * p.invSrcA,
                        denominator),
                    RoundDivToUint8(
                        p.srcG255 + static_cast<uint32>(dst.g) * dstA * p.invSrcA,
                        denominator),
                    RoundDivToUint8(
                        p.srcB255 + static_cast<uint32>(dst.b) * dstA * p.invSrcA,
                        denominator),
                    Div255Round8(denominator)
                };
            }

            [[nodiscard]]
            constexpr Color SourceOverFromPrecomputed(Color src, Color dst, const SourceOverPrecomputed& p) noexcept
            {
                if (src.a == 0)
                {
                    return dst;
                }

                if (src.a == 255)
                {
                    return src;
                }

                if (dst.a == 0)
                {
                    return src;
                }

                if (dst.a == 255)
                {
                    return SourceOverAssumeOpaqueDstFromPrecomputed(p, dst);
                }

                return SourceOverGeneralFromPrecomputed(p, dst);
            }
        }

        [[nodiscard]]
        constexpr Color Overwrite(Color src, Color) noexcept
        {
            return src;
        }

        [[nodiscard]]
        constexpr Color SourceOver(Color src, Color dst) noexcept
        {
            return detail::SourceOverFromPrecomputed(
                src,
                dst,
                detail::MakeSourceOverPrecomputed(src)
            );
        }

        [[nodiscard]]
        constexpr Color SourceOverAssumeOpaqueDst(Color src, Color dst) noexcept
        {
            if (src.a == 0)
            {
                return dst;
            }

            if (src.a == 255)
            {
                return Color{ src.r, src.g, src.b, 255 };
            }

            return detail::SourceOverAssumeOpaqueDstFromPrecomputed(
                detail::MakeSourceOverPrecomputed(src),
                dst
            );
        }

        [[nodiscard]]
        constexpr Color OverwriteWithCoverage(Color src, Color dst, uint8 coverage) noexcept
        {
            if (coverage == 0)
            {
                return dst;
            }

            if (coverage == 255)
            {
                return src;
            }

            return Color{
                detail::LerpChannel(src.r, dst.r, coverage),
                detail::LerpChannel(src.g, dst.g, coverage),
                detail::LerpChannel(src.b, dst.b, coverage),
                detail::LerpChannel(src.a, dst.a, coverage)
            };
        }

        [[nodiscard]]
        constexpr Color SourceOverWithCoverage(Color src, Color dst, uint8 coverage) noexcept
        {
            if (coverage == 0)
            {
                return dst;
            }

            if (coverage != 255)
            {
                src = detail::ScaleAlphaByCoverage(src, coverage);
            }

            return SourceOver(src, dst);
        }

        [[nodiscard]]
        constexpr Color SourceOverAssumeOpaqueDstWithCoverage(Color src, Color dst, uint8 coverage) noexcept
        {
            if (coverage == 0)
            {
                return dst;
            }

            if (coverage != 255)
            {
                src = detail::ScaleAlphaByCoverage(src, coverage);
            }

            return SourceOverAssumeOpaqueDst(src, dst);
        }

        [[nodiscard]]
        constexpr Color Apply(BlendMode blendMode, Color src, Color dst) noexcept
        {
            switch (blendMode)
            {
            case BlendMode::Overwrite:
                return Overwrite(src, dst);

            case BlendMode::SourceOver:
                return SourceOver(src, dst);
            }

            assert(!"Invalid ImagePixel::BlendMode");
            return SourceOver(src, dst);
        }

        [[nodiscard]]
        constexpr Color ApplyWithCoverage(BlendMode blendMode, Color src, Color dst, uint8 coverage) noexcept
        {
            switch (blendMode)
            {
            case BlendMode::Overwrite:
                return OverwriteWithCoverage(src, dst, coverage);

            case BlendMode::SourceOver:
                return SourceOverWithCoverage(src, dst, coverage);
            }

            assert(!"Invalid ImagePixel::BlendMode");
            return SourceOverWithCoverage(src, dst, coverage);
        }
    }
}
