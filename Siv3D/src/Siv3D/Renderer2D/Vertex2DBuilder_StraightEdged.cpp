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
# include "Vertex2DBuilder.hpp"
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/FloatQuad.hpp>

namespace s3d
{
	namespace
	{
		static constexpr Vertex2D::IndexType RectIndexTable[6] = { 0, 1, 2, 2, 1, 3 };

		static constexpr Vertex2D::IndexType RectFrameIndexTable[24] = { 0, 2, 1, 1, 2, 3, 2, 4, 3, 3, 4, 5, 4, 6, 5, 5, 6, 7, 6, 0, 7, 7, 0, 1 };

		struct RectFrameDashedQuad
		{
			float left;

			float top;

			float right;

			float bottom;
		};

		enum class RectFramePathComponent
		{
			TopSide,
			TopRightCorner,
			RightSide,
			BottomRightCorner,
			BottomSide,
			BottomLeftCorner,
			LeftSide,
			TopLeftCorner,
		};

		[[nodiscard]]
		static float CalculateRectFrameVirtualPerimeter(const FloatRect& innerRect, const float thickness) noexcept
		{
			const float width = (innerRect.right - innerRect.left);
			const float height = (innerRect.bottom - innerRect.top);

			return (width * 2.0f + height * 2.0f + thickness * 4.0f);
		}

		[[nodiscard]]
		static float NormalizeRectFrameDashOffset(const float offset, const float perimeter) noexcept
		{
			const float result = std::fmod(offset, perimeter);

			return ((result < 0.0f) ? (result + perimeter) : result);
		}

		template <class Fn>
		static void ForEachRectFrameDashRange(const FloatRect& innerRect, const float thickness, const float offset, const float dashRatio, const uint32 dashCount, Fn&& fn)
		{
			const float perimeter = CalculateRectFrameVirtualPerimeter(innerRect, thickness);
			const float period = (perimeter / dashCount);
			const float dashLength = (period * dashRatio);
			const float normalizedOffset = NormalizeRectFrameDashOffset(offset, perimeter);

			for (uint32 dashIndex = 0; dashIndex < dashCount; ++dashIndex)
			{
				float begin = (normalizedOffset + period * dashIndex);

				if (perimeter <= begin)
				{
					begin -= perimeter;
				}

				fn(begin, (begin + dashLength));
			}
		}

		template <class Fn>
		static void EmitRectFrameDashedQuad(const RectFrameDashedQuad& rect, Fn&& fn)
		{
			if ((rect.left < rect.right) && (rect.top < rect.bottom))
			{
				fn(rect);
			}
		}

		// Emits the quad for a single path component (side or corner). This merges the
		// former EmitRectFrameSideSegment / EmitRectFrameCornerSegment pair (which were
		// both invoked for every segment, with one always doing nothing) into a single
		// table of per-component quad geometries.
		//
		// A corner square is shared by its previous side and its next side (which run in
		// perpendicular directions). When a corner is only partially covered, the quad is
		// emitted collinear with whichever side the dash actually occupies, so the corner
		// is always a full-thickness extension of the dash rectangle (never a sub-thickness
		// sliver). localBegin == 0 means the dash reaches the corner from its PREVIOUS side;
		// localEnd == thickness means it continues onto its NEXT side. A fully covered
		// corner (both) fills the whole square either way.
		template <class Fn>
		static void EmitRectFrameComponentQuad(const FloatRect& innerRect, const float thickness, const RectFramePathComponent component, const float localBegin, const float localEnd, Fn&& fn)
		{
			constexpr float CornerEpsilon = 1e-4f;
			const float coverage = (localEnd - localBegin);
			const bool reachesNextSide = (localEnd > (thickness - CornerEpsilon));
			const bool reachesPrevSide = (localBegin < CornerEpsilon);
			// Render the corner collinear with the NEXT side only when the dash body is on
			// that side (reaches the next side but not the previous one).
			const bool alignWithNextSide = (reachesNextSide && (!reachesPrevSide));

			switch (component)
			{
			case RectFramePathComponent::TopSide:
			{
				EmitRectFrameDashedQuad({
					(innerRect.left + localBegin),
					(innerRect.top - thickness),
					(innerRect.left + localEnd),
					innerRect.top }, fn);
				break;
			}
			case RectFramePathComponent::RightSide:
			{
				EmitRectFrameDashedQuad({
					innerRect.right,
					(innerRect.top + localBegin),
					(innerRect.right + thickness),
					(innerRect.top + localEnd) }, fn);
				break;
			}
			case RectFramePathComponent::BottomSide:
			{
				EmitRectFrameDashedQuad({
					(innerRect.right - localEnd),
					innerRect.bottom,
					(innerRect.right - localBegin),
					(innerRect.bottom + thickness) }, fn);
				break;
			}
			case RectFramePathComponent::LeftSide:
			{
				EmitRectFrameDashedQuad({
					(innerRect.left - thickness),
					(innerRect.bottom - localEnd),
					innerRect.left,
					(innerRect.bottom - localBegin) }, fn);
				break;
			}
			case RectFramePathComponent::TopRightCorner:
			{
				if (alignWithNextSide)
				{
					// next side: RightSide (vertical)
					EmitRectFrameDashedQuad({ innerRect.right, (innerRect.top - coverage), (innerRect.right + thickness), innerRect.top }, fn);
				}
				else
				{
					// previous side: TopSide (horizontal)
					EmitRectFrameDashedQuad({ (innerRect.right + localBegin), (innerRect.top - thickness), (innerRect.right + localEnd), innerRect.top }, fn);
				}
				break;
			}
			case RectFramePathComponent::BottomRightCorner:
			{
				if (alignWithNextSide)
				{
					// next side: BottomSide (horizontal)
					EmitRectFrameDashedQuad({ innerRect.right, innerRect.bottom, (innerRect.right + coverage), (innerRect.bottom + thickness) }, fn);
				}
				else
				{
					// previous side: RightSide (vertical)
					EmitRectFrameDashedQuad({ innerRect.right, (innerRect.bottom + localBegin), (innerRect.right + thickness), (innerRect.bottom + localEnd) }, fn);
				}
				break;
			}
			case RectFramePathComponent::BottomLeftCorner:
			{
				if (alignWithNextSide)
				{
					// next side: LeftSide (vertical)
					EmitRectFrameDashedQuad({ (innerRect.left - thickness), innerRect.bottom, innerRect.left, (innerRect.bottom + coverage) }, fn);
				}
				else
				{
					// previous side: BottomSide (horizontal)
					EmitRectFrameDashedQuad({ (innerRect.left - localEnd), innerRect.bottom, (innerRect.left - localBegin), (innerRect.bottom + thickness) }, fn);
				}
				break;
			}
			case RectFramePathComponent::TopLeftCorner:
			{
				if (alignWithNextSide)
				{
					// next side: TopSide (horizontal)
					EmitRectFrameDashedQuad({ (innerRect.left - coverage), (innerRect.top - thickness), innerRect.left, innerRect.top }, fn);
				}
				else
				{
					// previous side: LeftSide (vertical)
					EmitRectFrameDashedQuad({ (innerRect.left - thickness), (innerRect.top - localEnd), innerRect.left, (innerRect.top - localBegin) }, fn);
				}
				break;
			}
			}
		}

		// Walks the dash range [begin, end] across the virtual perimeter and invokes
		// fn(component, localBegin, localEnd) for the part of each path component it
		// covers. A dash range lies within [0, 2 * perimeter), so two laps of the eight-
		// component cycle are sufficient (this replaces the former hand-duplicated
		// sixteen-entry table).
		template <class Fn>
		static void ForEachRectFrameComponentSegment(const FloatRect& innerRect, const float thickness, const float begin, const float end, Fn&& fn)
		{
			const float width = (innerRect.right - innerRect.left);
			const float height = (innerRect.bottom - innerRect.top);
			const float perimeter = CalculateRectFrameVirtualPerimeter(innerRect, thickness);

			const RectFramePathComponent components[8] =
			{
				RectFramePathComponent::TopSide,
				RectFramePathComponent::TopRightCorner,
				RectFramePathComponent::RightSide,
				RectFramePathComponent::BottomRightCorner,
				RectFramePathComponent::BottomSide,
				RectFramePathComponent::BottomLeftCorner,
				RectFramePathComponent::LeftSide,
				RectFramePathComponent::TopLeftCorner,
			};
			const float componentLengths[8] =
			{
				width, thickness, height, thickness, width, thickness, height, thickness,
			};

			for (int lap = 0; lap < 2; ++lap)
			{
				float componentBegin = (lap * perimeter);

				for (size_t i = 0; i < 8; ++i)
				{
					const float componentEnd = (componentBegin + componentLengths[i]);
					const float clippedBegin = Max(begin, componentBegin);
					const float clippedEnd = Min(end, componentEnd);

					if (clippedBegin < clippedEnd)
					{
						fn(components[i], (clippedBegin - componentBegin), (clippedEnd - componentBegin));
					}

					componentBegin = componentEnd;
				}
			}
		}

		template <class Fn>
		static void ForEachRectFrameDashedQuad(const FloatRect& innerRect, const float thickness, const float begin, const float end, Fn&& fn)
		{
			ForEachRectFrameComponentSegment(innerRect, thickness, begin, end,
				[&](const RectFramePathComponent component, const float localBegin, const float localEnd)
				{
					EmitRectFrameComponentQuad(innerRect, thickness, component, localBegin, localEnd, fn);
				});
		}
	}

	namespace Vertex2DBuilder
	{
		////////////////////////////////////////////////////////////////
		//
		//	BuildLine
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildLine(const BufferCreatorFunc& bufferCreator, const LineStyle& style, const Float2& start, const Float2& end, const float thickness, const Float4(&colors)[2], const float scale)
		{
			if (style.type == LineType::Solid)
			{
				return BuildLine(bufferCreator, style.cap, style.cap, start, end, thickness, colors, scale);
			}
			
			constexpr Vertex2D::IndexType VertexCount = 4;
			constexpr Vertex2D::IndexType IndexCount = 6;
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float halfThickness = (thickness * 0.5f);
			const float length = (end - start).length();
			float uMax = (length / thickness);
			const float uOffset = -static_cast<float>(style.dotOffset);
			const Float2 line = (length ? ((end - start) / length) : Float2{ 1, 0 });
			const Float2 vNormal{ (-line.y * halfThickness), (line.x * halfThickness) };

			if (style.cap == LineCap::Flat)
			{
				pVertex[0].set((start + vNormal), Float2{ uOffset, 1 }, colors[0]);
				pVertex[1].set((start - vNormal), Float2{ uOffset, 0 }, colors[0]);
				pVertex[2].set((end + vNormal), Float2{ (uOffset + uMax), 1 }, colors[1]);
				pVertex[3].set((end - vNormal), Float2{ (uOffset + uMax), 0 }, colors[1]);
			}
			else
			{
				const Float2 lineHalf{ line * halfThickness };
				const Float2 start2 = (start - lineHalf);
				const Float2 end2 = (end + lineHalf);

				if (style.type == LineType::RoundDot)
				{
					uMax = std::ceil(uMax / 2.0f) * 2.0f;
				}

				pVertex[0].set((start2 + vNormal), Float2{ uOffset, 1 }, colors[0]);
				pVertex[1].set((start2 - vNormal), Float2{ uOffset, 0 }, colors[0]);
				pVertex[2].set((end2 + vNormal), Float2{ ((uOffset + uMax) + 1.0f), 1}, colors[1]);
				pVertex[3].set((end2 - vNormal), Float2{ ((uOffset + uMax) + 1.0f), 0 }, colors[1]);
			}

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTriangle
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4& color)
		{
			constexpr Vertex2D::IndexType VertexCount	= 3;
			constexpr Vertex2D::IndexType IndexCount	= 3;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);
			
			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(points[0], color);
			pVertex[1].set(points[1], color);
			pVertex[2].set(points[2], color);

			pIndex[0] = indexOffset;
			pIndex[1] = (indexOffset + 1);
			pIndex[2] = (indexOffset + 2);

			return IndexCount;
		}

		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4(&colors)[3])
		{
			constexpr Vertex2D::IndexType VertexCount	= 3;
			constexpr Vertex2D::IndexType IndexCount	= 3;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);
			
			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(points[0], colors[0]);
			pVertex[1].set(points[1], colors[1]);
			pVertex[2].set(points[2], colors[2]);

			pIndex[0] = indexOffset;
			pIndex[1] = (indexOffset + 1);
			pIndex[2] = (indexOffset + 2);

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRect
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const Float4& color)
		{
			constexpr Vertex2D::IndexType VertexCount	= 4;
			constexpr Vertex2D::IndexType IndexCount	= 6;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(rect.left, rect.top, color);
			pVertex[1].set(rect.right, rect.top, color);
			pVertex[2].set(rect.left, rect.bottom, color);
			pVertex[3].set(rect.right, rect.bottom, color);

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}

		Vertex2D::IndexType BuildRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const Float4(&colors)[4])
		{
			constexpr Vertex2D::IndexType VertexCount	= 4;
			constexpr Vertex2D::IndexType IndexCount	= 6;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(rect.left, rect.top, colors[0]);
			pVertex[1].set(rect.right, rect.top, colors[1]);
			pVertex[2].set(rect.left, rect.bottom, colors[3]);
			pVertex[3].set(rect.right, rect.bottom, colors[2]);

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRectFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRectFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1)
		{
			constexpr Vertex2D::IndexType VertexCount	= 8;
			constexpr Vertex2D::IndexType IndexCount	= 24;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].pos.set((innerRect.left - thickness), (innerRect.top - thickness));
			pVertex[1].pos.set(innerRect.left, innerRect.top);
			pVertex[2].pos.set((innerRect.right + thickness), (innerRect.top - thickness));
			pVertex[3].pos.set(innerRect.right, innerRect.top);
			pVertex[4].pos.set((innerRect.right + thickness), (innerRect.bottom + thickness));
			pVertex[5].pos.set(innerRect.right, innerRect.bottom);
			pVertex[6].pos.set((innerRect.left - thickness), (innerRect.bottom + thickness));
			pVertex[7].pos.set(innerRect.left, innerRect.bottom);

			switch (colorType)
			{
			case ColorFillDirection::InOut:
				{
					for (size_t i = 0; i < 4; ++i)
					{
						(pVertex++)->color = color1;
						(pVertex++)->color = color0;
					}

					break;
				}
			case ColorFillDirection::TopBottom:
				{
					const float v1 = (thickness / (innerRect.bottom - innerRect.top + thickness * 2));
					const Float4 colors[4] = { color0, color0.lerp(color1, v1), color0.lerp(color1, (1.0f - v1)), color1 };

					pVertex[0].color = colors[0];
					pVertex[1].color = colors[1];
					pVertex[2].color = colors[0];
					pVertex[3].color = colors[1];
					pVertex[4].color = colors[3];
					pVertex[5].color = colors[2];
					pVertex[6].color = colors[3];
					pVertex[7].color = colors[2];

					break;
				}
			case ColorFillDirection::LeftRight:
				{
					const float v1 = (thickness / (innerRect.right - innerRect.left + thickness * 2));
					const Float4 colors[4] = { color0, color0.lerp(color1, v1), color0.lerp(color1, (1.0f - v1)), color1 };

					pVertex[0].color = colors[0];
					pVertex[1].color = colors[1];
					pVertex[2].color = colors[3];
					pVertex[3].color = colors[2];
					pVertex[4].color = colors[3];
					pVertex[5].color = colors[2];
					pVertex[6].color = colors[0];
					pVertex[7].color = colors[1];

					break;
				}
			}

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectFrameIndexTable[i]);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildRectDashedFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRectDashedFrame(const BufferCreatorFunc& bufferCreator, const FloatRect& innerRect, const float offset, const float thickness, const float dashRatio, const uint32 dashCount, const Float4& color)
		{
			const float width = (innerRect.right - innerRect.left);
			const float height = (innerRect.bottom - innerRect.top);
			const float clampedDashRatio = Clamp(dashRatio, 0.0f, 1.0f);

			if ((width <= 0.0f) || (height <= 0.0f) || (thickness <= 0.0f) || (dashCount == 0) || (clampedDashRatio == 0.0f))
			{
				return 0;
			}

			if (clampedDashRatio == 1.0f)
			{
				return BuildRectFrame(bufferCreator, innerRect, thickness, ColorFillDirection::InOut, color, color);
			}

			size_t quadCount = 0;

			ForEachRectFrameDashRange(innerRect, thickness, offset, clampedDashRatio, dashCount,
				[&](const float begin, const float end)
				{
					ForEachRectFrameDashedQuad(innerRect, thickness, begin, end,
						[&](const RectFrameDashedQuad&)
						{
							++quadCount;
						});
				});

			const size_t vertexCount = (quadCount * 4);
			const size_t indexCount = (quadCount * 6);
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

			Vertex2D::IndexType quadIndex = 0;

			ForEachRectFrameDashRange(innerRect, thickness, offset, clampedDashRatio, dashCount,
				[&](const float begin, const float end)
				{
					ForEachRectFrameDashedQuad(innerRect, thickness, begin, end,
						[&](const RectFrameDashedQuad& rect)
						{
							pVertex[0].set(rect.left, rect.top, color);
							pVertex[1].set(rect.right, rect.top, color);
							pVertex[2].set(rect.left, rect.bottom, color);
							pVertex[3].set(rect.right, rect.bottom, color);
							pVertex += 4;

							const Vertex2D::IndexType vertexOffset = (indexOffset + quadIndex * 4);

							for (Vertex2D::IndexType i = 0; i < 6; ++i)
							{
								*pIndex++ = (vertexOffset + RectIndexTable[i]);
							}

							++quadIndex;
						});
				});

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildQuad
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const Float4 color)
		{
			constexpr Vertex2D::IndexType VertexCount = 4;
			constexpr Vertex2D::IndexType IndexCount = 6;
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(quad.p[0], color);
			pVertex[1].set(quad.p[1], color);
			pVertex[2].set(quad.p[3], color);
			pVertex[3].set(quad.p[2], color);

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}

		Vertex2D::IndexType BuildQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const Float4(&colors)[4])
		{
			constexpr Vertex2D::IndexType VertexCount = 4;
			constexpr Vertex2D::IndexType IndexCount = 6;
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(quad.p[0], colors[0]);
			pVertex[1].set(quad.p[1], colors[1]);
			pVertex[2].set(quad.p[3], colors[3]);
			pVertex[3].set(quad.p[2], colors[2]);

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildPolygon
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildPolygon(const BufferCreatorFunc& bufferCreator, const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const Optional<Float2>& offset, const Float4& color)
		{
			const Vertex2D::IndexType VertexCount = static_cast<Vertex2D::IndexType>(vertices.size());
			const Vertex2D::IndexType IndexCount = static_cast<Vertex2D::IndexType>(triangleIndices.size() * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			// 頂点バッファへの書き込み
			{
				const Float2* pSrc = vertices.data();
				const Float2* pSrcEnd = (pSrc + vertices.size());

				if (offset)
				{
					const Float2 _offset = *offset;

					while (pSrc != pSrcEnd)
					{
						pVertex->pos = (_offset + *pSrc++);
						pVertex->color = color;
						++pVertex;
					}
				}
				else
				{
					while (pSrc != pSrcEnd)
					{
						pVertex->pos = *pSrc++;
						pVertex->color = color;
						++pVertex;
					}
				}
			}

			// インデックスバッファへの書き込み
			{
				std::memcpy(pIndex, triangleIndices.data(), triangleIndices.size_bytes());

				for (size_t i = 0; i < IndexCount; ++i)
				{
					*(pIndex++) += indexOffset;
				}
			}

			return IndexCount;
		}

		Vertex2D::IndexType BuildPolygon(const BufferCreatorFunc& bufferCreator, const std::span<const Float2> vertices, const std::span<const Vertex2D::IndexType> indices, const Float4& color)
		{
			const Vertex2D::IndexType VertexCount = static_cast<Vertex2D::IndexType>(vertices.size());
			const Vertex2D::IndexType IndexCount = static_cast<Vertex2D::IndexType>(indices.size());
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			// 頂点バッファへの書き込み
			{
				const Float2* pSrc = vertices.data();
				const Float2* pSrcEnd = (pSrc + vertices.size());

				while (pSrc != pSrcEnd)
				{
					pVertex->pos = *pSrc++;
					pVertex->color = color;
					++pVertex;
				}
			}

			// インデックスバッファへの書き込み
			{
				std::memcpy(pIndex, indices.data(), indices.size_bytes());

				for (size_t i = 0; i < IndexCount; ++i)
				{
					*(pIndex++) += indexOffset;
				}
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildPolygonTransformed
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildPolygonTransformed(const BufferCreatorFunc& bufferCreator, const std::span<const Float2> vertices, const std::span<const TriangleIndex> triangleIndices, const float s, const float c, const Float2& offset, const Float4& color)
		{
			const Vertex2D::IndexType VertexCount = static_cast<Vertex2D::IndexType>(vertices.size());
			const Vertex2D::IndexType IndexCount = static_cast<Vertex2D::IndexType>(triangleIndices.size() * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			// 頂点バッファへの書き込み
			{
				const Float2* pSrc = vertices.data();
				const Float2* pSrcEnd = (pSrc + vertices.size());
				const Float2 _offset = offset;

				while (pSrc != pSrcEnd)
				{
					const Float2 v = *pSrc++;
					const float x = (v.x * c - v.y * s + _offset.x);
					const float y = (v.x * s + v.y * c + _offset.y);
					pVertex->pos.set(x, y);
					pVertex->color = color;
					++pVertex;
				}
			}

			// インデックスバッファへの書き込み
			{
				std::memcpy(pIndex, triangleIndices.data(), triangleIndices.size_bytes());

				for (size_t i = 0; i < IndexCount; ++i)
				{
					*(pIndex++) += indexOffset;
				}
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedQuad
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildTexturedQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const FloatRect& uv, const Float4& color)
		{
			constexpr Vertex2D::IndexType VertexCount	= 4;
			constexpr Vertex2D::IndexType IndexCount	= 6;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(quad.p[0], uv.left, uv.top, color);
			pVertex[1].set(quad.p[1], uv.right, uv.top, color);
			pVertex[2].set(quad.p[3], uv.left, uv.bottom, color);
			pVertex[3].set(quad.p[2], uv.right, uv.bottom, color);

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}

		Vertex2D::IndexType BuildTexturedQuad(const BufferCreatorFunc& bufferCreator, const FloatQuad& quad, const FloatRect& uv, const Float4(&colors)[4])
		{
			constexpr Vertex2D::IndexType VertexCount	= 4;
			constexpr Vertex2D::IndexType IndexCount	= 6;
			auto [pVertex, pIndex, indexOffset]			= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			pVertex[0].set(quad.p[0], uv.left, uv.top, colors[0]);
			pVertex[1].set(quad.p[1], uv.right, uv.top, colors[1]);
			pVertex[2].set(quad.p[3], uv.left, uv.bottom, colors[3]);
			pVertex[3].set(quad.p[2], uv.right, uv.bottom, colors[2]);

			for (Vertex2D::IndexType i = 0; i < IndexCount; ++i)
			{
				*pIndex++ = (indexOffset + RectIndexTable[i]);
			}

			return IndexCount;
		}
	}
}
