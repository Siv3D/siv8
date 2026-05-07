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
# include <Siv3D/Polygon.hpp>
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

        struct PolygonEdge
        {
            int32 y0 = 0;
            int32 y1 = 0; // exclusive
            double x = 0.0;
            double dx = 0.0;
        };

        struct PolygonContourView
        {
            const Vec2* points = nullptr;
            size_t count = 0;
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
        uint8 CoverageFromSignedDistance(double signedDistance) noexcept
        {
            return ToCoverage(0.5 - signedDistance);
        }

        [[nodiscard]]
        double CircleSignedDistance(
            double pixelCenterX,
            double pixelCenterY,
            double centerX,
            double centerY,
            double radius) noexcept
        {
            const double dx = pixelCenterX - centerX;
            const double dy = pixelCenterY - centerY;

            return (std::sqrt(dx * dx + dy * dy) - radius);
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

        [[nodiscard]]
        uint8 CircleCoverageAt(
            double pixelCenterX,
            double pixelCenterY,
            double centerX,
            double centerY,
            double radius) noexcept
        {
            return CoverageFromSignedDistance(
                CircleSignedDistance(pixelCenterX, pixelCenterY, centerX, centerY, radius)
            );
        }

        [[nodiscard]]
        double EllipseSignedDistance(
            double pixelCenterX,
            double pixelCenterY,
            double centerX,
            double centerY,
            double radiusX,
            double radiusY) noexcept
        {
            if (!(0.0 < radiusX) || !(0.0 < radiusY))
            {
                return 1.0;
            }

            const double x = std::abs(pixelCenterX - centerX);
            const double y = std::abs(pixelCenterY - centerY);

            if ((x == 0.0) && (y == 0.0))
            {
                return -Min(radiusX, radiusY);
            }

            constexpr double HalfPi = 1.57079632679489661923;

            double t = std::atan2(y * radiusX, x * radiusY);
            t = Min(HalfPi, Max(0.0, t));

            for (int32 i = 0; i < 8; ++i)
            {
                const double cosT = std::cos(t);
                const double sinT = std::sin(t);

                const double ex = radiusX * cosT;
                const double ey = radiusY * sinT;

                const double qx = ex - x;
                const double qy = ey - y;

                const double dex = -radiusX * sinT;
                const double dey = radiusY * cosT;
                const double ddex = -radiusX * cosT;
                const double ddey = -radiusY * sinT;

                const double f = qx * dex + qy * dey;
                const double df = dex * dex + qx * ddex + dey * dey + qy * ddey;

                if (std::abs(df) <= 1.0e-12)
                {
                    break;
                }

                const double nextT = t - (f / df);
                const double clampedT = Min(HalfPi, Max(0.0, nextT));

                if (std::abs(clampedT - t) <= 1.0e-12)
                {
                    t = clampedT;
                    break;
                }

                t = clampedT;
            }

            const double closestX = radiusX * std::cos(t);
            const double closestY = radiusY * std::sin(t);

            const double dx = x - closestX;
            const double dy = y - closestY;
            const double distance = std::sqrt(dx * dx + dy * dy);

            const double nx = x / radiusX;
            const double ny = y / radiusY;
            const double implicit = nx * nx + ny * ny - 1.0;

            return (implicit <= 0.0) ? -distance : distance;
        }

        [[nodiscard]]
        uint8 EllipseCoverageAt(
            double pixelCenterX,
            double pixelCenterY,
            double centerX,
            double centerY,
            double radiusX,
            double radiusY) noexcept
        {
            return CoverageFromSignedDistance(
                EllipseSignedDistance(pixelCenterX, pixelCenterY, centerX, centerY, radiusX, radiusY)
            );
        }

        void WriteCoverageRow(
            const ImagePixel::detail::SolidColorWriter& writer,
            Color* dstRow,
            int32 x0,
            std::vector<uint8>& coverage)
        {
            bool anyCoverage = false;
            bool allFull = true;

            for (const uint8 c : coverage)
            {
                anyCoverage |= (c != 0);
                allFull &= (c == 255);
            }

            if (!anyCoverage)
            {
                return;
            }

            if (allFull)
            {
                writer.write(
                    dstRow + static_cast<size_t>(x0),
                    coverage.size()
                );
            }
            else
            {
                writer.writeWithCoverage(
                    dstRow + static_cast<size_t>(x0),
                    coverage.data(),
                    coverage.size()
                );
            }
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

        void FillCircleFrameNoAA(
            ImageView view,
            const Circle& circle,
            double innerThickness,
            double outerThickness,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            if (!(0.0 <= innerThickness) || !(0.0 <= outerThickness))
            {
                return;
            }

            if (!(0.0 < circle.r))
            {
                return;
            }

            if ((innerThickness == 0.0) && (outerThickness == 0.0))
            {
                return;
            }

            const double outerR = circle.r + outerThickness;

            if (!(0.0 < outerR))
            {
                return;
            }

            const double innerR = circle.r - innerThickness;

            if (!(0.0 < innerR))
            {
                FillCircleNoAAFast(view, Circle{ circle.x, circle.y, outerR }, blendMode, dstAlpha, color);
                return;
            }

            int32 y0 = FloorToInt(circle.y - outerR - 0.5);
            int32 y1 = CeilToInt(circle.y + outerR - 0.5) + 1;

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

            const Vec2 center{ circle.x, circle.y };

            for (int32 y = y0; y < y1; ++y)
            {
                int32 outerX0 = 0;
                int32 outerX1 = 0;

                if (!GetCircleSpanAtY(center, outerR, y, outerX0, outerX1))
                {
                    continue;
                }

                outerX0 = ClampInt(outerX0, 0, view.width);
                outerX1 = ClampInt(outerX1, 0, view.width);

                if (outerX0 >= outerX1)
                {
                    continue;
                }

                int32 innerX0 = 0;
                int32 innerX1 = 0;

                Color* row = view.data + static_cast<size_t>(y) * view.stride;

                if (!GetCircleSpanAtY(center, innerR, y, innerX0, innerX1))
                {
                    writer.write(
                        row + static_cast<size_t>(outerX0),
                        static_cast<size_t>(outerX1 - outerX0)
                    );

                    continue;
                }

                innerX0 = ClampInt(innerX0, outerX0, outerX1);
                innerX1 = ClampInt(innerX1, outerX0, outerX1);

                if (outerX0 < innerX0)
                {
                    writer.write(
                        row + static_cast<size_t>(outerX0),
                        static_cast<size_t>(innerX0 - outerX0)
                    );
                }

                if (innerX1 < outerX1)
                {
                    writer.write(
                        row + static_cast<size_t>(innerX1),
                        static_cast<size_t>(outerX1 - innerX1)
                    );
                }
            }
        }

        void FillCircleFrameAA(
            ImageView view,
            const Circle& circle,
            double innerThickness,
            double outerThickness,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            if (!(0.0 <= innerThickness) || !(0.0 <= outerThickness))
            {
                return;
            }

            if (!(0.0 < circle.r))
            {
                return;
            }

            if ((innerThickness == 0.0) && (outerThickness == 0.0))
            {
                return;
            }

            const double outerR = circle.r + outerThickness;

            if (!(0.0 < outerR))
            {
                return;
            }

            const double innerR = circle.r - innerThickness;

            if (!(0.0 < innerR))
            {
                FillCircleAA(view, Circle{ circle.x, circle.y, outerR }, blendMode, dstAlpha, color);
                return;
            }

            const double supportR = outerR + 0.5;

            int32 y0 = FloorToInt(circle.y - supportR - 0.5);
            int32 y1 = CeilToInt(circle.y + supportR - 0.5) + 1;

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

            std::vector<uint8> coverage;

            for (int32 y = y0; y < y1; ++y)
            {
                int32 x0 = 0;
                int32 x1 = 0;

                if (!GetCircleSpanAtY(Vec2{ circle.x, circle.y }, supportR, y, x0, x1))
                {
                    continue;
                }

                x0 = ClampInt(x0, 0, view.width);
                x1 = ClampInt(x1, 0, view.width);

                if (x0 >= x1)
                {
                    continue;
                }

                const size_t count = static_cast<size_t>(x1 - x0);
                coverage.assign(count, 0);

                const double py = static_cast<double>(y) + 0.5;

                for (size_t i = 0; i < count; ++i)
                {
                    const double px = static_cast<double>(x0) + static_cast<double>(i) + 0.5;

                    const uint8 outerCoverage = CoverageFromSignedDistance(
                        CircleSignedDistance(px, py, circle.x, circle.y, outerR)
                    );

                    const uint8 innerCoverage = CoverageFromSignedDistance(
                        CircleSignedDistance(px, py, circle.x, circle.y, innerR)
                    );

                    coverage[i] = static_cast<uint8>(
                        (innerCoverage < outerCoverage) ? (outerCoverage - innerCoverage) : 0
                    );
                }

                Color* row = view.data + static_cast<size_t>(y) * view.stride;

                WriteCoverageRow(writer, row, x0, coverage);
            }
        }

        [[nodiscard]]
        bool GetEllipseSpanAtY(
            double centerX,
            double centerY,
            double radiusX,
            double radiusY,
            int32 y,
            int32& outX0,
            int32& outX1) noexcept
        {
            if (!(0.0 < radiusX) || !(0.0 < radiusY))
            {
                return false;
            }

            const double py = static_cast<double>(y) + 0.5;
            const double ny = (py - centerY) / radiusY;
            const double yy = ny * ny;

            if (1.0 <= yy)
            {
                return false;
            }

            const double dx = radiusX * std::sqrt(1.0 - yy);

            outX0 = CeilToInt(centerX - dx - 0.5);
            outX1 = FloorToInt(centerX + dx - 0.5) + 1;

            return (outX0 < outX1);
        }

        void FillEllipseNoAA(
            ImageView view,
            const Ellipse& ellipse,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            if (!(0.0 < ellipse.a) || !(0.0 < ellipse.b))
            {
                return;
            }

            int32 y0 = FloorToInt(ellipse.y - ellipse.b - 0.5);
            int32 y1 = CeilToInt(ellipse.y + ellipse.b - 0.5) + 1;

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

            for (int32 y = y0; y < y1; ++y)
            {
                int32 x0 = 0;
                int32 x1 = 0;

                if (!GetEllipseSpanAtY(ellipse.x, ellipse.y, ellipse.a, ellipse.b, y, x0, x1))
                {
                    continue;
                }

                x0 = ClampInt(x0, 0, view.width);
                x1 = ClampInt(x1, 0, view.width);

                if (x0 >= x1)
                {
                    continue;
                }

                writer.write(
                    view.data
                        + static_cast<size_t>(y) * view.stride
                        + static_cast<size_t>(x0),
                    static_cast<size_t>(x1 - x0)
                );
            }
        }

        void FillEllipseAA(
            ImageView view,
            const Ellipse& ellipse,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color)
        {
            if (!(0.0 < ellipse.a) || !(0.0 < ellipse.b))
            {
                return;
            }

            int32 y0 = FloorToInt(ellipse.y - ellipse.b - 1.0);
            int32 y1 = CeilToInt(ellipse.y + ellipse.b + 1.0);

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

            std::vector<uint8> coverage;

            for (int32 y = y0; y < y1; ++y)
            {
                int32 x0 = FloorToInt(ellipse.x - ellipse.a - 1.0);
                int32 x1 = CeilToInt(ellipse.x + ellipse.a + 1.0);

                x0 = ClampInt(x0, 0, view.width);
                x1 = ClampInt(x1, 0, view.width);

                if (x0 >= x1)
                {
                    continue;
                }

                const size_t count = static_cast<size_t>(x1 - x0);
                coverage.assign(count, 0);

                const double py = static_cast<double>(y) + 0.5;

                for (size_t i = 0; i < count; ++i)
                {
                    const double px = static_cast<double>(x0) + static_cast<double>(i) + 0.5;

                    coverage[i] = EllipseCoverageAt(
                        px,
                        py,
                        ellipse.x,
                        ellipse.y,
                        ellipse.a,
                        ellipse.b
                    );
                }

                Color* row = view.data + static_cast<size_t>(y) * view.stride;

                WriteCoverageRow(writer, row, x0, coverage);
            }
        }

        void FillEllipseFrameByDistance(
            ImageView view,
            const Ellipse& ellipse,
            double innerThickness,
            double outerThickness,
            ImagePixel::BlendMode blendMode,
            ImageDraw::DstAlpha dstAlpha,
            Color color,
            bool antialiased)
        {
            if (!(0.0 <= innerThickness) || !(0.0 <= outerThickness))
            {
                return;
            }

            if (!(0.0 < ellipse.a) || !(0.0 < ellipse.b))
            {
                return;
            }

            if ((innerThickness == 0.0) && (outerThickness == 0.0))
            {
                return;
            }

            const bool hasInner = (innerThickness < Min(ellipse.a, ellipse.b));

            const double support = outerThickness + (antialiased ? 0.5 : 0.0);

            int32 y0 = FloorToInt(ellipse.y - ellipse.b - support - 0.5);
            int32 y1 = CeilToInt(ellipse.y + ellipse.b + support - 0.5) + 1;

            y0 = ClampInt(y0, 0, view.height);
            y1 = ClampInt(y1, 0, view.height);

            if (y0 >= y1)
            {
                return;
            }

            int32 x0 = FloorToInt(ellipse.x - ellipse.a - support - 0.5);
            int32 x1 = CeilToInt(ellipse.x + ellipse.a + support - 0.5) + 1;

            x0 = ClampInt(x0, 0, view.width);
            x1 = ClampInt(x1, 0, view.width);

            if (x0 >= x1)
            {
                return;
            }

            const size_t count = static_cast<size_t>(x1 - x0);

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            std::vector<uint8> coverage(count);

            for (int32 y = y0; y < y1; ++y)
            {
                const double py = static_cast<double>(y) + 0.5;

                bool anyCoverage = false;
                bool allFull = true;

                for (size_t i = 0; i < count; ++i)
                {
                    const double px = static_cast<double>(x0) + static_cast<double>(i) + 0.5;
                    const double sd = EllipseSignedDistance(px, py, ellipse.x, ellipse.y, ellipse.a, ellipse.b);

                    uint8 c = 0;

                    if (antialiased)
                    {
                        const uint8 outerCoverage = CoverageFromSignedDistance(sd - outerThickness);
                        const uint8 innerCoverage = hasInner
                            ? CoverageFromSignedDistance(sd + innerThickness)
                            : 0;

                        c = static_cast<uint8>(
                            (innerCoverage < outerCoverage) ? (outerCoverage - innerCoverage) : 0
                        );
                    }
                    else
                    {
                        c = ((sd <= outerThickness) && (!hasInner || (-innerThickness <= sd))) ? 255 : 0;
                    }

                    coverage[i] = c;

                    anyCoverage |= (c != 0);
                    allFull &= (c == 255);
                }

                if (!anyCoverage)
                {
                    continue;
                }

                Color* row = view.data + static_cast<size_t>(y) * view.stride;

                if (allFull)
                {
                    writer.write(row + static_cast<size_t>(x0), count);
                }
                else
                {
                    writer.writeWithCoverage(row + static_cast<size_t>(x0), coverage.data(), count);
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

        [[nodiscard]]
        bool GetPolygonBounds(
            const PolygonContourView* contours,
            const size_t contourCount,
            BoundsD& bounds) noexcept
        {
            bool found = false;

            for (size_t c = 0; c < contourCount; ++c)
            {
                const PolygonContourView& contour = contours[c];

                if ((contour.points == nullptr) || (contour.count == 0))
                {
                    continue;
                }

                for (size_t i = 0; i < contour.count; ++i)
                {
                    bounds.include(contour.points[i]);
                    found = true;
                }
            }

            return found && bounds.valid();
        }

        void CollectPolygonEdges(
            const Vec2* points,
            const size_t count,
            std::vector<PolygonEdge>& edges)
        {
            if ((points == nullptr) || (count < 3))
            {
                return;
            }

            for (size_t i = 0; i < count; ++i)
            {
                const Vec2& p0 = points[i];
                const Vec2& p1 = points[(i + 1) % count];

                if (p0.y == p1.y)
                {
                    continue;
                }

                Vec2 a = p0;
                Vec2 b = p1;

                if (b.y < a.y)
                {
                    std::swap(a, b);
                }

                // Pixel center y = y + 0.5 が [a.y, b.y) に入る y を対象にします。
                const int32 y0 = CeilToInt(a.y - 0.5);
                const int32 y1 = CeilToInt(b.y - 0.5);

                if (y0 >= y1)
                {
                    continue;
                }

                const double slope = (b.x - a.x) / (b.y - a.y);
                const double firstScanY = static_cast<double>(y0) + 0.5;
                const double x = a.x + (firstScanY - a.y) * slope;

                edges.push_back(PolygonEdge{
                    y0,
                    y1,
                    x,
                    slope
                });
            }
        }

        void CollectPolygonEdges(
            const PolygonContourView* contours,
            const size_t contourCount,
            std::vector<PolygonEdge>& edges)
        {
            if (contours == nullptr)
            {
                return;
            }

            for (size_t c = 0; c < contourCount; ++c)
            {
                CollectPolygonEdges(contours[c].points, contours[c].count, edges);
            }
        }

        void FillPolygonNoAA(
            const ImageView view,
            const PolygonContourView* contours,
            const size_t contourCount,
            const ImagePixel::detail::SolidColorWriter& writer)
        {
            if ((view.data == nullptr) || (contours == nullptr) || (contourCount == 0))
            {
                return;
            }

            std::vector<PolygonEdge> edges;
            edges.reserve(64);

            CollectPolygonEdges(contours, contourCount, edges);

            if (edges.empty())
            {
                return;
            }

            std::sort(edges.begin(), edges.end(), [](const PolygonEdge& a, const PolygonEdge& b)
                {
                    return (a.y0 < b.y0);
                });

            int32 y0 = edges.front().y0;
            int32 y1 = edges.front().y1;

            for (const PolygonEdge& e : edges)
            {
                y0 = Min(y0, e.y0);
                y1 = Max(y1, e.y1);
            }

            y0 = ClampInt(y0, 0, view.height);
            y1 = ClampInt(y1, 0, view.height);

            if (y0 >= y1)
            {
                return;
            }

            std::vector<PolygonEdge> active;
            active.reserve(16);

            size_t nextEdge = 0;

            // 画面上端より前から始まっている edge を y0 位置まで進めます。
            while ((nextEdge < edges.size()) && (edges[nextEdge].y0 < y0))
            {
                PolygonEdge e = edges[nextEdge++];

                if (y0 < e.y1)
                {
                    e.x += static_cast<double>(y0 - e.y0) * e.dx;
                    e.y0 = y0;
                    active.push_back(e);
                }
            }

            for (int32 y = y0; y < y1; ++y)
            {
                while ((nextEdge < edges.size()) && (edges[nextEdge].y0 == y))
                {
                    active.push_back(edges[nextEdge++]);
                }

                size_t writeIndex = 0;

                for (size_t readIndex = 0; readIndex < active.size(); ++readIndex)
                {
                    if (y < active[readIndex].y1)
                    {
                        active[writeIndex++] = active[readIndex];
                    }
                }

                active.resize(writeIndex);

                if (active.size() >= 2)
                {
                    std::sort(active.begin(), active.end(), [](const PolygonEdge& a, const PolygonEdge& b)
                        {
                            return (a.x < b.x);
                        });

                    Color* row = view.data + static_cast<size_t>(y) * view.stride;

                    // even-odd rule: 交点を 2 個ずつ組にして span を塗ります。
                    for (size_t i = 0; i + 1 < active.size(); i += 2)
                    {
                        int32 x0 = CeilToInt(active[i].x - 0.5);
                        int32 x1 = CeilToInt(active[i + 1].x - 0.5);

                        if ((x1 <= 0) || (view.width <= x0))
                        {
                            continue;
                        }

                        x0 = ClampInt(x0, 0, view.width);
                        x1 = ClampInt(x1, 0, view.width);

                        if (x0 < x1)
                        {
                            writer.write(
                                row + static_cast<size_t>(x0),
                                static_cast<size_t>(x1 - x0)
                            );
                        }
                    }
                }

                for (PolygonEdge& e : active)
                {
                    e.x += e.dx;
                }
            }
        }

        [[nodiscard]]
        bool IsPointInPolygonEvenOdd(
            const Vec2& p,
            const PolygonContourView* contours,
            const size_t contourCount) noexcept
        {
            bool inside = false;

            if (contours == nullptr)
            {
                return false;
            }

            for (size_t c = 0; c < contourCount; ++c)
            {
                const PolygonContourView& contour = contours[c];

                if ((contour.points == nullptr) || (contour.count < 3))
                {
                    continue;
                }

                for (size_t i = 0, j = contour.count - 1; i < contour.count; j = i++)
                {
                    const Vec2& pi = contour.points[i];
                    const Vec2& pj = contour.points[j];

                    const bool intersect =
                        ((pi.y > p.y) != (pj.y > p.y))
                        && (p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x);

                    if (intersect)
                    {
                        inside = !inside;
                    }
                }
            }

            return inside;
        }

        [[nodiscard]]
        uint8 PolygonCoverage4x4(
            const int32 x,
            const int32 y,
            const PolygonContourView* contours,
            const size_t contourCount) noexcept
        {
            int32 covered = 0;

            // Pixel-local sample positions: 1/8, 3/8, 5/8, 7/8
            for (int32 sy = 0; sy < 4; ++sy)
            {
                const double py = static_cast<double>(y) + (static_cast<double>(sy) * 2.0 + 1.0) * 0.125;

                for (int32 sx = 0; sx < 4; ++sx)
                {
                    const double px = static_cast<double>(x) + (static_cast<double>(sx) * 2.0 + 1.0) * 0.125;

                    if (IsPointInPolygonEvenOdd(Vec2{ px, py }, contours, contourCount))
                    {
                        ++covered;
                    }
                }
            }

            return static_cast<uint8>((covered * 255 + 8) / 16);
        }

        void FillPolygonAA4x4InBounds(
            const ImageView view,
            const PolygonContourView* contours,
            const size_t contourCount,
            const RectF& boundingRect,
            const ImagePixel::detail::SolidColorWriter& writer)
        {
            if ((view.data == nullptr) || (contours == nullptr) || (contourCount == 0))
            {
                return;
            }

            if (!(0.0 < boundingRect.w) || !(0.0 < boundingRect.h))
            {
                return;
            }

            int32 x0 = FloorToInt(boundingRect.x);
            int32 y0 = FloorToInt(boundingRect.y);
            int32 x1 = CeilToInt(boundingRect.x + boundingRect.w);
            int32 y1 = CeilToInt(boundingRect.y + boundingRect.h);

            // 4x4 supersampling のサポート領域として 1 pixel だけ広げます。
            --x0;
            --y0;
            ++x1;
            ++y1;

            if (!ClipRectI(x0, y0, x1, y1, view.width, view.height))
            {
                return;
            }

            const size_t rowLength = static_cast<size_t>(x1 - x0);

            std::vector<uint8> coverage(rowLength);

            for (int32 y = y0; y < y1; ++y)
            {
                bool anyCoverage = false;
                bool allFull = true;

                for (size_t i = 0; i < rowLength; ++i)
                {
                    const int32 x = x0 + static_cast<int32>(i);
                    const uint8 c = PolygonCoverage4x4(x, y, contours, contourCount);

                    coverage[i] = c;

                    anyCoverage |= (c != 0);
                    allFull &= (c == 255);
                }

                if (!anyCoverage)
                {
                    continue;
                }

                Color* row = view.data
                    + static_cast<size_t>(y) * view.stride
                    + static_cast<size_t>(x0);

                if (allFull)
                {
                    writer.write(row, rowLength);
                }
                else
                {
                    writer.writeWithCoverage(row, coverage.data(), rowLength);
                }
            }
        }

        void FillPolygonAA4x4(
            const ImageView view,
            const PolygonContourView* contours,
            const size_t contourCount,
            const ImagePixel::detail::SolidColorWriter& writer)
        {
            BoundsD bounds;

            if (!GetPolygonBounds(contours, contourCount, bounds))
            {
                return;
            }

            FillPolygonAA4x4InBounds(
                view,
                contours,
                contourCount,
                RectF{
                    bounds.minX,
                    bounds.minY,
                    bounds.maxX - bounds.minX,
                    bounds.maxY - bounds.minY
                },
                writer
            );
        }

        void FillPolygonInternal(
            const ImageView view,
            const PolygonContourView* contours,
            const size_t contourCount,
            const RectF* boundingRect,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const ImageDraw::DstAlpha dstAlpha,
            const Color color)
        {
            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            if ((contours == nullptr) || (contourCount == 0))
            {
                return;
            }

            ImagePixel::detail::SolidColorWriter writer{
                blendMode,
                color,
                ToDstAlphaMode(dstAlpha)
            };

            if (enableAntialiasing == EnableAntialiasing::Yes)
            {
                if (boundingRect)
                {
                    FillPolygonAA4x4InBounds(view, contours, contourCount, *boundingRect, writer);
                }
                else
                {
                    FillPolygonAA4x4(view, contours, contourCount, writer);
                }
            }
            else
            {
                FillPolygonNoAA(view, contours, contourCount, writer);
            }
        }

        void FillPolygonInternal(
            const ImageView view,
            const PolygonContourView* contours,
            const size_t contourCount,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const ImageDraw::DstAlpha dstAlpha,
            const Color color)
        {
            FillPolygonInternal(
                view,
                contours,
                contourCount,
                nullptr,
                blendMode,
                enableAntialiasing,
                dstAlpha,
                color
            );
        }

        [[nodiscard]]
        std::vector<PolygonContourView> MakePolygonContourViews(const Polygon& polygon)
        {
            std::vector<PolygonContourView> contours;

            const Array<Vec2>& outer = polygon.outer();

            if (outer.size() >= 3)
            {
                contours.push_back(PolygonContourView{
                    outer.data(),
                    outer.size()
                });
            }

            for (const Array<Vec2>& inner : polygon.inners())
            {
                if (inner.size() >= 3)
                {
                    contours.push_back(PolygonContourView{
                        inner.data(),
                        inner.size()
                    });
                }
            }

            return contours;
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
                const uint8 c = CircleCoverageAt(px, py, center.x, center.y, radius);

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
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0) || !(0.0 < circle.r))
            {
                return;
            }

            if (enableAntialiasing == EnableAntialiasing::Yes)
            {
                FillCircleAA(view, circle, blendMode, dstAlpha, color);
            }
            else
            {
                FillCircleNoAAFast(view, circle, blendMode, dstAlpha, color);
            }
        }

        void CircleFrame(
            Image& image,
            const Circle& circle,
            const double innerThickness,
            const double outerThickness,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            if (enableAntialiasing == EnableAntialiasing::Yes)
            {
                FillCircleFrameAA(view, circle, innerThickness, outerThickness, blendMode, dstAlpha, color);
            }
            else
            {
                FillCircleFrameNoAA(view, circle, innerThickness, outerThickness, blendMode, dstAlpha, color);
            }
        }

        void Fill(
            Image& image,
            const Ellipse& ellipse,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            if (enableAntialiasing == EnableAntialiasing::Yes)
            {
                FillEllipseAA(view, ellipse, blendMode, dstAlpha, color);
            }
            else
            {
                FillEllipseNoAA(view, ellipse, blendMode, dstAlpha, color);
            }
        }

        void EllipseFrame(
            Image& image,
            const Ellipse& ellipse,
            const double innerThickness,
            const double outerThickness,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            FillEllipseFrameByDistance(
                view,
                ellipse,
                innerThickness,
                outerThickness,
                blendMode,
                dstAlpha,
                color,
                (enableAntialiasing == EnableAntialiasing::Yes)
            );
        }


        void Fill(
            Image& image,
            const Triangle& triangle,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            const Vec2 points[3] =
            {
                triangle.p0,
                triangle.p1,
                triangle.p2,
            };

            const PolygonContourView contour{
                points,
                3
            };

            if (enableAntialiasing == EnableAntialiasing::No)
            {
                ImagePixel::detail::SolidColorWriter writer{
                    blendMode,
                    color,
                    ToDstAlphaMode(dstAlpha)
                };

                FillConvexPolygonNoAA(view, points, 3, writer);
                return;
            }

            FillPolygonInternal(
                view,
                &contour,
                1,
                blendMode,
                enableAntialiasing,
                dstAlpha,
                color
            );
        }

        void Fill(
            Image& image,
            const Quad& quad,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            const Vec2 points[4] =
            {
                quad.p0,
                quad.p1,
                quad.p2,
                quad.p3,
            };

            const PolygonContourView contour{
                points,
                4
            };

            if (enableAntialiasing == EnableAntialiasing::No)
            {
                ImagePixel::detail::SolidColorWriter writer{
                    blendMode,
                    color,
                    ToDstAlphaMode(dstAlpha)
                };

                FillConvexPolygonNoAA(view, points, 4, writer);
                return;
            }

            FillPolygonInternal(
                view,
                &contour,
                1,
                blendMode,
                enableAntialiasing,
                dstAlpha,
                color
            );
        }

        void FillPolygon(
            Image& image,
            const Vec2* points,
            const size_t count,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            if ((points == nullptr) || (count < 3))
            {
                return;
            }

            const ImageView view = MakeImageView(image);

            const PolygonContourView contour{
                points,
                count
            };

            FillPolygonInternal(
                view,
                &contour,
                1,
                blendMode,
                enableAntialiasing,
                dstAlpha,
                color
            );
        }

        void Fill(
            Image& image,
            const Polygon& polygon,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const DstAlpha dstAlpha)
        {
            if (polygon.isEmpty())
            {
                return;
            }

            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0))
            {
                return;
            }

            std::vector<PolygonContourView> contours = MakePolygonContourViews(polygon);

            if (contours.empty())
            {
                return;
            }

            const RectF boundingRect = polygon.boundingRect();

            FillPolygonInternal(
                view,
                contours.data(),
                contours.size(),
                &boundingRect,
                blendMode,
                enableAntialiasing,
                dstAlpha,
                color
            );
        }

        void Line(
            Image& image,
            const Vec2& from,
            const Vec2& to,
            const double thickness,
            const Color color,
            const ImagePixel::BlendMode blendMode,
            const EnableAntialiasing enableAntialiasing,
            const LineCap lineCap,
            const DstAlpha dstAlpha)
        {
            const ImageView view = MakeImageView(image);

            if ((view.data == nullptr) || (view.width <= 0) || (view.height <= 0) || !(0.0 < thickness))
            {
                return;
            }

            if (enableAntialiasing == EnableAntialiasing::No)
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
