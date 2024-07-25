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

		static constexpr Vertex2D::IndexType CircleFrameIndexTable[6] = { 0, 2, 1, 1, 2, 3 };

		static const std::array<Float2, 2016> SinCosTable = []()
		{
			std::array<Float2, 2016> table;

			Float2* pDst = table.data();

			for (int32 quality = 1; quality <= 63; ++quality)
			{
				const float radDelta = (Math::HalfPiF / quality);

				for (int32 i = 0; i < quality; ++i)
				{
					const float rad = (radDelta * i);
					*pDst++ = { std::sin(rad), -std::cos(rad) };
				}
			}

			return table;
		}();

		[[nodiscard]]
		static constexpr size_t GetSinCosTableIndex(Vertex2D::IndexType quality)
		{
			return ((quality - 1) * quality / 2);
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
				return static_cast<Vertex2D::IndexType>(Min((5 + static_cast<int32>((r - 8.0f) / 8.0f)), 63));
			}
		}

		[[nodiscard]]
		constexpr Vertex2D::IndexType CalculateCirclePieQuality(const float r, const float angle)
		{
			const Vertex2D::IndexType baseQuality = (CalculateCircleQuality(r) * 4);
			const float angleDelta = (Math::TwoPiF / baseQuality);
			return static_cast<Vertex2D::IndexType>(Max(std::ceil(angle / angleDelta), 5.0f));
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildCircleArcRoundCap(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const Float4& color, const float scale)
		{
			const Vertex2D::IndexType Quality = CalculateCirclePieQuality((r * scale), Math::PiF);
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
			pVertex[0].pos.set(centerX, centerY);

			// 周
			{
				const float radDelta = (Math::PiF / (Quality - 1));
				Vertex2D* pDst = &pVertex[1];

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const float rad = (startAngle + (radDelta * i));
					const auto [s, c] = FastMath::SinCos(rad);
					(pDst++)->pos.set((centerX + r * s), (centerY - r * c));
				}
			}

			for (size_t i = 0; i < VertexCount; ++i)
			{
				(pVertex++)->color = color;
			}

			for (Vertex2D::IndexType i = 0; i < (Quality - 1); ++i)
			{
				*pIndex++ = indexOffset;
				*pIndex++ = (indexOffset + i + 1);
				*pIndex++ = (indexOffset + i + 2);
			}

			return IndexCount;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildCircleArcRoundCap(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const Vertex2D::IndexType Quality = CalculateCirclePieQuality((r * scale), Math::PiF);
			const Vertex2D::IndexType VertexCount = (Quality + 1);
			const Vertex2D::IndexType IndexCount = ((Quality - 1) * 3);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

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
					const float radDelta = (Math::PiF / (Quality - 1));
					Vertex2D* pDst = &pVertex[1];

					for (Vertex2D::IndexType i = 0; i < Quality; ++i)
					{
						const float rad = (startAngle + (radDelta * i));
						const float f = std::sin(radDelta * i);
						const auto [s, c] = FastMath::SinCos(rad);
						(pDst++)->set((centerX + r * s), (centerY - r * c), (c0 + colorDiff * f));
					}
				}
			}
			else
			{
				// 中心
				pVertex[0].set(centerX, centerY, (c0 + colorDiff * 0.5f));

				// 周
				{
					const float radDelta = (Math::PiF / (Quality - 1));
					Vertex2D* pDst = &pVertex[1];

					for (Vertex2D::IndexType i = 0; i < Quality; ++i)
					{
						const float rad = (startAngle + (radDelta * i));
						const float f = (std::cos(radDelta * i) * -0.5f + 0.5f);
						const auto [s, c] = FastMath::SinCos(rad);
						(pDst++)->set((centerX + r * s), (centerY - r * c), (c0 + colorDiff * f));
					}
				}
			}

			for (Vertex2D::IndexType i = 0; i < (Quality - 1); ++i)
			{
				*pIndex++ = indexOffset;
				*pIndex++ = (indexOffset + i + 1);
				*pIndex++ = (indexOffset + i + 2);
			}

			return IndexCount;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildRoundCircleArc(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			Vertex2D::IndexType indexCount = 0;

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

			if (colorType == ColorFillDirection::LeftRight)
			{
				if (angle < 0.0)
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + angle + Math::PiF), colorType, c1, color1, scale);
				}
				else
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + Math::PiF), colorType, c0, color0, scale);
				}
			}
			else
			{
				if (angle < 0.0)
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + angle + Math::PiF), colorType, color0, color1, scale);
				}
				else
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + Math::PiF), colorType, color0, color1, scale);
				}
			}

			if (angle != 0.0)
			{
				const float rOuter = (rInner + thickness);
				const Vertex2D::IndexType Quality = CalculateCirclePieQuality((rOuter * scale), Abs(angle));
				const Vertex2D::IndexType VertexCount = (Quality * 2);
				const Vertex2D::IndexType IndexCount = ((Quality - 1) * 6);
				auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

				if (not pVertex)
				{
					return 0;
				}

				{
					const float centerX = center.x;
					const float centerY = center.y;
					const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
					const float radDelta = (Abs(angle) / (Quality - 1));
					Vertex2D* pDst = pVertex;

					for (Vertex2D::IndexType i = 0; i < Quality; ++i)
					{
						const float rad = (start + (radDelta * i));
						const auto [s, c] = FastMath::SinCos(rad);
						(pDst++)->pos.set((centerX + rInner * s), (centerY - rInner * c));
						(pDst++)->pos.set((centerX + rOuter * s), (centerY - rOuter * c));
					}
				}

				if (colorType == ColorFillDirection::LeftRight)
				{
					const Float4 startColor = ((angle < 0.0f) ? c1 : c0);
					const Float4 endColor = ((angle < 0.0f) ? c0 : c1);
					const Float4 colorDelta = ((endColor - startColor) / static_cast<float>((VertexCount / 2) - 1));

					for (Vertex2D::IndexType i = 0; i < (VertexCount / 2); ++i)
					{
						const Float4 color = (startColor + (colorDelta * i));
						(pVertex++)->color = color;
						(pVertex++)->color = color;
					}
				}
				else
				{
					for (size_t i = 0; i < VertexCount / 2; ++i)
					{
						(pVertex++)->color = color0;
						(pVertex++)->color = color1;
					}
				}

				for (Vertex2D::IndexType i = 0; i < (Quality - 1); ++i)
				{
					for (Vertex2D::IndexType k = 0; k < 6; ++k)
					{
						*pIndex++ = indexOffset + (i * 2 + RectIndexTable[k]);
					}
				}

				indexCount += IndexCount;
			}

			if (colorType == ColorFillDirection::LeftRight)
			{
				if (angle < 0.0)
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, endCenter, halfThickness, startAngle, colorType, c0, color0, scale);
				}
				else
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, endCenter, halfThickness, (startAngle + angle), colorType, c1, color1, scale);
				}
			}
			else
			{
				if (angle < 0.0)
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, endCenter, halfThickness, startAngle, colorType, color1, color0, scale);
				}
				else
				{
					indexCount += BuildCircleArcRoundCap(bufferCreator, endCenter, halfThickness, (startAngle + angle), colorType, color1, color0, scale);
				}
			}

			return indexCount;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildFlatCircleArc(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const float rOuter = (rInner + thickness);
			const Vertex2D::IndexType Quality = CalculateCirclePieQuality((rOuter * scale), Abs(angle));
			const Vertex2D::IndexType VertexCount = (Quality * 2);
			const Vertex2D::IndexType IndexCount = ((Quality - 1) * 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			{
				const float centerX = center.x;
				const float centerY = center.y;
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = (Abs(angle) / (Quality - 1));
				Vertex2D* pDst = pVertex;

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const float rad = (start + (radDelta * i));
					const auto [s, c] = FastMath::SinCos(rad);
					(pDst++)->pos.set((centerX + rInner * s), (centerY - rInner * c));
					(pDst++)->pos.set((centerX + rOuter * s), (centerY - rOuter * c));
				}
			}

			if (colorType == ColorFillDirection::LeftRight)
			{
				const Float4 startColor = ((angle < 0.0f) ? color1 : color0);
				const Float4 endColor = ((angle < 0.0f) ? color0 : color1);
				const Float4 colorDelta = ((endColor - startColor) / static_cast<float>((VertexCount / 2) - 1));

				for (Vertex2D::IndexType i = 0; i < (VertexCount / 2); ++i)
				{
					const Float4 color = (startColor + (colorDelta * i));
					(pVertex++)->color = color;
					(pVertex++)->color = color;
				}
			}
			else
			{
				for (size_t i = 0; i < VertexCount / 2; ++i)
				{
					(pVertex++)->color = color0;
					(pVertex++)->color = color1;
				}
			}

			for (Vertex2D::IndexType i = 0; i < (Quality - 1); ++i)
			{
				for (Vertex2D::IndexType k = 0; k < 6; ++k)
				{
					*pIndex++ = indexOffset + (i * 2 + RectIndexTable[k]);
				}
			}

			return IndexCount;
		}
	}

	namespace Vertex2DBuilder
	{
		////////////////////////////////////////////////////////////////
		//
		//	BuildLine
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildLine(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const Float2& start, const Float2& end, const float thickness, const Float4(&colors)[2], const float scale)
		{
			constexpr Vertex2D::IndexType VertexCount = 4;
			constexpr Vertex2D::IndexType IndexCount = 6;
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float halfThickness = (thickness * 0.5f);
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

			Vertex2D::IndexType roundIndexCount = 0;
			const float startAngle = std::atan2(vNormal.x, -vNormal.y);

			if (startCap == LineCap::Round)
			{
				roundIndexCount += BuildCircleArcRoundCap(bufferCreator, start, halfThickness, startAngle, colors[0], scale);
			}

			if (endCap == LineCap::Round)
			{
				roundIndexCount += BuildCircleArcRoundCap(bufferCreator, end, halfThickness, (startAngle + Math::PiF), colors[1], scale);
			}

			return (IndexCount + roundIndexCount);
		}

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
			constexpr Vertex2D::IndexType IndexCount		= 3;
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
			constexpr Vertex2D::IndexType IndexCount		= 3;
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
			constexpr Vertex2D::IndexType IndexCount		= 6;
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
			constexpr Vertex2D::IndexType IndexCount		= 6;
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
			constexpr Vertex2D::IndexType IndexCount		= 24;
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

			const float centerX = center.x;
			const float centerY = center.y;
			pVertex[0].pos.set(centerX, centerY);

			const Float4 colorDiff = (color1 - color0);

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));
			
			Vertex2D* pDst0 = &pVertex[1];
			Vertex2D* pDst1 = (pDst0 + Quality);
			Vertex2D* pDst2 = (pDst1 + Quality);
			Vertex2D* pDst3 = (pDst2 + Quality);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float x = (cs->x * r);
				const float y = (cs->y * r);

				if (colorType == ColorFillDirection::InOut)
				{
					pDst0->set((centerX + x), (centerY + y), color1);
					pDst1->set((centerX - y), (centerY + x), color1);
					pDst2->set((centerX - x), (centerY - y), color1);
					pDst3->set((centerX + y), (centerY - x), color1);
				}
				else if(colorType == ColorFillDirection::TopBottom)
				{
					const auto c0 = (color0 + ((cs->y + 1.0f) * 0.5f) * colorDiff);
					const auto c1 = (color0 + ((cs->x + 1.0f) * 0.5f) * colorDiff);
					const auto c2 = (color0 + ((-cs->y + 1.0f) * 0.5f) * colorDiff);
					const auto c3 = (color0 + ((-cs->x + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + x), (centerY + y), c0);
					pDst1->set((centerX - y), (centerY + x), c1);
					pDst2->set((centerX - x), (centerY - y), c2);
					pDst3->set((centerX + y), (centerY - x), c3);
				}
				else
				{
					const auto c0 = (color0 + ((cs->x + 1.0f) * 0.5f) * colorDiff);
					const auto c1 = (color0 + ((-cs->y + 1.0f) * 0.5f) * colorDiff);
					const auto c2 = (color0 + ((-cs->x + 1.0f) * 0.5f) * colorDiff);
					const auto c3 = (color0 + ((cs->y + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + x), (centerY + y), c0);
					pDst1->set((centerX - y), (centerY + x), c1);
					pDst2->set((centerX - x), (centerY - y), c2);
					pDst3->set((centerX + y), (centerY - x), c3);
				}

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// 中心の色
			if (colorType == ColorFillDirection::InOut)
			{
				pVertex->color = color0;
			}
			else
			{
				pVertex->color = color0.getMidpoint(color1);
			}

			{
				for (Vertex2D::IndexType i = 0; i < (FullQuality - 1); ++i)
				{
					*pIndex++ = indexOffset;
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = indexOffset + (i + 2);
				}

				*pIndex++ = indexOffset;
				*pIndex++ = (indexOffset + FullQuality);
				*pIndex++ = (indexOffset + 1);
			}

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

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			Vertex2D* pDst0 = &pVertex[0];
			Vertex2D* pDst1 = (pDst0 + Quality * 2);
			Vertex2D* pDst2 = (pDst0 + Quality * 4);
			Vertex2D* pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float ox = (cs->x * rOuter);
				const float ix = (cs->x * rInner);
				const float oy = (cs->y * rOuter);
				const float iy = (cs->y * rInner);

				(pDst0++)->pos.set((centerX + ox), (centerY + oy));
				(pDst0++)->pos.set((centerX + ix), (centerY + iy));

				(pDst1++)->pos.set((centerX - oy), (centerY + ox));
				(pDst1++)->pos.set((centerX - iy), (centerY + ix));

				(pDst2++)->pos.set((centerX - ox), (centerY - oy));
				(pDst2++)->pos.set((centerX - ix), (centerY - iy));

				(pDst3++)->pos.set((centerX + oy), (centerY - ox));
				(pDst3++)->pos.set((centerX + iy), (centerY - ix));
			}

			for (Vertex2D::IndexType i = 0; i < FullQuality; ++i)
			{
				(pVertex++)->color = outerColor;
				(pVertex++)->color = innerColor;
			}

			for (Vertex2D::IndexType i = 0; i < FullQuality; ++i)
			{
				for (Vertex2D::IndexType k = 0; k < 6; ++k)
				{
					*pIndex++ = (indexOffset + (i * 2 + CircleFrameIndexTable[k]) % (FullQuality * 2));
				}
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCirclePie
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCirclePie(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			if (angle == 0.0f)
			{
				return 0;
			}

			const Vertex2D::IndexType Quality = CalculateCirclePieQuality((r * scale), Abs(angle));
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
			pVertex[0].pos.set(centerX, centerY);

			// 周
			{
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = (Abs(angle) / (Quality - 1));
				Vertex2D* pDst = &pVertex[1];

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const float rad = (start + (radDelta * i));
					const auto [s, c] = FastMath::SinCos(rad);
					(pDst++)->pos.set((centerX + r * s), (centerY - r * c));
				}
			}

			{
				(pVertex++)->color = innerColor;

				for (size_t i = 1; i < VertexCount; ++i)
				{
					(pVertex++)->color = outerColor;
				}
			}

			for (Vertex2D::IndexType i = 0; i < (Quality - 1); ++i)
			{
				*pIndex++ = indexOffset;
				*pIndex++ = (indexOffset + i + 1);
				*pIndex++ = (indexOffset + i + 2);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildCircleArc
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleArc(const BufferCreatorFunc& bufferCreator, const LineCap lineCap, const Float2& center, const float rInner, const float startAngle, const float angle, const float thickness, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			if (lineCap == LineCap::Round)
			{
				return BuildRoundCircleArc(bufferCreator, center, rInner, startAngle, angle, thickness, colorType, color0, color1, scale);
			}
			else
			{
				if (angle == 0.0f)
				{
					return 0;
				}

				return BuildFlatCircleArc(bufferCreator, center, rInner, startAngle, angle, thickness, colorType, color0, color1, scale);
			}
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
	}
}
