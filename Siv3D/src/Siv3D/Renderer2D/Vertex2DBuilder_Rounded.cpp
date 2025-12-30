//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
		static Vertex2D::IndexType BuildRoundCap(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const Float4& color, const float scale)
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
		static Vertex2D::IndexType BuildRoundCap(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
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
					indexCount += BuildRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + angle + Math::PiF), colorType, c1, color1, scale);
				}
				else
				{
					indexCount += BuildRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + Math::PiF), colorType, c0, color0, scale);
				}
			}
			else
			{
				if (angle < 0.0)
				{
					indexCount += BuildRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + angle + Math::PiF), colorType, color0, color1, scale);
				}
				else
				{
					indexCount += BuildRoundCap(bufferCreator, startCenter, halfThickness, (startAngle + Math::PiF), colorType, color0, color1, scale);
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
					indexCount += BuildRoundCap(bufferCreator, endCenter, halfThickness, startAngle, colorType, c0, color0, scale);
				}
				else
				{
					indexCount += BuildRoundCap(bufferCreator, endCenter, halfThickness, (startAngle + angle), colorType, c1, color1, scale);
				}
			}
			else
			{
				if (angle < 0.0)
				{
					indexCount += BuildRoundCap(bufferCreator, endCenter, halfThickness, startAngle, colorType, color1, color0, scale);
				}
				else
				{
					indexCount += BuildRoundCap(bufferCreator, endCenter, halfThickness, (startAngle + angle), colorType, color1, color0, scale);
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

		[[nodiscard]]
		static Array<Float2> SimplifyOpen(const std::span<const Vec2> points, const float scale, const Optional<Float2>& offset)
		{
			const double threshold = (0.25f / scale);
			const double thresholdSq = (threshold * threshold);
			const size_t size = points.size();

			Array<Float2> buffer(Arg::reserve = size);
			{
				Vec2 previous = points.front();

				buffer.push_back(previous);

				for (size_t i = 1; i < (size - 1); ++i)
				{
					const Vec2 current = points[i];

					if (previous.distanceFromSq(current) < thresholdSq)
					{
						continue;
					}

					buffer.push_back(current);

					previous = current;
				}

				const Vec2 current = points.back();

				if (previous != current)
				{
					buffer.push_back(current);
				}
			}

			if (offset)
			{
				const Float2 v = *offset;

				for (Float2& point : buffer)
				{
					point += v;
				}
			}

			return buffer;
		}

		[[nodiscard]]
		static std::pair<Array<Float2>, Array<Float4>> SimplifyOpen(const std::span<const Vec2> points, const float scale, const Optional<Float2>& offset, const std::span<const ColorF> colors)
		{
			const double threshold = (0.25f / scale);
			const double thresholdSq = (threshold * threshold);
			const size_t size = points.size();

			Array<Float2> posBuffer(Arg::reserve = size);
			Array<Float4> colorBuffer(Arg::reserve = size);
			{
				Vec2 previous = points.front();

				posBuffer.push_back(previous);
				colorBuffer.push_back(colors.front().toFloat4());

				for (size_t i = 1; i < (size - 1); ++i)
				{
					const Vec2 current = points[i];

					if (previous.distanceFromSq(current) < thresholdSq)
					{
						continue;
					}

					posBuffer.push_back(current);
					colorBuffer.push_back(colors[i].toFloat4());

					previous = current;
				}

				const Vec2 current = points.back();

				if (previous != current)
				{
					posBuffer.push_back(current);
					colorBuffer.push_back(colors.back().toFloat4());
				}
			}

			if (offset)
			{
				const Float2 v = *offset;

				for (Float2& point : posBuffer)
				{
					point += v;
				}
			}

			return{ std::move(posBuffer), std::move(colorBuffer) };
		}

		[[nodiscard]]
		static Array<Float2> SimplifyClosed(const std::span<const Vec2> points, const float scale, const Optional<Float2>& offset)
		{
			const double threshold = (0.25f / scale);
			const double thresholdSq = (threshold * threshold);
			const size_t size = points.size();

			Array<Float2> buffer(Arg::reserve = size);
			{
				Vec2 previous = points.front();

				buffer.push_back(previous);

				for (size_t i = 1; i < (size - 1); ++i)
				{
					const Vec2 current = points[i];

					if (previous.distanceFromSq(current) < thresholdSq)
					{
						continue;
					}

					buffer.push_back(current);

					previous = current;
				}

				const Vec2 current = points.back();

				if (previous != current)
				{
					buffer.push_back(current);
				}

				if ((2 < buffer.size())
					&& (buffer.back().distanceFromSq(buffer.front()) <= thresholdSq))
				{
					buffer.pop_back();
				}
			}

			if (offset)
			{
				const Float2 v = *offset;

				for (Float2& point : buffer)
				{
					point += v;
				}
			}

			return buffer;
		}

		[[nodiscard]]
		static std::pair<Array<Float2>, Array<Float4>> SimplifyClosed(const std::span<const Vec2> points, const float scale, const Optional<Float2>& offset, const std::span<const ColorF> colors)
		{
			const double threshold = (0.25f / scale);
			const double thresholdSq = (threshold * threshold);
			const size_t size = points.size();

			Array<Float2> posBuffer(Arg::reserve = size);
			Array<Float4> colorBuffer(Arg::reserve = size);
			{
				Vec2 previous = points.front();

				posBuffer.push_back(previous);
				colorBuffer.push_back(colors.front().toFloat4());

				for (size_t i = 1; i < (size - 1); ++i)
				{
					const Vec2 current = points[i];

					if (previous.distanceFromSq(current) < thresholdSq)
					{
						continue;
					}

					posBuffer.push_back(current);
					colorBuffer.push_back(colors[i].toFloat4());

					previous = current;
				}

				const Vec2 current = points.back();

				if (previous != current)
				{
					posBuffer.push_back(current);
					colorBuffer.push_back(colors.back().toFloat4());
				}

				if ((2 < posBuffer.size())
					&& (posBuffer.back().distanceFromSq(posBuffer.front()) <= thresholdSq))
				{
					posBuffer.pop_back();
					colorBuffer.pop_back();
				}
			}

			if (offset)
			{
				const Float2 v = *offset;

				for (Float2& point : posBuffer)
				{
					point += v;
				}
			}

			return{ std::move(posBuffer), std::move(colorBuffer) };
		}

		[[nodiscard]]
		static Array<Float2> AdjustOpen(const Array<Float2>& base, const float scale, const bool inner)
		{
			const float epsilon = (0.001f / scale);
			constexpr float AngleThreshold = 0.55f;

			Array<Float2> results(Arg::reserve = base.size());
			{
				results.push_back(base.front());

				const size_t count = (base.size() - 1);

				for (size_t i = 1; i < count; ++i)
				{
					const Float2 back = base[i - 1];
					const Float2 current = base[i];
					const Float2 next = base[i + 1];
					const Float2 v1 = (back - current).normalized();
					const Float2 v2 = (next - current).normalized();

					results.push_back(current);

					if ((not inner)
						&& (AngleThreshold < v1.dot(v2)))
					{
						const Float2 line = (current - back);
						const Float2 tangent = ((next - current).normalized() + (current - back).normalized()).normalized();
						const Float2 line2 = (next - current);

						const float a = tangent.dot(line2);
						const float b = (-tangent).dot(line2);

						if (a >= b)
						{
							results.push_back(current + tangent.normalized() * epsilon);
						}
						else if (a <= b)
						{
							results.push_back(current + (-tangent).normalized() * epsilon);
						}
						else
						{
							const Float2 normal = Float2{ -line.y, line.x }.normalized();
							results.push_back(current + normal * epsilon);
						}
					}
				}

				results.push_back(base.back());
			}

			return results;
		}

		[[nodiscard]]
		static std::pair<Array<Float2>, Array<Float4>> AdjustOpen(const Array<Float2>& posBase, const Array<Float4>& colorBase, const float scale, const bool inner)
		{
			const float epsilon = (0.001f / scale);
			constexpr float AngleThreshold = 0.55f;

			Array<Float2> posResults(Arg::reserve = posBase.size());
			Array<Float4> colorResults(Arg::reserve = colorBase.size());
			{
				posResults.push_back(posBase.front());
				colorResults.push_back(colorBase.front());

				const size_t count = (posBase.size() - 1);

				for (size_t i = 1; i < count; ++i)
				{
					const Float2 back = posBase[i - 1];
					const Float2 current = posBase[i];
					const Float2 next = posBase[i + 1];
					const Float2 v1 = (back - current).normalized();
					const Float2 v2 = (next - current).normalized();
					const Float4& color = colorBase[i];

					posResults.push_back(current);
					colorResults.push_back(color);

					if ((not inner)
						&& (AngleThreshold < v1.dot(v2)))
					{
						const Float2 line = (current - back);
						const Float2 tangent = ((next - current).normalized() + (current - back).normalized()).normalized();
						const Float2 line2 = (next - current);

						const float a = tangent.dot(line2);
						const float b = (-tangent).dot(line2);

						if (a >= b)
						{
							posResults.push_back(current + tangent.normalized() * epsilon);
							colorResults.push_back(color);

						}
						else if (a <= b)
						{
							posResults.push_back(current + (-tangent).normalized() * epsilon);
							colorResults.push_back(color);
						}
						else
						{
							const Float2 normal = Float2{ -line.y, line.x }.normalized();
							posResults.push_back(current + normal * epsilon);
							colorResults.push_back(color);
						}
					}
				}

				posResults.push_back(posBase.back());
				colorResults.push_back(colorBase.back());
			}

			return{ std::move(posResults), std::move(colorResults) };
		}

		[[nodiscard]]
		static Array<Float2> AdjustClosed(const Array<Float2>& base, const float scale, const bool inner)
		{
			const float epsilon = (0.001f / scale);
			constexpr float AngleThreshold = 0.55f;

			Array<Float2> results(Arg::reserve = base.size());
			{
				results.push_back(base.front());

				const size_t count = base.size();

				for (size_t i = 1; i < count; ++i)
				{
					const Float2 back = base[i - 1];
					const Float2 current = base[i];
					const Float2 next = base[(i + 1) % base.size()];
					const Float2 v1 = (back - current).normalized();
					const Float2 v2 = (next - current).normalized();

					results.push_back(current);

					if ((not inner)
						&& (AngleThreshold < v1.dot(v2)))
					{
						const Float2 line = (current - back);
						const Float2 tangent = ((next - current).normalized() + (current - back).normalized()).normalized();
						const Float2 line2 = (next - current);

						const float a = tangent.dot(line2);
						const float b = (-tangent).dot(line2);

						if (a >= b)
						{
							results.push_back(current + tangent.normalized() * epsilon);
						}
						else if (a <= b)
						{
							results.push_back(current + (-tangent).normalized() * epsilon);
						}
						else
						{
							const Float2 normal = Float2{ -line.y, line.x }.normalized();
							results.push_back(current + normal * epsilon);
						}
					}
				}

				{
					const Float2 back = base[base.size() - 1];
					const Float2 current = base[0];
					const Float2 next = base[1];

					const Float2 v1 = (back - current).normalized();
					const Float2 v2 = (next - current).normalized();

					if ((not inner)
						&& (AngleThreshold < v1.dot(v2)))
					{
						const Float2 line = (current - back);
						const Float2 tangent = ((next - current).normalized() + (current - back).normalized()).normalized();
						const Float2 line2 = (next - current);

						const float a = tangent.dot(line2);
						const float b = (-tangent).dot(line2);

						if (a >= b)
						{
							results.push_back(current - tangent.normalized() * epsilon);
						}
						else if (a <= b)
						{
							results.push_back(current - (-tangent).normalized() * epsilon);
						}
						else
						{
							const Float2 normal = Float2{ -line.y, line.x }.normalized();
							results.push_back(current - normal * epsilon);
						}
					}
				}
			}

			return results;
		}

		[[nodiscard]]
		static std::pair<Array<Float2>, Array<Float4>> AdjustClosed(const Array<Float2>& posBase, const Array<Float4>& colorBase, const float scale, const bool inner)
		{
			const float epsilon = (0.001f / scale);
			constexpr float AngleThreshold = 0.55f;

			Array<Float2> posResults(Arg::reserve = posBase.size());
			Array<Float4> colorResults(Arg::reserve = colorBase.size());
			{
				posResults.push_back(posBase.front());
				colorResults.push_back(colorBase.front());

				const size_t count = posBase.size();

				for (size_t i = 1; i < count; ++i)
				{
					const Float2 back = posBase[i - 1];
					const Float2 current = posBase[i];
					const Float2 next = posBase[(i + 1) % posBase.size()];
					const Float2 v1 = (back - current).normalized();
					const Float2 v2 = (next - current).normalized();

					posResults.push_back(current);
					colorResults.push_back(colorBase[i]);

					if ((not inner)
						&& (AngleThreshold < v1.dot(v2)))
					{
						const Float2 line = (current - back);
						const Float2 tangent = ((next - current).normalized() + (current - back).normalized()).normalized();
						const Float2 line2 = (next - current);

						const float a = tangent.dot(line2);
						const float b = (-tangent).dot(line2);

						if (a >= b)
						{
							posResults.push_back(current + tangent.normalized() * epsilon);
							colorResults.push_back(colorBase[i]);
						}
						else if (a <= b)
						{
							posResults.push_back(current + (-tangent).normalized() * epsilon);
							colorResults.push_back(colorBase[i]);
						}
						else
						{
							const Float2 normal = Float2{ -line.y, line.x }.normalized();
							posResults.push_back(current + normal * epsilon);
							colorResults.push_back(colorBase[i]);
						}
					}
				}

				{
					const Float2 back = posBase[posBase.size() - 1];
					const Float2 current = posBase[0];
					const Float2 next = posBase[1];

					const Float2 v1 = (back - current).normalized();
					const Float2 v2 = (next - current).normalized();

					if ((not inner)
						&& (AngleThreshold < v1.dot(v2)))
					{
						const Float2 line = (current - back);
						const Float2 tangent = ((next - current).normalized() + (current - back).normalized()).normalized();
						const Float2 line2 = (next - current);

						const float a = tangent.dot(line2);
						const float b = (-tangent).dot(line2);

						if (a >= b)
						{
							posResults.push_back(current - tangent.normalized() * epsilon);
							colorResults.push_back(colorBase[0]);
						}
						else if (a <= b)
						{
							posResults.push_back(current - (-tangent).normalized() * epsilon);
							colorResults.push_back(colorBase[0]);
						}
						else
						{
							const Float2 normal = Float2{ -line.y, line.x }.normalized();
							posResults.push_back(current - normal * epsilon);
							colorResults.push_back(colorBase[0]);
						}
					}
				}
			}

			return{ std::move(posResults), std::move(colorResults) };
		}

		[[nodiscard]]
		static float CalculateLength(const std::span<const Float2> points) noexcept
		{
			float length = 0.0f;
			
			for (size_t i = 1; i < points.size(); ++i)
			{
				length += points[i].distanceFrom(points[i - 1]);
			}

			return length;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildUncappedLineString(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& color, const float scale, float& startAngle, float& endAngle)
		{
			const Array<Float2> base = SimplifyOpen(points, scale, offset);
			const Array<Float2> buffer = AdjustOpen(base, scale, inner);

			const Vertex2D::IndexType newSize = static_cast<Vertex2D::IndexType>(buffer.size());
			const Vertex2D::IndexType vertexSize = (newSize * 2), indexSize = (6 * (newSize - 1));
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexSize, indexSize);

			if (not pVertex)
			{
				return 0;
			}

			const float halfThickness = (thickness * 0.5f);

			const Float2* const pBuf = buffer.data();

			{
				const Float2 p0 = pBuf[0];
				const Float2 p1 = pBuf[1];
				const Float2 line = (p1 - p0).normalize();
				const Float2 vNormalBegin{ -line.y * halfThickness, line.x * halfThickness };

				{
					startAngle = std::atan2(vNormalBegin.x, -vNormalBegin.y);
				}

				pVertex[0].pos.set(p0 + vNormalBegin);
				pVertex[1].pos.set(p0 - vNormalBegin);
			}

			for (Vertex2D::IndexType i = 0; i < (newSize - 2); ++i)
			{
				const Float2 p0 = pBuf[i];
				const Float2 p1 = pBuf[i + 1];
				const Float2 p2 = pBuf[i + 2];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = halfThickness / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				pVertex[i * 2 + 2].pos.set(result0);
				pVertex[i * 2 + 3].pos.set(result1);
			}

			{
				const Float2 p0 = pBuf[newSize - 2];
				const Float2 p1 = pBuf[newSize - 1];
				const Float2 line = (p1 - p0).normalize();
				const Float2 vNormalEnd{ -line.y * halfThickness, line.x * halfThickness };

				{
					endAngle = std::atan2(vNormalEnd.x, -vNormalEnd.y);
				}

				pVertex[newSize * 2 - 2].pos.set(p1 + vNormalEnd);
				pVertex[newSize * 2 - 1].pos.set(p1 - vNormalEnd);
			}

			for (size_t i = 0; i < vertexSize; ++i)
			{
				(pVertex++)->color = color;
			}

			{
				const Vertex2D::IndexType count = static_cast<Vertex2D::IndexType>(newSize - 1);

				for (Vertex2D::IndexType k = 0; k < count; ++k)
				{
					for (Vertex2D::IndexType i = 0; i < 6; ++i)
					{
						*pIndex++ = (indexOffset + (RectIndexTable[i] + k * 2) % vertexSize);
					}
				}
			}

			return indexSize;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildUncappedLineString(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& colorStart, const Float4& colorEnd, const float scale, float& startAngle, float& endAngle, Float4& startColor, Float4& endColor, const bool hasStartCap, const bool hasEndCap)
		{
			const float halfThickness = (thickness * 0.5f);
			const Array<Float2> base = SimplifyOpen(points, scale, offset);
			const Array<Float2> buffer = AdjustOpen(base, scale, inner);
			const float baseLength = CalculateLength(buffer);
			const float allLength = (baseLength + (hasStartCap ? halfThickness : 0.0f) + (hasEndCap ? halfThickness : 0.0f));
			const Float4 color0 = (hasStartCap ? colorStart.lerp(colorEnd, (halfThickness / allLength)) : colorStart);
			const Float4 color1 = (hasEndCap ? colorStart.lerp(colorEnd, (1.0f - halfThickness / allLength)) : colorEnd);
			const Float4 colorDiff = (color1 - color0);
			const float lengthInv = (1.0f / allLength);

			const Vertex2D::IndexType newSize = static_cast<Vertex2D::IndexType>(buffer.size());
			const Vertex2D::IndexType vertexSize = (newSize * 2), indexSize = (6 * (newSize - 1));
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexSize, indexSize);

			if (not pVertex)
			{
				return 0;
			}

			const Float2* const pBuf = buffer.data();
			float distanceFromStart = (hasStartCap ? halfThickness : 0.0f);

			{
				const Float2 p0 = pBuf[0];
				const Float2 p1 = pBuf[1];
				const Float2 line = (p1 - p0).normalize();
				const Float2 vNormalBegin{ -line.y * halfThickness, line.x * halfThickness };

				{
					startAngle = std::atan2(vNormalBegin.x, -vNormalBegin.y);
					startColor = color0;
				}

				pVertex[0].set(p0 + vNormalBegin, color0);
				pVertex[1].set(p0 - vNormalBegin, color0);
			}

			for (Vertex2D::IndexType i = 0; i < (newSize - 2); ++i)
			{
				distanceFromStart += pBuf[i + 1].distanceFrom(pBuf[i]);

				const Float2 p0 = pBuf[i];
				const Float2 p1 = pBuf[i + 1];
				const Float2 p2 = pBuf[i + 2];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = halfThickness / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;
				const Float4 color = (color0 + colorDiff * (distanceFromStart * lengthInv));

				pVertex[i * 2 + 2].set(result0, color);
				pVertex[i * 2 + 3].set(result1, color);
			}

			{
				const Float2 p0 = pBuf[newSize - 2];
				const Float2 p1 = pBuf[newSize - 1];
				const Float2 line = (p1 - p0).normalize();
				const Float2 vNormalEnd{ -line.y * halfThickness, line.x * halfThickness };

				{
					endAngle = std::atan2(vNormalEnd.x, -vNormalEnd.y);
					endColor = color1;
				}

				pVertex[newSize * 2 - 2].set(p1 + vNormalEnd, color1);
				pVertex[newSize * 2 - 1].set(p1 - vNormalEnd, color1);
			}

			{
				const Vertex2D::IndexType count = static_cast<Vertex2D::IndexType>(newSize - 1);

				for (Vertex2D::IndexType k = 0; k < count; ++k)
				{
					for (Vertex2D::IndexType i = 0; i < 6; ++i)
					{
						*pIndex++ = (indexOffset + (RectIndexTable[i] + k * 2) % vertexSize);
					}
				}
			}

			return indexSize;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildUncappedLineString(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const std::span<const ColorF> colors, const float scale, float& startAngle, float& endAngle)
		{
			const std::pair<Array<Float2>,Array<Float4>> base = SimplifyOpen(points, scale, offset, colors);
			const std::pair<Array<Float2>, Array<Float4>> buffer = AdjustOpen(base.first, base.second, scale, inner);
			const auto& posBuffer = buffer.first;
			const auto& colorBuffer = buffer.second;

			const Vertex2D::IndexType newSize = static_cast<Vertex2D::IndexType>(posBuffer.size());
			const Vertex2D::IndexType vertexSize = (newSize * 2), indexSize = (6 * (newSize - 1));
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexSize, indexSize);

			if (not pVertex)
			{
				return 0;
			}

			const float halfThickness = (thickness * 0.5f);

			const Float2* const pBuf = posBuffer.data();
			const Float4* const cBuf = colorBuffer.data();

			{
				const Float2 p0 = pBuf[0];
				const Float2 p1 = pBuf[1];
				const Float2 line = (p1 - p0).normalize();
				const Float2 vNormalBegin{ -line.y * halfThickness, line.x * halfThickness };

				{
					startAngle = std::atan2(vNormalBegin.x, -vNormalBegin.y);
				}

				const Float4& color = cBuf[0];
				pVertex[0].set((p0 + vNormalBegin), color);
				pVertex[1].set((p0 - vNormalBegin), color);
			}

			for (Vertex2D::IndexType i = 0; i < (newSize - 2); ++i)
			{
				const Float2 p0 = pBuf[i];
				const Float2 p1 = pBuf[i + 1];
				const Float2 p2 = pBuf[i + 2];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = halfThickness / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				const Float4& color = cBuf[i + 1];
				pVertex[i * 2 + 2].set(result0, color);
				pVertex[i * 2 + 3].set(result1, color);
			}

			{
				const Float2 p0 = pBuf[newSize - 2];
				const Float2 p1 = pBuf[newSize - 1];
				const Float2 line = (p1 - p0).normalize();
				const Float2 vNormalEnd{ -line.y * halfThickness, line.x * halfThickness };

				{
					endAngle = std::atan2(vNormalEnd.x, -vNormalEnd.y);
				}

				const Float4& color = cBuf[newSize - 1];
				pVertex[newSize * 2 - 2].set((p1 + vNormalEnd), color);
				pVertex[newSize * 2 - 1].set((p1 - vNormalEnd), color);
			}

			{
				const Vertex2D::IndexType count = static_cast<Vertex2D::IndexType>(newSize - 1);

				for (Vertex2D::IndexType k = 0; k < count; ++k)
				{
					for (Vertex2D::IndexType i = 0; i < 6; ++i)
					{
						*pIndex++ = (indexOffset + (RectIndexTable[i] + k * 2) % vertexSize);
					}
				}
			}

			return indexSize;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildClosedLineString(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& color, const float scale)
		{
			const Array<Float2> base = SimplifyClosed(points, scale, offset);
			const Array<Float2> buffer = AdjustClosed(base, scale, inner);

			const Vertex2D::IndexType newSize = static_cast<Vertex2D::IndexType>(buffer.size());
			const Vertex2D::IndexType vertexSize = (newSize * 2), indexSize = (6 * (newSize - 1) + 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexSize, indexSize);

			if (not pVertex)
			{
				return 0;
			}

			const float thicknessHalf = (thickness * 0.5f);

			const Float2* const pBuf = buffer.data();

			{
				const Float2 p0 = pBuf[newSize - 1];
				const Float2 p1 = pBuf[0];
				const Float2 p2 = pBuf[1];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = thicknessHalf / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				pVertex[0].pos.set(result0);
				pVertex[1].pos.set(result1);
			}

			for (Vertex2D::IndexType i = 0; i < (newSize - 2); ++i)
			{
				const Float2 p0 = pBuf[i];
				const Float2 p1 = pBuf[i + 1];
				const Float2 p2 = pBuf[i + 2];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = thicknessHalf / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				pVertex[i * 2 + 2].pos.set(result0);
				pVertex[i * 2 + 3].pos.set(result1);
			}


			{
				const Float2 p0 = pBuf[newSize - 2];
				const Float2 p1 = pBuf[newSize - 1];
				const Float2 p2 = pBuf[0];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = thicknessHalf / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				pVertex[newSize * 2 - 2].pos.set(result0);
				pVertex[newSize * 2 - 1].pos.set(result1);
			}

			for (size_t i = 0; i < vertexSize; ++i)
			{
				(pVertex++)->color = color;
			}

			{
				const Vertex2D::IndexType count = static_cast<Vertex2D::IndexType>(newSize);

				for (Vertex2D::IndexType k = 0; k < count; ++k)
				{
					for (Vertex2D::IndexType i = 0; i < 6; ++i)
					{
						*pIndex++ = (indexOffset + (RectIndexTable[i] + k * 2) % vertexSize);
					}
				}
			}

			return indexSize;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildClosedLineString(const BufferCreatorFunc& bufferCreator, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const std::span<const ColorF> colors, const float scale)
		{
			const std::pair<Array<Float2>, Array<Float4>> base = SimplifyClosed(points, scale, offset, colors);
			const std::pair<Array<Float2>, Array<Float4>> buffer = AdjustClosed(base.first, base.second, scale, inner);
			const auto& posBuffer = buffer.first;
			const auto& colorBuffer = buffer.second;

			const Vertex2D::IndexType newSize = static_cast<Vertex2D::IndexType>(posBuffer.size());
			const Vertex2D::IndexType vertexSize = (newSize * 2), indexSize = (6 * (newSize - 1) + 6);
			auto [pVertex, pIndex, indexOffset] = bufferCreator(vertexSize, indexSize);

			if (not pVertex)
			{
				return 0;
			}

			const float thicknessHalf = (thickness * 0.5f);

			const Float2* const pBuf = posBuffer.data();
			const Float4* const cBuf = colorBuffer.data();

			{
				const Float2 p0 = pBuf[newSize - 1];
				const Float2 p1 = pBuf[0];
				const Float2 p2 = pBuf[1];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = thicknessHalf / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				const Float4& color = cBuf[0];
				pVertex[0].set(result0, color);
				pVertex[1].set(result1, color);
			}

			for (Vertex2D::IndexType i = 0; i < (newSize - 2); ++i)
			{
				const Float2 p0 = pBuf[i];
				const Float2 p1 = pBuf[i + 1];
				const Float2 p2 = pBuf[i + 2];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = thicknessHalf / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				const Float4& color = cBuf[i + 1];
				pVertex[i * 2 + 2].set(result0, color);
				pVertex[i * 2 + 3].set(result1, color);
			}


			{
				const Float2 p0 = pBuf[newSize - 2];
				const Float2 p1 = pBuf[newSize - 1];
				const Float2 p2 = pBuf[0];
				const Float2 line = p1 - p0;
				const Float2 normal = Float2{ -line.y, line.x }.normalized();
				const Float2 v = (p2 - p1).normalized() + (p1 - p0).normalized();
				const Float2 tangent = (0.001f < v.lengthSq()) ? v.normalized() : (p2 - p0).normalized();
				const Float2 miter = Float2{ -tangent.y, tangent.x };
				const float length = thicknessHalf / miter.dot(normal);
				const Float2 result0 = p1 + miter * length;
				const Float2 result1 = p1 - miter * length;

				const Float4& color = cBuf[newSize - 1];
				pVertex[newSize * 2 - 2].set(result0, color);
				pVertex[newSize * 2 - 1].set(result1, color);
			}

			{
				const Vertex2D::IndexType count = static_cast<Vertex2D::IndexType>(newSize);

				for (Vertex2D::IndexType k = 0; k < count; ++k)
				{
					for (Vertex2D::IndexType i = 0; i < 6; ++i)
					{
						*pIndex++ = (indexOffset + (RectIndexTable[i] + k * 2) % vertexSize);
					}
				}
			}

			return indexSize;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildLineStringCaps(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const Float2& center, const Optional<Float2>& offset, const float thickness, const Float4& color, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);

			const Float2 base = (center + offset.value_or(Float2{ 0, 0 }));

			Vertex2D::IndexType indexCount = 0;

			// draw startCap
			{
				if (startCap == LineCap::Square)
				{
					const FloatRect rect{ (base.x - halfThickness), (base.y - halfThickness), base.x, (base.y + halfThickness) };

					indexCount += Vertex2DBuilder::BuildRect(bufferCreator, rect, color);
				}
				else if (startCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, base, halfThickness, 180_degF, color, scale);
				}
			}

			// draw endCap
			{
				if (endCap == LineCap::Square)
				{
					const FloatRect rect{ base.x, (base.y - halfThickness), (base.x + halfThickness), (base.y + halfThickness) };

					indexCount += Vertex2DBuilder::BuildRect(bufferCreator, rect, color);
				}
				else if (endCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, base, halfThickness, 0_degF, color, scale);
				}
			}

			return indexCount;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildLineStringCaps(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const Float2& center, const Optional<Float2>& offset, const float thickness, const Float4& colorStart, const Float4& colorEnd, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);

			const Float2 base = (center + offset.value_or(Float2{ 0, 0 }));

			const bool hasStartCap = (startCap != LineCap::Flat);
			const bool hasEndCap = (endCap != LineCap::Flat);

			Vertex2D::IndexType indexCount = 0;

			// draw startCap
			if (hasStartCap)
			{
				const Float4 c0 = colorStart;
				const Float4 c1 = (hasEndCap ? colorStart.getMidpoint(colorEnd) : colorEnd);

				if (startCap == LineCap::Square)
				{
					const FloatRect rect{ (base.x - halfThickness), (base.y - halfThickness), base.x, (base.y + halfThickness) };

					indexCount += Vertex2DBuilder::BuildRect(bufferCreator, rect, { c0, c1, c1, c0 });
				}
				else if (startCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, base, halfThickness, 180_degF, ColorFillDirection::LeftRight, c1, c0, scale);
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

					indexCount += Vertex2DBuilder::BuildRect(bufferCreator, rect, { c0, c1, c1, c0 });
				}
				else if (endCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, base, halfThickness, 0_degF, ColorFillDirection::LeftRight, c0, c1, scale);
				}
			}

			return indexCount;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildLineStringCaps(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, Float2 start, const float startAngle, Float2 end, const float endAngle, const Optional<Float2>& offset, const float thickness, const Float4& colorStart, const Float4& colorEnd, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);

			Vertex2D::IndexType indexCount = 0;

			if (offset)
			{
				start += *offset;
				end += *offset;
			}

			// draw startCap
			{
				if (startCap == LineCap::Square)
				{
					const Quad quad = RectF{ Arg::middleLeft = start, halfThickness, thickness }.rotatedAt(start, startAngle);

					indexCount += Vertex2DBuilder::BuildQuad(bufferCreator, FloatQuad{ quad }, colorStart);
				}
				else if (startCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, start, halfThickness, startAngle, colorStart, scale);
				}
			}

			// draw endCap
			{
				if (endCap == LineCap::Square)
				{
					const Quad quad = RectF{ Arg::middleRight = end, halfThickness, thickness }.rotatedAt(end, endAngle);

					indexCount += Vertex2DBuilder::BuildQuad(bufferCreator, FloatQuad{ quad }, colorEnd);
				}
				else if (endCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, end, halfThickness, (endAngle + Math::PiF), colorEnd, scale);
				}
			}

			return indexCount;
		}

		[[nodiscard]]
		static Vertex2D::IndexType BuildLineStringCaps(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, Float2 start, const float startAngle, Float2 end, const float endAngle, const Optional<Float2>& offset, const float thickness, const Float4& c0, const Float4& c1, const Float4& c2, const Float4& c3, const float scale)
		{
			const float halfThickness = (thickness * 0.5f);

			Vertex2D::IndexType indexCount = 0;

			if (offset)
			{
				start += *offset;
				end += *offset;
			}

			// draw startCap
			{
				if (startCap == LineCap::Square)
				{
					const Quad quad = RectF{ Arg::middleLeft = start, halfThickness, thickness }.rotatedAt(start, startAngle);

					indexCount += Vertex2DBuilder::BuildQuad(bufferCreator, FloatQuad{ quad }, { c1, c0, c0, c1 });
				}
				else if (startCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, start, halfThickness, startAngle, ColorFillDirection::LeftRight, c1, c0, scale);
				}
			}

			// draw endCap
			{
				if (endCap == LineCap::Square)
				{
					const Quad quad = RectF{ Arg::middleRight = end, halfThickness, thickness }.rotatedAt(end, endAngle);

					indexCount += Vertex2DBuilder::BuildQuad(bufferCreator, FloatQuad{ quad }, { c2, c3, c3, c2 });
				}
				else if (endCap == LineCap::Round)
				{
					indexCount += BuildRoundCap(bufferCreator, end, halfThickness, (endAngle + Math::PiF), ColorFillDirection::LeftRight, c2, c3, scale);
				}
			}

			return indexCount;
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
				roundIndexCount += BuildRoundCap(bufferCreator, start, halfThickness, startAngle, colors[0], scale);
			}

			if (endCap == LineCap::Round)
			{
				roundIndexCount += BuildRoundCap(bufferCreator, end, halfThickness, (startAngle + Math::PiF), colors[1], scale);
			}

			return (IndexCount + roundIndexCount);
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
					const Float4 c0 = (color0 + ((cs->y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((cs->x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs->y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((-cs->x + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + x), (centerY + y), c0);
					pDst1->set((centerX - y), (centerY + x), c1);
					pDst2->set((centerX - x), (centerY - y), c2);
					pDst3->set((centerX + y), (centerY - x), c3);
				}
				else
				{
					const Float4 c0 = (color0 + ((cs->x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((-cs->y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs->x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((cs->y + 1.0f) * 0.5f) * colorDiff);

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
		//	BuildCircleSegment
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildCircleSegment(const BufferCreatorFunc& bufferCreator, const Float2& center, const float r, const float startAngle, const float angle, const Float4& color, const float scale)
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

			{
				const auto [s1, c1] = FastMath::SinCos(startAngle);
				const Float2 p1{ s1, -c1 };

				const auto [s2, c2] = FastMath::SinCos(startAngle + angle);
				const Float2 p2{ s2, -c2 };

				const Float2 mid = ((p1 + p2) * 0.5f * r);

				// 中心
				pVertex[0].pos = (mid + Float2{ centerX, centerY });
			}

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
				for (size_t i = 0; i < VertexCount; ++i)
				{
					(pVertex++)->color = color;
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
		//	BuildEllipse
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
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
				const float xa = (cs->x * a);
				const float xb = (cs->x * b);
				const float ya = (cs->y * a);
				const float yb = (cs->y * b);

				if (colorType == ColorFillDirection::InOut)
				{
					pDst0->set((centerX + xa), (centerY + yb), color1);
					pDst1->set((centerX - ya), (centerY + xb), color1);
					pDst2->set((centerX - xa), (centerY - yb), color1);
					pDst3->set((centerX + ya), (centerY - xb), color1);
				}
				else if (colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + ((cs->y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((cs->x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs->y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((-cs->x + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + xa), (centerY + yb), c0);
					pDst1->set((centerX - ya), (centerY + xb), c1);
					pDst2->set((centerX - xa), (centerY - yb), c2);
					pDst3->set((centerX + ya), (centerY - xb), c3);
				}
				else
				{
					const Float4 c0 = (color0 + ((cs->x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((-cs->y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs->x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((cs->y + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + xa), (centerY + yb), c0);
					pDst1->set((centerX - ya), (centerY + xb), c1);
					pDst2->set((centerX - xa), (centerY - yb), c2);
					pDst3->set((centerX + ya), (centerY - xb), c3);
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
		//	BuildEllipseFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildEllipseFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float aInner, const float bInner, const float thickness, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			const float aOuter = (aInner + thickness);
			const float bOuter = (bInner + thickness);
			const float majorAxis = Max(aOuter, bOuter);
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

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			Vertex2D* pDst0 = &pVertex[0];
			Vertex2D* pDst1 = (pDst0 + Quality * 2);
			Vertex2D* pDst2 = (pDst0 + Quality * 4);
			Vertex2D* pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float oxa = (cs->x * aOuter);
				const float ixa = (cs->x * aInner);
				const float oxb = (cs->x * bOuter);
				const float ixb = (cs->x * bInner);
				const float oya = (cs->y * aOuter);
				const float iya = (cs->y * aInner);
				const float oyb = (cs->y * bOuter);
				const float iyb = (cs->y * bInner);

				(pDst0++)->pos.set((centerX + oxa), (centerY + oyb));
				(pDst0++)->pos.set((centerX + ixa), (centerY + iyb));

				(pDst1++)->pos.set((centerX - oya), (centerY + oxb));
				(pDst1++)->pos.set((centerX - iya), (centerY + ixb));

				(pDst2++)->pos.set((centerX - oxa), (centerY - oyb));
				(pDst2++)->pos.set((centerX - ixa), (centerY - iyb));

				(pDst3++)->pos.set((centerX + oya), (centerY - oxb));
				(pDst3++)->pos.set((centerX + iya), (centerY - ixb));
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
		//	BuildSuperEllipse
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildSuperEllipse(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float n, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
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

			const float centerX = center.x;
			const float centerY = center.y;
			pVertex[0].pos.set(centerX, centerY);

			const Float4 colorDiff = (color1 - color0);
			const float minX = (centerX - a);
			const float minY = (centerY - b);
			const float invW = (1.0f / (2.0f * a));
			const float invH = (1.0f / (2.0f * b));

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			Vertex2D* pDst0 = &pVertex[1];
			Vertex2D* pDst1 = (pDst0 + Quality);
			Vertex2D* pDst2 = (pDst1 + Quality);
			Vertex2D* pDst3 = (pDst2 + Quality);

			const float p = (2.0f / n);
			const auto SignedPow = [p](float v) -> float
			{
				return std::copysign(std::pow(Abs(v), p), v);
			};

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float cp = SignedPow(cs->x);
				const float sp = SignedPow(cs->y);

				const float ax_cp = (a * cp);
				const float ax_sp = (a * sp);
				const float by_sp = (b * sp);
				const float by_cp = (b * cp);

				if (colorType == ColorFillDirection::InOut)
				{
					pDst0->set((centerX + ax_cp), (centerY + by_sp), color1);
					pDst1->set((centerX - ax_sp), (centerY + by_cp), color1);
					pDst2->set((centerX - ax_cp), (centerY - by_sp), color1);
					pDst3->set((centerX + ax_sp), (centerY - by_cp), color1);
				}
				else if (colorType == ColorFillDirection::TopBottom)
				{
					const float y0 = centerY + by_sp;
					const float y1 = centerY + by_cp;
					const float y2 = centerY - by_sp;
					const float y3 = centerY - by_cp;

					const Float4 c0 = color0 + ((y0 - minY) * invH) * colorDiff;
					const Float4 c1 = color0 + ((y1 - minY) * invH) * colorDiff;
					const Float4 c2 = color0 + ((y2 - minY) * invH) * colorDiff;
					const Float4 c3 = color0 + ((y3 - minY) * invH) * colorDiff;

					pDst0->set((centerX + ax_cp), y0, c0);
					pDst1->set((centerX - ax_sp), y1, c1);
					pDst2->set((centerX - ax_cp), y2, c2);
					pDst3->set((centerX + ax_sp), y3, c3);
				}
				else
				{
					const float x0 = centerX + ax_cp;
					const float x1 = centerX - ax_sp;
					const float x2 = centerX - ax_cp;
					const float x3 = centerX + ax_sp;

					const Float4 c0 = color0 + ((x0 - minX) * invW) * colorDiff;
					const Float4 c1 = color0 + ((x1 - minX) * invW) * colorDiff;
					const Float4 c2 = color0 + ((x2 - minX) * invW) * colorDiff;
					const Float4 c3 = color0 + ((x3 - minX) * invW) * colorDiff;

					pDst0->set(x0, (centerY + by_sp), c0);
					pDst1->set(x1, (centerY + by_cp), c1);
					pDst2->set(x2, (centerY - by_sp), c2);
					pDst3->set(x3, (centerY - by_cp), c3);
				}

				++pDst0; ++pDst1; ++pDst2; ++pDst3;
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
		//	BuildRoundRect
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float r, const Float4& color, const float scale)
		{
			const Vertex2D::IndexType Quality				= CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FanOuterVertexCount	= (Quality + 1); // 1 つの扇型の曲線部分の頂点数
			const Vertex2D::IndexType FanVertexCount		= (FanOuterVertexCount + 1); // 1 つの扇型の頂点数
			const Vertex2D::IndexType FanTriangleCount		= Quality; // 1 つの扇型の三角形の数
			const Vertex2D::IndexType VertexCount			= (FanVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount			= ((Quality * 3 * 4) + (6 * 3)); // インデックス数
			auto [pVertex, pIndex, indexOffset]				= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float middleLeftX		= (rect.left + r);
			const float middleRightX	= (rect.right - r);
			const float topCenterY		= (rect.top + r);
			const float bottomCenterY	= (rect.bottom - r);

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			Vertex2D* pDst0 = pVertex;
			Vertex2D* pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2D* pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2D* pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float x = (cs->x * r);
				const float y = (cs->y * r);

				pDst0->set((middleRightX + x), (topCenterY + y), color);
				pDst1->set((middleRightX - y), (bottomCenterY + x), color);
				pDst2->set((middleLeftX - x), (bottomCenterY - y), color);
				pDst3->set((middleLeftX + y), (topCenterY - x), color);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の最後の頂点
			{
				const float x = (pCS->x * r);
				const float y = (pCS->y * r);

				pDst0->set((middleRightX - y), (topCenterY + x), color);
				pDst1->set((middleRightX - x), (bottomCenterY - y), color);
				pDst2->set((middleLeftX + y), (bottomCenterY - x), color);
				pDst3->set((middleLeftX + x), (topCenterY + y), color);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の中心の頂点
			{
				(pDst3++)->set(middleRightX, topCenterY, color);
				(pDst3++)->set(middleRightX, bottomCenterY, color);
				(pDst3++)->set(middleLeftX, bottomCenterY, color);
				(pDst3++)->set(middleLeftX, topCenterY, color);
			}

			const Vertex2D::IndexType trFanStartIndex	= indexOffset;
			const Vertex2D::IndexType trFanEndIndex		= (trFanStartIndex + Quality);
			const Vertex2D::IndexType brFanStartIndex	= (trFanEndIndex + 1);
			const Vertex2D::IndexType brFanEndIndex		= (brFanStartIndex + Quality);
			const Vertex2D::IndexType blFanStartIndex	= (brFanEndIndex + 1);
			const Vertex2D::IndexType blFanEndIndex		= (blFanStartIndex + Quality);
			const Vertex2D::IndexType tlFanStartIndex	= (blFanEndIndex + 1);
			const Vertex2D::IndexType tlFanEndIndex		= (tlFanStartIndex + Quality);

			const Vertex2D::IndexType trCenterIndex = (indexOffset + FanOuterVertexCount * 4);
			const Vertex2D::IndexType brCenterIndex = (trCenterIndex + 1);
			const Vertex2D::IndexType blCenterIndex = (trCenterIndex + 2);
			const Vertex2D::IndexType tlCenterIndex = (trCenterIndex + 3);

			{
				// tr
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = (indexOffset + i + 1);
					*pIndex++ = trCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// br
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = brCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// bl
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = blCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// tl
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
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

			return IndexCount;
		}
		
		Vertex2D::IndexType BuildRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float r, const ColorFillDirection colorType, const Float4& color0, const Float4& color1, const float scale)
		{
			const Vertex2D::IndexType Quality				= CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FanOuterVertexCount	= (Quality + 1); // 1 つの扇型の曲線部分の頂点数
			const Vertex2D::IndexType FanVertexCount		= (FanOuterVertexCount + 1); // 1 つの扇型の頂点数
			const Vertex2D::IndexType FanTriangleCount		= Quality; // 1 つの扇型の三角形の数
			const Vertex2D::IndexType VertexCount			= (FanVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount			= ((Quality * 3 * 4) + (6 * 3)); // インデックス数
			auto [pVertex, pIndex, indexOffset]				= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float middleLeftX		= (rect.left + r);
			const float middleRightX	= (rect.right - r);
			const float topCenterY		= (rect.top + r);
			const float bottomCenterY	= (rect.bottom - r);

			const Float4 colorDiff = (color1 - color0);
			const float colorRScale = (colorType == ColorFillDirection::TopBottom) ? (r / (rect.bottom - rect.top)) : (r / (rect.right - rect.left));
			const float colorROffset = (1.0f - colorRScale);

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			Vertex2D* pDst0 = pVertex;
			Vertex2D* pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2D* pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2D* pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float x = (cs->x * r);
				const float y = (cs->y * r);

				if (colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + ((cs->y + 1.0f) * colorRScale) * colorDiff);
					const Float4 c1 = (color0 + (colorROffset + (cs->x * colorRScale)) * colorDiff);
					const Float4 c2 = (color0 + (colorROffset + (-cs->y * colorRScale)) * colorDiff);
					const Float4 c3 = (color0 + ((-cs->x + 1.0f) * colorRScale) * colorDiff);

					pDst0->set((middleRightX + x), (topCenterY + y), c0);
					pDst1->set((middleRightX - y), (bottomCenterY + x), c1);
					pDst2->set((middleLeftX - x), (bottomCenterY - y), c2);
					pDst3->set((middleLeftX + y), (topCenterY - x), c3);
				}
				else // ColorFillDirection::LeftRight
				{
					const Float4 c0 = (color0 + (colorROffset + (cs->x * colorRScale)) * colorDiff);
					const Float4 c1 = (color0 + (colorROffset + (-cs->y * colorRScale)) * colorDiff);
					const Float4 c2 = (color0 + ((-cs->x + 1.0f) * colorRScale) * colorDiff);
					const Float4 c3 = (color0 + ((cs->y + 1.0f) * colorRScale) * colorDiff);

					pDst0->set((middleRightX + x), (topCenterY + y), c0);
					pDst1->set((middleRightX - y), (bottomCenterY + x), c1);
					pDst2->set((middleLeftX - x), (bottomCenterY - y), c2);
					pDst3->set((middleLeftX + y), (topCenterY - x), c3);
				}

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の最後の頂点
			{
				const float x = (pCS->x * r);
				const float y = (pCS->y * r);

				if (colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + colorRScale * colorDiff);
					const Float4 c1 = color1;
					const Float4 c2 = (color0 + (1.0 - colorRScale) * colorDiff);
					const Float4 c3 = color0;

					pDst0->set((middleRightX - y), (topCenterY + x), c0);
					pDst1->set((middleRightX - x), (bottomCenterY - y), c1);
					pDst2->set((middleLeftX + y), (bottomCenterY - x), c2);
					pDst3->set((middleLeftX + x), (topCenterY + y), c3);
				}
				else // ColorFillDirection::LeftRight
				{
					const Float4 c0 = color1;
					const Float4 c1 = (color0 + (1.0 - colorRScale) * colorDiff);
					const Float4 c2 = color0;
					const Float4 c3 = (color0 + colorRScale * colorDiff);

					pDst0->set((middleRightX - y), (topCenterY + x), c0);
					pDst1->set((middleRightX - x), (bottomCenterY - y), c1);
					pDst2->set((middleLeftX + y), (bottomCenterY - x), c2);
					pDst3->set((middleLeftX + x), (topCenterY + y), c3);
				}

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の中心の頂点
			{
				if (colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + colorRScale * colorDiff);
					const Float4 c1 = (color0 + (1.0 - colorRScale) * colorDiff);

					(pDst3++)->set(middleRightX, topCenterY, c0);
					(pDst3++)->set(middleRightX, bottomCenterY, c1);
					(pDst3++)->set(middleLeftX, bottomCenterY, c1);
					(pDst3++)->set(middleLeftX, topCenterY, c0);
				}
				else // ColorFillDirection::LeftRight
				{
					const Float4 c0 = (color0 + (1.0 - colorRScale) * colorDiff);
					const Float4 c1 = (color0 + colorRScale * colorDiff);

					(pDst3++)->set(middleRightX, topCenterY, c0);
					(pDst3++)->set(middleRightX, bottomCenterY, c0);
					(pDst3++)->set(middleLeftX, bottomCenterY, c1);
					(pDst3++)->set(middleLeftX, topCenterY, c1);
				}
			}

			const Vertex2D::IndexType trFanStartIndex	= indexOffset;
			const Vertex2D::IndexType trFanEndIndex		= (trFanStartIndex + Quality);
			const Vertex2D::IndexType brFanStartIndex	= (trFanEndIndex + 1);
			const Vertex2D::IndexType brFanEndIndex		= (brFanStartIndex + Quality);
			const Vertex2D::IndexType blFanStartIndex	= (brFanEndIndex + 1);
			const Vertex2D::IndexType blFanEndIndex		= (blFanStartIndex + Quality);
			const Vertex2D::IndexType tlFanStartIndex	= (blFanEndIndex + 1);
			const Vertex2D::IndexType tlFanEndIndex		= (tlFanStartIndex + Quality);

			const Vertex2D::IndexType trCenterIndex = (indexOffset + FanOuterVertexCount * 4);
			const Vertex2D::IndexType brCenterIndex = (trCenterIndex + 1);
			const Vertex2D::IndexType blCenterIndex = (trCenterIndex + 2);
			const Vertex2D::IndexType tlCenterIndex = (trCenterIndex + 3);

			{
				// tr
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = (indexOffset + i + 1);
					*pIndex++ = trCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// br
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = brCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// bl
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = blCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// tl
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
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

			{
				const float innerLeftCenterX = (innerRect.left + innerR);
				const float innerRightCenterX = (innerRect.right - innerR);
				const float innerTopCenterY = (innerRect.top + innerR);
				const float innerBottomCenterY = (innerRect.bottom - innerR);

				const float outerLeftCenterX = (outerRect.left + outerR);
				const float outerRightCenterX = (outerRect.right - outerR);
				const float outerTopCenterY = (outerRect.top + outerR);
				const float outerBottomCenterY = (outerRect.bottom - outerR);

				const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

				Vertex2D* pDst0 = pVertex;
				Vertex2D* pDst1 = (pVertex + ArcVertexCount * 1);
				Vertex2D* pDst2 = (pVertex + ArcVertexCount * 2);
				Vertex2D* pDst3 = (pVertex + ArcVertexCount * 3);

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const Float2* cs = (pCS + i);
					const float ox = (cs->x * outerR);
					const float oy = (cs->y * outerR);
					const float ix = (cs->x * innerR);
					const float iy = (cs->y * innerR);

					pDst0->set((outerRightCenterX + ox), (outerTopCenterY + oy), color);
					pDst1->set((outerRightCenterX - oy), (outerBottomCenterY + ox), color);
					pDst2->set((outerLeftCenterX - ox), (outerBottomCenterY - oy), color);
					pDst3->set((outerLeftCenterX + oy), (outerTopCenterY - ox), color);

					++pDst0;
					++pDst1;
					++pDst2;
					++pDst3;

					pDst0->set((innerRightCenterX + ix), (innerTopCenterY + iy), color);
					pDst1->set((innerRightCenterX - iy), (innerBottomCenterY + ix), color);
					pDst2->set((innerLeftCenterX - ix), (innerBottomCenterY - iy), color);
					pDst3->set((innerLeftCenterX + iy), (innerTopCenterY - ix), color);

					++pDst0;
					++pDst1;
					++pDst2;
					++pDst3;
				}

				// 弧の最後の頂点
				{
					const float ox = (pCS->x * outerR);
					const float oy = (pCS->y * outerR);
					const float ix = (pCS->x * innerR);
					const float iy = (pCS->y * innerR);

					pDst0->set((outerRightCenterX - oy), (outerTopCenterY + ox), color);
					pDst1->set((outerRightCenterX - ox), (outerBottomCenterY - oy), color);
					pDst2->set((outerLeftCenterX + oy), (outerBottomCenterY - ox), color);
					pDst3->set((outerLeftCenterX + ox), (outerTopCenterY + oy), color);

					++pDst0;
					++pDst1;
					++pDst2;
					++pDst3;

					pDst0->set((innerRightCenterX - iy), (innerTopCenterY + ix), color);
					pDst1->set((innerRightCenterX - ix), (innerBottomCenterY - iy), color);
					pDst2->set((innerLeftCenterX + iy), (innerBottomCenterY - ix), color);
					pDst3->set((innerLeftCenterX + ix), (innerTopCenterY + iy), color);
				}
			}

			{
				Vertex2D::IndexType* pDst0 = pIndex;
				Vertex2D::IndexType* pDst1 = (pIndex + ArcIndexCount * 1 + 6);
				Vertex2D::IndexType* pDst2 = (pIndex + ArcIndexCount * 2 + 12);
				Vertex2D::IndexType* pDst3 = (pIndex + ArcIndexCount * 3 + 18);

				const Vertex2D::IndexType trIndexBase = indexOffset;
				const Vertex2D::IndexType brIndexBase = (indexOffset + ArcVertexCount * 1);
				const Vertex2D::IndexType blIndexBase = (indexOffset + ArcVertexCount * 2);
				const Vertex2D::IndexType tlIndexBase = (indexOffset + ArcVertexCount * 3);

				for (Vertex2D::IndexType i = 0; i <= Quality; ++i)
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

					if (i < Quality)
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

			return IndexCount;
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

			{
				const float innerLeftCenterX = (innerRect.left + innerR);
				const float innerRightCenterX = (innerRect.right - innerR);
				const float innerTopCenterY = (innerRect.top + innerR);
				const float innerBottomCenterY = (innerRect.bottom - innerR);

				const float outerLeftCenterX = (outerRect.left + outerR);
				const float outerRightCenterX = (outerRect.right - outerR);
				const float outerTopCenterY = (outerRect.top + outerR);
				const float outerBottomCenterY = (outerRect.bottom - outerR);

				const Float4 colorDiff = (color1 - color0);
				const float rectSize = ((colorType == ColorFillDirection::TopBottom) ? (outerRect.bottom - outerRect.top) : (outerRect.right - outerRect.left));
				const float thickness = (((outerRect.right - outerRect.left) - (innerRect.right - innerRect.left)) / 2.0f);
				const float ot1 = (outerR / rectSize);
				const float ot2 = (1.0f - ot1);
				const float it1 = (thickness / rectSize);
				const float it2 = ((thickness + innerR) / rectSize);
				const float it3 = (1.0f - it2);
				const float it4 = (1.0f - it1);
				const float irt = (it2 - it1);

				const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

				Vertex2D* pDst0 = pVertex;
				Vertex2D* pDst1 = (pVertex + ArcVertexCount * 1);
				Vertex2D* pDst2 = (pVertex + ArcVertexCount * 2);
				Vertex2D* pDst3 = (pVertex + ArcVertexCount * 3);

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const Float2* cs = (pCS + i);
					const float ox = (cs->x * outerR);
					const float oy = (cs->y * outerR);
					const float ix = (cs->x * innerR);
					const float iy = (cs->y * innerR);

					if (colorType == ColorFillDirection::TopBottom)
					{
						const Float4 c0 = (color0 + ((cs->y + 1.0f) * ot1) * colorDiff);
						const Float4 c1 = (color0 + (ot2 + (cs->x * ot1)) * colorDiff);
						const Float4 c2 = (color0 + (ot2 + (-cs->y * ot1)) * colorDiff);
						const Float4 c3 = (color0 + ((-cs->x + 1.0f) * ot1) * colorDiff);

						pDst0->set((outerRightCenterX + ox), (outerTopCenterY + oy), c0);
						pDst1->set((outerRightCenterX - oy), (outerBottomCenterY + ox), c1);
						pDst2->set((outerLeftCenterX - ox), (outerBottomCenterY - oy), c2);
						pDst3->set((outerLeftCenterX + oy), (outerTopCenterY - ox), c3);
					}
					else // ColorFillDirection::LeftRight
					{
						const Float4 c0 = (color0 + (ot2 + (cs->x * ot1)) * colorDiff);
						const Float4 c1 = (color0 + (ot2 + (-cs->y * ot1)) * colorDiff);
						const Float4 c2 = (color0 + ((-cs->x + 1.0f) * ot1) * colorDiff);
						const Float4 c3 = (color0 + ((cs->y + 1.0f) * ot1) * colorDiff);

						pDst0->set((outerRightCenterX + ox), (outerTopCenterY + oy), c0);
						pDst1->set((outerRightCenterX - oy), (outerBottomCenterY + ox), c1);
						pDst2->set((outerLeftCenterX - ox), (outerBottomCenterY - oy), c2);
						pDst3->set((outerLeftCenterX + oy), (outerTopCenterY - ox), c3);
					}

					++pDst0;
					++pDst1;
					++pDst2;
					++pDst3;

					if (colorType == ColorFillDirection::TopBottom)
					{
						const Float4 c0 = (color0 + (it1 + (cs->y + 1.0f) * irt) * colorDiff);
						const Float4 c1 = (color0 + (it3 + (cs->x * irt)) * colorDiff);
						const Float4 c2 = (color0 + (it3 + (-cs->y * irt)) * colorDiff);
						const Float4 c3 = (color0 + (it1 + (-cs->x + 1.0f) * irt) * colorDiff);

						pDst0->set((innerRightCenterX + ix), (innerTopCenterY + iy), c0);
						pDst1->set((innerRightCenterX - iy), (innerBottomCenterY + ix), c1);
						pDst2->set((innerLeftCenterX - ix), (innerBottomCenterY - iy), c2);
						pDst3->set((innerLeftCenterX + iy), (innerTopCenterY - ix), c3);
					}
					else // ColorFillDirection::LeftRight
					{
						const Float4 c0 = (color0 + (it3 + (cs->x * irt)) * colorDiff);
						const Float4 c1 = (color0 + (it3 + (-cs->y * irt)) * colorDiff);
						const Float4 c2 = (color0 + (it1 + (-cs->x + 1.0f) * irt) * colorDiff);
						const Float4 c3 = (color0 + (it1 + (cs->y + 1.0f) * irt) * colorDiff);

						pDst0->set((innerRightCenterX + ix), (innerTopCenterY + iy), c0);
						pDst1->set((innerRightCenterX - iy), (innerBottomCenterY + ix), c1);
						pDst2->set((innerLeftCenterX - ix), (innerBottomCenterY - iy), c2);
						pDst3->set((innerLeftCenterX + iy), (innerTopCenterY - ix), c3);
					}

					++pDst0;
					++pDst1;
					++pDst2;
					++pDst3;
				}

				// 弧の最後の頂点
				{
					const float ox = (pCS->x * outerR);
					const float oy = (pCS->y * outerR);
					const float ix = (pCS->x * innerR);
					const float iy = (pCS->y * innerR);
					
					if (colorType == ColorFillDirection::TopBottom)
					{
						pDst0->set((outerRightCenterX - oy), (outerTopCenterY + ox), (color0 + ot1 * colorDiff));
						pDst1->set((outerRightCenterX - ox), (outerBottomCenterY - oy), color1);
						pDst2->set((outerLeftCenterX + oy), (outerBottomCenterY - ox), (color0 + ot2 * colorDiff));
						pDst3->set((outerLeftCenterX + ox), (outerTopCenterY + oy), color0);
					}
					else
					{
						pDst0->set((outerRightCenterX - oy), (outerTopCenterY + ox), color1);
						pDst1->set((outerRightCenterX - ox), (outerBottomCenterY - oy), (color0 + ot2 * colorDiff));
						pDst2->set((outerLeftCenterX + oy), (outerBottomCenterY - ox), color0);
						pDst3->set((outerLeftCenterX + ox), (outerTopCenterY + oy), (color0 + ot1 * colorDiff));
					}

					++pDst0;
					++pDst1;
					++pDst2;
					++pDst3;

					if (colorType == ColorFillDirection::TopBottom)
					{
						pDst0->set((innerRightCenterX - iy), (innerTopCenterY + ix), (color0 + it2 * colorDiff));
						pDst1->set((innerRightCenterX - ix), (innerBottomCenterY - iy), (color0 + it4 * colorDiff));
						pDst2->set((innerLeftCenterX + iy), (innerBottomCenterY - ix), (color0 + it3 * colorDiff));
						pDst3->set((innerLeftCenterX + ix), (innerTopCenterY + iy), (color0 + it1 * colorDiff));
					}
					else
					{
						pDst0->set((innerRightCenterX - iy), (innerTopCenterY + ix), (color0 + it4 * colorDiff));
						pDst1->set((innerRightCenterX - ix), (innerBottomCenterY - iy), (color0 + it3 * colorDiff));
						pDst2->set((innerLeftCenterX + iy), (innerBottomCenterY - ix), (color0 + it1 * colorDiff));
						pDst3->set((innerLeftCenterX + ix), (innerTopCenterY + iy), (color0 + it2 * colorDiff));
					}
				}
			}

			{
				Vertex2D::IndexType* pDst0 = pIndex;
				Vertex2D::IndexType* pDst1 = (pIndex + ArcIndexCount * 1 + 6);
				Vertex2D::IndexType* pDst2 = (pIndex + ArcIndexCount * 2 + 12);
				Vertex2D::IndexType* pDst3 = (pIndex + ArcIndexCount * 3 + 18);

				const Vertex2D::IndexType trIndexBase = indexOffset;
				const Vertex2D::IndexType brIndexBase = (indexOffset + ArcVertexCount * 1);
				const Vertex2D::IndexType blIndexBase = (indexOffset + ArcVertexCount * 2);
				const Vertex2D::IndexType tlIndexBase = (indexOffset + ArcVertexCount * 3);

				for (Vertex2D::IndexType i = 0; i <= Quality; ++i)
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

					if (i < Quality)
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

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildLineString
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildLineString(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const Float4& color, const float scale)
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

			if (closeRing && (3 <= num_points))
			{
				return BuildClosedLineString(bufferCreator, points, offset, thickness, inner, color, scale);
			}
			else
			{
				float startAngle = 0.0f, endAngle = 0.0f;

				Vertex2D::IndexType indexCount = BuildUncappedLineString(bufferCreator, points, offset, thickness, inner, color, scale, startAngle, endAngle);

				indexCount += BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, color, color, scale);
		
				return indexCount;
			}
		}

		Vertex2D::IndexType BuildLineString(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const Float4& colorStart, const Float4& colorEnd, const float scale)
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

			{
				float startAngle = 0.0f, endAngle = 0.0f;
				Float4 c1, c2;
				const bool hasStartCap = (startCap != LineCap::Flat);
				const bool hasEndCap = (endCap != LineCap::Flat);

				Vertex2D::IndexType indexCount = BuildUncappedLineString(bufferCreator, points, offset, thickness, inner, colorStart, colorEnd, scale, startAngle, endAngle, c1, c2, hasStartCap, hasEndCap);

				indexCount += BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, colorStart, c1, c2, colorEnd, scale);

				return indexCount;
			}
		}

		Vertex2D::IndexType BuildLineString(const BufferCreatorFunc& bufferCreator, const LineCap startCap, const LineCap endCap, const std::span<const Vec2> points, const Optional<Float2>& offset, const float thickness, const bool inner, const CloseRing closeRing, const std::span<const ColorF> colors, const float scale)
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

			if (closeRing && (3 <= num_points))
			{
				return BuildClosedLineString(bufferCreator, points, offset, thickness, inner, colors, scale);
			}
			else
			{
				float startAngle = 0.0f, endAngle = 0.0f;

				Vertex2D::IndexType indexCount = BuildUncappedLineString(bufferCreator, points, offset, thickness, inner, colors, scale, startAngle, endAngle);

				indexCount += BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, colorStart, colorEnd, scale);

				return indexCount;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedCircle
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildTexturedCircle(const BufferCreatorFunc& bufferCreator, const Circle& circle, const FloatRect& uv, const Float4& color, const float scale)
		{
			const float rf = Abs(static_cast<float>(circle.r));
			const Vertex2D::IndexType Quality = CalculateCircleQuality(rf * scale);
			const Vertex2D::IndexType VertexCount = (Quality + 1);
			const Vertex2D::IndexType IndexCount = (Quality * 3);

			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			// 中心
			const float centerX = static_cast<float>(circle.x);
			const float centerY = static_cast<float>(circle.y);
			
			const float centerU = ((uv.left + uv.right) * 0.5f);
			const float centerV = ((uv.top + uv.bottom) * 0.5f);
			const float rU = ((uv.right - uv.left) * 0.5f);
			const float rV = ((uv.bottom - uv.top) * 0.5f);
			
			pVertex[0].set(centerX, centerY, centerU, centerV);

			// 周
			{
				const float radDelta = (Math::TwoPiF / Quality);
				Vertex2D* pDst = &pVertex[1];

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const float rad = (radDelta * i);
					const auto [s, c] = FastMath::SinCos(rad);
					(pDst++)->set((centerX + rf * c), (centerY - rf * s), (centerU + rU * c), (centerV - rV * s));
				}
			}

			for (size_t i = 0; i < VertexCount; ++i)
			{
				(pVertex++)->color = color;
			}

			{
				for (Vertex2D::IndexType i = 0; i < (Quality - 1); ++i)
				{
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = indexOffset;
					*pIndex++ = indexOffset + (i + 2);
				}

				*pIndex++ = (indexOffset + Quality);
				*pIndex++ = indexOffset;
				*pIndex++ = (indexOffset + 1);
			}

			return IndexCount;
		}

		////////////////////////////////////////////////////////////////
		//
		//	BuildTexturedRoundRect
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildTexturedRoundRect(const BufferCreatorFunc& bufferCreator, const FloatRect& rect, const float w, const float h, const float r, const FloatRect& uvRect, const Float4& color, const float scale)
		{
			const Vertex2D::IndexType Quality				= CalculateCircleQuality(r * scale); // 円周の 1/4 に相当する品質
			const Vertex2D::IndexType FanOuterVertexCount	= (Quality + 1); // 1 つの扇型の曲線部分の頂点数
			const Vertex2D::IndexType FanVertexCount		= (FanOuterVertexCount + 1); // 1 つの扇型の頂点数
			const Vertex2D::IndexType FanTriangleCount		= Quality; // 1 つの扇型の三角形の数
			const Vertex2D::IndexType VertexCount			= (FanVertexCount * 4); // 頂点数
			const Vertex2D::IndexType IndexCount			= ((Quality * 3 * 4) + (6 * 3)); // インデックス数
			auto [pVertex, pIndex, indexOffset]				= bufferCreator(VertexCount, IndexCount);

			if (not pVertex)
			{
				return 0;
			}

			const float middleLeftX		= (rect.left + r);
			const float middleRightX	= (rect.right - r);
			const float topCenterY		= (rect.top + r);
			const float bottomCenterY	= (rect.bottom - r);

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			Vertex2D* pDst0 = pVertex;
			Vertex2D* pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2D* pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2D* pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2* cs = (pCS + i);
				const float x = (cs->x * r);
				const float y = (cs->y * r);

				pDst0->set((middleRightX + x), (topCenterY + y), color);
				pDst1->set((middleRightX - y), (bottomCenterY + x), color);
				pDst2->set((middleLeftX - x), (bottomCenterY - y), color);
				pDst3->set((middleLeftX + y), (topCenterY - x), color);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の最後の頂点
			{
				const float x = (pCS->x * r);
				const float y = (pCS->y * r);

				pDst0->set((middleRightX - y), (topCenterY + x), color);
				pDst1->set((middleRightX - x), (bottomCenterY - y), color);
				pDst2->set((middleLeftX + y), (bottomCenterY - x), color);
				pDst3->set((middleLeftX + x), (topCenterY + y), color);

				++pDst0;
				++pDst1;
				++pDst2;
				++pDst3;
			}

			// Fan の中心の頂点
			{
				(pDst3++)->set(middleRightX, topCenterY, color);
				(pDst3++)->set(middleRightX, bottomCenterY, color);
				(pDst3++)->set(middleLeftX, bottomCenterY, color);
				(pDst3++)->set(middleLeftX, topCenterY, color);
			}

			// UV 座標を設定する
			{
				const float uOffset	= uvRect.left;
				const float vOffset	= uvRect.top;
				const float left	= rect.left;
				const float top		= rect.top;
				const float ws		= ((uvRect.right - uvRect.left) / w);
				const float hs		= ((uvRect.bottom - uvRect.top) / h);

				Vertex2D* pDst = pVertex;

				for (size_t i = 0; i < VertexCount; ++i)
				{
					const float u = (uOffset + (pDst->pos.x - left) * ws);
					const float v = (vOffset + (pDst->pos.y - top) * hs);
					(pDst++)->tex.set(u, v);
				}
			}

			const Vertex2D::IndexType trFanStartIndex	= indexOffset;
			const Vertex2D::IndexType trFanEndIndex		= (trFanStartIndex + Quality);
			const Vertex2D::IndexType brFanStartIndex	= (trFanEndIndex + 1);
			const Vertex2D::IndexType brFanEndIndex		= (brFanStartIndex + Quality);
			const Vertex2D::IndexType blFanStartIndex	= (brFanEndIndex + 1);
			const Vertex2D::IndexType blFanEndIndex		= (blFanStartIndex + Quality);
			const Vertex2D::IndexType tlFanStartIndex	= (blFanEndIndex + 1);
			const Vertex2D::IndexType tlFanEndIndex		= (tlFanStartIndex + Quality);

			const Vertex2D::IndexType trCenterIndex = (indexOffset + FanOuterVertexCount * 4);
			const Vertex2D::IndexType brCenterIndex = (trCenterIndex + 1);
			const Vertex2D::IndexType blCenterIndex = (trCenterIndex + 2);
			const Vertex2D::IndexType tlCenterIndex = (trCenterIndex + 3);

			{
				// tr
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = (indexOffset + i + 1);
					*pIndex++ = trCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// br
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = brCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// bl
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
				{
					*pIndex++ = (indexOffset + i);
					*pIndex++ = indexOffset + (i + 1);
					*pIndex++ = blCenterIndex;
				}

				indexOffset += FanOuterVertexCount;

				// tl
				for (Vertex2D::IndexType i = 0; i < FanTriangleCount; ++i)
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

			return IndexCount;
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

				(pDst0++)->set((centerX + ox), (centerY + oy), 0.0f, 0.5f, color);
				(pDst0++)->set((centerX + ix), (centerY + iy), 0.5f, 0.5f, color);

				(pDst1++)->set((centerX - oy), (centerY + ox), 0.0f, 0.5f, color);
				(pDst1++)->set((centerX - iy), (centerY + ix), 0.5f, 0.5f, color);

				(pDst2++)->set((centerX - ox), (centerY - oy), 0.0f, 0.5f, color);
				(pDst2++)->set((centerX - ix), (centerY - iy), 0.5f, 0.5f, color);

				(pDst3++)->set((centerX + oy), (centerY - ox), 0.0f, 0.5f, color);
				(pDst3++)->set((centerX + iy), (centerY - ix), 0.5f, 0.5f, color);
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

			const Float2* pCS = (SinCosTable.data() + GetSinCosTableIndex(Quality));

			// 中心頂点を最初に配置
			pVertex[0].set(centerX, centerY, 0.5f, 0.5f, color);

			Vertex2D* pDst0 = &pVertex[1];
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

				(pDst0++)->set((centerX + ox), (centerY + oy), 0.0f, 0.5f, color);
				(pDst0++)->set((centerX + ix), (centerY + iy), 0.5f, 0.5f, color);

				(pDst1++)->set((centerX - oy), (centerY + ox), 0.0f, 0.5f, color);
				(pDst1++)->set((centerX - iy), (centerY + ix), 0.5f, 0.5f, color);

				(pDst2++)->set((centerX - ox), (centerY - oy), 0.0f, 0.5f, color);
				(pDst2++)->set((centerX - ix), (centerY - iy), 0.5f, 0.5f, color);

				(pDst3++)->set((centerX + oy), (centerY - ox), 0.0f, 0.5f, color);
				(pDst3++)->set((centerX + iy), (centerY - ix), 0.5f, 0.5f, color);
			}

			{
				const Vertex2D::IndexType ringStartIndex = (indexOffset + 1);

				// 外側のリングのインデックス
				for (Vertex2D::IndexType i = 0; i < FullQuality; ++i)
				{
					for (Vertex2D::IndexType k = 0; k < 6; ++k)
					{
						*pIndex++ = (ringStartIndex + (i * 2 + CircleFrameIndexTable[k]) % (FullQuality * 2));
					}
				}

				// 内円を中心頂点で埋めるインデックス
				{
					const Vertex2D::IndexType centerIndex = indexOffset; // 中心頂点のインデックス

					for (Vertex2D::IndexType i = 0; i < FullQuality; ++i)
					{
						const Vertex2D::IndexType innerCurrent = (ringStartIndex + i * 2 + 1); // 内円の現在の頂点
						const Vertex2D::IndexType innerNext = (ringStartIndex + ((i * 2 + 3) % (FullQuality * 2))); // 内円の次の頂点

						*pIndex++ = centerIndex;
						*pIndex++ = innerCurrent;
						*pIndex++ = innerNext;
					}
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
						pVertex->pos.set(xs[x], ys[y]);
						pVertex->tex.set(uvs[x], uvs[y]);
						pVertex->color = color;
						++pVertex;
					}
				}
			}

			// インデックス情報の書き込み
			{
				Vertex2D::IndexType* pBase = pIndex;

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
					pBase[29] = pBase[28] = pBase[27] = pBase[26] = pBase[25] = pBase[24];
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

					Vertex2D* pOuter = &pVertex[corner * roundVertexCount];
					Vertex2D* pInner = (pOuter + fanVertexCount);
					Vertex2D* pC = (pInner + fanVertexCount);

					const auto offset = angleOffsets[corner];

					for (Vertex2D::IndexType i = 0; i < quality; ++i)
					{
						const float rad = (radDelta * i);
						const auto [s, c] = FastMath::SinCos(rad + offset);

						pOuter->pos.set((cx + farRadius * c), (cy + farRadius * s));
						pOuter->tex.set(0.0f, 0.5f);
						pOuter->color = color;

						pInner->pos.set((cx + nearRadius * c), (cy + nearRadius * s));
						pInner->tex.set(0.5f, 0.5f);
						pInner->color = color;

						++pOuter;
						++pInner;
					}

					pC->pos.set(cx, cy);
					pC->tex.set(0.5f, 0.5f);
					pC->color = color;
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
