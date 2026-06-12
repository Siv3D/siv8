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

#pragma once
#include <Siv3D/ImagePixel.hpp>

namespace s3d
{
    namespace ImagePixel
    {
        namespace detail
        {
            enum class DstAlphaMode : uint8
            {
                Preserve,
                AssumeOpaque,
            };

            void SourceOverLineUnchecked(
                Color src,
                Color* dst,
                size_t count,
                const SourceOverPrecomputed& precomputed) noexcept;

            void SourceOverAssumeOpaqueDstLineUnchecked(
                const SourceOverPrecomputed& precomputed,
                Color* dst,
                size_t count) noexcept;

            void OverwriteWithCoverageLineUnchecked(
                Color src,
                Color* dst,
                const uint8* coverage,
                size_t count) noexcept;

            void SourceOverWithCoverageLineUnchecked(
                Color src,
                Color* dst,
                const uint8* coverage,
                size_t count,
                const SourceOverPrecomputed& full) noexcept;

            void SourceOverAssumeOpaqueDstWithCoverageLineUnchecked(
                Color src,
                Color* dst,
                const uint8* coverage,
                size_t count,
                const SourceOverPrecomputed& full) noexcept;

            class SolidColorWriter
            {
            public:

                SolidColorWriter(
                    BlendMode blendMode,
                    Color src,
                    DstAlphaMode dstAlphaMode = DstAlphaMode::Preserve) noexcept;

                void write(Color* dst, size_t count) const noexcept;

                void writeWithCoverage(Color* dst, const uint8* coverage, size_t count) const noexcept;

                void writeRows(
                    Color* dst,
                    size_t width,
                    size_t height,
                    size_t dstStride) const noexcept;

                void writeRowsWithCoverage(
                    Color* dst,
                    const uint8* coverage,
                    size_t width,
                    size_t height,
                    size_t dstStride,
                    size_t coverageStride) const noexcept;

            private:

                void writeSourceOver(Color* dst, size_t count) const noexcept;

                void writeSourceOverWithCoverage(Color* dst, const uint8* coverage, size_t count) const noexcept;

                BlendMode m_blendMode;
                
                DstAlphaMode m_dstAlphaMode;
                
                Color m_src;
                
                SourceOverPrecomputed m_sourceOver;
            };
        }
    }
}
