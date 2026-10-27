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

# include <array>
# include <limits>
# include <type_traits>
# include "Vertex2DBuilder.hpp"
# include "SuperEllipseTessellation.hpp"
# include "RoundRectBalloon.hpp"
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/JoinStyle.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/QuarterArcTable.hpp>

namespace s3d
{
	namespace
	{
		static constexpr int32 MaxCircleQuarterSegments = 63;

		static constexpr uint32 MaxCircleSegments = (MaxCircleQuarterSegments * 4);

		static_assert(MaxCircleQuarterSegments <= QuarterArcTable::MaxQuality);

		static constexpr Vertex2D::IndexType RectIndexTable[6] = { 0, 1, 2, 2, 1, 3 };

		// Keep index emission in the builder without an extra call per shape.
		SIV3D_ALWAYS_INLINE
		static inline void WriteOpenFanIndices(Index2DWritePointer& pIndex, const Vertex2D::IndexType indexOffset, const Vertex2D::IndexType rimVertexCount)
		{
			for (Vertex2D::IndexType i = 0; i < (rimVertexCount - 1); ++i)
			{
				*pIndex++ = indexOffset;
				*pIndex++ = indexOffset + (i + 1);
				*pIndex++ = indexOffset + (i + 2);
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteClosedFanIndices(Index2DWritePointer pIndex, const Vertex2D::IndexType indexOffset, const Vertex2D::IndexType rimVertexCount)
		{
			WriteOpenFanIndices(pIndex, indexOffset, rimVertexCount);

			*pIndex++ = indexOffset;
			*pIndex++ = (indexOffset + rimVertexCount);
			*pIndex++ = (indexOffset + 1);
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteRoundRectIndices(Index2DWritePointer pIndex, Vertex2D::IndexType indexOffset, const Vertex2D::IndexType quality)
		{
			const Vertex2D::IndexType FanOuterVertexCount = (quality + 1);
			const Vertex2D::IndexType trFanStartIndex	= indexOffset;
			const Vertex2D::IndexType trFanEndIndex		= (trFanStartIndex + quality);
			const Vertex2D::IndexType brFanStartIndex	= (trFanEndIndex + 1);
			const Vertex2D::IndexType brFanEndIndex		= (brFanStartIndex + quality);
			const Vertex2D::IndexType blFanStartIndex	= (brFanEndIndex + 1);
			const Vertex2D::IndexType blFanEndIndex		= (blFanStartIndex + quality);
			const Vertex2D::IndexType tlFanStartIndex	= (blFanEndIndex + 1);
			const Vertex2D::IndexType tlFanEndIndex		= (tlFanStartIndex + quality);

			const Vertex2D::IndexType trCenterIndex = (indexOffset + FanOuterVertexCount * 4);
			const Vertex2D::IndexType brCenterIndex = (trCenterIndex + 1);
			const Vertex2D::IndexType blCenterIndex = (trCenterIndex + 2);
			const Vertex2D::IndexType tlCenterIndex = (trCenterIndex + 3);

			{
				// tr
				for (Vertex2D::IndexType i = 0; i < quality; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = (indexOffset + i + 1);
					*pIndex++ = trCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// br
				for (Vertex2D::IndexType i = 0; i < quality; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = brCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// bl
				for (Vertex2D::IndexType i = 0; i < quality; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = blCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// tl
				for (Vertex2D::IndexType i = 0; i < quality; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = tlCenterIndex;
				}

				// rectangles
				{
					// topRect
					*pIndex++ = tlFanEndIndex;
					*pIndex++ = trFanStartIndex;
					*pIndex++ = tlCenterIndex;

					*pIndex++ = tlCenterIndex;
					*pIndex++ = trFanStartIndex;
					*pIndex++ = trCenterIndex;

					// middleRect
					*pIndex++ = tlFanStartIndex;
					*pIndex++ = trFanEndIndex;
					*pIndex++ = blFanEndIndex;

					*pIndex++ = blFanEndIndex;
					*pIndex++ = trFanEndIndex;
					*pIndex++ = brFanStartIndex;

					// bottomRect
					*pIndex++ = blCenterIndex;
					*pIndex++ = brCenterIndex;
					*pIndex++ = blFanStartIndex;

					*pIndex++ = blFanStartIndex;
					*pIndex++ = brCenterIndex;
					*pIndex++ = brFanEndIndex;
				}
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteOpenStripIndices(Index2DWritePointer& pIndex, const Vertex2D::IndexType indexOffset, const Vertex2D::IndexType pairCount)
		{
			for (Vertex2D::IndexType i = 0; i < (pairCount - 1); ++i)
			{
				const Vertex2D::IndexType base = (indexOffset + i * 2);
				*pIndex++ = base;
				*pIndex++ = (base + 1);
				*pIndex++ = (base + 2);
				*pIndex++ = (base + 2);
				*pIndex++ = (base + 1);
				*pIndex++ = (base + 3);
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteClosedStripIndices(Index2DWritePointer& pIndex, const Vertex2D::IndexType indexOffset, const Vertex2D::IndexType pairCount)
		{
			Vertex2D::IndexType base = indexOffset;
			for (Vertex2D::IndexType i = 0; i < (pairCount - 1); ++i)
			{
				*pIndex++ = base;
				*pIndex++ = (base + 2);
				*pIndex++ = (base + 1);
				*pIndex++ = (base + 1);
				*pIndex++ = (base + 2);
				*pIndex++ = (base + 3);
				base += 2;
			}

			// Only the final quad wraps; preserve the ring's winding and order.
			*pIndex++ = base;
			*pIndex++ = indexOffset;
			*pIndex++ = (base + 1);
			*pIndex++ = (base + 1);
			*pIndex++ = indexOffset;
			*pIndex++ = (indexOffset + 1);
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteRoundRectFrameIndices(Index2DWritePointer pIndex, const Vertex2D::IndexType indexOffset, const Vertex2D::IndexType quality)
		{
			const Vertex2D::IndexType ArcVertexCount = ((quality + 1) * 2);
			const Vertex2D::IndexType ArcIndexCount = (quality * 6);

			Index2DWritePointer pDst0 = pIndex;
			Index2DWritePointer pDst1 = (pIndex + ArcIndexCount * 1 + 6);
			Index2DWritePointer pDst2 = (pIndex + ArcIndexCount * 2 + 12);
			Index2DWritePointer pDst3 = (pIndex + ArcIndexCount * 3 + 18);

			const Vertex2D::IndexType trIndexBase = indexOffset;
			const Vertex2D::IndexType brIndexBase = (indexOffset + ArcVertexCount * 1);
			const Vertex2D::IndexType blIndexBase = (indexOffset + ArcVertexCount * 2);
			const Vertex2D::IndexType tlIndexBase = (indexOffset + ArcVertexCount * 3);

			for (Vertex2D::IndexType i = 0; i <= quality; ++i)
			{
				const Vertex2D::IndexType i0 = (i * 2);
				const Vertex2D::IndexType i1 = (i * 2 + 1);
				const Vertex2D::IndexType i2 = (i * 2 + 2);
				const Vertex2D::IndexType i3 = (i * 2 + 3);

				*pDst0++ = (trIndexBase + i0);
				*pDst0++ = (trIndexBase + i2);
				*pDst0++ = (trIndexBase + i1);
				*pDst0++ = (trIndexBase + i1);
				*pDst0++ = (trIndexBase + i2);
				*pDst0++ = (trIndexBase + i3);

				*pDst1++ = (brIndexBase + i0);
				*pDst1++ = (brIndexBase + i2);
				*pDst1++ = (brIndexBase + i1);
				*pDst1++ = (brIndexBase + i1);
				*pDst1++ = (brIndexBase + i2);
				*pDst1++ = (brIndexBase + i3);

				*pDst2++ = (blIndexBase + i0);
				*pDst2++ = (blIndexBase + i2);
				*pDst2++ = (blIndexBase + i1);
				*pDst2++ = (blIndexBase + i1);
				*pDst2++ = (blIndexBase + i2);
				*pDst2++ = (blIndexBase + i3);

				if (i < quality)
				{
					*pDst3++ = (tlIndexBase + i0);
					*pDst3++ = (tlIndexBase + i2);
					*pDst3++ = (tlIndexBase + i1);
					*pDst3++ = (tlIndexBase + i1);
					*pDst3++ = (tlIndexBase + i2);
					*pDst3++ = (tlIndexBase + i3);
				}
				else
				{
					*pDst3++ = (tlIndexBase + i0);
					*pDst3++ = trIndexBase;
					*pDst3++ = (tlIndexBase + i1);
					*pDst3++ = (tlIndexBase + i1);
					*pDst3++ = trIndexBase;
					*pDst3++ = (trIndexBase + 1);
				}
			}
		}

		enum class EllipseFanType
		{
			Circle,
			Ellipse,
			RotatedEllipse,
			TexturedCircle,
			TexturedCircleGradient,
		};

		template <EllipseFanType Type>
		SIV3D_ALWAYS_INLINE
		static inline void WriteEllipseFanVertices(Vertex2DWritePointer const& pVertex, const float centerX, const float centerY, const float a, const float b,
			const Vertex2D::IndexType quality, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const FloatRect& uv = {}, const float rotationCos = 1.0f, const float rotationSin = 0.0f)
		{
			constexpr bool Circular = ((Type == EllipseFanType::Circle) || (Type == EllipseFanType::TexturedCircle) || (Type == EllipseFanType::TexturedCircleGradient));
			constexpr bool Textured = ((Type == EllipseFanType::TexturedCircle) || (Type == EllipseFanType::TexturedCircleGradient));
			constexpr bool Gradient = (Type != EllipseFanType::TexturedCircle);
			constexpr bool Rotated = (Type == EllipseFanType::RotatedEllipse);

			Float4 colorDiff;
			float centerU, centerV, rU, rV;
			if constexpr (Textured)
			{
				centerU = ((uv.left + uv.right) * 0.5f);
				centerV = ((uv.top + uv.bottom) * 0.5f);
				rU = ((uv.right - uv.left) * 0.5f);
				rV = ((uv.bottom - uv.top) * 0.5f);
				pVertex[0].set(centerX, centerY, centerU, centerV, color0);
			}
			else
			{
				pVertex[0].setPosition(centerX, centerY);
			}

			if constexpr (Gradient)
			{
				colorDiff = (color1 - color0);
			}

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(quality);
			Vertex2DWritePointer pDst0 = (pVertex + 1);
			Vertex2DWritePointer pDst1 = (pDst0 + quality);
			Vertex2DWritePointer pDst2 = (pDst1 + quality);
			Vertex2DWritePointer pDst3 = (pDst2 + quality);

			for (Vertex2D::IndexType i = 0; i < quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float xa = (cs.x * a);
				const float xb = (Circular ? xa : (cs.x * b));
				const float ya = (cs.y * a);
				const float yb = (Circular ? ya : (cs.y * b));

				const auto writeVertex = [&](Vertex2DWriteReference vertex, const float x, const float y, const float ux, const float uy, const Float4& color)
				{
					if constexpr (Rotated)
					{
						const float ox = (a * ux), oy = (b * uy);
						vertex.set((centerX + rotationCos * ox - rotationSin * oy), (centerY + rotationSin * ox + rotationCos * oy), color);
					}
					else if constexpr (Textured)
					{
						vertex.set(x, y, (centerU + rU * ux), (centerV + rV * uy), color);
					}
					else
					{
						vertex.set(x, y, color);
					}
				};

				if constexpr (Textured && (not Gradient))
				{
					pDst0->set((centerX + xa), (centerY + yb), (centerU + rU * cs.x), (centerV + rV * cs.y), color0);
					pDst1->set((centerX - ya), (centerY + xb), (centerU - rU * cs.y), (centerV + rV * cs.x), color0);
					pDst2->set((centerX - xa), (centerY - yb), (centerU - rU * cs.x), (centerV - rV * cs.y), color0);
					pDst3->set((centerX + ya), (centerY - xb), (centerU + rU * cs.y), (centerV - rV * cs.x), color0);
				}
				else
				{
					if (colorType == ColorFillDirection::InOut)
					{
						writeVertex(*pDst0, (centerX + xa), (centerY + yb), cs.x, cs.y, color1);
						writeVertex(*pDst1, (centerX - ya), (centerY + xb), -cs.y, cs.x, color1);
						writeVertex(*pDst2, (centerX - xa), (centerY - yb), -cs.x, -cs.y, color1);
						writeVertex(*pDst3, (centerX + ya), (centerY - xb), cs.y, -cs.x, color1);
					}
					else if (colorType == ColorFillDirection::TopBottom)
					{
						const Float4 c0 = (color0 + ((cs.y + 1.0f) * 0.5f) * colorDiff);
						const Float4 c1 = (color0 + ((cs.x + 1.0f) * 0.5f) * colorDiff);
						const Float4 c2 = (color0 + ((-cs.y + 1.0f) * 0.5f) * colorDiff);
						const Float4 c3 = (color0 + ((-cs.x + 1.0f) * 0.5f) * colorDiff);

						writeVertex(*pDst0, (centerX + xa), (centerY + yb), cs.x, cs.y, c0);
						writeVertex(*pDst1, (centerX - ya), (centerY + xb), -cs.y, cs.x, c1);
						writeVertex(*pDst2, (centerX - xa), (centerY - yb), -cs.x, -cs.y, c2);
						writeVertex(*pDst3, (centerX + ya), (centerY - xb), cs.y, -cs.x, c3);
					}
					else
					{
						const Float4 c0 = (color0 + ((cs.x + 1.0f) * 0.5f) * colorDiff);
						const Float4 c1 = (color0 + ((-cs.y + 1.0f) * 0.5f) * colorDiff);
						const Float4 c2 = (color0 + ((-cs.x + 1.0f) * 0.5f) * colorDiff);
						const Float4 c3 = (color0 + ((cs.y + 1.0f) * 0.5f) * colorDiff);

						writeVertex(*pDst0, (centerX + xa), (centerY + yb), cs.x, cs.y, c0);
						writeVertex(*pDst1, (centerX - ya), (centerY + xb), -cs.y, cs.x, c1);
						writeVertex(*pDst2, (centerX - xa), (centerY - yb), -cs.x, -cs.y, c2);
						writeVertex(*pDst3, (centerX + ya), (centerY - xb), cs.y, -cs.x, c3);
					}
				}

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			if constexpr (Gradient)
			{
				// 中心の色
				if (colorType == ColorFillDirection::InOut)
				{
					pVertex->setColor(color0);
				}
				else
				{
					pVertex->setColor(color0.getMidpoint(color1));
				}
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteCircleShadowRingVertices(Vertex2DWritePointer pDst0, const std::span<const Float2> pCS,
			const float centerX, const float centerY, const float rInner, const float rOuter, const Vertex2D::IndexType quality, const Float4& color)
		{
			Vertex2DWritePointer pDst1 = (pDst0 + quality * 2);
			Vertex2DWritePointer pDst2 = (pDst0 + quality * 4);
			Vertex2DWritePointer pDst3 = (pDst0 + quality * 6);

			for (Vertex2D::IndexType i = 0; i < quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float ox = (cs.x * rOuter);
				const float ix = (cs.x * rInner);
				const float oy = (cs.y * rOuter);
				const float iy = (cs.y * rInner);

				(pDst0++)->set((centerX + ox), (centerY + oy), 0.0f, 0.5f, color);
				(pDst0++)->set((centerX + ix), (centerY + iy), 0.5f, 0.5f, color);

				(pDst1++)->set((centerX - oy), (centerY + ox), 0.0f, 0.5f, color);
				(pDst1++)->set((centerX - iy), (centerY + ix), 0.5f, 0.5f, color);

				(pDst2++)->set((centerX - ox), (centerY - oy), 0.0f, 0.5f, color);
				(pDst2++)->set((centerX - ix), (centerY - iy), 0.5f, 0.5f, color);

				(pDst3++)->set((centerX + oy), (centerY - ox), 0.0f, 0.5f, color);
				(pDst3++)->set((centerX + iy), (centerY - ix), 0.5f, 0.5f, color);
			}
		}

		template <bool Gradient, class VertexWriter>
		SIV3D_ALWAYS_INLINE
		static inline void WriteRoundRectVertices(Vertex2DWritePointer const& pVertex, const FloatRect& rect, const float r, const Vertex2D::IndexType quality, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const VertexWriter& writeVertex)
		{
			using CornerColor = std::conditional_t<Gradient, Float4, const Float4&>;

			const Vertex2D::IndexType FanOuterVertexCount = (quality + 1);

			const float middleLeftX		= (rect.left + r);
			const float middleRightX	= (rect.right - r);
			const float topCenterY		= (rect.top + r);
			const float bottomCenterY	= (rect.bottom - r);

			Float4 colorDiff;
			float colorRScale;
			float colorROffset;
			if constexpr (Gradient)
			{
				colorDiff = (color1 - color0);
				colorRScale = (colorType == ColorFillDirection::TopBottom) ? (r / (rect.bottom - rect.top)) : (r / (rect.right - rect.left));
				colorROffset = (1.0f - colorRScale);
			}

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(quality);

			Vertex2DWritePointer pDst0 = pVertex;
			Vertex2DWritePointer pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2DWritePointer pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2DWritePointer pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

				CornerColor c0 = color0;
				CornerColor c1 = color0;
				CornerColor c2 = color0;
				CornerColor c3 = color0;
				if constexpr (Gradient)
				{
					if (colorType == ColorFillDirection::TopBottom)
					{
						c0 = (color0 + ((cs.y + 1.0f) * colorRScale) * colorDiff);
						c1 = (color0 + (colorROffset + (cs.x * colorRScale)) * colorDiff);
						c2 = (color0 + (colorROffset + (-cs.y * colorRScale)) * colorDiff);
						c3 = (color0 + ((-cs.x + 1.0f) * colorRScale) * colorDiff);
					}
					else // ColorFillDirection::LeftRight
					{
						c0 = (color0 + (colorROffset + (cs.x * colorRScale)) * colorDiff);
						c1 = (color0 + (colorROffset + (-cs.y * colorRScale)) * colorDiff);
						c2 = (color0 + ((-cs.x + 1.0f) * colorRScale) * colorDiff);
						c3 = (color0 + ((cs.y + 1.0f) * colorRScale) * colorDiff);
					}
				}

				writeVertex(*pDst0, (middleRightX + x), (topCenterY + y), c0);
				writeVertex(*pDst1, (middleRightX - y), (bottomCenterY + x), c1);
				writeVertex(*pDst2, (middleLeftX - x), (bottomCenterY - y), c2);
				writeVertex(*pDst3, (middleLeftX + y), (topCenterY - x), c3);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の最後の頂点
			{
				const Float2& cs = pCS[0];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

				CornerColor c0 = color0;
				CornerColor c1 = color0;
				CornerColor c2 = color0;
				CornerColor c3 = color0;
				CornerColor centerColor0 = color0;
				CornerColor centerColor1 = color0;
				CornerColor centerColor2 = color0;
				CornerColor centerColor3 = color0;
				if constexpr (Gradient)
				{
					if (colorType == ColorFillDirection::TopBottom)
					{
						c0 = (color0 + colorRScale * colorDiff);
						c1 = color1;
						c2 = (color0 + (1.0 - colorRScale) * colorDiff);
						c3 = color0;
						centerColor0 = c0;
						centerColor1 = c2;
						centerColor2 = c2;
						centerColor3 = c0;
					}
					else // ColorFillDirection::LeftRight
					{
						c0 = color1;
						c1 = (color0 + (1.0 - colorRScale) * colorDiff);
						c2 = color0;
						c3 = (color0 + colorRScale * colorDiff);
						centerColor0 = c1;
						centerColor1 = c1;
						centerColor2 = c3;
						centerColor3 = c3;
					}
				}

				writeVertex(*pDst0, (middleRightX - y), (topCenterY + x), c0);
				writeVertex(*pDst1, (middleRightX - x), (bottomCenterY - y), c1);
				writeVertex(*pDst2, (middleLeftX + y), (bottomCenterY - x), c2);
				writeVertex(*pDst3, (middleLeftX + x), (topCenterY + y), c3);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;

				// Fan の中心の頂点
				writeVertex(*pDst3++, middleRightX, topCenterY, centerColor0);
				writeVertex(*pDst3++, middleRightX, bottomCenterY, centerColor1);
				writeVertex(*pDst3++, middleLeftX, bottomCenterY, centerColor2);
				writeVertex(*pDst3++, middleLeftX, topCenterY, centerColor3);
			}
		}

		template <bool Gradient>
		SIV3D_ALWAYS_INLINE
		static inline void WriteRoundRectFrameVertices(Vertex2DWritePointer const& pVertex, const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Vertex2D::IndexType quality, const ColorFillDirection colorType, const Float4& color0, const Float4& color1)
		{
			using CornerColor = std::conditional_t<Gradient, Float4, const Float4&>;

			const Vertex2D::IndexType ArcVertexCount = ((quality + 1) * 2);

			const float innerLeftCenterX = (innerRect.left + innerR);
			const float innerRightCenterX = (innerRect.right - innerR);
			const float innerTopCenterY = (innerRect.top + innerR);
			const float innerBottomCenterY = (innerRect.bottom - innerR);

			const float outerLeftCenterX = (outerRect.left + outerR);
			const float outerRightCenterX = (outerRect.right - outerR);
			const float outerTopCenterY = (outerRect.top + outerR);
			const float outerBottomCenterY = (outerRect.bottom - outerR);

			Float4 colorDiff;
			float ot1;
			float ot2;
			float it1;
			float it2;
			float it3;
			float it4;
			float irt;
			if constexpr (Gradient)
			{
				colorDiff = (color1 - color0);
				const float rectSize = ((colorType == ColorFillDirection::TopBottom) ? (outerRect.bottom - outerRect.top) : (outerRect.right - outerRect.left));
				const float thickness = (((outerRect.right - outerRect.left) - (innerRect.right - innerRect.left)) / 2.0f);
				ot1 = (outerR / rectSize);
				ot2 = (1.0f - ot1);
				it1 = (thickness / rectSize);
				it2 = ((thickness + innerR) / rectSize);
				it3 = (1.0f - it2);
				it4 = (1.0f - it1);
				irt = (it2 - it1);
			}

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(quality);

			Vertex2DWritePointer pDst0 = pVertex;
			Vertex2DWritePointer pDst1 = (pVertex + ArcVertexCount * 1);
			Vertex2DWritePointer pDst2 = (pVertex + ArcVertexCount * 2);
			Vertex2DWritePointer pDst3 = (pVertex + ArcVertexCount * 3);

			for (Vertex2D::IndexType i = 0; i < quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float ox = (cs.x * outerR);
				const float oy = (cs.y * outerR);
				const float ix = (cs.x * innerR);
				const float iy = (cs.y * innerR);

				CornerColor c0 = color0;
				CornerColor c1 = color0;
				CornerColor c2 = color0;
				CornerColor c3 = color0;
				CornerColor innerColor0 = color0;
				CornerColor innerColor1 = color0;
				CornerColor innerColor2 = color0;
				CornerColor innerColor3 = color0;
				if constexpr (Gradient)
				{
					if (colorType == ColorFillDirection::TopBottom)
					{
						c0 = (color0 + ((cs.y + 1.0f) * ot1) * colorDiff);
						c1 = (color0 + (ot2 + (cs.x * ot1)) * colorDiff);
						c2 = (color0 + (ot2 + (-cs.y * ot1)) * colorDiff);
						c3 = (color0 + ((-cs.x + 1.0f) * ot1) * colorDiff);
						innerColor0 = (color0 + (it1 + (cs.y + 1.0f) * irt) * colorDiff);
						innerColor1 = (color0 + (it3 + (cs.x * irt)) * colorDiff);
						innerColor2 = (color0 + (it3 + (-cs.y * irt)) * colorDiff);
						innerColor3 = (color0 + (it1 + (-cs.x + 1.0f) * irt) * colorDiff);
					}
					else // ColorFillDirection::LeftRight
					{
						c0 = (color0 + (ot2 + (cs.x * ot1)) * colorDiff);
						c1 = (color0 + (ot2 + (-cs.y * ot1)) * colorDiff);
						c2 = (color0 + ((-cs.x + 1.0f) * ot1) * colorDiff);
						c3 = (color0 + ((cs.y + 1.0f) * ot1) * colorDiff);
						innerColor0 = (color0 + (it3 + (cs.x * irt)) * colorDiff);
						innerColor1 = (color0 + (it3 + (-cs.y * irt)) * colorDiff);
						innerColor2 = (color0 + (it1 + (-cs.x + 1.0f) * irt) * colorDiff);
						innerColor3 = (color0 + (it1 + (cs.y + 1.0f) * irt) * colorDiff);
					}
				}

				pDst0->set((outerRightCenterX + ox), (outerTopCenterY + oy), c0);
				pDst1->set((outerRightCenterX - oy), (outerBottomCenterY + ox), c1);
				pDst2->set((outerLeftCenterX - ox), (outerBottomCenterY - oy), c2);
				pDst3->set((outerLeftCenterX + oy), (outerTopCenterY - ox), c3);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;

				pDst0->set((innerRightCenterX + ix), (innerTopCenterY + iy), innerColor0);
				pDst1->set((innerRightCenterX - iy), (innerBottomCenterY + ix), innerColor1);
				pDst2->set((innerLeftCenterX - ix), (innerBottomCenterY - iy), innerColor2);
				pDst3->set((innerLeftCenterX + iy), (innerTopCenterY - ix), innerColor3);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// 弧の最後の頂点
			{
				const Float2& cs = pCS[0];
				const float ox = (cs.x * outerR);
				const float oy = (cs.y * outerR);
				const float ix = (cs.x * innerR);
				const float iy = (cs.y * innerR);

				CornerColor c0 = color0;
				CornerColor c1 = color0;
				CornerColor c2 = color0;
				CornerColor c3 = color0;
				CornerColor innerColor0 = color0;
				CornerColor innerColor1 = color0;
				CornerColor innerColor2 = color0;
				CornerColor innerColor3 = color0;
				if constexpr (Gradient)
				{
					if (colorType == ColorFillDirection::TopBottom)
					{
						c0 = (color0 + ot1 * colorDiff);
						c1 = color1;
						c2 = (color0 + ot2 * colorDiff);
						c3 = color0;
						innerColor0 = (color0 + it2 * colorDiff);
						innerColor1 = (color0 + it4 * colorDiff);
						innerColor2 = (color0 + it3 * colorDiff);
						innerColor3 = (color0 + it1 * colorDiff);
					}
					else // ColorFillDirection::LeftRight
					{
						c0 = color1;
						c1 = (color0 + ot2 * colorDiff);
						c2 = color0;
						c3 = (color0 + ot1 * colorDiff);
						innerColor0 = (color0 + it4 * colorDiff);
						innerColor1 = (color0 + it3 * colorDiff);
						innerColor2 = (color0 + it1 * colorDiff);
						innerColor3 = (color0 + it2 * colorDiff);
					}
				}

				pDst0->set((outerRightCenterX - oy), (outerTopCenterY + ox), c0);
				pDst1->set((outerRightCenterX - ox), (outerBottomCenterY - oy), c1);
				pDst2->set((outerLeftCenterX + oy), (outerBottomCenterY - ox), c2);
				pDst3->set((outerLeftCenterX + ox), (outerTopCenterY + oy), c3);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;

				pDst0->set((innerRightCenterX - iy), (innerTopCenterY + ix), innerColor0);
				pDst1->set((innerRightCenterX - ix), (innerBottomCenterY - iy), innerColor1);
				pDst2->set((innerLeftCenterX + iy), (innerBottomCenterY - ix), innerColor2);
				pDst3->set((innerLeftCenterX + ix), (innerTopCenterY + iy), innerColor3);
			}
		}

		/// @brief 円の品質を計算します。
		/// @param r 円の半径
		/// @return 円の品質
		[[nodiscard]]
		static constexpr Vertex2D::IndexType CalculateCircleQuality(const float r) noexcept
		{
			if (r <= 2.0f)
			{
				return 2;
			}
			else if (r <= 4.0f)
			{
				return 3;
			}
			else if (r < 8.0f)
			{
				return 4;
			}
			else
			{
				return static_cast<Vertex2D::IndexType>(Min((5 + static_cast<int32>((r - 8.0f) / 8.0f)), MaxCircleQuarterSegments));
			}
		}

		// One contour writer for independent-radius fills and both frame boundaries.
		// Quality 0 emits a single square corner; a collapsed inner arc repeats that
		// corner at the outer arc's quality so the frame remains a simple strip.
		static void WriteRoundedRectContour(Vertex2DWritePointer const pVertex, const FloatRect& rect,
			const std::array<float, 4>& radii, const std::array<Vertex2D::IndexType, 4>& qualities,
			const Float4& color, const uint16 stride)
		{
			constexpr Float2 startDirections[]{ { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } };
			constexpr Float2 endDirections[]{ { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };
			const Float2 corners[]{ { rect.left, rect.top }, { rect.right, rect.top },
				{ rect.right, rect.bottom }, { rect.left, rect.bottom } };

			uint16 vertexIndex = 0;
			for (size_t corner = 0; corner < 4; ++corner)
			{
				const float r = radii[corner];
				const auto quality = qualities[corner];
				const Float2 startDirection = startDirections[corner];
				const Float2 endDirection = endDirections[corner];
				const Float2 center = (corners[corner] - (startDirection + endDirection) * r);
				const Float2 start = (corners[corner] - endDirection * r);
				pVertex[vertexIndex].set(start.x, start.y, color);
				vertexIndex += stride;

				if (quality == 0)
				{
					continue;
				}

				const auto table = QuarterArcTable::GetUnitVectors(quality);
				for (Vertex2D::IndexType i = 1; i < quality; ++i)
				{
					const Float2 position = (center + (startDirection * -table[i].y + endDirection * table[i].x) * r);
					pVertex[vertexIndex].set(position.x, position.y, color);
					vertexIndex += stride;
				}

				const Float2 end = (corners[corner] - startDirection * r);
				pVertex[vertexIndex].set(end.x, end.y, color);
				vertexIndex += stride;
			}
		}

		// 円弓形と半円キャップには、弦と弧の間の領域を保つ最低 4 区間の密度を用いる。
		[[nodiscard]]
		static Vertex2D::IndexType CalculateCircleSegmentQuality(const float r, const float angle) noexcept
		{
			const Vertex2D::IndexType baseQuality = (CalculateCircleQuality(r) * 4);
			const float angleDelta = (Math::TwoPiF / baseQuality);
			return static_cast<Vertex2D::IndexType>(Max(std::ceil(angle / angleDelta), 5.0f));
		}

		using Vertex2DBuilder::CalculateCircleArcQuality;

		[[nodiscard]]
		static Float2 CalculateEllipseNormal(
			const float a,
			const float b,
			const float ux,
			const float uy) noexcept
		{
			// 基準楕円:
			//   x = a * ux
			//   y = b * uy
			//   ux^2 + uy^2 = 1
			//
			// 楕円 x^2/a^2 + y^2/b^2 = 1 の外向き法線は
			//   normalize({ x/a^2, y/b^2 })
			// = normalize({ ux/a, uy/b })

			const float nx = (ux / a);
			const float ny = (uy / b);
			const float lengthInv = (1.0f / std::sqrt(nx * nx + ny * ny));

			return{ (nx * lengthInv), (ny * lengthInv) };
		}

		template <bool Rotated = false>
		static void SetEllipseFrameVertexPair(
			Vertex2DWritePointer& pDst,
			const float centerX,
			const float centerY,
			const float a,
			const float b,
			const float ux,
			const float uy,
			const float innerThickness,
			const float outerThickness, const float rotationCos = 1.0f, const float rotationSin = 0.0f) noexcept
		{
			if constexpr (Rotated)
			{
				const Float2 normal = CalculateEllipseNormal(a, b, ux, uy);
				const Float2 outer{ a * ux + normal.x * outerThickness, b * uy + normal.y * outerThickness };
				const Float2 inner{ a * ux - normal.x * innerThickness, b * uy - normal.y * innerThickness };
				(pDst++)->setPosition((centerX + rotationCos * outer.x - rotationSin * outer.y), (centerY + rotationSin * outer.x + rotationCos * outer.y));
				(pDst++)->setPosition((centerX + rotationCos * inner.x - rotationSin * inner.y), (centerY + rotationSin * inner.x + rotationCos * inner.y));
			}
			else
			{
				const float baseX = (centerX + a * ux);
				const float baseY = (centerY + b * uy);

				const Float2 normal = CalculateEllipseNormal(a, b, ux, uy);

				// outer vertex
				(pDst++)->setPosition(
					(baseX + normal.x * outerThickness),
					(baseY + normal.y * outerThickness));

				// inner vertex
				(pDst++)->setPosition(
					(baseX - normal.x * innerThickness),
					(baseY - normal.y * innerThickness));
			}
		}

		template <class VertexWriter>
		SIV3D_ALWAYS_INLINE
		static inline void WriteArcVertices(Vertex2DWritePointer pDst, const float centerX, const float centerY, const float rx, const float ry,
			const float startAngle, const float radDelta, const Vertex2D::IndexType quality, const VertexWriter& writeVertex)
		{
			for (Vertex2D::IndexType i = 0; i < quality; ++i)
			{
				// Keep the angle in one expression to preserve multiply-add contraction.
				const float rad = (startAngle + (radDelta * i));
				const auto [s, c] = FastMath::SinCos(rad);
				writeVertex(*pDst++, (centerX + rx * s), (centerY - ry * c), (radDelta * i), s, c);
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteArcPositions(Vertex2DWritePointer pDst, const float centerX, const float centerY, const float rx, const float ry,
			const float startAngle, const float radDelta, const Vertex2D::IndexType quality)
		{
			WriteArcVertices(pDst, centerX, centerY, rx, ry, startAngle, radDelta, quality,
				[](Vertex2DWriteReference vertex, const float x, const float y, float, float, float) { vertex.setPosition(x, y); });
		}

		template <bool SquareCaps = false>
		SIV3D_ALWAYS_INLINE
		static inline void WriteArcStripPositions(Vertex2DWritePointer pDst, const float centerX, const float centerY, const float rInner, const float rOuter,
			const float startAngle, const float radDelta, const Vertex2D::IndexType quality)
		{
			const auto writePair = [&](const Vertex2D::IndexType i)
			{
				const float rad = (startAngle + (radDelta * i));
				const auto [s, c] = FastMath::SinCos(rad);
				const Float2 inner{ (centerX + rInner * s), (centerY - rInner * c) };
				const Float2 outer{ (centerX + rOuter * s), (centerY - rOuter * c) };
				const size_t base = ((i + (SquareCaps ? 1 : 0)) * 2);
				pDst[base].setPosition(inner);
				pDst[base + 1].setPosition(outer);
				return std::array{ inner, outer };
			};
			if constexpr (SquareCaps)
			{
				// Retain endpoint positions before storing; upload memory must not be read.
				const auto first = writePair(0);
				const auto last = ((quality == 1) ? first : writePair(quality - 1));
				const Float2 startHalfWidth = ((first[1] - first[0]) * 0.5f);
				const Float2 endHalfWidth = ((last[1] - last[0]) * 0.5f);
				const Float2 startExtension{ startHalfWidth.y, -startHalfWidth.x };
				const Float2 endExtension{ -endHalfWidth.y, endHalfWidth.x };
				pDst[0].setPosition((first[0] + startExtension));
				pDst[1].setPosition((first[1] + startExtension));
				pDst[(quality + 1) * 2].setPosition((last[0] + endExtension));
				pDst[(quality + 1) * 2 + 1].setPosition((last[1] + endExtension));
				for (Vertex2D::IndexType i = 1; (i + 1) < quality; ++i) writePair(i);
			}
			else
			{
				for (Vertex2D::IndexType i = 0; i < quality; ++i) writePair(i);
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteArcStripAlongColors(Vertex2DWritePointer pDst, const int32 quality, const Float4& startColor, const Float4& colorDelta)
		{
			for (Vertex2D::IndexType i = 0; i < quality; ++i)
			{
				const Float4 color = (startColor + (colorDelta * i));
				(pDst++)->setColor(color);
				(pDst++)->setColor(color);
			}
		}

		SIV3D_ALWAYS_INLINE
		static inline void WriteArcStripRadialColors(Vertex2DWritePointer pDst, const size_t pairCount, const Float4& color0, const Float4& color1)
		{
			for (size_t i = 0; i < pairCount; ++i)
			{
				(pDst++)->setColor(color0);
				(pDst++)->setColor(color1);
			}
		}

		// 確保済みの領域に半円を書き込む。領域の確保と品質の決定は呼び出し側が行う。
		static void EmitRoundCap(const Vertex2DBufferPointer buffer, const Float2& center, const float r, const float startAngle, const Float4& color, const Vertex2D::IndexType quality)
		{
			auto [pVertex, pIndex, indexOffset] = buffer;
			const Vertex2D::IndexType VertexCount = (quality + 1);

			const float centerX = center.x;
			const float centerY = center.y;

			// 中心
			pVertex[0].setPosition(centerX, centerY);

			// 周
			{
				const float radDelta = (Math::PiF / (quality - 1));
				WriteArcPositions((pVertex + 1), centerX, centerY, r, r, startAngle, radDelta, quality);
			}

			for (size_t i = 0; i < VertexCount; ++i)
			{
				(pVertex++)->setColor(color);
			}

			WriteOpenFanIndices(pIndex, indexOffset, quality);
		}

		static void EmitRoundCap(const Vertex2DBufferPointer buffer, const Float2& center, const float r, const float startAngle, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const Vertex2D::IndexType quality)
		{
			auto [pVertex, pIndex, indexOffset] = buffer;

			const float centerX = center.x;
			const float centerY = center.y;
			const Float4 c0 = color0;
			const Float4 c1 = color1;
			const Float4 colorDiff = (c1 - c0);

			if (colorType == ColorFillDirection::LeftRight)
			{
				// 中心
				pVertex[0].set(centerX, centerY, c0);

				// 周
				{
					const float radDelta = (Math::PiF / (quality - 1));
					WriteArcVertices((pVertex + 1), centerX, centerY, r, r, startAngle, radDelta, quality,
						[&](Vertex2DWriteReference vertex, const float x, const float y, const float delta, float, float)
						{
							const float f = std::sin(delta);
							vertex.set(x, y, (c0 + colorDiff * f));
						});
				}
			}
			else
			{
				// 中心
				pVertex[0].set(centerX, centerY, (c0 + colorDiff * 0.5f));

				// 周
				{
					const float radDelta = (Math::PiF / (quality - 1));
					WriteArcVertices((pVertex + 1), centerX, centerY, r, r, startAngle, radDelta, quality,
						[&](Vertex2DWriteReference vertex, const float x, const float y, const float delta, float, float)
						{
							const float f = (std::cos(delta) * -0.5f + 0.5f);
							vertex.set(x, y, (c0 + colorDiff * f));
						});
				}
			}

			WriteOpenFanIndices(pIndex, indexOffset, quality);
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildRoundCircleArc(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);
			const float r = (rInner + halfThickness);
			const float length = (r * Abs(angle) + halfThickness * 2);
			const float fOffset = (halfThickness / length);
			const Float4 c0 = color0.lerp(color1, fOffset);
			const Float4 c1 = color0.lerp(color1, (1.0f - fOffset));

			Float2 startCenter;
			Float2 endCenter;
			{
				const float centerX = center.x;
				const float centerY = center.y;
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				
				{
					const auto [s, c] = FastMath::SinCos(start);
					startCenter = { (centerX + r * s), (centerY - r * c) };
				}

				{
					const auto [s, c] = FastMath::SinCos(start + Abs(angle));
					endCenter = { (centerX + r * s), (centerY - r * c) };
				}
			}

			const float rOuter = (rInner + thickness);
			const Vertex2D::IndexType capQuality = CalculateCircleSegmentQuality((halfThickness * scale), Math::PiF);
			const Vertex2D::IndexType capVertexCount = (capQuality + 1);
			const Vertex2D::IndexType capIndexCount = ((capQuality - 1) * 3);
			Vertex2D::IndexType arcQuality = 0;
			Vertex2D::IndexType arcVertexCount = 0;
			Vertex2D::IndexType arcIndexCount = 0;

			// 角度が 0 の場合は本体を作らず、両端の半円だけを描く。
			if (angle != 0.0)
			{
				arcQuality = CalculateCircleArcQuality((rOuter * scale), Abs(angle));
				arcVertexCount = (arcQuality * 2);
				arcIndexCount = ((arcQuality - 1) * 6);
			}

			// 始端、本体、終端を一度に確保する。途中失敗で始端だけが予約されることはない。
			const Vertex2D::IndexType vertexCount = (capVertexCount * 2 + arcVertexCount);
			const Vertex2D::IndexType indexCount = (capIndexCount * 2 + arcIndexCount);
			const auto buffer = bufferCreator(vertexCount, indexCount);
			if (not buffer.pVertex)
			{
				return 0;
			}

			// 重なる部分の合成結果を保つため、従来どおり始端 → 本体 → 終端の順に配置する。
			if (colorType == ColorFillDirection::LeftRight)
			{
				if (angle < 0.0)
				{
					EmitRoundCap(buffer, startCenter, halfThickness, (startAngle + angle + Math::PiF), colorType, c1, color1, capQuality);
				}
				else
				{
					EmitRoundCap(buffer, startCenter, halfThickness, (startAngle + Math::PiF), colorType, c0, color0, capQuality);
				}
			}
			else
			{
				if (angle < 0.0)
				{
					EmitRoundCap(buffer, startCenter, halfThickness, (startAngle + angle + Math::PiF), colorType, color0, color1, capQuality);
				}
				else
				{
					EmitRoundCap(buffer, startCenter, halfThickness, (startAngle + Math::PiF), colorType, color0, color1, capQuality);
				}
			}

			if (angle != 0.0)
			{
				// 本体の書き込み先は、始端の半円の直後。
				Vertex2DWritePointer pVertex = (buffer.pVertex + capVertexCount);
				Index2DWritePointer pIndex = (buffer.pIndex + capIndexCount);
				const Vertex2D::IndexType indexOffset = (buffer.indexOffset + capVertexCount);

				{
					const float centerX = center.x;
					const float centerY = center.y;
					const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
					const float radDelta = (Abs(angle) / (arcQuality - 1));
					WriteArcStripPositions(pVertex, centerX, centerY, rInner, rOuter, start, radDelta, arcQuality);
				}

				if (colorType == ColorFillDirection::LeftRight)
				{
					const Float4 startColor = ((angle < 0.0f) ? c1 : c0);
					const Float4 endColor = ((angle < 0.0f) ? c0 : c1);
					const Float4 colorDelta = ((endColor - startColor) / static_cast<float>((arcVertexCount / 2) - 1));
					WriteArcStripAlongColors(pVertex, (arcVertexCount / 2), startColor, colorDelta);
				}
				else
				{
					WriteArcStripRadialColors(pVertex, (arcVertexCount / 2), color0, color1);
				}

				WriteOpenStripIndices(pIndex, indexOffset, arcQuality);
			}

			// 本体がない場合も、終端は始端の直後になる。
			const Vertex2DBufferPointer endCapBuffer{ (buffer.pVertex + capVertexCount + arcVertexCount), (buffer.pIndex + capIndexCount + arcIndexCount), static_cast<Vertex2D::IndexType>(buffer.indexOffset + capVertexCount + arcVertexCount) };
			if (colorType == ColorFillDirection::LeftRight)
			{
				if (angle < 0.0)
				{
					EmitRoundCap(endCapBuffer, endCenter, halfThickness, startAngle, colorType, c0, color0, capQuality);
				}
				else
				{
					EmitRoundCap(endCapBuffer, endCenter, halfThickness, (startAngle + angle), colorType, c1, color1, capQuality);
				}
			}
			else
			{
				if (angle < 0.0)
				{
					EmitRoundCap(endCapBuffer, endCenter, halfThickness, startAngle, colorType, color1, color0, capQuality);
				}
				else
				{
					EmitRoundCap(endCapBuffer, endCenter, halfThickness, (startAngle + angle), colorType, color1, color0, capQuality);
				}
			}

			return indexCount;
		}

		template <bool SquareCaps>
		[[nodiscard]]
		static Vertex2D::IndexType BuildCircleArcStrip(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const float rOuter = (rInner + thickness);
			const Vertex2D::IndexType Quality = ((SquareCaps && (angle == 0.0f)) ? 1 : CalculateCircleArcQuality((rOuter * scale), Abs(angle)));
			const Vertex2D::IndexType PairCount = (Quality + (SquareCaps ? 2 : 0));
			const Vertex2D::IndexType VertexCount = (PairCount * 2);
			const Vertex2D::IndexType IndexCount = ((PairCount - 1) * 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			{
				const float centerX = center.x;
				const float centerY = center.y;
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = ((SquareCaps && (Quality == 1)) ? 0.0f : (Abs(angle) / (Quality - 1)));
				WriteArcStripPositions<SquareCaps>(pVertex, centerX, centerY, rInner, rOuter, start, radDelta, Quality);
			}

			if (colorType == ColorFillDirection::LeftRight)
			{
				Float4 startColor = ((angle < 0.0f) ? color1 : color0);
				Float4 endColor = ((angle < 0.0f) ? color0 : color1);
				if constexpr (SquareCaps)
				{
					pVertex[0].setColor(startColor);
					pVertex[1].setColor(startColor);
					pVertex[VertexCount - 2].setColor(endColor);
					pVertex[VertexCount - 1].setColor(endColor);
					const float halfThickness = (thickness * 0.5f);
					const float length = ((rInner + halfThickness) * Abs(angle) + thickness);
					const float offset = ((Quality == 1) ? 0.5f : (halfThickness / length));
					const Float4 arcStart = startColor.lerp(endColor, offset);
					endColor = startColor.lerp(endColor, (1.0f - offset));
					startColor = arcStart;
					pVertex += 2;
				}
				const Float4 colorDelta = ((SquareCaps && (Quality == 1)) ? Float4{ 0, 0, 0, 0 }
					: ((endColor - startColor) / static_cast<float>(Quality - 1)));
				WriteArcStripAlongColors(pVertex, Quality, startColor, colorDelta);
			}
			else
			{
				WriteArcStripRadialColors(pVertex, (VertexCount / 2), color0, color1);
			}

			WriteOpenStripIndices(pIndex, indexOffset, PairCount);

			return IndexCount;
		}

		static Vertex2D::IndexType ArcCapVertexCount(const LineCap cap, const Vertex2D::IndexType quality)
		{
			return ((cap == LineCap::Round) ? (quality + 1) : ((cap == LineCap::Square) ? 4 : 0));
		}

		static Vertex2D::IndexType ArcCapIndexCount(const LineCap cap, const Vertex2D::IndexType quality)
		{
			return ((cap == LineCap::Round) ? ((quality - 1) * 3) : ((cap == LineCap::Square) ? 6 : 0));
		}

		// normal points outward; end selects the clockwise end of the sampled arc.
		static void EmitArcEndCap(const Vertex2DBufferPointer buffer, const LineCap cap, const Float2 center,
			const Float2 normal, const float halfWidth, const bool end, const ColorFillDirection coloring,
			const Float4& bodyColor0, const Float4& bodyColor1, const Float4& tipColor, const Vertex2D::IndexType quality)
		{
			if (cap == LineCap::Round)
			{
				const float angle = (std::atan2(normal.x, -normal.y) + (end ? 0.0f : Math::PiF));
				if (coloring == ColorFillDirection::LeftRight)
				{
					EmitRoundCap(buffer, center, halfWidth, angle, coloring, bodyColor0, tipColor, quality);
				}
				else
				{
					EmitRoundCap(buffer, center, halfWidth, angle, coloring,
						(end ? bodyColor1 : bodyColor0), (end ? bodyColor0 : bodyColor1), quality);
				}
			}
			else if (cap == LineCap::Square)
			{
				const Float2 radial = (normal * halfWidth);
				const Float2 extension{ -radial.y, radial.x };
				const Float2 inner = (center - radial), outer = (center + radial);
				auto v = buffer.pVertex;
				const Float4& tip0 = ((coloring == ColorFillDirection::LeftRight) ? tipColor : bodyColor0);
				const Float4& tip1 = ((coloring == ColorFillDirection::LeftRight) ? tipColor : bodyColor1);
				if (end)
				{
					v[0].set(inner, bodyColor0); v[1].set(outer, bodyColor1);
					v[2].set(inner + extension, tip0); v[3].set(outer + extension, tip1);
				}
				else
				{
					v[0].set(inner - extension, tip0); v[1].set(outer - extension, tip1);
					v[2].set(inner, bodyColor0); v[3].set(outer, bodyColor1);
				}
				auto indices = buffer.pIndex;
				WriteOpenStripIndices(indices, buffer.indexOffset, 2);
			}
		}

		struct PolylineBuffer
		{
			Array<Float2> simplified;	// 間引き適用後（中間バッファ）

			Array<uint32> simplifiedSrc;

			Array<Float2> points;		// 角分割適用後（最終出力）

			Array<uint32> srcIndices;	// points[i] が入力点列の何番目由来か

			Array<Float2> directions;	// 各セグメントの単位方向。角分割判定とメッシュ生成で共有

			Array<float> segmentLengths;	// グラデーション専用。前処理時に resize して全要素を上書きする

			float totalLength = 0.0f;
		};

		/// @brief 前処理用の共有スクラッチバッファを返します。
		/// @remark Vertex2DBuilder はシングルスレッドからのみ呼ばれる前提。
		///         capacity を呼び出し間で再利用するため、定常状態ではヒープ確保が発生しない。
		[[nodiscard]]
		static PolylineBuffer& GetPolylineBuffer()
		{
			static PolylineBuffer buffer;
			buffer.simplified.clear();
			buffer.simplifiedSrc.clear();
			buffer.points.clear();
			buffer.srcIndices.clear();
			buffer.directions.clear();
			return buffer;
		}

		/// @brief 鋭角頂点を分割する際の補助点の変位方向（角を抜けて進む向きの二等分線）を返します。
		[[nodiscard]]
		static Float2 CornerSplitDirection(const Float2& inDir, const Float2& outDir) noexcept
		{
			const Float2 v = (inDir + outDir);

			return ((0.0001f < v.lengthSq()) ? v.normalized() : Float2{ -inDir.y, inDir.x });
		}

		[[nodiscard]]
		static Float2 CornerSplitPoint(const Float2 point, const Float2 direction, const float epsilon) noexcept
		{
			Float2 result = (point + direction * epsilon);
			// At high zoom or translated coordinates the displacement may round away.
			// Keep the auxiliary segment nonzero; only sharp corners take this path.
			if (result == point)
			{
				constexpr float Infinity = std::numeric_limits<float>::infinity();
				if (Abs(direction.x) >= Abs(direction.y))
				{
					result.x = std::nextafter(point.x, std::copysign(Infinity, direction.x));
				}
				else
				{
					result.y = std::nextafter(point.y, std::copysign(Infinity, direction.y));
				}
			}
			return result;
		}

		struct PreparedPolyline
		{
			std::span<const Float2> points;

			std::span<const uint32> srcIndices;

			std::span<const Float2> directions;	// points[i] から次の点へ。closed は末尾から先頭も含む
		};

		template <bool StoreLengths>
		static void CalculatePolylineDirections(const std::span<const Float2> points, const bool closed, PolylineBuffer& buffer)
		{
			auto& directions = buffer.directions;
			const size_t size = points.size();
			const bool ring = (closed && (3 <= size));
			directions.resize(ring ? size : (size - 1));
			if constexpr (StoreLengths)
			{
				buffer.segmentLengths.resize(directions.size());
			}
			float totalLength = 0.0f;
			const auto setSegment = [&](const size_t i, const Float2 delta)
			{
				if constexpr (StoreLengths)
				{
					const float lengthSq = delta.lengthSq();
					const float length = std::sqrt(lengthSq);
					buffer.segmentLengths[i] = length;
					totalLength += length;
					// Match Float2::normalized(), sharing its square root with the gradient.
					directions[i] = ((lengthSq == 0.0f) ? delta : (delta * (1.0f / length)));
				}
				else
				{
					directions[i] = delta.normalized();
				}
			};
			for (size_t i = 0; i < (size - 1); ++i)
			{
				setSegment(i, (points[i + 1] - points[i]));
			}
			if (ring)
			{
				setSegment((size - 1), (points.front() - points.back()));
			}
			if constexpr (StoreLengths)
			{
				buffer.totalLength = totalLength;
			}
		}

		/// @brief 入力点列に間引きと鋭角頂点の分割を適用します。
		/// @remark 間引き: 直前の採用点からスクリーン上で 0.25px 未満しか離れていない点を除去。
		/// @remark 角分割: 内角が acos(0.55) ≒ 56.6° 未満の鋭角頂点の直後
		///         （closed の先頭頂点については巡回順で直前 = 点列末尾）に、二等分線方向へ
		///         ε = 0.001px ずらした補助点を挿入し、マイター長の暴走を防ぐ。
		/// @param trackSrcIndices true のとき、各出力点の入力インデックスを記録する
		///        （点ごとの色指定の参照用。補助点は元の頂点と同じインデックスを持つ）。
		/// @tparam StoreLengths true のとき、出力点列の区間長と全長を buffer に保存する。
		/// @return buffer 内を指す span。次に GetPolylineBuffer() が呼ばれるまで有効。
		template <class PointType, bool StoreLengths = false>
		[[nodiscard]]
		static PreparedPolyline PreparePolyline(const std::span<const PointType> points, const Optional<Float2>& offset, const float scale, const bool closed, const bool splitSharpCorners, const bool trackSrcIndices, PolylineBuffer& buffer)
		{
			// inDir・outDir がこの値未満なら鋭角とみなす
			constexpr float SharpCornerThreshold = -0.55f;

			const Float2 offsetValue = offset.value_or(Float2{ 0, 0 });
			const size_t size = points.size();

			Array<Float2>& simplified = buffer.simplified;
			Array<uint32>& simplifiedSrc = buffer.simplifiedSrc;

			// 間引き
			{
				const double threshold = (0.25 / scale);
				const double thresholdSq = (threshold * threshold);

				PointType previous = points.front();
				{
					const Float2 p = previous;
					simplified.push_back(p + offsetValue);
				}

				if (trackSrcIndices)
				{
					simplifiedSrc.push_back(0);
				}

				for (size_t i = 1; i < (size - 1); ++i)
				{
					const PointType current = points[i];

					if (previous.distanceFromSq(current) < thresholdSq)
					{
						continue;
					}

					const Float2 p = current;
					simplified.push_back(p + offsetValue);

					if (trackSrcIndices)
					{
						simplifiedSrc.push_back(static_cast<uint32>(i));
					}

					previous = current;
				}

				const PointType last = points.back();

				if (previous != last)
				{
					const Float2 p = last;
					simplified.push_back(p + offsetValue);

					if (trackSrcIndices)
					{
						simplifiedSrc.push_back(static_cast<uint32>(size - 1));
					}
				}

				// リングの場合、終点が始点と十分近ければ重複とみなして除去する
				if (closed && (2 < simplified.size())
					&& (simplified.back().distanceFromSq(simplified.front()) <= thresholdSq))
				{
					simplified.pop_back();

					if (trackSrcIndices)
					{
						simplifiedSrc.pop_back();
					}
				}
			}

			const size_t simplifiedSize = simplified.size();
			CalculatePolylineDirections<StoreLengths>(simplified, closed, buffer);

			// 角分割の対象が無い場合は間引き結果をそのまま返す（コピーしない）
			if ((simplifiedSize < 3) || (not splitSharpCorners))
			{
				return{ simplified, simplifiedSrc, buffer.directions };
			}

			// 角分割
			{
				Array<Float2>& result = buffer.points;
				Array<uint32>& resultSrc = buffer.srcIndices;
				const Float2* const pSrc = simplified.data();
				const float epsilon = (0.001f / scale);

				// 補助点が必要になった時点で初めて点列をコピーする。
				const auto copyPrefix = [&](const size_t count)
				{
					result.assign(simplified.begin(), (simplified.begin() + count));
					if (trackSrcIndices)
					{
						resultSrc.assign(simplifiedSrc.begin(), (simplifiedSrc.begin() + count));
					}
				};

				const size_t last = (closed ? simplifiedSize : (simplifiedSize - 1));

				for (size_t i = 1; i < last; ++i)
				{
					const Float2 current = pSrc[i];
					const Float2 inDir = buffer.directions[i - 1];
					const Float2 outDir = buffer.directions[i];

					if (not result.empty())
					{
						result.push_back(current);
						if (trackSrcIndices)
						{
							resultSrc.push_back(simplifiedSrc[i]);
						}
					}

					if (inDir.dot(outDir) < SharpCornerThreshold)
					{
						if (result.empty()) copyPrefix(i + 1);
						result.push_back(CornerSplitPoint(current, CornerSplitDirection(inDir, outDir), epsilon));

						if (trackSrcIndices)
						{
							resultSrc.push_back(simplifiedSrc[i]);
						}
					}
				}

				if (closed)
				{
					// 先頭頂点の角分割。補助点は巡回順で先頭頂点の「直前」に当たるため、
					// 点列の末尾に、二等分線の逆方向で追加する
					const Float2 inDir = buffer.directions.back(), outDir = buffer.directions.front();
					if (inDir.dot(outDir) < SharpCornerThreshold)
					{
						if (result.empty()) copyPrefix(simplifiedSize);
						result.push_back(CornerSplitPoint(pSrc[0], -CornerSplitDirection(inDir, outDir), epsilon));

						if (trackSrcIndices)
						{
							resultSrc.push_back(simplifiedSrc[0]);
						}
					}
				}
				else if (not result.empty())
				{
					result.push_back(pSrc[simplifiedSize - 1]);

					if (trackSrcIndices)
					{
						resultSrc.push_back(simplifiedSrc[simplifiedSize - 1]);
					}
				}

				if (result.empty())
				{
					return{ simplified, simplifiedSrc, buffer.directions };
				}

				// 補助点を挿入した場合だけ、変更された点列から方向を求め直す。
				CalculatePolylineDirections<StoreLengths>(result, closed, buffer);
				return{ result, resultSrc, buffer.directions };
			}
		}

		struct MiterPair
		{
			Float2 a;

			Float2 b;
		};

		/// @brief 頂点 point におけるマイター接合の頂点ペアを返します。
		/// @param inDir 流入セグメントの単位方向ベクトル
		/// @param outDir 流出セグメントの単位方向ベクトル
		/// @param prevPoint, nextPoint 180° 折り返し（二等分線が定義できない）時のフォールバックにのみ使用
		[[nodiscard]]
		static MiterPair ComputeMiterPair(const Float2& point, const Float2& inDir, const Float2& outDir, const Float2& prevPoint, const Float2& nextPoint, const float halfThickness) noexcept
		{
			const Float2 v = (inDir + outDir);
			// 二等分線の長さは miter / dot(tangent, inDir) で相殺されるため、正規化しない。
			const Float2 tangent = ((0.001f < v.lengthSq()) ? v : (nextPoint - prevPoint));
			const Float2 miter{ -tangent.y, tangent.x };
			const float length = (halfThickness / tangent.dot(inDir));
			const Float2 m = (miter * length);

			return{ (point + m), (point - m) };
		}

		// 本体の既存の点数制限を維持する。キャップを足した後のインデックス個数は uint32 で扱う。
		static constexpr size_t MaxLineStringPointCount = (std::numeric_limits<Vertex2D::IndexType>::max() / 6);

		struct LineStringBuffer
		{
			// 1 回の確保で得た領域内の書き込み先。本体、始端、終端の順に並ぶ。
			Vertex2DBufferPointer strip{};
			Vertex2DBufferPointer startCap{};
			Vertex2DBufferPointer endCap{};
			Vertex2D::IndexType capQuality = 0;
			uint32 indexCount = 0;
		};

		[[nodiscard]]
		static LineStringBuffer CreateLineStringBuffer(const BufferCreatorFunc& bufferCreator, const size_t pointCount, const LineCap startCap, const LineCap endCap, const float halfThickness, const float scale)
		{
			if (MaxLineStringPointCount < pointCount)
			{
				return {};
			}

			const Vertex2D::IndexType quality = (((startCap == LineCap::Round) || (endCap == LineCap::Round))
				? CalculateCircleSegmentQuality((halfThickness * scale), Math::PiF) : 0);
			// 四角い端は 4 頂点・6 インデックス、丸い端は半円の品質に応じた個数になる。
			const Vertex2D::IndexType startVertexCount = ((startCap == LineCap::Square) ? 4 : ((startCap == LineCap::Round) ? (quality + 1) : 0));
			const Vertex2D::IndexType endVertexCount = ((endCap == LineCap::Square) ? 4 : ((endCap == LineCap::Round) ? (quality + 1) : 0));
			const Vertex2D::IndexType startIndexCount = ((startCap == LineCap::Square) ? 6 : ((startCap == LineCap::Round) ? ((quality - 1) * 3) : 0));
			const Vertex2D::IndexType endIndexCount = ((endCap == LineCap::Square) ? 6 : ((endCap == LineCap::Round) ? ((quality - 1) * 3) : 0));

			// pointCount == 0 は、1 点に縮退してキャップだけを描く場合。本体の領域は不要。
			const Vertex2D::IndexType stripVertexCount = static_cast<Vertex2D::IndexType>(pointCount * 2);
			const uint32 stripIndexCount = (pointCount ? static_cast<uint32>((pointCount - 1) * 6) : 0);
			const Vertex2D::IndexType vertexCount = (stripVertexCount + startVertexCount + endVertexCount);
			const uint32 indexCount = (stripIndexCount + startIndexCount + endIndexCount);
			if (indexCount == 0)
			{
				return {};
			}

			// 全体が確保できた場合だけ書き込む。本体や片方のキャップだけが予約されることはない。
			const auto buffer = bufferCreator(vertexCount, indexCount);
			if (not buffer.pVertex)
			{
				return {};
			}

			const Vertex2DBufferPointer startBuffer{ (buffer.pVertex + stripVertexCount), (buffer.pIndex + stripIndexCount), static_cast<Vertex2D::IndexType>(buffer.indexOffset + stripVertexCount) };
			const Vertex2DBufferPointer endBuffer{ (startBuffer.pVertex + startVertexCount), (startBuffer.pIndex + startIndexCount), static_cast<Vertex2D::IndexType>(startBuffer.indexOffset + startVertexCount) };
			return { buffer, startBuffer, endBuffer, quality, indexCount };
		}

		static void EmitSquareCap(const Vertex2DBufferPointer buffer, const FloatQuad& quad, const Float4(&colors)[4])
		{
			auto [pVertex, pIndex, indexOffset] = buffer;
			pVertex[0].set(quad.p[0], colors[0]);
			pVertex[1].set(quad.p[1], colors[1]);
			pVertex[2].set(quad.p[3], colors[3]);
			pVertex[3].set(quad.p[2], colors[2]);
			for (const auto index : RectIndexTable)
			{
				*pIndex++ = (indexOffset + index);
			}
		}

		static void EmitSquareCap(const Vertex2DBufferPointer buffer, const FloatQuad& quad, const Float4& color)
		{
			EmitSquareCap(buffer, quad, { color, color, color, color });
		}

		/// @brief 前処理済みの点列から、幅 (halfThickness * 2) のクアッドストリップを構築します。
		/// @param buffer 本体の頂点・インデックスを書き込める確保済み領域。
		/// @param prepared PreparePolyline 済みの点列と方向。open は 2 点以上、closed は 3 点以上であること。
		/// @param colorAt pts[i] に対応する頂点ペアの色を返す関数。
		///        i の昇順に、各 i につきちょうど 1 回呼び出される（状態を持つラムダを許容するための契約）。
		/// @param pStartAngle, pEndAngle open のとき、始端・終端の法線角度を返す（キャップ描画用）。
		template <class ColorFn>
		static void EmitLineStringStrip(const Vertex2DBufferPointer buffer, const PreparedPolyline& prepared, const float halfThickness, const bool closed, ColorFn&& colorAt, float* pStartAngle = nullptr, float* pEndAngle = nullptr)
		{
			auto [pVertex, pIndex, indexOffset] = buffer;
			const auto pts = prepared.points;
			const auto directions = prepared.directions;
			const size_t n = pts.size();
			const Vertex2D::IndexType pointCount = static_cast<Vertex2D::IndexType>(n);

			const Float2* const pBuf = pts.data();
			const Float2 firstDir = (closed ? directions.back() : directions.front());
			Float2 inDir = firstDir;

			// 始端の頂点ペア
			if (closed)
			{
				const Float2 outDir = directions.front();
				const auto [a, b] = ComputeMiterPair(pBuf[0], inDir, outDir, pBuf[n - 1], pBuf[1], halfThickness);
				const Float4 color = colorAt(size_t{ 0 });
				pVertex[0].set(a, color);
				pVertex[1].set(b, color);
				inDir = outDir;
			}
			else
			{
				const Float2 vNormalBegin{ (-inDir.y * halfThickness), (inDir.x * halfThickness) };

				if (pStartAngle)
				{
					*pStartAngle = std::atan2(vNormalBegin.x, -vNormalBegin.y);
				}

				const Float4 color = colorAt(size_t{ 0 });
				pVertex[0].set((pBuf[0] + vNormalBegin), color);
				pVertex[1].set((pBuf[0] - vNormalBegin), color);
			}

			// 中間頂点（マイター接合）
			for (size_t i = 1; i < (n - 1); ++i)
			{
				const Float2 outDir = directions[i];
				const auto [a, b] = ComputeMiterPair(pBuf[i], inDir, outDir, pBuf[i - 1], pBuf[i + 1], halfThickness);
				const Float4 color = colorAt(i);
				pVertex[i * 2].set(a, color);
				pVertex[i * 2 + 1].set(b, color);
				inDir = outDir;
			}

			// 終端の頂点ペア
			if (closed)
			{
				const auto [a, b] = ComputeMiterPair(pBuf[n - 1], inDir, firstDir, pBuf[n - 2], pBuf[0], halfThickness);
				const Float4 color = colorAt(n - 1);
				pVertex[(n - 1) * 2].set(a, color);
				pVertex[(n - 1) * 2 + 1].set(b, color);
			}
			else
			{
				const Float2 vNormalEnd{ (-inDir.y * halfThickness), (inDir.x * halfThickness) };

				if (pEndAngle)
				{
					*pEndAngle = std::atan2(vNormalEnd.x, -vNormalEnd.y);
				}

				const Float4 color = colorAt(n - 1);
				pVertex[(n - 1) * 2].set((pBuf[n - 1] + vNormalEnd), color);
				pVertex[(n - 1) * 2 + 1].set((pBuf[n - 1] - vNormalEnd), color);
			}

			{
				Vertex2D::IndexType base = indexOffset;

				for (Vertex2D::IndexType k = 0; k < (pointCount - 1); ++k)
				{
					pIndex[0] = base;
					pIndex[1] = (base + 1);
					pIndex[2] = (base + 2);
					pIndex[3] = (base + 2);
					pIndex[4] = (base + 1);
					pIndex[5] = (base + 3);
					pIndex += 6;
					base += 2;
				}

				if (closed)
				{
					// 最終クアッドのみ先頭の頂点ペアへラップする
					pIndex[0] = base;
					pIndex[1] = (base + 1);
					pIndex[2] = indexOffset;
					pIndex[3] = indexOffset;
					pIndex[4] = (base + 1);
					pIndex[5] = (indexOffset + 1);
				}
			}
		}

		template <class ColorFn>
		[[nodiscard]]
		static Vertex2D::IndexType BuildLineStringStrip(const BufferCreatorFunc& bufferCreator, const PreparedPolyline& prepared, const float halfThickness, const bool closed, ColorFn&& colorAt, float* pStartAngle = nullptr, float* pEndAngle = nullptr)
		{
			const size_t n = prepared.points.size();

			if ((n < 2) || (MaxLineStringPointCount < n))
			{
				return 0;
			}

			const Vertex2D::IndexType pointCount = static_cast<Vertex2D::IndexType>(n);
			const Vertex2D::IndexType vertexCount = (pointCount * 2);
			const Vertex2D::IndexType indexCount = (closed ? (6 * pointCount) : (6 * (pointCount - 1)));
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexCount, indexCount);

			if (not pVertex)
			{
				return 0;
			}

			EmitLineStringStrip({ pVertex, pIndex, indexOffset }, prepared, halfThickness, closed, colorAt, pStartAngle, pEndAngle);

			return indexCount;
		}

		[[nodiscard]]
		static uint32 BuildLineStringCaps(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const Float2& center, const Optional<Float2>& offset, const float thickness, const Float4& color, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);

			const Float2 base = (center + offset.value_or(Float2{ 0, 0 }));

			const auto buffer = CreateLineStringBuffer(bufferCreator, 0, startCap, endCap, halfThickness, scale);
			if (not buffer.strip.pVertex)
			{
				return 0;
			}

			// draw startCap
			{
				if (startCap == LineCap::Square)
				{
					const FloatRect rect{ (base.x - halfThickness), (base.y - halfThickness), base.x, (base.y + halfThickness) };

					EmitSquareCap(buffer.startCap, FloatQuad{ rect }, color);
				}
				else if (startCap == LineCap::Round)
				{
					EmitRoundCap(buffer.startCap, base, halfThickness, 180_degF, color, buffer.capQuality);
				}
			}

			// draw endCap
			{
				if (endCap == LineCap::Square)
				{
					const FloatRect rect{ base.x, (base.y - halfThickness), (base.x + halfThickness), (base.y + halfThickness) };

					EmitSquareCap(buffer.endCap, FloatQuad{ rect }, color);
				}
				else if (endCap == LineCap::Round)
				{
					EmitRoundCap(buffer.endCap, base, halfThickness, 0_degF, color, buffer.capQuality);
				}
			}

			return buffer.indexCount;
		}

		[[nodiscard]]
		static uint32 BuildLineStringCaps(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const Float2& center, const Optional<Float2>& offset, const float thickness, const Float4& colorStart, const Float4& colorEnd, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);

			const Float2 base = (center + offset.value_or(Float2{ 0, 0 }));

			const bool hasStartCap = (startCap != LineCap::Flat);
			const bool hasEndCap = (endCap != LineCap::Flat);

			const auto buffer = CreateLineStringBuffer(bufferCreator, 0, startCap, endCap, halfThickness, scale);
			if (not buffer.strip.pVertex)
			{
				return 0;
			}

			// draw startCap
			if (hasStartCap)
			{
				const Float4 c0 = colorStart;
				const Float4 c1 = (hasEndCap ? colorStart.getMidpoint(colorEnd) : colorEnd);

				if (startCap == LineCap::Square)
				{
					const FloatRect rect{ (base.x - halfThickness), (base.y - halfThickness), base.x, (base.y + halfThickness) };

					EmitSquareCap(buffer.startCap, FloatQuad{ rect }, { c0, c1, c1, c0 });
				}
				else if (startCap == LineCap::Round)
				{
					EmitRoundCap(buffer.startCap, base, halfThickness, 180_degF, ColorFillDirection::LeftRight, c1, c0, buffer.capQuality);
				}
			}

			// draw endCap
			if (hasEndCap)
			{
				const Float4 c0 = (hasStartCap ? colorStart.getMidpoint(colorEnd) : colorStart);
				const Float4 c1 = colorEnd;

				if (endCap == LineCap::Square)
				{
					const FloatRect rect{ base.x, (base.y - halfThickness), (base.x + halfThickness), (base.y + halfThickness) };

					EmitSquareCap(buffer.endCap, FloatQuad{ rect }, { c0, c1, c1, c0 });
				}
				else if (endCap == LineCap::Round)
				{
					EmitRoundCap(buffer.endCap, base, halfThickness, 0_degF, ColorFillDirection::LeftRight, c0, c1, buffer.capQuality);
				}
			}

			return buffer.indexCount;
		}

		static void EmitLineStringCaps(const LineStringBuffer& buffer, const LineCap startCap, const LineCap endCap, Float2 start, const float startAngle, Float2 end, const float endAngle, const Optional<Float2>& offset, const float thickness, const Float4& colorStart, const Float4& colorEnd)
		{
			const float halfThickness = (thickness * 0.5f);

			if (offset)
			{
				start += *offset;
				end += *offset;
			}

			// draw startCap
			{
				if (startCap == LineCap::Square)
				{
					const Quad quad = RectF{ Anchor::MiddleLeft, start, halfThickness, thickness }.rotatedAt(start, startAngle);

					EmitSquareCap(buffer.startCap, FloatQuad{ quad }, colorStart);
				}
				else if (startCap == LineCap::Round)
				{
					EmitRoundCap(buffer.startCap, start, halfThickness, startAngle, colorStart, buffer.capQuality);
				}
			}

			// draw endCap
			{
				if (endCap == LineCap::Square)
				{
					const Quad quad = RectF{ Anchor::MiddleRight, end, halfThickness, thickness }.rotatedAt(end, endAngle);

					EmitSquareCap(buffer.endCap, FloatQuad{ quad }, colorEnd);
				}
				else if (endCap == LineCap::Round)
				{
					EmitRoundCap(buffer.endCap, end, halfThickness, (endAngle + Math::PiF), colorEnd, buffer.capQuality);
				}
			}
		}

		static void EmitLineStringCaps(const LineStringBuffer& buffer, const LineCap startCap, const LineCap endCap, Float2 start, const float startAngle, Float2 end, const float endAngle, const Optional<Float2>& offset, const float thickness, const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3)
		{
			const float halfThickness = (thickness * 0.5f);

			if (offset)
			{
				start += *offset;
				end += *offset;
			}

			// draw startCap
			{
				if (startCap == LineCap::Square)
				{
					const Quad quad = RectF{ Anchor::MiddleLeft, start, halfThickness, thickness }.rotatedAt(start, startAngle);

					EmitSquareCap(buffer.startCap, FloatQuad{ quad }, { c1, c0, c0, c1 });
				}
				else if (startCap == LineCap::Round)
				{
					EmitRoundCap(buffer.startCap, start, halfThickness, startAngle, ColorFillDirection::LeftRight, c1, c0, buffer.capQuality);
				}
			}

			// draw endCap
			{
				if (endCap == LineCap::Square)
				{
					const Quad quad = RectF{ Anchor::MiddleRight, end, halfThickness, thickness }.rotatedAt(end, endAngle);

					EmitSquareCap(buffer.endCap, FloatQuad{ quad }, { c2, c3, c3, c2 });
				}
				else if (endCap == LineCap::Round)
				{
					EmitRoundCap(buffer.endCap, end, halfThickness, (endAngle + Math::PiF), ColorFillDirection::LeftRight, c2, c3, buffer.capQuality);
				}
			}
		}

		static constexpr size_t MaxEllipseArcLengthTableSegmentCount = (MaxCircleSegments * 8);

		[[nodiscard]]
		static std::span<const float> GetEllipseArcLengthTable(const float a, const float b, const size_t segmentCount) noexcept
		{
			struct Table
			{
				Float2 axes{ 0, 0 };
				size_t segmentCount = 0;
				std::array<float, (MaxEllipseArcLengthTableSegmentCount + 1)> lengths;
			};

			// 他の描画用スクラッチと同様、単一の描画スレッドで使用する。
			static std::array<Table, 4> tables{};
			static size_t nextTable = 0;
			for (const auto& table : tables)
			{
				if ((table.axes == Float2{ a, b }) && (table.segmentCount == segmentCount))
				{
					return{ table.lengths.data(), (segmentCount + 1) };
				}
			}

			auto& table = tables[nextTable];
			nextTable = ((nextTable + 1) % tables.size());
			auto& lengths = table.lengths;
			lengths[0] = 0.0f;
			const size_t quarterCount = (segmentCount / 4);
			float previousX = 0.0f;
			float previousY = -b;
			double length = 0.0;

			for (size_t i = 1; i <= quarterCount; ++i)
			{
				const float rad = (Math::HalfPiF * static_cast<float>(i) / quarterCount);
				const auto [s, c] = FastMath::SinCos(rad);
				const float x = (a * s);
				const float y = (-b * c);
				const float dx = (x - previousX);
				const float dy = (y - previousY);
				length += std::sqrt(dx * dx + dy * dy);
				lengths[i] = static_cast<float>(length);
				previousX = x;
				previousY = y;
			}

			// 同じ角度刻みのまま、残りの象限を対称性から復元する。
			const float halfLength = static_cast<float>(length * 2.0);
			const float perimeter = static_cast<float>(length * 4.0);
			for (size_t i = 1; i <= quarterCount; ++i)
			{
				lengths[quarterCount + i] = (halfLength - lengths[quarterCount - i]);
				lengths[quarterCount * 2 + i] = (halfLength + lengths[i]);
				lengths[quarterCount * 3 + i] = (perimeter - lengths[quarterCount - i]);
			}

			table.axes = { a, b };
			table.segmentCount = segmentCount;
			return{ lengths.data(), (segmentCount + 1) };
		}

		[[nodiscard]]
		static float CalculateEllipseParameterAtLength(const std::span<const float> cumulativeLengths, const float length, const float perimeter) noexcept
		{
			if ((perimeter <= 0.0f) || (cumulativeLengths.size() < 2))
			{
				return 0.0f;
			}

			float cycle = std::floor(length / perimeter);
			float wrappedLength = (length - cycle * perimeter);

			if (wrappedLength < 0.0f)
			{
				wrappedLength += perimeter;
				cycle -= 1.0f;
			}
			else if (perimeter <= wrappedLength)
			{
				wrappedLength = 0.0f;
				cycle += 1.0f;
			}

			size_t first = 0;
			size_t last = (cumulativeLengths.size() - 1);

			while (first < last)
			{
				const size_t mid = ((first + last) / 2);

				if (cumulativeLengths[mid] < wrappedLength)
				{
					first = (mid + 1);
				}
				else
				{
					last = mid;
				}
			}

			if (first == 0)
			{
				return (cycle * Math::TwoPiF);
			}

			const float s0 = cumulativeLengths[first - 1];
			const float s1 = cumulativeLengths[first];
			const float t = ((s0 < s1) ? ((wrappedLength - s0) / (s1 - s0)) : 0.0f);
			const float segment = (static_cast<float>(first - 1) + t);
			const float segmentCount = static_cast<float>(cumulativeLengths.size() - 1);

			return ((cycle * Math::TwoPiF) + (segment * Math::TwoPiF / segmentCount));
		}
	}

	void detail::RoundRectBalloon::MakeContour(Contour& destination, const Placement& placement, const float scale)
	{
		destination.count = 0;
		const auto push = [&](const Vec2 p)
		{
			if (destination.count && (destination.vertices[destination.count - 1] == p)) return;
			destination.vertices[destination.count++] = p;
		};
		const auto& body = placement.body;
		const double r = body.r, l = body.x, t = body.y, right = (l + body.w), b = (t + body.h);
		const Vec2 centers[]{ { right - r, t + r }, { right - r, b - r }, { l + r, b - r }, { l + r, t + r } };
		const Vec2 starts[]{ { right - r, t }, { right, b - r }, { l + r, b }, { l, t + r } };
		const Vec2 ends[]{ { right, t + r }, { right - r, b }, { l, b - r }, { l + r, t } };
		const uint16 quality = (r ? CalculateCircleQuality(static_cast<float>(r) * scale) : 0);
		const auto table = QuarterArcTable::GetUnitVectors(quality);
		for (uint8 corner = 0; corner < 4; ++corner)
		{
			push(starts[corner]);
			for (uint16 i = 1; i < quality; ++i)
			{
				const Vec2 cs = table[i];
				const Vec2 v = ((corner == 0) ? cs : ((corner == 1) ? Vec2{ -cs.y, cs.x }
					: ((corner == 2) ? -cs : Vec2{ cs.y, -cs.x })));
				push(centers[corner] + v * r);
			}
			push(ends[corner]);
			if (((corner + 1) % 4) == placement.side)
			{
				push(placement.base0);
				destination.tipIndex = destination.count;
				push(placement.tip);
				push(placement.base1);
			}
		}
		if (destination.vertices[destination.count - 1] == destination.vertices[0]) --destination.count;
	}

	namespace Vertex2DBuilder
	{
		Vertex2D::IndexType CalculateCircleArcQuality(const float r, const float angle) noexcept
		{
			const Vertex2D::IndexType baseQuality = (CalculateCircleQuality(r) * 4);
			// 端点を含む点数を返す。全周の分割予算は通常の円とそろえる。
			const float intervals = Clamp(std::ceil((angle / Math::TwoPiF) * baseQuality), 1.0f, static_cast<float>(baseQuality));
			return static_cast<Vertex2D::IndexType>(intervals + 1);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildLine
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildLine(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const Float2& start, const Float2& end, const float thickness, const Float4(&colors)[2], const float scale)
		{
			constexpr Vertex2D::IndexType VertexCount = 4;
			constexpr Vertex2D::IndexType IndexCount = 6;
			const float halfThickness = (thickness * 0.5f);
			const Vertex2D::IndexType roundCapCount = ((startCap == LineCap::Round) + (endCap == LineCap::Round));
			Vertex2D::IndexType capQuality = 0;
			Vertex2D::IndexType capVertexCount = 0;
			Vertex2D::IndexType capIndexCount = 0;

			if (roundCapCount)
			{
				capQuality = CalculateCircleSegmentQuality((halfThickness * scale), Math::PiF);
				capVertexCount = (capQuality + 1);
				capIndexCount = ((capQuality - 1) * 3);
			}

			// 本体と丸い両端を一度に確保し、図形の途中でのバッチ切り替えや確保失敗を避ける。
			const Vertex2D::IndexType totalVertexCount = (VertexCount + roundCapCount * capVertexCount);
			const Vertex2D::IndexType totalIndexCount = (IndexCount + roundCapCount * capIndexCount);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(totalVertexCount, totalIndexCount);
			if (not pVertex)
			{
				return 0;
			}

			const float length = (end - start).length();
			const Float2 line = (length ? ((end - start) / length) : Float2{ 1, 0 });
			const Float2 vNormal{ (-line.y * halfThickness), (line.x * halfThickness) };
			const Float2 lineHalf{ line * halfThickness };
			
			if (startCap == LineCap::Square)
			{
				const Float2 start2 = (start - lineHalf);
				pVertex[0].set((start2 + vNormal), colors[0]);
				pVertex[1].set((start2 - vNormal), colors[0]);
			}
			else
			{
				pVertex[0].set((start + vNormal), colors[0]);
				pVertex[1].set((start - vNormal), colors[0]);
			}
			
			if (endCap == LineCap::Square)
			{
				const Float2 end2 = (end + lineHalf);
				pVertex[2].set((end2 + vNormal), colors[1]);
				pVertex[3].set((end2 - vNormal), colors[1]);
			}
			else
			{
				pVertex[2].set((end + vNormal), colors[1]);
				pVertex[3].set((end - vNormal), colors[1]);
			}

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			// インデックスの書き込み先 pIndex は、本体の書き込みで既に IndexCount 個進んでいる。
			// 頂点の書き込み先と頂点番号も本体の直後に合わせ、始端、終端の順に書き込む。
			Vertex2DBufferPointer capBuffer{ (pVertex + VertexCount), pIndex, static_cast<Vertex2D::IndexType>(indexOffset + VertexCount) };
			const float startAngle = std::atan2(vNormal.x, -vNormal.y);

			if (startCap == LineCap::Round)
			{
				EmitRoundCap(capBuffer, start, halfThickness, startAngle, colors[0], capQuality);
				capBuffer.pVertex += capVertexCount;
				capBuffer.pIndex += capIndexCount;
				capBuffer.indexOffset += capVertexCount;
			}

			if (endCap == LineCap::Round)
			{
				EmitRoundCap(capBuffer, end, halfThickness, (startAngle + Math::PiF), colors[1], capQuality);
			}

			return totalIndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildArrow
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildArrow(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const Float2& start, const Float2& end, const float thickness, const Float2& headSize, const Float4(&colors)[2], const float scale)
		{
			const float length = start.distanceFrom(end);

			if (length == 0.0f)
			{
				if (startCap == LineCap::Round)
				{
					return BuildCircle(bufferCreator, start, (thickness * 0.5f), ColorFillDirection::InOut, colors[0], colors[0], scale);
				}

				return 0;
			}

			const float clampedRatio = (Min(headSize.y, length) / headSize.y);
			const Float2 direction = ((end - start) / length);
			const Float2 normalDirection{ direction.y, -direction.x };
			const float halfThickness = (thickness * 0.5f);
			const Float2 leftOffset = (normalDirection * halfThickness);
			const Float2 clampedHeadSize = (clampedRatio * headSize);
			const Float2 gutterOffset = (direction * clampedHeadSize.y);
			const Float2 edgeOffset = (normalDirection * clampedHeadSize.x * 0.5f);

			// startCap: Square は始点を後ろに伸ばす
			const Float2 startOffset = (direction * halfThickness);
			const Float2 start2 = ((startCap == LineCap::Square) ? (start - startOffset) : start);

			const float headBaseT = Clamp(((length - clampedHeadSize.y) / length), 0.0f, 1.0f);
			const Float4 headBaseColor = colors[0].lerp(colors[1], headBaseT);

			constexpr Vertex2D::IndexType VertexCount = 7;
			constexpr Vertex2D::IndexType IndexCount = 9;
			Vertex2D::IndexType capQuality = 0;
			Vertex2D::IndexType totalVertexCount = VertexCount;
			Vertex2D::IndexType totalIndexCount = IndexCount;

			if (startCap == LineCap::Round)
			{
				capQuality = CalculateCircleSegmentQuality((halfThickness * scale), Math::PiF);
				totalVertexCount += (capQuality + 1);
				totalIndexCount += ((capQuality - 1) * 3);
			}

			// 矢印の本体と丸い始端を一度に確保し、途中でのバッチ切り替えや確保失敗を避ける。
			auto [pVertex, pIndex, indexOffset] = bufferCreator(totalVertexCount, totalIndexCount);
			if (not pVertex)
			{
				return 0;
			}

			{
				pVertex[0].set((start2 + leftOffset), colors[0]);
				pVertex[1].set((end + leftOffset - gutterOffset), headBaseColor);
				pVertex[2].set((end + leftOffset - gutterOffset + edgeOffset), headBaseColor);

				pVertex[3].set(end, colors[1]);

				pVertex[4].set((end - leftOffset - gutterOffset - edgeOffset), headBaseColor);
				pVertex[5].set((end - leftOffset - gutterOffset), headBaseColor);
				pVertex[6].set((start2 - leftOffset), colors[0]);
			}

			{
				pIndex[0] = (indexOffset + 1);
				pIndex[1] = (indexOffset + 5);
				pIndex[2] = (indexOffset + 0);

				pIndex[3] = (indexOffset + 0);
				pIndex[4] = (indexOffset + 5);
				pIndex[5] = (indexOffset + 6);

				pIndex[6] = (indexOffset + 3);
				pIndex[7] = (indexOffset + 4);
				pIndex[8] = (indexOffset + 2);
			}

			if (startCap == LineCap::Round)
			{
				const float startAngle = std::atan2(leftOffset.x, -leftOffset.y);
				// 本体は添字で書き込んだため、pVertex と pIndex はまだ確保領域の先頭を指している。
				// 両方を本体の直後へずらした書き込み先を、始端用に渡す。
				const Vertex2DBufferPointer capBuffer{ (pVertex + VertexCount), (pIndex + IndexCount), static_cast<Vertex2D::IndexType>(indexOffset + VertexCount) };
				EmitRoundCap(capBuffer, start, halfThickness, (startAngle + Math::PiF), colors[0], capQuality);
			}

			return totalIndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircle
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircle(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const Vertex2D::IndexType Quality = CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality + 1);
			const Vertex2D::IndexType IndexCount = (FullQuality * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteEllipseFanVertices<EllipseFanType::Circle>(pVertex, center.x, center.y, r, r, Quality, colorType, color0, color1);

			WriteClosedFanIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rInner, const float thickness, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			const float rOuter = (rInner + thickness);
			const Vertex2D::IndexType Quality = CalculateCircleQuality(rOuter * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality * 2);
			const Vertex2D::IndexType IndexCount = (FullQuality * 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float centerX = center.x;
			const float centerY = center.y;

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2DWritePointer pDst0 = (pVertex + 0);
			Vertex2DWritePointer pDst1 = (pDst0 + Quality * 2);
			Vertex2DWritePointer pDst2 = (pDst0 + Quality * 4);
			Vertex2DWritePointer pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float ox = (cs.x * rOuter);
				const float ix = (cs.x * rInner);
				const float oy = (cs.y * rOuter);
				const float iy = (cs.y * rInner);

				(pDst0++)->setPosition((centerX + ox), (centerY + oy));
				(pDst0++)->setPosition((centerX + ix), (centerY + iy));

				(pDst1++)->setPosition((centerX - oy), (centerY + ox));
				(pDst1++)->setPosition((centerX - iy), (centerY + ix));

				(pDst2++)->setPosition((centerX - ox), (centerY - oy));
				(pDst2++)->setPosition((centerX - ix), (centerY - iy));

				(pDst3++)->setPosition((centerX + oy), (centerY - ox));
				(pDst3++)->setPosition((centerX + iy), (centerY - ix));
			}

			for (Vertex2D::IndexType i = 0; i < FullQuality; ++i)
			{
				(pVertex++)->setColor(outerColor);
				(pVertex++)->setColor(innerColor);
			}

			WriteClosedStripIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleDashedFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleDashedFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rInner, const float startAngle, const float thickness, const float dashRatio, const uint32 dashCount, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			const float clampedDashRatio = Clamp(dashRatio, 0.0f, 1.0f);

			if ((dashCount == 0) || (clampedDashRatio == 0.0f))
			{
				return 0;
			}

			if (clampedDashRatio == 1.0f)
			{
				return BuildCircleFrame(bufferCreator, center, rInner, thickness, innerColor, outerColor, scale);
			}

			const float rOuter = (rInner + thickness);
			const float angleStep = (Math::TwoPiF / dashCount);
			const float dashAngle = (angleStep * clampedDashRatio);
			const Vertex2D::IndexType Quality = CalculateCircleArcQuality((rOuter * scale), dashAngle);

			const size_t vertexCount = (static_cast<size_t>(dashCount) * Quality * 2);
			const size_t indexCount = (static_cast<size_t>(dashCount) * (Quality - 1) * 6);

			constexpr size_t MaxIndexValue = static_cast<size_t>(std::numeric_limits<Vertex2D::IndexType>::max());

			if ((MaxIndexValue < vertexCount) || (MaxIndexValue < indexCount))
			{
				return 0;
			}

			const Vertex2D::IndexType VertexCount = static_cast<Vertex2D::IndexType>(vertexCount);
			const Vertex2D::IndexType IndexCount = static_cast<Vertex2D::IndexType>(indexCount);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			{
				const float centerX = center.x;
				const float centerY = center.y;
				const float radDelta = (dashAngle / (Quality - 1));
				float dashStartAngle = startAngle;
				Vertex2DWritePointer pDst = pVertex;

				for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
				{
					for (Vertex2D::IndexType i = 0; i < Quality; ++i)
					{
						const float rad = (dashStartAngle + radDelta * i);
						const auto [s, c] = FastMath::SinCos(rad);

						(pDst++)->set((centerX + rInner * s), (centerY - rInner * c), innerColor);
						(pDst++)->set((centerX + rOuter * s), (centerY - rOuter * c), outerColor);
					}

					dashStartAngle += angleStep;
				}
			}

			for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
			{
				const Vertex2D::IndexType vertexOffset = (indexOffset + static_cast<Vertex2D::IndexType>(static_cast<size_t>(dashIndex) * Quality * 2));

				WriteOpenStripIndices(pIndex, vertexOffset, Quality);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCirclePie
		//
		////////////////////////////////////////////////////////////////

		template <bool Textured>
		SIV3D_ALWAYS_INLINE
		static inline Vertex2D::IndexType BuildCirclePie(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor, const float scale, const FloatRect& uv = {})
		{
			if (angle == 0.0f)
			{
				return 0;
			}

			const Vertex2D::IndexType Quality = CalculateCircleArcQuality((r * scale), Abs(angle));
			const Vertex2D::IndexType VertexCount = (Quality + 1);
			const Vertex2D::IndexType IndexCount = ((Quality - 1) * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float centerX = center.x;
			const float centerY = center.y;

			if constexpr (Textured)
			{
				const float centerU = ((uv.left + uv.right) * 0.5f);
				const float centerV = ((uv.top + uv.bottom) * 0.5f);
				const float rU = ((uv.right - uv.left) * 0.5f);
				const float rV = ((uv.bottom - uv.top) * 0.5f);
				pVertex[0].set(centerX, centerY, centerU, centerV, innerColor);

				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = (Abs(angle) / (Quality - 1));
				WriteArcVertices((pVertex + 1), centerX, centerY, r, r, start, radDelta, Quality,
					[&](Vertex2DWriteReference vertex, const float x, const float y, float, const float s, const float c)
					{
						vertex.set(x, y, (centerU + rU * s), (centerV - rV * c), outerColor);
					});
			}
			else
			{
				// 中心
				pVertex[0].setPosition(centerX, centerY);

				// 周
				{
					const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
					const float radDelta = (Abs(angle) / (Quality - 1));
					WriteArcPositions((pVertex + 1), centerX, centerY, r, r, start, radDelta, Quality);
				}

				{
					(pVertex++)->setColor(innerColor);

					for (size_t i = 1; i < VertexCount; ++i)
					{
						(pVertex++)->setColor(outerColor);
					}
				}
			}

			WriteOpenFanIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		Vertex2D::IndexType BuildCirclePie(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			return BuildCirclePie<false>(bufferCreator, center, r, startAngle, angle, innerColor, outerColor, scale);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleArc
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleArc(const BufferCreatorFunc& bufferCreator, const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			// Keep the default Flat path free of cap geometry and gradient work.
			if (lineCap != LineCap::Flat)
			{
				if (lineCap == LineCap::Round)
				{
					return BuildRoundCircleArc(bufferCreator, center, rInner, startAngle, angle, thickness, colorType, color0, color1, scale);
				}
				if (Abs(angle) < Math::TwoPiF)
				{
					return BuildCircleArcStrip<true>(bufferCreator, center, rInner, startAngle, angle, thickness, colorType, color0, color1, scale);
				}
			}
			if (angle == 0.0f)
			{
				return 0;
			}
			// A full Square arc has no endpoints to extend.
			return BuildCircleArcStrip<false>(bufferCreator, center, rInner, startAngle, angle, thickness, colorType, color0, color1, scale);
		}

		Vertex2D::IndexType BuildCircleArc(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap,
			const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness,
			const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			if (startCap == endCap)
			{
				return BuildCircleArc(bufferCreator, startCap, center, rInner, startAngle, angle, thickness, colorType, color0, color1, scale);
			}
			if (thickness <= 0.0f) return 0;
			const bool reverse = (angle < 0.0f);
			LineCap firstCap = (reverse ? endCap : startCap), lastCap = (reverse ? startCap : endCap);
			if (Abs(angle) >= Math::TwoPiF)
			{
				if (firstCap == LineCap::Square) firstCap = LineCap::Flat;
				if (lastCap == LineCap::Square) lastCap = LineCap::Flat;
			}
			const float halfWidth = (thickness * 0.5f), radius = (rInner + halfWidth);
			const auto quality = ((angle != 0.0f) ? CalculateCircleArcQuality((rInner + thickness) * scale, Abs(angle)) : Vertex2D::IndexType{ 1 });
			const auto capQuality = (((firstCap == LineCap::Round) || (lastCap == LineCap::Round))
				? CalculateCircleSegmentQuality(halfWidth * scale, Math::PiF) : Vertex2D::IndexType{ 0 });
			const auto firstVertices = ArcCapVertexCount(firstCap, capQuality), lastVertices = ArcCapVertexCount(lastCap, capQuality);
			const auto firstIndices = ArcCapIndexCount(firstCap, capQuality), lastIndices = ArcCapIndexCount(lastCap, capQuality);
			const auto bodyVertices = ((angle != 0.0f) ? quality * 2 : 0);
			const auto bodyIndices = ((quality - 1) * 6);
			const Vertex2D::IndexType indexCount = (firstIndices + bodyIndices + lastIndices);
			if (indexCount == 0) return 0;
			const auto buffer = bufferCreator(static_cast<uint16>(firstVertices + bodyVertices + lastVertices), indexCount);
			if (not buffer.pVertex) return 0;

			const Float4& firstColor = (reverse ? color1 : color0);
			const Float4& lastColor = (reverse ? color0 : color1);
			const bool along = (colorType == ColorFillDirection::LeftRight);
			Float4 bodyFirst = color0, bodyLast = color1;
			if (along)
			{
				const float firstLength = ((firstCap != LineCap::Flat) ? halfWidth : 0.0f);
				const float length = (radius * Abs(angle) + firstLength + ((lastCap != LineCap::Flat) ? halfWidth : 0.0f));
				bodyFirst = firstColor.lerp(lastColor, firstLength / length);
				bodyLast = firstColor.lerp(lastColor, (firstLength + radius * Abs(angle)) / length);
			}
			const float start = (startAngle + (reverse ? angle : 0.0f));
			const auto [s0, c0] = FastMath::SinCos(start);
			const auto [s1, c1] = FastMath::SinCos(start + Abs(angle));
			const Float2 normal0{ s0, -c0 }, normal1{ s1, -c1 };
			EmitArcEndCap(buffer, firstCap, center + normal0 * radius, normal0, halfWidth, false, colorType,
				(along ? bodyFirst : color0), (along ? bodyFirst : color1), firstColor, capQuality);
			if (bodyVertices)
			{
				auto vertices = (buffer.pVertex + firstVertices);
				auto indices = (buffer.pIndex + firstIndices);
				WriteArcStripPositions(vertices, center.x, center.y, rInner, (rInner + thickness), start, (Abs(angle) / (quality - 1)), quality);
				if (along) WriteArcStripAlongColors(vertices, quality, bodyFirst, ((bodyLast - bodyFirst) / (quality - 1)));
				else WriteArcStripRadialColors(vertices, quality, color0, color1);
				WriteOpenStripIndices(indices, static_cast<Vertex2D::IndexType>(buffer.indexOffset + firstVertices), quality);
			}
			EmitArcEndCap({ buffer.pVertex + firstVertices + bodyVertices, buffer.pIndex + firstIndices + bodyIndices,
				static_cast<Vertex2D::IndexType>(buffer.indexOffset + firstVertices + bodyVertices) }, lastCap,
				center + normal1 * radius, normal1, halfWidth, true, colorType,
				(along ? bodyLast : color0), (along ? bodyLast : color1), lastColor, capQuality);
			return indexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleSegment
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleSegment(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const float angle, const Float4& color, const float scale)
		{
			if (angle == 0.0f)
			{
				return 0;
			}

			const Vertex2D::IndexType Quality = CalculateCircleSegmentQuality((r * scale), Abs(angle));
			const Vertex2D::IndexType VertexCount = (Quality + 1);
			const Vertex2D::IndexType IndexCount = ((Quality - 1) * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float centerX = center.x;
			const float centerY = center.y;

			{
				const auto [s1, c1] = FastMath::SinCos(startAngle);
				const Float2 p1{ s1, -c1 };

				const auto [s2, c2] = FastMath::SinCos(startAngle + angle);
				const Float2 p2{ s2, -c2 };

				const Float2 mid = ((p1 + p2) * 0.5f * r);

				// 中心
				pVertex[0].setPosition((mid + Float2{ centerX, centerY }));
			}

			// 周
			{
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = (Abs(angle) / (Quality - 1));
				WriteArcPositions((pVertex + 1), centerX, centerY, r, r, start, radDelta, Quality);
			}

			{
				for (size_t i = 0; i < VertexCount; ++i)
				{
					(pVertex++)->setColor(color);
				}
			}

			WriteOpenFanIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipse
		//
		////////////////////////////////////////////////////////////////

		template <bool Rotated>
		SIV3D_ALWAYS_INLINE
		static inline Vertex2D::IndexType BuildEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale, const float rotationCos = 1.0f, const float rotationSin = 0.0f)
		{
			const float majorAxis = Max(a, b);
			const Vertex2D::IndexType Quality = CalculateCircleQuality(majorAxis * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality + 1);
			const Vertex2D::IndexType IndexCount = (FullQuality * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteEllipseFanVertices<Rotated ? EllipseFanType::RotatedEllipse : EllipseFanType::Ellipse>(pVertex, center.x, center.y, a, b, Quality, colorType, color0, color1, {}, rotationCos, rotationSin);

			WriteClosedFanIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		Vertex2D::IndexType BuildEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			return BuildEllipse<false>(bufferCreator, center, a, b, colorType, color0, color1, scale);
		}

		Vertex2D::IndexType BuildEllipseRotated(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float rotation, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const auto [s, c] = FastMath::SinCos(rotation);
			return BuildEllipse<true>(bufferCreator, center, a, b, colorType, color0, color1, scale, c, s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseFrame
		//
		////////////////////////////////////////////////////////////////

		template <bool Rotated>
		SIV3D_ALWAYS_INLINE
		static inline Vertex2D::IndexType BuildEllipseFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float innerThickness,	const float outerThickness,	const Float4& innerColor, const Float4& outerColor, const float scale, const float rotationCos = 1.0f, const float rotationSin = 0.0f)
		{
			if ((innerThickness <= 0.0f) && (outerThickness <= 0.0f))
			{
				return 0;
			}

			const float majorAxis = (Max(a, b) + Max(Abs(innerThickness), Abs(outerThickness)));
			const Vertex2D::IndexType Quality = CalculateCircleQuality(majorAxis * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality * 2);
			const Vertex2D::IndexType IndexCount = (FullQuality * 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float centerX = center.x;
			const float centerY = center.y;

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2DWritePointer pDst0 = (pVertex + 0);
			Vertex2DWritePointer pDst1 = (pDst0 + Quality * 2);
			Vertex2DWritePointer pDst2 = (pDst0 + Quality * 4);
			Vertex2DWritePointer pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float ux = cs.x;
				const float uy = cs.y;

				SetEllipseFrameVertexPair<Rotated>(
					pDst0,
					centerX,
					centerY,
					a,
					b,
					ux,
					uy,
					innerThickness,
					outerThickness, rotationCos, rotationSin);

				SetEllipseFrameVertexPair<Rotated>(
					pDst1,
					centerX,
					centerY,
					a,
					b,
					-uy,
					ux,
					innerThickness,
					outerThickness, rotationCos, rotationSin);

				SetEllipseFrameVertexPair<Rotated>(
					pDst2,
					centerX,
					centerY,
					a,
					b,
					-ux,
					-uy,
					innerThickness,
					outerThickness, rotationCos, rotationSin);

				SetEllipseFrameVertexPair<Rotated>(
					pDst3,
					centerX,
					centerY,
					a,
					b,
					uy,
					-ux,
					innerThickness,
					outerThickness, rotationCos, rotationSin);
			}

			for (Vertex2D::IndexType i = 0; i < FullQuality; ++i)
			{
				(pVertex++)->setColor(outerColor);
				(pVertex++)->setColor(innerColor);
			}

			WriteClosedStripIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		Vertex2D::IndexType BuildEllipseFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float innerThickness,	const float outerThickness,	const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			return BuildEllipseFrame<false>(bufferCreator, center, a, b, innerThickness, outerThickness, innerColor, outerColor, scale);
		}

		Vertex2D::IndexType BuildEllipseFrameRotated(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float rotation, const float innerThickness,	const float outerThickness,	const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			const auto [s, c] = FastMath::SinCos(rotation);
			return BuildEllipseFrame<true>(bufferCreator, center, a, b, innerThickness, outerThickness, innerColor, outerColor, scale, c, s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseDashedFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildEllipseDashedFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float innerThickness, const float outerThickness, const float offset, const float dashRatio, const uint32 dashCount, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			if ((a <= 0.0f) || (b <= 0.0f) || ((innerThickness <= 0.0f) && (outerThickness <= 0.0f)))
			{
				return 0;
			}

			const float clampedDashRatio = Clamp(dashRatio, 0.0f, 1.0f);

			if ((dashCount == 0) || (clampedDashRatio == 0.0f))
			{
				return 0;
			}

			if (clampedDashRatio == 1.0f)
			{
				return BuildEllipseFrame(bufferCreator, center, a, b, innerThickness, outerThickness, innerColor, outerColor, scale);
			}

			constexpr size_t MaxIndexValue = static_cast<size_t>(std::numeric_limits<Vertex2D::IndexType>::max());
			constexpr size_t MaxDashCount = (MaxIndexValue / 6);

			if (MaxDashCount < dashCount)
			{
				return 0;
			}

			const float majorAxis = (Max(a, b) + Max(Abs(innerThickness), Abs(outerThickness)));
			const Vertex2D::IndexType baseQuality = CalculateCircleQuality(majorAxis * scale);
			const Vertex2D::IndexType fullQuality = (baseQuality * 4);
			const size_t requestedTableSegmentCount = Max(static_cast<size_t>(64), (static_cast<size_t>(fullQuality) * 8));
			const size_t tableSegmentCount = Min(requestedTableSegmentCount, MaxEllipseArcLengthTableSegmentCount);

			const auto arcLengthTable = GetEllipseArcLengthTable(a, b, tableSegmentCount);
			const float perimeter = arcLengthTable.back();

			if (perimeter <= 0.0f)
			{
				return 0;
			}

			const float periodLength = (perimeter / dashCount);
			const float dashLength = (periodLength * clampedDashRatio);
			const float segmentLength = (perimeter / fullQuality);
			const Vertex2D::IndexType pointCountPerDash = static_cast<Vertex2D::IndexType>(Max((std::ceil(dashLength / segmentLength) + 1.0f), 5.0f));

			float wrappedOffset = std::fmod(offset, perimeter);

			if (wrappedOffset < 0.0f)
			{
				wrappedOffset += perimeter;
			}

			std::array<uint8, ((MaxDashCount + 7) / 8)> shortDashes{};
			size_t shortDashCount = 0;

			if (dashLength <= segmentLength)
			{
				const float angleDelta = (Math::TwoPiF / fullQuality);
				const float halfAngleSin = std::sin(angleDelta * 0.5f);
				const float maxNormalDistanceSq = (4.0f * halfAngleSin * halfAngleSin);

				for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
				{
					const float startLength = (wrappedOffset + periodLength * dashIndex);
					const float start = CalculateEllipseParameterAtLength(arcLengthTable, startLength, perimeter);
					const float end = CalculateEllipseParameterAtLength(arcLengthTable, (startLength + dashLength), perimeter);

					if (angleDelta < (end - start))
					{
						continue;
					}

					const auto [s0, c0] = FastMath::SinCos(start);
					const auto [s1, c1] = FastMath::SinCos(end);
					const Float2 normal0 = CalculateEllipseNormal(a, b, s0, -c0);
					const Float2 normal1 = CalculateEllipseNormal(a, b, s1, -c1);

					// 中心線と法線の両方を通常の円の 1 区間以内に制限し、扁平な楕円の先端を保護する。
					if (normal0.distanceFromSq(normal1) <= maxNormalDistanceSq)
					{
						shortDashes[dashIndex / 8] |= static_cast<uint8>(1u << (dashIndex % 8));
						++shortDashCount;
					}
				}
			}

			const size_t pointCount = ((static_cast<size_t>(dashCount) * pointCountPerDash) - shortDashCount * (pointCountPerDash - 2));
			const size_t vertexCount = (pointCount * 2);
			const size_t indexCount = ((pointCount - dashCount) * 6);

			if ((MaxIndexValue < vertexCount) || (MaxIndexValue < indexCount))
			{
				return 0;
			}

			const Vertex2D::IndexType VertexCount = static_cast<Vertex2D::IndexType>(vertexCount);
			const Vertex2D::IndexType IndexCount = static_cast<Vertex2D::IndexType>(indexCount);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			{
				const float centerX = center.x;
				const float centerY = center.y;
				Vertex2DWritePointer pDst = pVertex;

				for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
				{
					const float dashStartLength = (wrappedOffset + periodLength * dashIndex);
					const bool shortDash = ((shortDashes[dashIndex / 8] & (1u << (dashIndex % 8))) != 0);
					const Vertex2D::IndexType dashPointCount = (shortDash ? 2 : pointCountPerDash);
					const Vertex2D::IndexType vertexOffset = (indexOffset + static_cast<Vertex2D::IndexType>(pDst - pVertex));

					for (Vertex2D::IndexType i = 0; i < dashPointCount; ++i)
					{
						const float f = (static_cast<float>(i) / (dashPointCount - 1));
						const float rad = CalculateEllipseParameterAtLength(arcLengthTable, (dashStartLength + dashLength * f), perimeter);
						const auto [s, c] = FastMath::SinCos(rad);
						const float ux = s;
						const float uy = -c;

						SetEllipseFrameVertexPair(
							pDst,
							centerX,
							centerY,
							a,
							b,
							ux,
							uy,
							innerThickness,
							outerThickness);

						pDst[-2].setColor(outerColor);
						pDst[-1].setColor(innerColor);
					}

					WriteOpenStripIndices(pIndex, vertexOffset, dashPointCount);
				}
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipseArc
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildEllipseArc(const BufferCreatorFunc& bufferCreator, LineCap startCap, LineCap endCap,
			const Float2& center, const float a, const float b, const float startAngle, const float angle,
			const float innerThickness, const float outerThickness, const ColorFillDirection colorType,
			const Float4& color0, const Float4& color1, const float scale)
		{
			const float thickness = (innerThickness + outerThickness);
			if ((a <= 0.0f) || (b <= 0.0f) || (thickness <= 0.0f)) return 0;
			const float sweep = Abs(angle);
			if (sweep >= Math::TwoPiF) startCap = endCap = LineCap::Flat;
			if (a == b)
			{
				return BuildCircleArc(bufferCreator, startCap, endCap, center, (a - innerThickness), startAngle, angle, thickness, colorType, color0, color1, scale);
			}
			const bool reverse = (angle < 0.0f), along = (colorType == ColorFillDirection::LeftRight);
			const LineCap firstCap = (reverse ? endCap : startCap), lastCap = (reverse ? startCap : endCap);
			if ((angle == 0.0f) && (firstCap == LineCap::Flat) && (lastCap == LineCap::Flat)) return 0;
			const float start = (startAngle + (reverse ? angle : 0.0f));
			const float halfWidth = (thickness * 0.5f), middleOffset = ((outerThickness - innerThickness) * 0.5f);
			std::array<float, (MaxCircleSegments + 1)> parameters;
			uint16 pointCount = 1;
			parameters[0] = start;
			if (sweep > 0.0f)
			{
				const float offsetRadius = Max(Abs(innerThickness), Abs(outerThickness));
				uint16 positionIntervals = (CalculateCircleArcQuality((Max(a, b) + offsetRadius) * scale, sweep) - 1);
				const auto [s0, c0] = FastMath::SinCos(start);
				const auto [s1, c1] = FastMath::SinCos(start + sweep);
				const float normalStart = std::atan2(b * s0, a * c0);
				float normalSweep = (std::atan2(b * s1, a * c1) - normalStart);
				if (sweep >= Math::TwoPiF) normalSweep = Math::TwoPiF;
				else if (normalSweep < 0.0f) normalSweep += Math::TwoPiF;
				uint16 normalIntervals = (CalculateCircleArcQuality(offsetRadius * scale, normalSweep) - 1);
				// Distribute the bounded budget over both position and normal-angle grids.
				if ((positionIntervals + normalIntervals) > MaxCircleSegments)
				{
					positionIntervals = static_cast<uint16>((MaxCircleSegments * positionIntervals) / (positionIntervals + normalIntervals));
					normalIntervals = (MaxCircleSegments - positionIntervals);
				}
				for (uint16 i = 1; i <= positionIntervals; ++i)
				{
					parameters[pointCount++] = (start + sweep * (float(i) / positionIntervals));
				}
				const float wrappedStart = std::atan2(s0, c0);
				for (uint16 i = 1; i < normalIntervals; ++i)
				{
					const auto [s, c] = FastMath::SinCos(normalStart + normalSweep * (float(i) / normalIntervals));
					float delta = (std::atan2(a * s, b * c) - wrappedStart);
					if (delta < 0.0f) delta += Math::TwoPiF;
					if ((0.0f < delta) && (delta < sweep)) parameters[pointCount++] = (start + delta);
				}
				std::sort(parameters.begin(), parameters.begin() + pointCount);
				pointCount = static_cast<uint16>(std::unique(parameters.begin(), parameters.begin() + pointCount) - parameters.begin());
			}
			const auto capQuality = (((firstCap == LineCap::Round) || (lastCap == LineCap::Round))
				? CalculateCircleSegmentQuality(halfWidth * scale, Math::PiF) : Vertex2D::IndexType{ 0 });
			const auto firstVertices = ArcCapVertexCount(firstCap, capQuality), lastVertices = ArcCapVertexCount(lastCap, capQuality);
			const auto firstIndices = ArcCapIndexCount(firstCap, capQuality), lastIndices = ArcCapIndexCount(lastCap, capQuality);
			const auto bodyVertices = ((pointCount > 1) ? pointCount * 2 : 0);
			const auto bodyIndices = ((pointCount - 1) * 6);
			const Vertex2D::IndexType indexCount = (firstIndices + bodyIndices + lastIndices);
			const auto buffer = bufferCreator(static_cast<uint16>(firstVertices + bodyVertices + lastVertices), indexCount);
			if (not buffer.pVertex) return 0;
			auto vertices = (buffer.pVertex + firstVertices);
			Float2 firstNormal, lastNormal, firstCenter, lastCenter, previous;
			std::array<float, (MaxCircleSegments + 1)> lengths;
			float bodyLength = 0.0f;
			for (uint16 i = 0; i < pointCount; ++i)
			{
				const auto [s, c] = FastMath::SinCos(parameters[i]);
				const Float2 normal = CalculateEllipseNormal(a, b, s, -c);
				const Float2 base{ center.x + a * s, center.y - b * c };
				const Float2 middle = (base + normal * middleOffset);
				if (i == 0) { firstNormal = normal; firstCenter = middle; }
				if (i == (pointCount - 1)) { lastNormal = normal; lastCenter = middle; }
				if (along)
				{
					if (i) bodyLength += middle.distanceFrom(previous);
					lengths[i] = bodyLength;
					previous = middle;
				}
				if (bodyVertices)
				{
					vertices[i * 2].set(base - normal * innerThickness, color0);
					vertices[i * 2 + 1].set(base + normal * outerThickness, color1);
				}
			}
			const Float4& firstColor = (reverse ? color1 : color0);
			const Float4& lastColor = (reverse ? color0 : color1);
			Float4 bodyFirst = color0, bodyLast = color1;
			if (along)
			{
				const float firstLength = ((firstCap != LineCap::Flat) ? halfWidth : 0.0f);
				const float totalLength = (firstLength + bodyLength + ((lastCap != LineCap::Flat) ? halfWidth : 0.0f));
				const float inverseLength = ((totalLength > 0.0f) ? (1.0f / totalLength) : 0.0f);
				bodyFirst = firstColor.lerp(lastColor, firstLength * inverseLength);
				bodyLast = firstColor.lerp(lastColor, (firstLength + bodyLength) * inverseLength);
				for (uint16 i = 0; bodyVertices && (i < pointCount); ++i)
				{
					const Float4 color = firstColor.lerp(lastColor, (firstLength + lengths[i]) * inverseLength);
					vertices[i * 2].setColor(color);
					vertices[i * 2 + 1].setColor(color);
				}
			}
			EmitArcEndCap(buffer, firstCap, firstCenter, firstNormal, halfWidth, false, colorType,
				(along ? bodyFirst : color0), (along ? bodyFirst : color1), firstColor, capQuality);
			auto indices = (buffer.pIndex + firstIndices);
			WriteOpenStripIndices(indices, static_cast<Vertex2D::IndexType>(buffer.indexOffset + firstVertices), pointCount);
			EmitArcEndCap({ buffer.pVertex + firstVertices + bodyVertices, buffer.pIndex + firstIndices + bodyIndices,
				static_cast<Vertex2D::IndexType>(buffer.indexOffset + firstVertices + bodyVertices) }, lastCap, lastCenter, lastNormal,
				halfWidth, true, colorType, (along ? bodyLast : color0), (along ? bodyLast : color1), lastColor, capQuality);
			return indexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildEllipsePie
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildEllipsePie(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rx, const float ry, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			if (angle == 0.0f)
			{
				return 0;
			}

			const Vertex2D::IndexType Quality = CalculateCircleArcQuality((Max(rx, ry) * scale), Abs(angle));

			const Vertex2D::IndexType VertexCount = (Quality + 1);
			const Vertex2D::IndexType IndexCount = ((Quality - 1) * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);
			if (not pVertex)
			{
				return 0;
			}

			const float centerX = center.x;
			const float centerY = center.y;

			// 中心
			pVertex[0].setPosition(centerX, centerY);

			// 周（楕円）
			{
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = (Abs(angle) / (Quality - 1));
				WriteArcPositions((pVertex + 1), centerX, centerY, rx, ry, start, radDelta, Quality);
			}

			// 色
			{
				(pVertex++)->setColor(innerColor);

				for (size_t i = 1; i < VertexCount; ++i)
				{
					(pVertex++)->setColor(outerColor);
				}
			}

			WriteOpenFanIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildSuperEllipse
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildSuperEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float n, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			detail::SuperEllipseTessellation::Quarter quarter;
			detail::SuperEllipseTessellation::Tessellate(quarter, a, b, n, scale);
			const auto FullQuality = static_cast<Vertex2D::IndexType>((quarter.count - 1) * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality + 1);
			const Vertex2D::IndexType IndexCount = (FullQuality * 3);

			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);
			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].setPosition(center);
			const Float4 colorDiff = (color1 - color0);
			const float invW = (1.0f / (2.0f * a));
			const float invH = (1.0f / (2.0f * b));
			Vertex2DWritePointer destination = (pVertex + 1);
			detail::SuperEllipseTessellation::VisitContour(quarter, [&](const Vec2 point)
			{
				const Float2 offset{ point };
				Float4 color = color1;
				if (colorType == ColorFillDirection::TopBottom)
				{
					color = (color0 + ((offset.y + b) * invH) * colorDiff);
				}
				else if (colorType == ColorFillDirection::LeftRight)
				{
					color = (color0 + ((offset.x + a) * invW) * colorDiff);
				}
				destination->set((center.x + offset.x), (center.y + offset.y), color);
				++destination;
			});

			// 中心の色
			if (colorType == ColorFillDirection::InOut)
			{
				pVertex->setColor(color0);
			}
			else
			{
				pVertex->setColor(color0.getMidpoint(color1));
			}

			WriteClosedFanIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRect
		//
		////////////////////////////////////////////////////////////////

		uint32 BuildRoundRectBalloon(const BufferCreatorFunc& bufferCreator, const detail::RoundRectBalloon::Placement& balloon, const Float4& color, const float scale)
		{
			detail::RoundRectBalloon::Contour contour;
			detail::RoundRectBalloon::MakeContour(contour, balloon, scale);
			const uint16 n = contour.count, tip = contour.tipIndex;
			const uint32 indexCount = (n * 3);
			auto [vertices, indices, offset] = bufferCreator(n + 1, indexCount);
			if (not vertices) return 0;
			vertices[0].set(balloon.body.rect.center(), color);
			for (uint16 i = 0; i < n; ++i) vertices[i + 1].set(contour.vertices[i], color);
			// The convex body fan skips the tip. Its base and the tail share an edge,
			// so neither triangulation nor upload-destination reads are necessary.
			for (uint16 i = 0; i < n; ++i)
			{
				if (i == tip) continue;
				uint16 next = ((i + 1 == n) ? 0 : (i + 1));
				if (next == tip) next = ((next + 1 == n) ? 0 : (next + 1));
				*indices++ = offset;
				*indices++ = (offset + i + 1);
				*indices++ = (offset + next + 1);
			}
			*indices++ = (offset + tip);
			*indices++ = (offset + tip + 1);
			*indices++ = (offset + ((tip + 1 == n) ? 0 : (tip + 1)) + 1);
			return indexCount;
		}

		uint32 BuildRoundRectBalloonFrame(const BufferCreatorFunc& bufferCreator, const detail::RoundRectBalloon::Placement& balloon, const float thickness, const Float4& color, const float scale)
		{
			detail::RoundRectBalloon::Contour contour;
			detail::RoundRectBalloon::MakeContour(contour, balloon, scale);
			return BuildLineString(bufferCreator, LineCap::Flat, LineCap::Flat, contour.points(), none,
				thickness, false, CloseRing::Yes, color, scale);
		}

		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float r, const Float4& color, const float scale)
		{
			const Vertex2D::IndexType Quality				= CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FanOuterVertexCount	= (Quality + 1); // 1 つの扇型の曲線部分の頂点数
			const Vertex2D::IndexType FanVertexCount		= (FanOuterVertexCount + 1); // 1 つの扇型の頂点数
			const Vertex2D::IndexType VertexCount			= (FanVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount			= ((Quality * 3 * 4) + (6 * 3)); // インデックス数
			auto [pVertex, pIndex, indexOffset]				= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteRoundRectVertices<false>(pVertex, rect, r, Quality, ColorFillDirection::TopBottom, color, color,
				[](Vertex2DWriteReference vertex, const float x, const float y, const Float4& c) { vertex.set(x, y, c); });

			WriteRoundRectIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}
		
		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const std::array<float, 4>& radii, const Float4& color, const float scale)
		{
			std::array<Vertex2D::IndexType, 4> qualities;
			Vertex2D::IndexType vertexCount = 4;
			for (size_t corner = 0; corner < 4; ++corner)
			{
				qualities[corner] = (radii[corner] ? CalculateCircleQuality(radii[corner] * scale) : 0);
				vertexCount += qualities[corner];
			}

			const Vertex2D::IndexType indexCount = ((vertexCount - 2) * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexCount, indexCount);
			if (not pVertex)
			{
				return 0;
			}

			WriteRoundedRectContour(pVertex, rect, radii, qualities, color, 1);
			// Touching arc endpoints remain as degenerate triangles. Keeping them
			// avoids a compaction pass and keeps reservation/submission counts equal.
			WriteOpenFanIndices(pIndex, indexOffset, (vertexCount - 1));
			return indexCount;
		}

		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float r, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const Vertex2D::IndexType Quality				= CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FanOuterVertexCount	= (Quality + 1); // 1 つの扇型の曲線部分の頂点数
			const Vertex2D::IndexType FanVertexCount		= (FanOuterVertexCount + 1); // 1 つの扇型の頂点数
			const Vertex2D::IndexType VertexCount			= (FanVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount			= ((Quality * 3 * 4) + (6 * 3)); // インデックス数
			auto [pVertex, pIndex, indexOffset]				= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteRoundRectVertices<true>(pVertex, rect, r, Quality, colorType, color0, color1,
				[](Vertex2DWriteReference vertex, const float x, const float y, const Float4& c) { vertex.set(x, y, c); });

			WriteRoundRectIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRoundRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const Float4& color, const float scale)
		{
			const Vertex2D::IndexType Quality = CalculateCircleQuality(outerR * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType ArcOuterVertexCount = (Quality + 1); // 1 つの弧の外側曲線部分の頂点数
			const Vertex2D::IndexType ArcInnerVertexCount = (Quality + 1); // 1 つの弧の内側曲線部分の頂点数
			const Vertex2D::IndexType ArcVertexCount = (ArcOuterVertexCount + ArcInnerVertexCount); // 1 つの弧の頂点数
			const Vertex2D::IndexType ArcTriangleCount = (Quality * 2); // 1 つの弧の三角形の数
			const Vertex2D::IndexType ArcIndexCount = (ArcTriangleCount * 3); // 1 つの弧のインデックス数
			const Vertex2D::IndexType VertexCount = (ArcVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount = (ArcIndexCount * 4 + (6 * 4)); // インデックス数

			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteRoundRectFrameVertices<false>(pVertex, innerRect, innerR, outerRect, outerR, Quality, ColorFillDirection::TopBottom, color, color);

			WriteRoundRectFrameIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		Vertex2D::IndexType BuildRoundRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, const std::array<float, 4>& innerRadii, const FloatRect& outerRect, const std::array<float, 4>& outerRadii, const Float4& color, const float scale)
		{
			std::array<Vertex2D::IndexType, 4> qualities;
			Vertex2D::IndexType contourCount = 4;
			for (size_t corner = 0; corner < 4; ++corner)
			{
				qualities[corner] = (outerRadii[corner] ? CalculateCircleQuality(outerRadii[corner] * scale) : 0);
				contourCount += qualities[corner];
			}

			const Vertex2D::IndexType vertexCount = (contourCount * 2);
			const Vertex2D::IndexType indexCount = (contourCount * 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexCount, indexCount);
			if (not pVertex)
			{
				return 0;
			}

			WriteRoundedRectContour(pVertex, outerRect, outerRadii, qualities, color, 2);
			WriteRoundedRectContour((pVertex + 1), innerRect, innerRadii, qualities, color, 2);
			for (Vertex2D::IndexType i = 0; i < contourCount; ++i)
			{
				const Vertex2D::IndexType current = (indexOffset + i * 2);
				const Vertex2D::IndexType next = (indexOffset + ((i + 1 == contourCount) ? 0 : ((i + 1) * 2)));
				*pIndex++ = current;
				*pIndex++ = next;
				*pIndex++ = (current + 1);
				*pIndex++ = (current + 1);
				*pIndex++ = next;
				*pIndex++ = (next + 1);
			}

			return indexCount;
		}

		Vertex2D::IndexType BuildRoundRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, float innerR, const FloatRect& outerRect, float outerR, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const Vertex2D::IndexType Quality = CalculateCircleQuality(outerR * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType ArcOuterVertexCount = (Quality + 1); // 1 つの弧の外側曲線部分の頂点数
			const Vertex2D::IndexType ArcInnerVertexCount = (Quality + 1); // 1 つの弧の内側曲線部分の頂点数
			const Vertex2D::IndexType ArcVertexCount = (ArcOuterVertexCount + ArcInnerVertexCount); // 1 つの弧の頂点数
			const Vertex2D::IndexType ArcTriangleCount = (Quality * 2); // 1 つの弧の三角形の数
			const Vertex2D::IndexType ArcIndexCount = (ArcTriangleCount * 3); // 1 つの弧のインデックス数
			const Vertex2D::IndexType VertexCount = (ArcVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount = (ArcIndexCount * 4 + (6 * 4)); // インデックス数

			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteRoundRectFrameVertices<true>(pVertex, innerRect, innerR, outerRect, outerR, Quality, colorType, color0, color1);

			WriteRoundRectFrameIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectDashedFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRoundRectDashedFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, const float innerR, const FloatRect& outerRect, const float outerR, const float offset, const float dashRatio, const uint32 dashCount, const Float4& color, const float scale)
		{
			const float clampedDashRatio = Clamp(dashRatio, 0.0f, 1.0f);

			if ((dashCount == 0) || (clampedDashRatio == 0.0f))
			{
				return 0;
			}

			if (clampedDashRatio == 1.0f)
			{
				return BuildRoundRectFrame(bufferCreator, innerRect, innerR, outerRect, outerR, color, scale);
			}

			constexpr float HalfPi = (Math::TwoPiF * 0.25f);

			const float iL = innerRect.left;
			const float iT = innerRect.top;
			const float iR = innerRect.right;
			const float iB = innerRect.bottom;
			const float oL = outerRect.left;
			const float oT = outerRect.top;
			const float oR = outerRect.right;
			const float oB = outerRect.bottom;
			const float irad = innerR;
			const float orad = outerR;

			// 周長は内側・外側の中間線で測る（直線部の長さは内外で一致、弧部は平均半径で算出）。
			// 構成要素は時計回りに 8 個: TopStraight, TR弧, RightStraight, BR弧, BottomStraight, BL弧, LeftStraight, TL弧。
			// 添字が奇数の要素が弧。r→0 で BuildRectDashedFrame と同じ並びに収束する。
			const float horizontalStraightMid = ((((iR - irad) - (iL + irad)) + ((oR - orad) - (oL + orad))) * 0.5f);
			const float verticalStraightMid = ((((iB - irad) - (iT + irad)) + ((oB - orad) - (oT + orad))) * 0.5f);
			const float arcMid = (HalfPi * (irad + orad) * 0.5f);

			const float componentLengths[8] =
			{
				horizontalStraightMid, arcMid, verticalStraightMid, arcMid,
				horizontalStraightMid, arcMid, verticalStraightMid, arcMid,
			};

			float cumulativeLengths[9];
			cumulativeLengths[0] = 0.0f;
			for (int i = 0; i < 8; ++i)
			{
				cumulativeLengths[i + 1] = (cumulativeLengths[i] + componentLengths[i]);
			}

			const float perimeter = cumulativeLengths[8];

			if (perimeter <= 0.0f)
			{
				return 0;
			}

			const float period = (perimeter / dashCount);
			const float dashLength = (period * clampedDashRatio);
			const Vertex2D::IndexType baseQuality = CalculateCircleQuality(orad * scale); // 四分円弧あたりの分割品質
			const float angleStep = (HalfPi / baseQuality);

			float wrappedOffset = std::fmod(offset, perimeter);

			if (wrappedOffset < 0.0f)
			{
				wrappedOffset += perimeter;
			}

			// 累積長 L（中間線）から (要素 k, 要素内パラメータ f) を求め、内側・外側頂点を返す。
			const auto EvalPoint = [&](const int k, const float f, float& ix, float& iy, float& ox, float& oy)
				{
					switch (k)
					{
					case 0: // TopStraight
						ix = ((iL + irad) + (((iR - irad) - (iL + irad)) * f)); iy = iT;
						ox = ((oL + orad) + (((oR - orad) - (oL + orad)) * f)); oy = oT;
						break;
					case 2: // RightStraight
						ix = iR; iy = ((iT + irad) + (((iB - irad) - (iT + irad)) * f));
						ox = oR; oy = ((oT + orad) + (((oB - orad) - (oT + orad)) * f));
						break;
					case 4: // BottomStraight
						ix = ((iR - irad) + (((iL + irad) - (iR - irad)) * f)); iy = iB;
						ox = ((oR - orad) + (((oL + orad) - (oR - orad)) * f)); oy = oB;
						break;
					case 6: // LeftStraight
						ix = iL; iy = ((iB - irad) + (((iT + irad) - (iB - irad)) * f));
						ox = oL; oy = ((oB - orad) + (((oT + orad) - (oB - orad)) * f));
						break;
					default: // 弧 (k = 1,3,5,7)
					{
						const float phi = (f * HalfPi);
						const auto [s, c] = FastMath::SinCos(phi);
						float icx, icy, ocx, ocy, dx, dy;

						switch (k)
						{
						case 1: icx = (iR - irad); icy = (iT + irad); ocx = (oR - orad); ocy = (oT + orad); dx = s; dy = -c; break;  // TR
						case 3: icx = (iR - irad); icy = (iB - irad); ocx = (oR - orad); ocy = (oB - orad); dx = c; dy = s; break;   // BR
						case 5: icx = (iL + irad); icy = (iB - irad); ocx = (oL + orad); ocy = (oB - orad); dx = -s; dy = c; break;  // BL
						default: icx = (iL + irad); icy = (iT + irad); ocx = (oL + orad); ocy = (oT + orad); dx = -c; dy = -s; break; // TL (k == 7)
						}

						ix = (icx + (irad * dx)); iy = (icy + (irad * dy));
						ox = (ocx + (orad * dx)); oy = (ocy + (orad * dy));
						break;
					}
					}
				};

			const auto LengthToComponent = [&](const float length, int& kOut, float& fOut)
				{
					float reduced = ((length >= perimeter) ? (length - perimeter) : length);

					if (reduced >= perimeter)
					{
						reduced -= perimeter;
					}

					if (reduced < 0.0f)
					{
						reduced = 0.0f;
					}

					int k = 7;

					for (int i = 0; i < 8; ++i)
					{
						if (reduced <= cumulativeLengths[i + 1])
						{
							k = i;
							break;
						}
					}

					const float segment = (cumulativeLengths[k + 1] - cumulativeLengths[k]);
					float f = ((segment > 0.0f) ? ((reduced - cumulativeLengths[k]) / segment) : 0.0f);
					f = Clamp(f, 0.0f, 1.0f);
					kOut = k;
					fOut = f;
				};

			// ダッシュ [begin, end]（中間線弧長）をたどり、各サンプル点の累積長を fn に渡す。
			// 直線部は端点のみ（厳密）、弧部は angleStep で細分。要素境界は共有点として一度だけ含める。
			// ダッシュは [0, 2*perimeter) に収まるため 2 周で十分。
			const auto ForEachSample = [&](const float begin, const float end, auto&& fn)
				{
					bool first = true;

					for (int lap = 0; lap < 2; ++lap)
					{
						float componentBegin = (lap * perimeter);

						for (int i = 0; i < 8; ++i)
						{
							const float length = componentLengths[i];
							const float componentEnd = (componentBegin + length);
							const float lo = Max(begin, componentBegin);
							const float hi = Min(end, componentEnd);

							if (lo < hi)
							{
								if (first)
								{
									fn(lo);
									first = false;
								}

								if ((i & 1) == 1) // 弧
								{
									const float span = (((hi - lo) / length) * HalfPi);
									int steps = static_cast<int>(std::ceil(span / angleStep));

									if (steps < 1)
									{
										steps = 1;
									}

									for (int sIndex = 1; sIndex <= steps; ++sIndex)
									{
										fn(lo + ((hi - lo) * (static_cast<float>(sIndex) / steps)));
									}
								}
								else // 直線
								{
									fn(hi);
								}
							}

							componentBegin = componentEnd;
						}
					}
				};

			// パス 1: 頂点数・インデックス数を数える（ダッシュごとにサンプル点数が異なるため）。
			size_t totalVertexPairs = 0;
			size_t totalQuads = 0;

			for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
			{
				float begin = (wrappedOffset + period * dashIndex);

				if (perimeter <= begin)
				{
					begin -= perimeter;
				}

				const float end = (begin + dashLength);

				size_t pointCount = 0;
				ForEachSample(begin, end, [&](const float) { ++pointCount; });

				if (2 <= pointCount)
				{
					totalVertexPairs += pointCount;
					totalQuads += (pointCount - 1);
				}
			}

			const size_t vertexCount = (totalVertexPairs * 2);
			const size_t indexCount = (totalQuads * 6);

			constexpr size_t MaxIndexValue = static_cast<size_t>(std::numeric_limits<Vertex2D::IndexType>::max());

			if ((vertexCount == 0) || (indexCount == 0) || (MaxIndexValue < vertexCount) || (MaxIndexValue < indexCount))
			{
				return 0;
			}

			const Vertex2D::IndexType VertexCount = static_cast<Vertex2D::IndexType>(vertexCount);
			const Vertex2D::IndexType IndexCount = static_cast<Vertex2D::IndexType>(indexCount);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			// パス 2: 頂点とインデックスを書き込む。ダッシュごとに頂点数が異なるので頂点オフセットを加算していく。
			Vertex2DWritePointer pDst = pVertex;
			Index2DWritePointer pIndexDst = pIndex;
			Vertex2D::IndexType vertexBase = indexOffset;

			for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
			{
				float begin = (wrappedOffset + period * dashIndex);

				if (perimeter <= begin)
				{
					begin -= perimeter;
				}

				const float end = (begin + dashLength);

				Vertex2D::IndexType pointCount = 0;
				ForEachSample(begin, end, [&](const float length)
					{
						int k;
						float f;
						LengthToComponent(length, k, f);

						float ix, iy, ox, oy;
						EvalPoint(k, f, ix, iy, ox, oy);

						(pDst++)->set(ix, iy, color);
						(pDst++)->set(ox, oy, color);
						++pointCount;
					});

				WriteOpenStripIndices(pIndexDst, vertexBase, pointCount);

				vertexBase += (pointCount * 2);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildShape2DFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildShape2DFrame(const BufferCreatorFunc& bufferCreator, const std::span<const Float2> vertices, const Optional<Float2>& offset, const float thickness, const Float4& color, const float scale)
		{
			const size_t num_points = vertices.size();

			if (num_points < 3)
			{
				return 0;
			}
			else if (32760 <= num_points)
			{
				return 0;
			}

			if (thickness <= 0.0f)
			{
				return 0;
			}

			PolylineBuffer& buffer = GetPolylineBuffer();
			const PreparedPolyline prepared = PreparePolyline<Float2>(vertices, offset, scale, true, true, false, buffer);

			if (prepared.points.size() < 3)
			{
				return 0;
			}

			return BuildLineStringStrip(bufferCreator, prepared, (thickness * 0.5f), true,
				[&color](size_t) { return color; });
		}

		////////////////////////////////////////////////////////////////
		//
		//	TryBuildSmallConvexFrame
		//
		////////////////////////////////////////////////////////////////

		Optional<uint32> TryBuildSmallConvexFrame(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points,
			const JoinStyle joinStyle, const double thickness, const Float4& color, const float scale)
		{
			return TryBuildSmallConvexFrame(bufferCreator, points, joinStyle, thickness * 0.5, thickness * 0.5, color, scale);
		}

		Optional<uint32> TryBuildSmallConvexFrame(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points,
			const JoinStyle joinStyle, const double innerThickness, const double outerThickness, const Float4& color, const float scale)
		{
			// The input is a unique 3/4-point ring. Rejected geometry is handled by the
			// polygon buffer path, before any renderer storage or commands are reserved.
			const size_t n = points.size();
			if (((n != 3) && (n != 4)) || (innerThickness < 0.0) || (outerThickness < 0.0)
				|| ((innerThickness == 0.0) && (outerThickness == 0.0))
				|| ((joinStyle != JoinStyle::Bevel) && (joinStyle != JoinStyle::Miter) && (joinStyle != JoinStyle::Round)))
			{
				return none;
			}

			struct Corner
			{
				Vec2 inner;
				Vec2 miter;
				Vec2 first;
				Vec2 last;
				double sweep = 0;
				uint16 count = 1;
				uint16 start = 0;
			};
			std::array<Vec2, 4> directions, normals;
			std::array<Corner, 4> corners;
			const double orientation = (((points[1] - points[0]).cross(points[2] - points[1]) > 0.0) ? 1.0 : -1.0);
			const double radius = outerThickness;
			for (size_t i = 0; i < n; ++i)
			{
				const Vec2 edge = (points[(i + 1) % n] - points[i]);
				const double length = edge.length();
				if (not (0.0 < length)) return none;
				directions[i] = (edge / length);
				normals[i] = Vec2{ directions[i].y, -directions[i].x } * orientation;
			}

			uint16 vertexCount = static_cast<uint16>(n);
			for (size_t i = 0; i < n; ++i)
			{
				const size_t prev = ((i + n - 1) % n);
				const double turn = (directions[prev].cross(directions[i]) * orientation);
				const double dot = directions[prev].dot(directions[i]);
				// Strict convexity excludes duplicate/collinear corners, concave quads,
				// and crossing quads. A reversal has no offset-line intersection.
				if ((not (0.0 < turn)) || (not (0.0 < (1.0 + dot)))) return none;
				auto& corner = corners[i];
				const Vec2 bisector = ((normals[prev] + normals[i]) / (1.0 + dot));
				corner.miter = (bisector * radius);
				corner.inner = (points[i] - bisector * innerThickness);
				corner.first = corner.last = (points[i] + corner.miter);
				// Keep 60-degree corners on the miter side despite rotation roundoff.
				if ((0.0 < radius) && (joinStyle == JoinStyle::Bevel) && (dot < (-0.5 - 1e-12)))
				{
					// Historical Siv3D bevel: below 60 degrees, clip the miter by a
					// line at radius along the outward bisector (not a simple bevel).
					const double cosine = std::sqrt((1.0 + dot) * 0.5);
					const double t = (cosine / (1.0 + cosine));
					corner.first = (points[i] + normals[prev] * (radius * (1.0 - t)) + corner.miter * t);
					corner.last = (points[i] + normals[i] * (radius * (1.0 - t)) + corner.miter * t);
					corner.count = 2;
				}
				else if ((0.0 < radius) && (joinStyle == JoinStyle::Round))
				{
					corner.first = (points[i] + normals[prev] * radius);
					corner.last = (points[i] + normals[i] * radius);
					corner.sweep = std::atan2(turn, dot);
					corner.count = CalculateCircleArcQuality(static_cast<float>(radius) * scale, static_cast<float>(corner.sweep));
				}
				corner.start = vertexCount;
				vertexCount += corner.count;
			}

			for (size_t i = 0; i < n; ++i)
			{
				// Convexity alone is insufficient: every inset edge must still run
				// forward. This rejects a vanished hole or a quad whose short inset
				// edge has disappeared even though another part of the hole remains.
				if (not (0.0 < (corners[(i + 1) % n].inner - corners[i].inner).dot(directions[i]))) return none;
			}

			const uint32 indexCount = (3u * vertexCount);
			const auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexCount, indexCount);
			if (not pVertex) return 0; // Handled: an allocation failure must not trigger fallback.

			for (size_t i = 0; i < n; ++i)
			{
				pVertex[i].setPosition(corners[i].inner);
				pVertex[corners[i].start].setPosition(corners[i].first);
				pVertex[corners[i].start + corners[i].count - 1].setPosition(corners[i].last);
				if ((1 < corners[i].count) && (joinStyle == JoinStyle::Round))
				{
					const auto& corner = corners[i];
					const Vec2 normal = normals[(i + n - 1) % n];
					const double start = std::atan2(normal.y, normal.x);
					const double step = (orientation * corner.sweep / (corner.count - 1));
					for (uint16 k = 1; k + 1 < corner.count; ++k)
					{
						const double angle = (start + step * k);
						pVertex[corner.start + k].setPosition((points[i] + Vec2{ std::cos(angle), std::sin(angle) } * radius));
					}
				}
			}
			for (uint16 i = 0; i < vertexCount; ++i) pVertex[i].setColor(color);

			auto dst = pIndex;
			const auto triangle = [&](const uint16 a, const uint16 b, const uint16 c)
			{
				*dst++ = (indexOffset + a);
				*dst++ = (indexOffset + ((orientation > 0.0) ? b : c));
				*dst++ = (indexOffset + ((orientation > 0.0) ? c : b));
			};
			for (uint16 i = 0; i < n; ++i)
			{
				const uint16 next = static_cast<uint16>((i + 1) % n);
				const auto& corner = corners[i];
				const uint16 last = (corner.start + corner.count - 1);
				triangle(i, last, corners[next].start);
				triangle(i, corners[next].start, next);
				for (uint16 k = 1; k < corner.count; ++k) triangle(i, (corner.start + k - 1), (corner.start + k));
			}
			return indexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildLineStringArrow
		//
		////////////////////////////////////////////////////////////////

		Optional<uint32> BuildLineStringArrow(const BufferCreatorFunc& bufferCreator, const LineCap startCap,
			const std::span<const Vec2> points, const Float2& tip, const float thickness, const float headWidth,
			const Float4& color, const float scale)
		{
			if (points.empty() || (thickness <= 0.0f) || (headWidth < thickness)) return none;
			PolylineBuffer& scratch = GetPolylineBuffer();
			const auto prepared = PreparePolyline<Vec2>(points, none, scale, false, true, false, scratch);
			const auto pts = prepared.points;
			const Float2 base = pts.back();
			const Float2 headVector = (tip - base);
			const float headLength = headVector.length();
			if (headLength == 0.0f) return none;
			if (pts.size() == 1)
			{
				return BuildArrow(bufferCreator, startCap, base, tip, thickness,
					Float2{ (headWidth - thickness), headLength }, { color, color }, scale);
			}
			if (MaxLineStringPointCount < pts.size()) return none;

			const size_t n = pts.size();
			const Float2 lastVector = (base - pts[n - 2]);
			const float lastLength = lastVector.length();
			if (lastLength == 0.0f) return none;
			const Float2 shaftDirection = (lastVector / lastLength), headDirection = (headVector / headLength);
			const float cosine = shaftDirection.dot(headDirection);
			// The oblique cross-section must fit inside the head's base. This
			// also bounds the projection without exceptional-value guards.
			if ((cosine <= 0.0f) || (headWidth * cosine < thickness)) return none;
			const float halfThickness = (thickness * 0.5f), joinRadius = (halfThickness / cosine);
			const Float2 normal{ -headDirection.y, headDirection.x };
			MiterPair previous;
			if (n == 2)
			{
				const Float2 offset{ -shaftDirection.y * halfThickness, shaftDirection.x * halfThickness };
				previous = { (pts[0] + offset), (pts[0] - offset) };
			}
			else
			{
				previous = ComputeMiterPair(pts[n - 2], prepared.directions[n - 3],
					shaftDirection, pts[n - 3], base, halfThickness);
			}
			const bool project = (((previous.a - base).dot(headDirection) <= 0.0f)
				&& ((previous.b - base).dot(headDirection) <= 0.0f));

			const uint16 quality = ((startCap == LineCap::Round) ? CalculateCircleSegmentQuality(halfThickness * scale, Math::PiF) : 0);
			const uint16 capVertices = ((startCap == LineCap::Square) ? 4 : ((startCap == LineCap::Round) ? (quality + 1) : 0));
			const uint32 capIndices = ((startCap == LineCap::Square) ? 6 : ((startCap == LineCap::Round) ? ((quality - 1) * 3) : 0));
			const uint16 stripVertices = static_cast<uint16>(n * 2);
			const uint32 stripIndices = static_cast<uint32>((n - 1) * 6);
			const uint16 headOffset = (stripVertices + capVertices);
			const uint16 vertexCount = (headOffset + 3 + (project ? 0 : 3));
			const uint32 indexCount = (stripIndices + capIndices + 3 + (project ? 0 : 6));
			const auto buffer = bufferCreator(vertexCount, indexCount);
			if (not buffer.pVertex) return uint32{ 0 };

			float startAngle = 0.0f;
			EmitLineStringStrip(buffer, prepared, halfThickness, false, [&](size_t) { return color; }, &startAngle);
			const Vertex2DBufferPointer capBuffer{ buffer.pVertex + stripVertices, buffer.pIndex + stripIndices,
				static_cast<uint16>(buffer.indexOffset + stripVertices) };
			const LineStringBuffer caps{ buffer, capBuffer, {}, quality, indexCount };
			EmitLineStringCaps(caps, startCap, LineCap::Flat, pts.front(), startAngle, base, 0.0f, none, thickness, color, color);

			const Float2 a = (base + normal * joinRadius), b = (base - normal * joinRadius);
			if (project)
			{
				buffer.pVertex[stripVertices - 2].setPosition(a);
				buffer.pVertex[stripVertices - 1].setPosition(b);
			}
			const Float2 headA = (base + normal * (headWidth * 0.5f)), headB = (base - normal * (headWidth * 0.5f));
			buffer.pVertex[headOffset].set(tip, color);
			buffer.pVertex[headOffset + 1].set(headA, color);
			buffer.pVertex[headOffset + 2].set(headB, color);
			auto indices = (buffer.pIndex + stripIndices + capIndices);
			const auto triangle = [&](uint16 i, uint16 j, uint16 k, const Float2& p0, const Float2& p1, const Float2& p2)
			{
				if ((p1 - p0).cross(p2 - p0) < 0.0f) std::swap(j, k);
				*indices++ = (buffer.indexOffset + i);
				*indices++ = (buffer.indexOffset + j);
				*indices++ = (buffer.indexOffset + k);
			};
			triangle(headOffset, (headOffset + 1), (headOffset + 2), tip, headA, headB);
			if (not project)
			{
				// A short/turning terminal strip may already cross the head plane.
				// Keep its ordinary end and bridge locally; overlap is intentional.
				buffer.pVertex[headOffset + 3].set(base, color);
				buffer.pVertex[headOffset + 4].set(a, color);
				buffer.pVertex[headOffset + 5].set(b, color);
				const Float2 shaftNormal{ -shaftDirection.y * halfThickness, shaftDirection.x * halfThickness };
				triangle((headOffset + 3), (stripVertices - 2), (headOffset + 4), base, (base + shaftNormal), a);
				triangle((headOffset + 3), (headOffset + 5), (stripVertices - 1), base, b, (base - shaftNormal));
			}
			return indexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildLineString
		//
		////////////////////////////////////////////////////////////////

		uint32 BuildLineString(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const Float4& color, const float scale)
		{
			const size_t num_points = points.size();

			if (num_points == 0)
			{
				return 0;
			}
			else if (num_points == 1)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, color, scale);
			}
			else if (32760 <= num_points)
			{
				return 0;
			}

			if (thickness <= 0.0f)
			{
				return 0;
			}

			PolylineBuffer& buffer = GetPolylineBuffer();
			const bool closed = (closeRing && (3 <= num_points));
			const PreparedPolyline prepared = PreparePolyline<Vec2>(points, offset, scale, closed, (not inner), false, buffer);

			// 間引きの結果 1 点に縮退した場合はキャップのみ描画する
			if (prepared.points.size() < 2)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, color, scale);
			}

			const float halfThickness = (thickness * 0.5f);
			const auto colorAt = [&color](size_t) { return color; };

			if (closed)
			{
				if (3 <= prepared.points.size())
				{
					return BuildLineStringStrip(bufferCreator, prepared, halfThickness, true, colorAt);
				}

				// 2 点に縮退したリングは、キャップなしの開いた線分として描画する
				return BuildLineStringStrip(bufferCreator, prepared, halfThickness, false, colorAt);
			}
			else
			{
				float startAngle = 0.0f, endAngle = 0.0f;

				const auto drawBuffer = CreateLineStringBuffer(bufferCreator, prepared.points.size(), startCap, endCap, halfThickness, scale);
				if (not drawBuffer.strip.pVertex)
				{
					return 0;
				}

				EmitLineStringStrip(drawBuffer.strip, prepared, halfThickness, false, colorAt, &startAngle, &endAngle);

				EmitLineStringCaps(drawBuffer, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, color, color);

				return drawBuffer.indexCount;
			}
		}

		uint32 BuildLineString(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& colorStart, const Float4& colorEnd, const float scale)
		{
			const size_t num_points = points.size();

			if (num_points == 0)
			{
				return 0;
			}
			else if (num_points == 1)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, colorStart, colorEnd, scale);
			}
			else if (32760 <= num_points)
			{
				return 0;
			}

			if (thickness <= 0.0f)
			{
				return 0;
			}

			PolylineBuffer& buffer = GetPolylineBuffer();
			const PreparedPolyline prepared = PreparePolyline<Vec2, true>(points, offset, scale, false, (not inner), false, buffer);

			if (prepared.points.size() < 2)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, colorStart, colorEnd, scale);
			}

			const float halfThickness = (thickness * 0.5f);
			const bool hasStartCap = (startCap != LineCap::Flat);
			const bool hasEndCap = (endCap != LineCap::Flat);

			// 色はキャップ部分も含めた全長 (allLength) に対して線形に補間する。
			const float baseLength = buffer.totalLength;
			const float startDistance = (hasStartCap ? halfThickness : 0.0f);
			const float allLength = (baseLength + startDistance + (hasEndCap ? halfThickness : 0.0f));
			const float lengthInv = ((0.0f < allLength) ? (1.0f / allLength) : 0.0f);
			const Float4 colorDiff = (colorEnd - colorStart);

			// 始端・終端の頂点色。キャップ側のグラデーションに引き継ぐ
			const Float4 stripStartColor = (colorStart + colorDiff * (startDistance * lengthInv));
			const Float4 stripEndColor = (colorStart + colorDiff * ((startDistance + baseLength) * lengthInv));

			const float* const segmentLengths = buffer.segmentLengths.data();
			auto colorAt = [&, distance = startDistance](const size_t i) mutable
			{
				if (0 < i)
				{
					distance += segmentLengths[i - 1];
				}

				return (colorStart + colorDiff * (distance * lengthInv));
			};

			float startAngle = 0.0f, endAngle = 0.0f;

			const auto drawBuffer = CreateLineStringBuffer(bufferCreator, prepared.points.size(), startCap, endCap, halfThickness, scale);
			if (not drawBuffer.strip.pVertex)
			{
				return 0;
			}

			EmitLineStringStrip(drawBuffer.strip, prepared, halfThickness, false, colorAt, &startAngle, &endAngle);

			EmitLineStringCaps(drawBuffer, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, colorStart, stripStartColor, stripEndColor, colorEnd);

			return drawBuffer.indexCount;
		}

		uint32 BuildLineString(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const std::span<const ColorF> colors, const float scale)
		{
			const size_t num_points = points.size();

			if (num_points == 0)
			{
				return 0;
			}

			const Float4 colorStart = colors.front().toFloat4();
			const Float4 colorEnd = colors.back().toFloat4();

			if (num_points == 1)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, colorStart, colorEnd, scale);
			}
			else if (32760 <= num_points)
			{
				return 0;
			}

			if (thickness <= 0.0f)
			{
				return 0;
			}

			PolylineBuffer& buffer = GetPolylineBuffer();
			const bool closed = (closeRing && (3 <= num_points));
			const PreparedPolyline prepared = PreparePolyline<Vec2>(points, offset, scale, closed, (not inner), true, buffer);

			if (prepared.points.size() < 2)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, colorStart, colorEnd, scale);
			}

			const float halfThickness = (thickness * 0.5f);
			const uint32* const srcIndices = prepared.srcIndices.data();
			const auto colorAt = [&colors, srcIndices](const size_t i) { return colors[srcIndices[i]].toFloat4(); };

			if (closed)
			{
				if (3 <= prepared.points.size())
				{
					return BuildLineStringStrip(bufferCreator, prepared, halfThickness, true, colorAt);
				}

				// 2 点に縮退したリングは、キャップなしの開いた線分として描画する
				return BuildLineStringStrip(bufferCreator, prepared, halfThickness, false, colorAt);
			}
			else
			{
				float startAngle = 0.0f, endAngle = 0.0f;

				const auto drawBuffer = CreateLineStringBuffer(bufferCreator, prepared.points.size(), startCap, endCap, halfThickness, scale);
				if (not drawBuffer.strip.pVertex)
				{
					return 0;
				}

				EmitLineStringStrip(drawBuffer.strip, prepared, halfThickness, false, colorAt, &startAngle, &endAngle);

				EmitLineStringCaps(drawBuffer, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, colorStart, colorEnd);

				return drawBuffer.indexCount;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedCircle
		//
		////////////////////////////////////////////////////////////////

		template <bool Gradient>
		SIV3D_ALWAYS_INLINE
		static inline Vertex2D::IndexType BuildTexturedCircle(const BufferCreatorFunc& bufferCreator, const Circle& circle, const FloatRect& uv, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const float rf = Abs(static_cast<float>(circle.r));
			const Vertex2D::IndexType Quality = CalculateCircleQuality(rf * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality + 1);
			const Vertex2D::IndexType IndexCount = (FullQuality * 3);

			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			WriteEllipseFanVertices<Gradient ? EllipseFanType::TexturedCircleGradient : EllipseFanType::TexturedCircle>(pVertex, static_cast<float>(circle.x), static_cast<float>(circle.y), rf, rf, Quality, colorType, color0, color1, uv);

			WriteClosedFanIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		Vertex2D::IndexType BuildTexturedCircle(const BufferCreatorFunc& bufferCreator, const Circle& circle, const FloatRect& uv, const Float4& color, const float scale)
		{
			return BuildTexturedCircle<false>(bufferCreator, circle, uv, ColorFillDirection::InOut, color, color, scale);
		}

		Vertex2D::IndexType BuildTexturedCircle(const BufferCreatorFunc& bufferCreator, const Circle& circle, const FloatRect& uv, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			return BuildTexturedCircle<true>(bufferCreator, circle, uv, colorType, color0, color1, scale);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedCirclePie
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildTexturedCirclePie(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const float angle, const FloatRect& uv, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			return BuildCirclePie<true>(bufferCreator, center, r, startAngle, angle, innerColor, outerColor, scale, uv);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedRoundRect
		//
		////////////////////////////////////////////////////////////////

		template <bool Gradient>
		SIV3D_ALWAYS_INLINE
		static inline Vertex2D::IndexType BuildTexturedRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const Vertex2D::IndexType Quality				= CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FanOuterVertexCount	= (Quality + 1); // 1 つの扇型の曲線部分の頂点数
			const Vertex2D::IndexType FanVertexCount		= (FanOuterVertexCount + 1); // 1 つの扇型の頂点数
			const Vertex2D::IndexType VertexCount			= (FanVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount			= ((Quality * 3 * 4) + (6 * 3)); // インデックス数
			auto [pVertex, pIndex, indexOffset]				= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float ws = ((uvRect.right - uvRect.left) / w);
			const float hs = ((uvRect.bottom - uvRect.top) / h);
			WriteRoundRectVertices<Gradient>(pVertex, rect, r, Quality, colorType, color0, color1,
				[&](Vertex2DWriteReference vertex, const float x, const float y, const Float4& color)
				{
					const float u = (uvRect.left + (x - rect.left) * ws);
					const float v = (uvRect.top + (y - rect.top) * hs);
					vertex.set(x, y, u, v, color);
				});

			WriteRoundRectIndices(pIndex, indexOffset, Quality);

			return IndexCount;
		}

		Vertex2D::IndexType BuildTexturedRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const Float4& color, const float scale)
		{
			return BuildTexturedRoundRect<false>(bufferCreator, rect, w, h, r, uvRect, ColorFillDirection::TopBottom, color, color, scale);
		}

		Vertex2D::IndexType BuildTexturedRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			return BuildTexturedRoundRect<true>(bufferCreator, rect, w, h, r, uvRect, colorType, color0, color1, scale);
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleShadowNoFill
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleShadowNoFill(const BufferCreatorFunc& bufferCreator, const Circle& circle, float blur, const Float4& color, float scale)
		{
			// ベースとなる円の半径, ぼかしの半分
			const float r = static_cast<float>(circle.r);
			const float halfBlur = (blur * 0.5f);

			// 影の開始半径, 終了半径
			const float rInner = (r - halfBlur);
			const float rOuter = (r + halfBlur);

			// 円の品質
			const Vertex2D::IndexType Quality = CalculateCircleQuality(rOuter * scale);
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality * 2);
			const Vertex2D::IndexType IndexCount = (FullQuality * 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);
			if (not pVertex)
			{
				return 0;
			}

			const float centerX = static_cast<float>(circle.x);
			const float centerY = static_cast<float>(circle.y);

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			WriteCircleShadowRingVertices(pVertex, pCS, centerX, centerY, rInner, rOuter, Quality, color);

			WriteClosedStripIndices(pIndex, indexOffset, FullQuality);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleShadow
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleShadow(const BufferCreatorFunc& bufferCreator, const Circle& circle, const float blur, const Float4& color, const float scale, const bool fill)
		{
			if (not fill)
			{
				return BuildCircleShadowNoFill(bufferCreator, circle, blur, color, scale);
			}

			// ベースとなる円の半径, ぼかしの半分
			const float r = static_cast<float>(circle.r);
			const float halfBlur = (blur * 0.5f);

			// 影の開始半径, 終了半径
			const float rInner = (r - halfBlur);
			const float rOuter = (r + halfBlur);

			// 円の品質
			const Vertex2D::IndexType Quality = CalculateCircleQuality(rOuter * scale);
			const Vertex2D::IndexType FullQuality = (Quality * 4);
			const Vertex2D::IndexType VertexCount = (FullQuality * 2 + 1); // 中心頂点を追加
			const Vertex2D::IndexType IndexCount = (FullQuality * 6 + FullQuality * 3); // 外側のリング + 内側の三角形
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);
			if (not pVertex)
			{
				return 0;
			}

			const float centerX = static_cast<float>(circle.x);
			const float centerY = static_cast<float>(circle.y);

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			// 中心頂点を最初に配置
			pVertex[0].set(centerX, centerY, 0.5f, 0.5f, color);

			WriteCircleShadowRingVertices((pVertex + 1), pCS, centerX, centerY, rInner, rOuter, Quality, color);

			{
				const Vertex2D::IndexType ringStartIndex = (indexOffset + 1);

				// 外側のリングのインデックス
				WriteClosedStripIndices(pIndex, ringStartIndex, FullQuality);

				// 内円を中心頂点で埋めるインデックス
				{
					const Vertex2D::IndexType centerIndex = indexOffset; // 中心頂点のインデックス

					for (Vertex2D::IndexType i = 0; i < (FullQuality - 1); ++i)
					{
						const Vertex2D::IndexType innerCurrent = (ringStartIndex + i * 2 + 1); // 内円の現在の頂点
						const Vertex2D::IndexType innerNext = (innerCurrent + 2);

						*pIndex++ = centerIndex;
						*pIndex++ = innerCurrent;
						*pIndex++ = innerNext;
					}
					*pIndex++ = centerIndex;
					*pIndex++ = (ringStartIndex + FullQuality * 2 - 1);
					*pIndex++ = (ringStartIndex + 1);
				}
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRectShadow
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRectShadow(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float blur, const Float4& color, const bool fill)
		{
			constexpr Vertex2D::IndexType vertexCount = 16, indexCount = 54;
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexCount, indexCount);

			if (not pVertex)
			{
				return 0;
			}

			// 頂点情報の書き込み
			{
				// ぼかしサイズの半分
				const float halfBlur = (blur * 0.5f);
				const float xs[4] =
				{
					static_cast<float>(rect.left - halfBlur),
					static_cast<float>(rect.left + halfBlur),
					static_cast<float>(rect.right - halfBlur),
					static_cast<float>(rect.right + halfBlur)
				};
				const float ys[4] =
				{
					static_cast<float>(rect.top - halfBlur),
					static_cast<float>(rect.top + halfBlur),
					static_cast<float>(rect.bottom - halfBlur),
					static_cast<float>(rect.bottom + halfBlur)
				};
				const float uvs[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

				for (int32 y = 0; y < 4; ++y)
				{
					for (int32 x = 0; x < 4; ++x)
					{
						pVertex->setPosition(xs[x], ys[y]);
						pVertex->setTex(uvs[x], uvs[y]);
						pVertex->setColor(color);
						++pVertex;
					}
				}
			}

			// インデックス情報の書き込み
			{
				Index2DWritePointer pBase = pIndex;

				for (Vertex2D::IndexType ty = 0; ty < 3; ++ty)
				{
					for (Vertex2D::IndexType tx = 0; tx < 3; ++tx)
					{
						const Vertex2D::IndexType base = (((ty * 4) + tx) + indexOffset);

						*pIndex++ = base;
						*pIndex++ = (base + 1);
						*pIndex++ = (base + 4);

						*pIndex++ = (base + 4);
						*pIndex++ = (base + 1);
						*pIndex++ = (base + 5);
					}
				}

				if (not fill)
				{
					// 縮退三角形
					for (size_t i = 25; i < 30; ++i) pBase[i] = (indexOffset + 5);
				}
			}

			return indexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRoundRectShadow
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRoundRectShadow(const BufferCreatorFunc& bufferCreator, const RoundRect& roundRect, const float blur, const Float4& color, const float scale, const bool fill)
		{
			const float halfBlur	= (blur * 0.5f);
			const float baseRadius	= static_cast<float>(roundRect.r);
			const float nearRadius	= Max(baseRadius - halfBlur, 0.0f);
			const float farRadius	= (baseRadius + halfBlur);
			const FloatRect innerRect{
				(roundRect.x + baseRadius),
				(roundRect.y + baseRadius),
				(roundRect.x + roundRect.w - baseRadius),
				(roundRect.y + roundRect.h - baseRadius)
			};
			const Vertex2D::IndexType quality = static_cast<Vertex2D::IndexType>(CalculateCircleQuality(farRadius * scale));

			// 1 corner = [outer arc: quality] + [inner arc: quality] + [center: 1]
			const Vertex2D::IndexType fanVertexCount		= quality;
			const Vertex2D::IndexType roundVertexCount		= (fanVertexCount + fanVertexCount + 1);
			const Vertex2D::IndexType vertexCount			= (roundVertexCount * 4);
			const Vertex2D::IndexType cornerTriangles		= ((quality - 1) * 3) * 4;
			const Vertex2D::IndexType connectionTriangles	= (4 * 4);
			const Vertex2D::IndexType centerTriangles		= 2;
			const Vertex2D::IndexType triangleCount			= (cornerTriangles + connectionTriangles + centerTriangles);
			const Vertex2D::IndexType indexCount			= (triangleCount * 3);

			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexCount, indexCount);
			if (not pVertex || not pIndex)
			{
				return 0;
			}

			auto Emit = [&](Vertex2D::IndexType a, Vertex2D::IndexType b, Vertex2D::IndexType c)
			{
				*pIndex++ = a;
				*pIndex++ = b;
				*pIndex++ = c;
			};

			auto CornerBase = [&](Vertex2D::IndexType corner) -> Vertex2D::IndexType
			{
				return (indexOffset + corner * roundVertexCount);
			};

			auto Outer = [&](Vertex2D::IndexType corner, Vertex2D::IndexType i) -> Vertex2D::IndexType
			{
				return (CornerBase(corner) + i);
			};

			auto Inner = [&](Vertex2D::IndexType corner, Vertex2D::IndexType i) -> Vertex2D::IndexType
			{
				return (CornerBase(corner) + fanVertexCount + i);
			};

			auto Center = [&](Vertex2D::IndexType corner) -> Vertex2D::IndexType
			{
				return (CornerBase(corner) + (roundVertexCount - 1));
			};

			// ---- write vertices ----
			{
				const float radDelta = (Math::HalfPiF / (quality - 1));

				// 左上, 右上, 右下, 左下 の順
				const float centersX[4] = { innerRect.left,  innerRect.right, innerRect.right, innerRect.left };
				const float centersY[4] = { innerRect.top,   innerRect.top,   innerRect.bottom, innerRect.bottom };

				// 角度オフセット
				const auto angleOffsets = std::array{ -180_deg, -90_deg, 0_deg, 90_deg };

				auto writeCornerVertices = [&](Vertex2D::IndexType corner)
				{
					const float cx = centersX[corner];
					const float cy = centersY[corner];

					Vertex2DWritePointer pOuter = (pVertex + corner * roundVertexCount);
					Vertex2DWritePointer pInner = (pOuter + fanVertexCount);
					Vertex2DWritePointer pC = (pInner + fanVertexCount);

					const auto offset = angleOffsets[corner];

					for (Vertex2D::IndexType i = 0; i < quality; ++i)
					{
						const float rad = (radDelta * i);
						const auto [s, c] = FastMath::SinCos(rad + offset);

						pOuter->setPosition((cx + farRadius * c), (cy + farRadius * s));
						pOuter->setTex(0.0f, 0.5f);
						pOuter->setColor(color);

						pInner->setPosition((cx + nearRadius * c), (cy + nearRadius * s));
						pInner->setTex(0.5f, 0.5f);
						pInner->setColor(color);

						++pOuter;
						++pInner;
					}

					pC->setPosition(cx, cy);
					pC->setTex(0.5f, 0.5f);
					pC->setColor(color);
				};

				for (Vertex2D::IndexType corner = 0; corner < 4; ++corner)
				{
					writeCornerVertices(corner);
				}
			}

			{
				auto writeCornerIndices = [&](Vertex2D::IndexType corner)
				{
					const Vertex2D::IndexType tc = Center(corner);

					for (Vertex2D::IndexType i = 0; i < (quality - 1); ++i)
					{
						const Vertex2D::IndexType t0 = Outer(corner, i);
						const Vertex2D::IndexType t1 = Outer(corner, (i + 1));
						const Vertex2D::IndexType t2 = Inner(corner, i);
						const Vertex2D::IndexType t3 = Inner(corner, (i + 1));

						// outer-inner quad
						Emit(t0, t1, t2);
						Emit(t2, t1, t3);

						// inner to center (fan)
						Emit(t2, t3, tc);
					}
				};

				auto WriteConnection = [&](Vertex2D::IndexType a, Vertex2D::IndexType b)
				{
					const Vertex2D::IndexType t0 = Outer(a, quality - 1);
					const Vertex2D::IndexType t1 = Inner(a, quality - 1);
					const Vertex2D::IndexType t2 = Center(a);

					const Vertex2D::IndexType t3 = Outer(b, 0);
					const Vertex2D::IndexType t4 = Inner(b, 0);
					const Vertex2D::IndexType t5 = Center(b);

					Emit(t0, t3, t1);
					Emit(t1, t3, t4);
					Emit(t1, t4, t2);
					Emit(t2, t4, t5);
				};

				// corners
				for (Vertex2D::IndexType corner = 0; corner < 4; ++corner)
				{
					writeCornerIndices(corner);
				}

				// connections: (0->1), (1->2), (2->3), (3->0)
				WriteConnection(0, 1);
				WriteConnection(1, 2);
				WriteConnection(2, 3);
				WriteConnection(3, 0);

				// center (fill or degenerate)
				const Vertex2D::IndexType c0 = Center(0);
				const Vertex2D::IndexType c1 = Center(1);
				const Vertex2D::IndexType c2 = Center(2);
				const Vertex2D::IndexType c3 = Center(3);

				if (fill)
				{
					Emit(c0, c1, c3);
					Emit(c3, c1, c2);
				}
				else
				{
					// 縮退三角形
					Emit(c0, c0, c0);
					Emit(c0, c0, c0);
				}
			}

			return indexCount;
		}
	}
}
