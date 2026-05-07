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

#include "SolidColorWriter.hpp"

namespace s3d
{
    namespace ImagePixel
    {
        namespace detail
        {
            void SourceOverLineUnchecked(
                Color src,
                Color* dst,
                size_t count,
                const SourceOverPrecomputed& precomputed) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    const Color d = dst[i];

                    if (d.a == 0)
                    {
                        dst[i] = src;
                    }
                    else if (d.a == 255)
                    {
                        dst[i] = SourceOverAssumeOpaqueDstFromPrecomputed(precomputed, d);
                    }
                    else
                    {
                        dst[i] = SourceOverGeneralFromPrecomputed(precomputed, d);
                    }
                }
            }

            void SourceOverAssumeOpaqueDstLineUnchecked(
                const SourceOverPrecomputed& precomputed,
                Color* dst,
                size_t count) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    dst[i] = SourceOverAssumeOpaqueDstFromPrecomputed(precomputed, dst[i]);
                }
            }

            void OverwriteWithCoverageLineUnchecked(
                Color src,
                Color* dst,
                const uint8* coverage,
                size_t count) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    const uint8 c = coverage[i];

                    if (c == 0)
                    {
                        continue;
                    }

                    if (c == 255)
                    {
                        dst[i] = src;
                    }
                    else
                    {
                        dst[i] = Color{
                            LerpChannel(src.r, dst[i].r, c),
                            LerpChannel(src.g, dst[i].g, c),
                            LerpChannel(src.b, dst[i].b, c),
                            LerpChannel(src.a, dst[i].a, c)
                        };
                    }
                }
            }

            void SourceOverWithCoverageLineUnchecked(
                Color src,
                Color* dst,
                const uint8* coverage,
                size_t count,
                const SourceOverPrecomputed& full) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    const uint8 c = coverage[i];

                    if (c == 0)
                    {
                        continue;
                    }

                    if (c == 255)
                    {
                        dst[i] = SourceOverFromPrecomputed(src, dst[i], full);
                    }
                    else
                    {
                        const Color covered = ScaleAlphaByCoverage(src, c);

                        if (covered.a != 0)
                        {
                            dst[i] = SourceOverFromPrecomputed(
                                covered,
                                dst[i],
                                MakeSourceOverPrecomputed(covered)
                            );
                        }
                    }
                }
            }

            void SourceOverAssumeOpaqueDstWithCoverageLineUnchecked(
                Color src,
                Color* dst,
                const uint8* coverage,
                size_t count,
                const SourceOverPrecomputed& full) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    const uint8 c = coverage[i];

                    if (c == 0)
                    {
                        continue;
                    }

                    if (c == 255)
                    {
                        dst[i] = SourceOverAssumeOpaqueDstFromPrecomputed(full, dst[i]);
                    }
                    else
                    {
                        const Color covered = ScaleAlphaByCoverage(src, c);

                        if (covered.a != 0)
                        {
                            dst[i] = SourceOverAssumeOpaqueDstFromPrecomputed(
                                MakeSourceOverPrecomputed(covered),
                                dst[i]
                            );
                        }
                    }
                }
            }

            SolidColorWriter::SolidColorWriter(
                BlendMode blendMode,
                Color src,
                DstAlphaMode dstAlphaMode) noexcept
                : m_blendMode{ blendMode }
                , m_dstAlphaMode{ dstAlphaMode }
                , m_src{ src }
                , m_sourceOver{ MakeSourceOverPrecomputed(src) } {
            }

            void SolidColorWriter::write(Color* dst, size_t count) const noexcept
            {
                if ((dst == nullptr) || (count == 0))
                {
                    return;
                }

                switch (m_blendMode)
                {
                case BlendMode::Overwrite:
                    std::fill_n(dst, count, m_src);
                    return;

                case BlendMode::SourceOver:
                    writeSourceOver(dst, count);
                    return;
                }

                assert(false && "Invalid ImagePixel::BlendMode");
                writeSourceOver(dst, count);
            }

            void SolidColorWriter::writeWithCoverage(Color* dst, const uint8* coverage, size_t count) const noexcept
            {
                if ((dst == nullptr) || (coverage == nullptr) || (count == 0))
                {
                    return;
                }

                switch (m_blendMode)
                {
                case BlendMode::Overwrite:
                    OverwriteWithCoverageLineUnchecked(m_src, dst, coverage, count);
                    return;

                case BlendMode::SourceOver:
                    writeSourceOverWithCoverage(dst, coverage, count);
                    return;
                }

                assert(false && "Invalid ImagePixel::BlendMode");
                writeSourceOverWithCoverage(dst, coverage, count);
            }

            void SolidColorWriter::writeRows(
                Color* dst,
                size_t width,
                size_t height,
                size_t dstStride) const noexcept
            {
                if ((dst == nullptr) || (width == 0) || (height == 0))
                {
                    return;
                }

                for (size_t y = 0; y < height; ++y)
                {
                    write(dst, width);
                    dst += dstStride;
                }
            }

            void SolidColorWriter::writeRowsWithCoverage(
                Color* dst,
                const uint8* coverage,
                size_t width,
                size_t height,
                size_t dstStride,
                size_t coverageStride) const noexcept
            {
                if ((dst == nullptr) || (coverage == nullptr) || (width == 0) || (height == 0))
                {
                    return;
                }

                for (size_t y = 0; y < height; ++y)
                {
                    writeWithCoverage(dst, coverage, width);

                    dst += dstStride;
                    coverage += coverageStride;
                }
            }

            void SolidColorWriter::writeSourceOver(Color* dst, size_t count) const noexcept
            {
                if (m_src.a == 0)
                {
                    return;
                }

                if (m_src.a == 255)
                {
                    const Color fillColor = (m_dstAlphaMode == DstAlphaMode::AssumeOpaque)
                        ? Color{ m_src.r, m_src.g, m_src.b, 255 }
                    : m_src;

                    std::fill_n(dst, count, fillColor);
                    return;
                }

                if (m_dstAlphaMode == DstAlphaMode::AssumeOpaque)
                {
                    SourceOverAssumeOpaqueDstLineUnchecked(m_sourceOver, dst, count);
                }
                else
                {
                    SourceOverLineUnchecked(m_src, dst, count, m_sourceOver);
                }
            }

            void SolidColorWriter::writeSourceOverWithCoverage(Color* dst, const uint8* coverage, size_t count) const noexcept
            {
                if (m_src.a == 0)
                {
                    return;
                }

                if (m_dstAlphaMode == DstAlphaMode::AssumeOpaque)
                {
                    SourceOverAssumeOpaqueDstWithCoverageLineUnchecked(m_src, dst, coverage, count, m_sourceOver);
                }
                else
                {
                    SourceOverWithCoverageLineUnchecked(m_src, dst, coverage, count, m_sourceOver);
                }
            }
        }

        //-------------------------------------------------------------------------
        //
        //  Contiguous pixel operations
        //
        //-------------------------------------------------------------------------

        void Overwrite(Color src, Color* dst, size_t count) noexcept
        {
            if ((dst == nullptr) || (count == 0))
            {
                return;
            }

            std::fill_n(dst, count, src);
        }

        void SourceOver(Color src, Color* dst, size_t count) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src }.write(dst, count);
        }

        void SourceOverAssumeOpaqueDst(Color src, Color* dst, size_t count) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src, detail::DstAlphaMode::AssumeOpaque }.write(dst, count);
        }

        void OverwriteWithCoverage(Color src, Color* dst, const uint8* coverage, size_t count) noexcept
        {
            detail::SolidColorWriter{ BlendMode::Overwrite, src }.writeWithCoverage(dst, coverage, count);
        }

        void SourceOverWithCoverage(Color src, Color* dst, const uint8* coverage, size_t count) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src }.writeWithCoverage(dst, coverage, count);
        }

        void SourceOverAssumeOpaqueDstWithCoverage(Color src, Color* dst, const uint8* coverage, size_t count) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src, detail::DstAlphaMode::AssumeOpaque }.writeWithCoverage(dst, coverage, count);
        }

        void Apply(BlendMode blendMode, Color src, Color* dst, size_t count) noexcept
        {
            detail::SolidColorWriter{ blendMode, src }.write(dst, count);
        }

        void ApplyWithCoverage(BlendMode blendMode, Color src, Color* dst, const uint8* coverage, size_t count) noexcept
        {
            detail::SolidColorWriter{ blendMode, src }.writeWithCoverage(dst, coverage, count);
        }

        //-------------------------------------------------------------------------
        //
        //  Row / stride pixel operations
        //
        //-------------------------------------------------------------------------

        void OverwriteRows(Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept
        {
            detail::SolidColorWriter{ BlendMode::Overwrite, src }.writeRows(dst, width, height, dstStride);
        }

        void SourceOverRows(Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src }.writeRows(dst, width, height, dstStride);
        }

        void SourceOverAssumeOpaqueDstRows(Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src, detail::DstAlphaMode::AssumeOpaque }.writeRows(dst, width, height, dstStride);
        }

        void OverwriteWithCoverageRows(
            Color src,
            Color* dst,
            const uint8* coverage,
            size_t width,
            size_t height,
            size_t dstStride,
            size_t coverageStride) noexcept
        {
            detail::SolidColorWriter{ BlendMode::Overwrite, src }.writeRowsWithCoverage(
                dst,
                coverage,
                width,
                height,
                dstStride,
                coverageStride
            );
        }

        void SourceOverWithCoverageRows(
            Color src,
            Color* dst,
            const uint8* coverage,
            size_t width,
            size_t height,
            size_t dstStride,
            size_t coverageStride) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src }.writeRowsWithCoverage(
                dst,
                coverage,
                width,
                height,
                dstStride,
                coverageStride
            );
        }

        void SourceOverAssumeOpaqueDstWithCoverageRows(
            Color src,
            Color* dst,
            const uint8* coverage,
            size_t width,
            size_t height,
            size_t dstStride,
            size_t coverageStride) noexcept
        {
            detail::SolidColorWriter{ BlendMode::SourceOver, src, detail::DstAlphaMode::AssumeOpaque }.writeRowsWithCoverage(
                dst,
                coverage,
                width,
                height,
                dstStride,
                coverageStride
            );
        }

        void ApplyRows(BlendMode blendMode, Color src, Color* dst, size_t width, size_t height, size_t dstStride) noexcept
        {
            detail::SolidColorWriter{ blendMode, src }.writeRows(dst, width, height, dstStride);
        }

        void ApplyWithCoverageRows(
            BlendMode blendMode,
            Color src,
            Color* dst,
            const uint8* coverage,
            size_t width,
            size_t height,
            size_t dstStride,
            size_t coverageStride) noexcept
        {
            detail::SolidColorWriter{ blendMode, src }.writeRowsWithCoverage(
                dst,
                coverage,
                width,
                height,
                dstStride,
                coverageStride
            );
        }
    }
}
