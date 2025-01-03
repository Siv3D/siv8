//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
