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

# include <Siv3D/Image.hpp>
# include <Siv3D/ImageDraw.hpp>
# include <Siv3D/2DShapes.hpp>
# include "../ImagePixel/SolidColorWriter.hpp"

namespace s3d
{
    namespace
    {
        struct ImageView
        {
            Color* data = nullptr;
            int32 width = 0;
            int32 height = 0;
            size_t stride = 0;
        };

        struct SpanI
        {
            int32 x0 = 0;
            int32 x1 = 0; // exclusive
        };

        struct BoundsD
        {
            double minX = std::numeric_limits<double>::infinity();
            double minY = std::numeric_limits<double>::infinity();
            double maxX = -std::numeric_limits<double>::infinity();
            double maxY = -std::numeric_limits<double>::infinity();

            void include(const Vec2& p) noexcept
            {
                minX = Min(minX, p.x);
                minY = Min(minY, p.y);
                maxX = Max(maxX, p.x);
                maxY = Max(maxY, p.y);
            }

            void includeCircle(const Vec2& center, double radius) noexcept
            {
                minX = Min(minX, center.x - radius);
                minY = Min(minY, center.y - radius);
                maxX = Max(maxX, center.x + radius);
                maxY = Max(maxY, center.y + radius);
            }

            [[nodiscard]]
            bool valid() const noexcept
            {
                return (minX <= maxX) && (minY <= maxY);
            }
        };

        struct OrientedRectBasis
        {
            Vec2 center;
            Vec2 dir;
            Vec2 normal;
            double halfLength = 0.0;
            double halfThickness = 0.0;
        };

        [[nodiscard]]
        ImageView MakeImageView(Image& image) noexcept
        {
            return ImageView{
                image.data(),
                static_cast<int32>(image.width()),
                static_cast<int32>(image.height()),
                static_cast<size_t>(image.width())
            };
        }

        [[nodiscard]]
        ImagePixel::detail::DstAlphaMode ToDstAlphaMode(ImageDraw::DstAlpha dstAlpha) noexcept
        {
            return (dstAlpha == ImageDraw::DstAlpha::AssumeOpaque)
                ? ImagePixel::detail::DstAlphaMode::AssumeOpaque
                : ImagePixel::detail::DstAlphaMode::Preserve;
        }

        [[nodiscard]]
        int32 ClampInt(int32 value, int32 minValue, int32 maxValue) noexcept
        {
            return Max(minValue, Min(value, maxValue));
        }

        [[nodiscard]]
        int32 FloorToInt(double x) noexcept
        {
            return static_cast<int32>(std::floor(x));
        }

        [[nodiscard]]
        int32 CeilToInt(double x) noexcept
        {
            return static_cast<int32>(std::ceil(x));
        }

        [[nodiscard]]
        uint8 ToCoverage(double value) noexcept
        {
            if (value <= 0.0)
            {
                return 0;
            }

            if (1.0 <= value)
            {
                return 255;
            }

            return static_cast<uint8>(value * 255.0 + 0.5);
        }

        [[nodiscard]]
        uint8 IntervalCoverage1D(double shapeMin, double shapeMax, int32 pixelIndex) noexcept
        {
            const double pixelMin = static_cast<double>(pixelIndex);
            const double pixelMax = pixelMin + 1.0;

            const double overlap = Min(shapeMax, pixelMax) - Max(shapeMin, pixelMin);

            return ToCoverage(overlap);
        }

        [[nodiscard]]
        bool ClipRectI(
            int32& x0,
            int32& y0,
            int32& x1,
            int32& y1,
            int32 width,
            int32 height) noexcept
        {
            x0 = ClampInt(x0, 0, width);
            y0 = ClampInt(y0, 0, height);
            x1 = ClampInt(x1, 0, width);
            y1 = ClampInt(y1, 0, height);

            return ((x0 < x1) && (y0 < y1));
        }

        void AddSpanClipped(
            std::vector<SpanI>& spans,
            int32 x0,
            int32 x1,
            int32 width)
        {
            if ((x1 <= 0) || (width <= x0))
            {
                return;
            }

            x0 = ClampInt(x0, 0, width);
            x1 = ClampInt(x1, 0, width);

            if (x0 < x1)
            {
                spans.push_back(SpanI{ x0, x1 });
            }
        }

        void SortAndMergeSpans(std::vector<SpanI>& spans)
        {
            if (spans.empty())
            {
                return;
            }

            std::sort(spans.begin(), spans.end(), [](const SpanI& a, const SpanI& b)
                {
                    return (a.x0 < b.x0) || ((a.x0 == b.x0) && (a.x1 < b.x1));
                });

            size_t writeIndex = 0;

            for (size_t readIndex = 1; readIndex < spans.size(); ++readIndex)
            {
                SpanI& current = spans[writeIndex];
                const SpanI& next = spans[readIndex];

                if (next.x0 <= current.x1)
                {
                    current.x1 = Max(current.x1, next.x1);
                }
                else
                {
                    ++writeIndex;
                    spans[writeIndex] = next;
                }
            }

            spans.resize(writeIndex + 1);
        }

        void WriteMergedSpans(
            const ImagePixel::detail::SolidColorWriter& writer,
            ImageView view,
            int32 y,
            std::vector<SpanI>& spans)
        {
            if (spans.empty())
            {
                return;
            }

            SortAndMergeSpans(spans);

            Color* row = view.data + static_cast<size_t>(y) * view.stride;

            for (const SpanI& span : spans)
            {
                writer.write(
                    row + static_cast<size_t>(span.x0),
                    static_cast<size_t>(span.x1 - span.x0)
                );
            }

            spans.clear();
        }

        void WriteSpanHalfOpenClipped(
            const ImagePixel::detail::SolidColorWriter& writer,
            ImageView view,
            int32 y,
            int32 x0,
            int32 x1)
        {
            if ((y < 0) || (view.height <= y) || (x1 <= 0) || (view.width <= x0))
            {
                return;
            }

            x0 = ClampInt(x0, 0, view.width);
            x1 = ClampInt(x1, 0, view.width);

            if (x0 >= x1)
            {
                return;
            }

            Color* dst = view.data
                + static_cast<size_t>(y) * view.stride
                + static_cast<size_t>(x0);

            writer.write(dst, static_cast<size_t>(x1 - x0));
        }

        void WriteSpanInclusiveClipped(
            const ImagePixel::detail::SolidColorWriter& writer,
            ImageView view,
            int32 y,
            int32 x0,
            int32 x1)
        {
            if ((y < 0) || (view.height <= y) || (x1 < 0) || (view.width <= x0))
            {
                return;
            }

            x0 = ClampInt(x0, 0, view.width - 1);
            x1 = ClampInt(x1, 0, view.width - 1);

            if (x0 > x1)
            {
                return;
            }

            Color* dst = view.data
                + static_cast<size_t>(y) * view.stride
                + static_cast<size_t>(x0);

            writer.write(dst, static_cast<size_t>(x1 - x0 + 1));
        }

        void FillRectInteger(
            ImageView view,
            const Rect& rect,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            int32 x0 = rect.x;
            int32 y0 = rect.y;
            int32 x1 = rect.x + rect.w;
            int32 y1 = rect.y + rect.h;

            if (!ClipRectI(x0, y0, x1, y1, view.width, view.height))
            {
                return;
            }

            const size_t width = static_cast<size_t>(x1 - x0);
            const size_t height = static_cast<size_t>(y1 - y0);

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            writer.writeRows(
                view.data + (static_cast<size_t>(y0) * view.stride) + static_cast<size_t>(x0),
                width,
                height,
                view.stride
            );
        }

        void FillRectFloat(
            ImageView view,
            const RectF& rect,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            const double left = rect.x;
            const double top = rect.y;
            const double right = rect.x + rect.w;
            const double bottom = rect.y + rect.h;

            if (!(left < right) || !(top < bottom))
            {
                return;
            }

            int32 x0 = FloorToInt(left);
            int32 y0 = FloorToInt(top);
            int32 x1 = CeilToInt(right);
            int32 y1 = CeilToInt(bottom);

            if (!ClipRectI(x0, y0, x1, y1, view.width, view.height))
            {
                return;
            }

            const size_t rowLength = static_cast<size_t>(x1 - x0);

            std::vector<uint8> xCoverage(rowLength);
            std::vector<uint8> rowCoverage(rowLength);

            bool xAllFull = true;

            for (size_t i = 0; i < rowLength; ++i)
            {
                const int32 x = x0 + static_cast<int32>(i);
                xCoverage[i] = IntervalCoverage1D(left, right, x);
                xAllFull &= (xCoverage[i] == 255);
            }

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            for (int32 y = y0; y < y1; ++y)
            {
                const uint8 yCoverage = IntervalCoverage1D(top, bottom, y);

                if (yCoverage == 0)
                {
                    continue;
                }

                Color* dstRow = view.data
                    + (static_cast<size_t>(y) * view.stride)
                    + static_cast<size_t>(x0);

                if ((yCoverage == 255) && xAllFull)
                {
                    writer.write(dstRow, rowLength);
                    continue;
                }

                bool anyCoverage = false;

                if (yCoverage == 255)
                {
                    for (size_t i = 0; i < rowLength; ++i)
                    {
                        rowCoverage[i] = xCoverage[i];
                        anyCoverage |= (rowCoverage[i] != 0);
                    }
                }
                else
                {
                    for (size_t i = 0; i < rowLength; ++i)
                    {
                        rowCoverage[i] = ImagePixel::detail::Div255Round8(
                            static_cast<uint32>(xCoverage[i]) * static_cast<uint32>(yCoverage)
                        );

                        anyCoverage |= (rowCoverage[i] != 0);
                    }
                }

                if (anyCoverage)
                {
                    writer.writeWithCoverage(dstRow, rowCoverage.data(), rowLength);
                }
            }
        }

        void FillRectClipped(
            const ImageView view,
            int32 x0,
            int32 y0,
            int32 x1,
            int32 y1,
            const ImagePixel::detail::SolidColorWriter& writer)
        {
            if (!ClipRectI(x0, y0, x1, y1, view.width, view.height))
            {
                return;
            }

            writer.writeRows(
                view.data
                + static_cast<size_t>(y0) * view.stride
                + static_cast<size_t>(x0),
                static_cast<size_t>(x1 - x0),
                static_cast<size_t>(y1 - y0),
                view.stride
            );
        }

        void FillRectFrameInteger(
            const ImageView view,
            const Rect& rect,
            const int32 innerThickness,
            const int32 outerThickness,
            const ImagePixel::BlendMode blendMode,
            const ImageDraw::DstAlpha dstAlpha,
            const Color color)
        {
            if ((innerThickness < 0) || (outerThickness < 0))
            {
                return;
            }

            if ((rect.w <= 0) || (rect.h <= 0))
            {
                return;
            }

            if ((innerThickness == 0) && (outerThickness == 0))
            {
                return;
            }

            const int32 outerLeft = rect.x - outerThickness;
            const int32 outerTop = rect.y - outerThickness;
            const int32 outerRight = rect.x + rect.w + outerThickness;
            const int32 outerBottom = rect.y + rect.h + outerThickness;

            if ((outerLeft >= outerRight) || (outerTop >= outerBottom))
            {
                return;
            }

            const int32 innerLeft = rect.x + innerThickness;
            const int32 innerTop = rect.y + innerThickness;
            const int32 innerRight = rect.x + rect.w - innerThickness;
            const int32 innerBottom = rect.y + rect.h - innerThickness;

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            // inner が消滅する場合は outer 全体を塗ります。
            if ((innerLeft >= innerRight) || (innerTop >= innerBottom))
            {
                FillRectClipped(view, outerLeft, outerTop, outerRight, outerBottom, writer);
                return;
            }

            // frame = outer - inner
            // 4 つの矩形は互いに pixel 単位で重ならないように分解します。

            // Top
            FillRectClipped(
                view,
                outerLeft,
                outerTop,
                outerRight,
                innerTop,
                writer
            );

            // Bottom
            FillRectClipped(
                view,
                outerLeft,
                innerBottom,
                outerRight,
                outerBottom,
                writer
            );

            // Left
            FillRectClipped(
                view,
                outerLeft,
                innerTop,
                innerLeft,
                innerBottom,
                writer
            );

            // Right
            FillRectClipped(
                view,
                innerRight,
                innerTop,
                outerRight,
                innerBottom,
                writer
            );
        }

        void FillRectFrameFloat(
            const ImageView view,
            const RectF& rect,
            const double innerThickness,
            const double outerThickness,
            const ImagePixel::BlendMode blendMode,
            const ImageDraw::DstAlpha dstAlpha,
            const Color color)
        {
            if (!(0.0 <= innerThickness) || !(0.0 <= outerThickness))
            {
                return;
            }

            if (!(0.0 < rect.w) || !(0.0 < rect.h))
            {
                return;
            }

            if ((innerThickness == 0.0) && (outerThickness == 0.0))
            {
                return;
            }

            const double outerLeft = rect.x - outerThickness;
            const double outerTop = rect.y - outerThickness;
            const double outerRight = rect.x + rect.w + outerThickness;
            const double outerBottom = rect.y + rect.h + outerThickness;

            if (!(outerLeft < outerRight) || !(outerTop < outerBottom))
            {
                return;
            }

            const double innerLeft = rect.x + innerThickness;
            const double innerTop = rect.y + innerThickness;
            const double innerRight = rect.x + rect.w - innerThickness;
            const double innerBottom = rect.y + rect.h - innerThickness;

            const bool hasInner =
                (innerLeft < innerRight)
                && (innerTop < innerBottom);

            // inner が消滅する場合は outer 全体を通常の RectF fill として描画します。
            if (!hasInner)
            {
                FillRectFloat(
                    view,
                    RectF{
                        outerLeft,
                        outerTop,
                        outerRight - outerLeft,
                        outerBottom - outerTop
                    },
                    blendMode,
                    dstAlpha,
                    color
                );

                return;
            }

            int32 x0 = FloorToInt(outerLeft);
            int32 y0 = FloorToInt(outerTop);
            int32 x1 = CeilToInt(outerRight);
            int32 y1 = CeilToInt(outerBottom);

            if (!ClipRectI(x0, y0, x1, y1, view.width, view.height))
            {
                return;
            }

            const size_t rowLength = static_cast<size_t>(x1 - x0);

            std::vector<uint8> outerX(rowLength);
            std::vector<uint8> innerX(rowLength);
            std::vector<uint8> coverage(rowLength);

            for (size_t i = 0; i < rowLength; ++i)
            {
                const int32 x = x0 + static_cast<int32>(i);

                outerX[i] = IntervalCoverage1D(outerLeft, outerRight, x);
                innerX[i] = IntervalCoverage1D(innerLeft, innerRight, x);
            }

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            for (int32 y = y0; y < y1; ++y)
            {
                const uint8 outerY = IntervalCoverage1D(outerTop, outerBottom, y);

                if (outerY == 0)
                {
                    continue;
                }

                const uint8 innerY = IntervalCoverage1D(innerTop, innerBottom, y);

                bool anyCoverage = false;
                bool allFull = true;

                for (size_t i = 0; i < rowLength; ++i)
                {
                    const uint8 outerCoverage = ImagePixel::detail::Div255Round8(
                        static_cast<uint32>(outerX[i]) * static_cast<uint32>(outerY)
                    );

                    const uint8 innerCoverage = ImagePixel::detail::Div255Round8(
                        static_cast<uint32>(innerX[i]) * static_cast<uint32>(innerY)
                    );

                    const uint8 frameCoverage = static_cast<uint8>(
                        (innerCoverage < outerCoverage) ? (outerCoverage - innerCoverage) : 0
                        );

                    coverage[i] = frameCoverage;

                    anyCoverage |= (frameCoverage != 0);
                    allFull &= (frameCoverage == 255);
                }

                if (!anyCoverage)
                {
                    continue;
                }

                Color* dstRow = view.data
                    + static_cast<size_t>(y) * view.stride
                    + static_cast<size_t>(x0);

                if (allFull)
                {
                    writer.write(dstRow, rowLength);
                }
                else
                {
                    writer.writeWithCoverage(dstRow, coverage.data(), rowLength);
                }
            }
        }

        void FillCircleNoAAFast(
            ImageView view,
            const Circle& circle,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            const int32 cx = static_cast<int32>(std::floor(circle.x + 0.5));
            const int32 cy = static_cast<int32>(std::floor(circle.y + 0.5));
            const int32 radius = static_cast<int32>(std::floor(circle.r + 0.5));

            if (radius <= 0)
            {
                return;
            }

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            int32 err = 0;
            int32 dx = radius;
            int32 dy = 0;
            int32 plus = 1;
            int32 minus = (radius << 1) - 1;

            const bool fullyInside =
                (radius <= cx)
                && (cx < view.width - radius)
                && (radius <= cy)
                && (cy < view.height - radius);

            auto writeSpan = [&](int32 y, int32 x0, int32 x1)
                {
                    if (fullyInside)
                    {
                        Color* dst = view.data
                            + static_cast<size_t>(y) * view.stride
                            + static_cast<size_t>(x0);

                        writer.write(dst, static_cast<size_t>(x1 - x0 + 1));
                    }
                    else
                    {
                        WriteSpanInclusiveClipped(writer, view, y, x0, x1);
                    }
                };

            while (dx >= dy)
            {
                const int32 y11 = cy - dy;
                const int32 y12 = cy + dy;
                const int32 y21 = cy - dx;
                const int32 y22 = cy + dx;

                const int32 x11 = cx - dx;
                const int32 x12 = cx + dx;
                const int32 x21 = cx - dy;
                const int32 x22 = cx + dy;

                writeSpan(y11, x11, x12);

                if (y12 != y11)
                {
                    writeSpan(y12, x11, x12);
                }

                if (dx != dy)
                {
                    writeSpan(y21, x21, x22);

                    if (y22 != y21)
                    {
                        writeSpan(y22, x21, x22);
                    }
                }

                ++dy;
                err += plus;
                plus += 2;

                const int32 mask = (err <= 0) - 1;

                err -= minus & mask;
                dx += mask;
                minus -= mask & 2;
            }
        }

        [[nodiscard]]
        uint8 CircleCoverageAt(
            double pixelCenterX,
            double pixelCenterY,
            double centerX,
            double centerY,
            double radius) noexcept
        {
            const double dx = pixelCenterX - centerX;
            const double dy = pixelCenterY - centerY;
            const double distance = std::sqrt(dx * dx + dy * dy);

            return ToCoverage(radius + 0.5 - distance);
        }

        void WriteCircleCoverageSpan(
            const ImagePixel::detail::SolidColorWriter& writer,
            Color* dstRow,
            int32 x0,
            int32 x1,
            int32 y,
            double centerX,
            double centerY,
            double radius,
            std::vector<uint8>& coverageBuffer)
        {
            if (x0 >= x1)
            {
                return;
            }

            const size_t count = static_cast<size_t>(x1 - x0);
            coverageBuffer.resize(count);

            const double py = static_cast<double>(y) + 0.5;

            for (size_t i = 0; i < count; ++i)
            {
                const double px = static_cast<double>(x0) + static_cast<double>(i) + 0.5;
                coverageBuffer[i] = CircleCoverageAt(px, py, centerX, centerY, radius);
            }

            writer.writeWithCoverage(
                dstRow + static_cast<size_t>(x0),
                coverageBuffer.data(),
                count
            );
        }

        void FillCircleAA(
            ImageView view,
            const Circle& circle,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            const double cx = circle.x;
            const double cy = circle.y;
            const double r = circle.r;

            if (!(0.0 < r))
            {
                return;
            }

            const double outerR = r + 0.5;
            const double outerR2 = outerR * outerR;
            const double innerR = Max(0.0, r - 0.5);
            const double innerR2 = innerR * innerR;

            int32 y0 = FloorToInt(cy - outerR - 0.5);
            int32 y1 = CeilToInt(cy + outerR - 0.5) + 1;

            y0 = ClampInt(y0, 0, view.height);
            y1 = ClampInt(y1, 0, view.height);

            if (y0 >= y1)
            {
                return;
            }

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            std::vector<uint8> coverageBuffer;

            for (int32 y = y0; y < y1; ++y)
            {
                const double py = static_cast<double>(y) + 0.5;
                const double dy = py - cy;
                const double dy2 = dy * dy;

                if (outerR2 <= dy2)
                {
                    continue;
                }

                const double outerDx = std::sqrt(outerR2 - dy2);

                int32 xOuter0 = CeilToInt(cx - outerDx - 0.5);
                int32 xOuter1 = FloorToInt(cx + outerDx - 0.5) + 1;

                xOuter0 = ClampInt(xOuter0, 0, view.width);
                xOuter1 = ClampInt(xOuter1, 0, view.width);

                if (xOuter0 >= xOuter1)
                {
                    continue;
                }

                int32 xFull0 = xOuter1;
                int32 xFull1 = xOuter0;

                if ((0.0 < innerR) && (dy2 < innerR2))
                {
                    const double innerDx = std::sqrt(innerR2 - dy2);

                    xFull0 = CeilToInt(cx - innerDx - 0.5);
                    xFull1 = FloorToInt(cx + innerDx - 0.5) + 1;

                    xFull0 = ClampInt(xFull0, xOuter0, xOuter1);
                    xFull1 = ClampInt(xFull1, xOuter0, xOuter1);
                }

                Color* dstRow = view.data + static_cast<size_t>(y) * view.stride;

                if (xFull0 < xFull1)
                {
                    WriteCircleCoverageSpan(writer, dstRow, xOuter0, xFull0, y, cx, cy, r, coverageBuffer);

                    writer.write(
                        dstRow + static_cast<size_t>(xFull0),
                        static_cast<size_t>(xFull1 - xFull0)
                    );

                    WriteCircleCoverageSpan(writer, dstRow, xFull1, xOuter1, y, cx, cy, r, coverageBuffer);
                }
                else
                {
                    WriteCircleCoverageSpan(writer, dstRow, xOuter0, xOuter1, y, cx, cy, r, coverageBuffer);
                }
            }
        }

        template <class PointType>
        bool GetConvexPolygonSpanAtY(
            const PointType* points,
            size_t count,
            int32 y,
            int32& outX0,
            int32& outX1) noexcept
        {
            if (count < 3)
            {
                return false;
            }

            const double scanY = static_cast<double>(y) + 0.5;

            double intersections[16];
            size_t intersectionCount = 0;

            for (size_t i = 0; i < count; ++i)
            {
                const PointType& p0 = points[i];
                const PointType& p1 = points[(i + 1) % count];

                const double yA = p0.y;
                const double yB = p1.y;

                if (yA == yB)
                {
                    continue;
                }

                const bool crosses =
                    ((yA <= scanY) && (scanY < yB))
                    || ((yB <= scanY) && (scanY < yA));

                if (!crosses)
                {
                    continue;
                }

                const double t = (scanY - yA) / (yB - yA);
                const double x = p0.x + (p1.x - p0.x) * t;

                if (intersectionCount < 16)
                {
                    intersections[intersectionCount++] = x;
                }
            }

            if (intersectionCount < 2)
            {
                return false;
            }

            std::sort(intersections, intersections + intersectionCount);

            const double xA = intersections[0];
            const double xB = intersections[intersectionCount - 1];

            outX0 = CeilToInt(xA - 0.5);
            outX1 = CeilToInt(xB - 0.5);

            return (outX0 < outX1);
        }

        template <class PointType>
        void FillConvexPolygonNoAA(
            ImageView view,
            const PointType* points,
            size_t count,
            const ImagePixel::detail::SolidColorWriter& writer)
        {
            if ((count < 3) || (view.data == nullptr))
            {
                return;
            }

            double minY = points[0].y;
            double maxY = points[0].y;

            for (size_t i = 1; i < count; ++i)
            {
                minY = Min(minY, points[i].y);
                maxY = Max(maxY, points[i].y);
            }

            int32 y0 = FloorToInt(minY - 0.5);
            int32 y1 = CeilToInt(maxY - 0.5) + 1;

            y0 = ClampInt(y0, 0, view.height);
            y1 = ClampInt(y1, 0, view.height);

            if (y0 >= y1)
            {
                return;
            }

            for (int32 y = y0; y < y1; ++y)
            {
                int32 x0 = 0;
                int32 x1 = 0;

                if (!GetConvexPolygonSpanAtY(points, count, y, x0, x1))
                {
                    continue;
                }

                WriteSpanHalfOpenClipped(writer, view, y, x0, x1);
            }
        }

        void MakeOrientedRectPoints(
            const OrientedRectBasis& basis,
            double expandLength,
            double expandThickness,
            Vec2 out[4]) noexcept
        {
            const double hl = basis.halfLength + expandLength;
            const double ht = basis.halfThickness + expandThickness;

            const Vec2 a{
                basis.dir.x * hl,
                basis.dir.y * hl
            };

            const Vec2 b{
                basis.normal.x * ht,
                basis.normal.y * ht
            };

            out[0] = Vec2{ basis.center.x + a.x + b.x, basis.center.y + a.y + b.y };
            out[1] = Vec2{ basis.center.x - a.x + b.x, basis.center.y - a.y + b.y };
            out[2] = Vec2{ basis.center.x - a.x - b.x, basis.center.y - a.y - b.y };
            out[3] = Vec2{ basis.center.x + a.x - b.x, basis.center.y + a.y - b.y };
        }

        void IncludeOrientedRectBounds(
            BoundsD& bounds,
            const OrientedRectBasis& basis,
            double expandLength,
            double expandThickness) noexcept
        {
            Vec2 points[4];
            MakeOrientedRectPoints(basis, expandLength, expandThickness, points);

            for (const Vec2& p : points)
            {
                bounds.include(p);
            }
        }

        bool GetOrientedRectSupportSpanAtY(
            const OrientedRectBasis& basis,
            int32 y,
            int32& outX0,
            int32& outX1) noexcept
        {
            Vec2 points[4];
            MakeOrientedRectPoints(basis, 0.5, 0.5, points);

            return GetConvexPolygonSpanAtY(points, 4, y, outX0, outX1);
        }

        [[nodiscard]]
        double SignedDistanceToOrientedRect(
            Vec2 p,
            const OrientedRectBasis& basis) noexcept
        {
            const double vx = p.x - basis.center.x;
            const double vy = p.y - basis.center.y;

            const double localX = vx * basis.dir.x + vy * basis.dir.y;
            const double localY = vx * basis.normal.x + vy * basis.normal.y;

            const double qx = std::abs(localX) - basis.halfLength;
            const double qy = std::abs(localY) - basis.halfThickness;

            const double outsideX = Max(qx, 0.0);
            const double outsideY = Max(qy, 0.0);
            const double outsideDistance = std::sqrt(outsideX * outsideX + outsideY * outsideY);
            const double insideDistance = Min(Max(qx, qy), 0.0);

            return outsideDistance + insideDistance;
        }

        bool GetCircleSpanAtY(
            const Vec2& center,
            double radius,
            int32 y,
            int32& outX0,
            int32& outX1) noexcept
        {
            if (!(0.0 < radius))
            {
                return false;
            }

            const double py = static_cast<double>(y) + 0.5;
            const double dy = py - center.y;
            const double dy2 = dy * dy;
            const double r2 = radius * radius;

            if (r2 <= dy2)
            {
                return false;
            }

            const double dx = std::sqrt(r2 - dy2);

            outX0 = CeilToInt(center.x - dx - 0.5);
            outX1 = FloorToInt(center.x + dx - 0.5) + 1;

            return (outX0 < outX1);
        }

        void MaxCoverage(uint8& dst, uint8 src) noexcept
        {
            if (dst < src)
            {
                dst = src;
            }
        }

        void AddOrientedRectCoverageToRange(
            const OrientedRectBasis& basis,
            int32 y,
            int32 x0,
            std::vector<uint8>& coverage)
        {
            const double py = static_cast<double>(y) + 0.5;

            for (size_t i = 0; i < coverage.size(); ++i)
            {
                const double px = static_cast<double>(x0) + static_cast<double>(i) + 0.5;

                const double sd = SignedDistanceToOrientedRect(
                    Vec2{ px, py },
                    basis
                );

                const uint8 c = ToCoverage(0.5 - sd);

                MaxCoverage(coverage[i], c);
            }
        }

        void AddCircleCoverageToRange(
            const Vec2& center,
            double radius,
            int32 y,
            int32 x0,
            std::vector<uint8>& coverage)
        {
            const double py = static_cast<double>(y) + 0.5;

            for (size_t i = 0; i < coverage.size(); ++i)
            {
                const double px = static_cast<double>(x0) + static_cast<double>(i) + 0.5;

                const double dx = px - center.x;
                const double dy = py - center.y;
                const double distance = std::sqrt(dx * dx + dy * dy);

                const uint8 c = ToCoverage(radius + 0.5 - distance);

                MaxCoverage(coverage[i], c);
            }
        }

        void DrawLineNoAA(
            ImageView view,
            const Vec2& from,
            const Vec2& to,
            double thickness,
            Color color,
            ImagePixel::BlendMode blendMode,
            LineCap lineCap,
            ImageDraw::DstAlpha dstAlpha)
        {
            if (!(0.0 < thickness))
            {
                return;
            }

            const double dx = to.x - from.x;
            const double dy = to.y - from.y;
            const double length = std::sqrt(dx * dx + dy * dy);
            const double half = thickness * 0.5;

            if (!(0.0 < length))
            {
                if (lineCap == LineCap::Round)
                {
                    const Circle circle{ from.x, from.y, half };
                    FillCircleNoAAFast(view, circle, blendMode, dstAlpha, color);
                }
                else if (lineCap == LineCap::Square)
                {
                    const Vec2 quad[4] =
                    {
                        Vec2{ from.x - half, from.y - half },
                        Vec2{ from.x + half, from.y - half },
                        Vec2{ from.x + half, from.y + half },
                        Vec2{ from.x - half, from.y + half },
                    };

                    ImagePixel::detail::SolidColorWriter writer{
                        blendMode,
                        color,
                        ToDstAlphaMode(dstAlpha)
                    };

                    FillConvexPolygonNoAA(view, quad, 4, writer);
                }

                return;
            }

            const double ux = dx / length;
            const double uy = dy / length;

            const double nx = -uy;
            const double ny = ux;

            Vec2 body0 = from;
            Vec2 body1 = to;

            if (lineCap == LineCap::Square)
            {
                body0.x -= ux * half;
                body0.y -= uy * half;
                body1.x += ux * half;
                body1.y += uy * half;
            }

            const Vec2 quad[4] =
            {
                Vec2{ body0.x + nx * half, body0.y + ny * half },
                Vec2{ body0.x - nx * half, body0.y - ny * half },
                Vec2{ body1.x - nx * half, body1.y - ny * half },
                Vec2{ body1.x + nx * half, body1.y + ny * half },
            };

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            if (lineCap != LineCap::Round)
            {
                FillConvexPolygonNoAA(view, quad, 4, writer);
                return;
            }

            double minY = quad[0].y;
            double maxY = quad[0].y;

            for (size_t i = 1; i < 4; ++i)
            {
                minY = Min(minY, quad[i].y);
                maxY = Max(maxY, quad[i].y);
            }

            minY = Min(minY, Min(from.y - half, to.y - half));
            maxY = Max(maxY, Max(from.y + half, to.y + half));

            int32 y0 = FloorToInt(minY - 0.5);
            int32 y1 = CeilToInt(maxY - 0.5) + 1;

            y0 = ClampInt(y0, 0, view.height);
            y1 = ClampInt(y1, 0, view.height);

            if (y0 >= y1)
            {
                return;
            }

            std::vector<SpanI> spans;
            spans.reserve(3);

            for (int32 y = y0; y < y1; ++y)
            {
                spans.clear();

                int32 x0 = 0;
                int32 x1 = 0;

                if (GetConvexPolygonSpanAtY(quad, 4, y, x0, x1))
                {
                    AddSpanClipped(spans, x0, x1, view.width);
                }

                if (GetCircleSpanAtY(from, half, y, x0, x1))
                {
                    AddSpanClipped(spans, x0, x1, view.width);
                }

                if (GetCircleSpanAtY(to, half, y, x0, x1))
                {
                    AddSpanClipped(spans, x0, x1, view.width);
                }

                WriteMergedSpans(writer, view, y, spans);
            }
        }

        void DrawLineAA(
            ImageView view,
            const Vec2& from,
            const Vec2& to,
            double thickness,
            Color color,
            ImagePixel::BlendMode blendMode,
            LineCap lineCap,
            ImageDraw::DstAlpha dstAlpha)
        {
            if (!(0.0 < thickness))
            {
                return;
            }

            const double dx = to.x - from.x;
            const double dy = to.y - from.y;
            const double length = std::sqrt(dx * dx + dy * dy);
            const double half = thickness * 0.5;

            if (!(0.0 < length))
            {
                if (lineCap == LineCap::Flat)
                {
                    return;
                }

                if (lineCap == LineCap::Round)
                {
                    const Circle circle{ from.x, from.y, half };
                    FillCircleAA(view, circle, blendMode, dstAlpha, color);
                    return;
                }

                const OrientedRectBasis squareBasis{
                    from,
                    Vec2{ 1.0, 0.0 },
                    Vec2{ 0.0, 1.0 },
                    half,
                    half
                };

                BoundsD bounds;
                IncludeOrientedRectBounds(bounds, squareBasis, 0.5, 0.5);

                int32 y0 = FloorToInt(bounds.minY - 0.5);
                int32 y1 = CeilToInt(bounds.maxY - 0.5) + 1;

                y0 = ClampInt(y0, 0, view.height);
                y1 = ClampInt(y1, 0, view.height);

                if (y0 >= y1)
                {
                    return;
                }

                ImagePixel::detail::SolidColorWriter writer{
                    blendMode,
                    color,
                    ToDstAlphaMode(dstAlpha)
                };

                std::vector<SpanI> supportSpans;
                std::vector<uint8> coverage;

                for (int32 y = y0; y < y1; ++y)
                {
                    supportSpans.clear();

                    int32 x0 = 0;
                    int32 x1 = 0;

                    if (GetOrientedRectSupportSpanAtY(squareBasis, y, x0, x1))
                    {
                        AddSpanClipped(supportSpans, x0, x1, view.width);
                    }

                    SortAndMergeSpans(supportSpans);

                    Color* row = view.data + static_cast<size_t>(y) * view.stride;

                    for (const SpanI& span : supportSpans)
                    {
                        const size_t count = static_cast<size_t>(span.x1 - span.x0);

                        coverage.assign(count, 0);
                        AddOrientedRectCoverageToRange(squareBasis, y, span.x0, coverage);

                        bool anyCoverage = false;
                        bool allFull = true;

                        for (const uint8 c : coverage)
                        {
                            anyCoverage |= (c != 0);
                            allFull &= (c == 255);
                        }

                        if (!anyCoverage)
                        {
                            continue;
                        }

                        if (allFull)
                        {
                            writer.write(row + static_cast<size_t>(span.x0), count);
                        }
                        else
                        {
                            writer.writeWithCoverage(row + static_cast<size_t>(span.x0), coverage.data(), count);
                        }
                    }
                }

                return;
            }

            const double ux = dx / length;
            const double uy = dy / length;

            const Vec2 dir{ ux, uy };
            const Vec2 normal{ -uy, ux };

            double halfLength = length * 0.5;

            if (lineCap == LineCap::Square)
            {
                halfLength += half;
            }

            const OrientedRectBasis body{
                Vec2{ (from.x + to.x) * 0.5, (from.y + to.y) * 0.5 },
                dir,
                normal,
                halfLength,
                half
            };

            BoundsD bounds;
            IncludeOrientedRectBounds(bounds, body, 0.5, 0.5);

            if (lineCap == LineCap::Round)
            {
                bounds.includeCircle(from, half + 0.5);
                bounds.includeCircle(to, half + 0.5);
            }

            if (!bounds.valid())
            {
                return;
            }

            int32 y0 = FloorToInt(bounds.minY - 0.5);
            int32 y1 = CeilToInt(bounds.maxY - 0.5) + 1;

            y0 = ClampInt(y0, 0, view.height);
            y1 = ClampInt(y1, 0, view.height);

            if (y0 >= y1)
            {
                return;
            }

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            std::vector<SpanI> supportSpans;
            supportSpans.reserve(3);

            std::vector<uint8> coverage;

            for (int32 y = y0; y < y1; ++y)
            {
                supportSpans.clear();

                int32 x0 = 0;
                int32 x1 = 0;

                if (GetOrientedRectSupportSpanAtY(body, y, x0, x1))
                {
                    AddSpanClipped(supportSpans, x0, x1, view.width);
                }

                if (lineCap == LineCap::Round)
                {
                    if (GetCircleSpanAtY(from, half + 0.5, y, x0, x1))
                    {
                        AddSpanClipped(supportSpans, x0, x1, view.width);
                    }

                    if (GetCircleSpanAtY(to, half + 0.5, y, x0, x1))
                    {
                        AddSpanClipped(supportSpans, x0, x1, view.width);
                    }
                }

                if (supportSpans.empty())
                {
                    continue;
                }

                SortAndMergeSpans(supportSpans);

                Color* row = view.data + static_cast<size_t>(y) * view.stride;

                for (const SpanI& span : supportSpans)
                {
                    const size_t count = static_cast<size_t>(span.x1 - span.x0);

                    coverage.assign(count, 0);

                    AddOrientedRectCoverageToRange(body, y, span.x0, coverage);

                    if (lineCap == LineCap::Round)
                    {
                        AddCircleCoverageToRange(from, half, y, span.x0, coverage);
                        AddCircleCoverageToRange(to, half, y, span.x0, coverage);
                    }

                    bool anyCoverage = false;
                    bool allFull = true;

                    for (const uint8 c : coverage)
                    {
                        anyCoverage |= (c != 0);
                        allFull &= (c == 255);
                    }

                    if (!anyCoverage)
                    {
                        continue;
                    }

                    if (allFull)
                    {
                        writer.write(row + static_cast<size_t>(span.x0), count);
                    }
                    else
                    {
                        writer.writeWithCoverage(
                            row + static_cast<size_t>(span.x0),
                            coverage.data(),
                            count
                        );
                    }
                }
            }
        }
    }

    namespace ImageDraw
    {
        void Fill(
            Image& image,
            const Rect& rect,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0) || (rect.w <= 0) || (rect.h <= 0))
            {
                return;
            }

            FillRectInteger(view, rect, blendMode, dstAlpha, color);
        }

        void Fill(
            Image& image,
            const RectF& rect,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0) || !(0.0 < rect.w) || !(0.0 < rect.h))
            {
                return;
            }

            FillRectFloat(view, rect, blendMode, dstAlpha, color);
        }

        void RectFrame(
            Image& image,
            const Rect& rect,
            const int32 innerThickness,
            const int32 outerThickness,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            FillRectFrameInteger(
                view,
                rect,
                innerThickness,
                outerThickness,
                blendMode,
                dstAlpha,
                color
            );
        }

        void RectFrame(
            Image& image,
            const RectF& rect,
            const double innerThickness,
            const double outerThickness,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            FillRectFrameFloat(
                view,
                rect,
                innerThickness,
                outerThickness,
                blendMode,
                dstAlpha,
                color
            );
        }

        void Fill(
            Image& image,
            const Circle& circle,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const Antialiasing antialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0) || !(0.0 < circle.r))
            {
                return;
            }

            if (antialiasing == Antialiasing::Yes)
            {
                FillCircleAA(view, circle, blendMode, dstAlpha, color);
            }
            else
            {
                FillCircleNoAAFast(view, circle, blendMode, dstAlpha, color);
            }
        }

        void Line(
            Image& image,
            const Vec2& from,
            const Vec2& to,
            const double thickness,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const Antialiasing antialiasing,
            const LineCap lineCap,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0) || !(0.0 < thickness))
            {
                return;
            }

            if (antialiasing == Antialiasing::No)
            {
                DrawLineNoAA(view, from, to, thickness, color, blendMode, lineCap, dstAlpha);
            }
            else
            {
                DrawLineAA(view, from, to, thickness, color, blendMode, lineCap, dstAlpha);
            }
        }
    }
}
