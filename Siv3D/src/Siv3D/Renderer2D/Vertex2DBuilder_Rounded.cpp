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
# include "Vertex2DBuilder.hpp"
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/QuarterArcTable.hpp>

namespace s3d
{
	namespace
	{
		static constexpr Vertex2D::IndexType RectIndexTable[6] = { 0, 1, 2, 2, 1, 3 };

		static constexpr Vertex2D::IndexType CircleFrameIndexTable[6] = { 0, 2, 1, 1, 2, 3 };

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

		static void SetEllipseFrameVertexPair(
			Vertex2D*& pDst,
			const float centerX,
			const float centerY,
			const float a,
			const float b,
			const float ux,
			const float uy,
			const float innerThickness,
			const float outerThickness) noexcept
		{
			const float baseX = (centerX + a * ux);
			const float baseY = (centerY + b * uy);

			const Float2 normal = CalculateEllipseNormal(a, b, ux, uy);

			// outer vertex
			(pDst++)->pos.set(
				(baseX + normal.x * outerThickness),
				(baseY + normal.y * outerThickness));

			// inner vertex
			(pDst++)->pos.set(
				(baseX - normal.x * innerThickness),
				(baseY - normal.y * innerThickness));
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

		struct PolylineBuffer
		{
			Array<Float2> simplified;	// 間引き適用後（中間バッファ）

			Array<uint32> simplifiedSrc;

			Array<Float2> points;		// 角分割適用後（最終出力）

			Array<uint32> srcIndices;	// points[i] が入力点列の何番目由来か
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
			return buffer;
		}

		/// @brief 鋭角頂点を分割する際の補助点の変位方向（角を抜けて進む向きの二等分線）を返します。
		[[nodiscard]]
		static Float2 CornerSplitDirection(const Float2& inDir, const Float2& outDir) noexcept
		{
			const Float2 v = (inDir + outDir);

			return ((0.0001f < v.lengthSq()) ? v.normalized() : Float2{ -inDir.y, inDir.x });
		}

		struct PreparedPolyline
		{
			std::span<const Float2> points;

			std::span<const uint32> srcIndices;
		};

		/// @brief 入力点列に間引きと鋭角頂点の分割を適用します。
		/// @remark 間引き: 直前の採用点からスクリーン上で 0.25px 未満しか離れていない点を除去。
		/// @remark 角分割: 内角が acos(0.55) ≒ 56.6° 未満の鋭角頂点の直後
		///         （closed の先頭頂点については巡回順で直前 = 点列末尾）に、二等分線方向へ
		///         ε = 0.001px ずらした補助点を挿入し、マイター長の暴走を防ぐ。
		/// @param trackSrcIndices true のとき、各出力点の入力インデックスを記録する
		///        （点ごとの色指定の参照用。補助点は元の頂点と同じインデックスを持つ）。
		/// @return buffer 内を指す span。次に GetPolylineBuffer() が呼ばれるまで有効。
		template <class PointType>
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

			// 角分割の対象が無い場合は間引き結果をそのまま返す（コピーしない）
			if ((simplifiedSize < 3) || (not splitSharpCorners))
			{
				return{ { simplified.data(), simplified.size() }, { simplifiedSrc.data(), simplifiedSrc.size() } };
			}

			// 角分割
			{
				Array<Float2>& result = buffer.points;
				Array<uint32>& resultSrc = buffer.srcIndices;
				const Float2* const pSrc = simplified.data();
				const float epsilon = (0.001f / scale);

				result.push_back(pSrc[0]);

				if (trackSrcIndices)
				{
					resultSrc.push_back(simplifiedSrc[0]);
				}

				// セグメントの単位方向ベクトルは隣接頂点間で共有されるため持ち回り、
				// 正規化 (sqrt) の重複を避ける
				const Float2 d01 = (pSrc[1] - pSrc[0]).normalized();
				Float2 inDir = d01;

				const size_t last = (closed ? simplifiedSize : (simplifiedSize - 1));

				for (size_t i = 1; i < last; ++i)
				{
					const Float2 current = pSrc[i];
					const Float2 next = ((i + 1) < simplifiedSize) ? pSrc[i + 1] : pSrc[0];
					const Float2 outDir = (next - current).normalized();

					result.push_back(current);

					if (trackSrcIndices)
					{
						resultSrc.push_back(simplifiedSrc[i]);
					}

					if (inDir.dot(outDir) < SharpCornerThreshold)
					{
						result.push_back(current + (CornerSplitDirection(inDir, outDir) * epsilon));

						if (trackSrcIndices)
						{
							resultSrc.push_back(simplifiedSrc[i]);
						}
					}

					inDir = outDir;
				}

				if (closed)
				{
					// 先頭頂点の角分割。補助点は巡回順で先頭頂点の「直前」に当たるため、
					// 点列の末尾に、二等分線の逆方向で追加する
					if (inDir.dot(d01) < SharpCornerThreshold)
					{
						result.push_back(pSrc[0] - (CornerSplitDirection(inDir, d01) * epsilon));

						if (trackSrcIndices)
						{
							resultSrc.push_back(simplifiedSrc[0]);
						}
					}
				}
				else
				{
					result.push_back(pSrc[simplifiedSize - 1]);

					if (trackSrcIndices)
					{
						resultSrc.push_back(simplifiedSrc[simplifiedSize - 1]);
					}
				}

				return{ { result.data(), result.size() }, { resultSrc.data(), resultSrc.size() } };
			}
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
			const Float2 tangent = ((0.001f < v.lengthSq()) ? v.normalized() : (nextPoint - prevPoint).normalized());
			const Float2 miter{ -tangent.y, tangent.x };
			const float length = (halfThickness / tangent.dot(inDir));
			const Float2 m = (miter * length);

			return{ (point + m), (point - m) };
		}

		/// @brief 前処理済みの点列から、幅 (halfThickness * 2) のクアッドストリップを構築します。
		/// @param pts PreparePolyline 済みの点列。open は 2 点以上、closed は 3 点以上であること。
		/// @param colorAt pts[i] に対応する頂点ペアの色を返す関数。
		///        i の昇順に、各 i につきちょうど 1 回呼び出される（状態を持つラムダを許容するための契約）。
		/// @param pStartAngle, pEndAngle open のとき、始端・終端の法線角度を返す（キャップ描画用）。
		template <class ColorFn>
		[[nodiscard]]
		static Vertex2D::IndexType EmitLineStringStrip(const BufferCreatorFunc& bufferCreator, const std::span<const Float2> pts, const float halfThickness, const bool closed, ColorFn&& colorAt, float* pStartAngle = nullptr, float* pEndAngle = nullptr)
		{
			// 頂点数 (2n) とインデックス数 (6n) の双方が IndexType に収まる点数に制限する。
			constexpr size_t MaxPointCount = (std::numeric_limits<Vertex2D::IndexType>::max() / 6);

			const size_t n = pts.size();

			if ((n < 2) || (MaxPointCount < n))
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

			const Float2* const pBuf = pts.data();
			const Float2 firstDir = (closed ? (pBuf[0] - pBuf[n - 1]) : (pBuf[1] - pBuf[0])).normalized();
			Float2 inDir = firstDir;

			// 始端の頂点ペア
			if (closed)
			{
				const Float2 outDir = (pBuf[1] - pBuf[0]).normalized();
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
				const Float2 outDir = (pBuf[i + 1] - pBuf[i]).normalized();
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

			return indexCount;
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
			auto [pVertex, pIndex, indexOffset] = bufferCreator(VertexCount, IndexCount);
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

			Vertex2D::IndexType roundIndexCount = 0;

			if (startCap == LineCap::Round)
			{
				const float startAngle = std::atan2(leftOffset.x, -leftOffset.y);
				roundIndexCount += BuildRoundCap(bufferCreator, start, halfThickness, (startAngle + Math::PiF), colors[0], scale);
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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);
			
			Vertex2D* pDst0 = &pVertex[1];
			Vertex2D* pDst1 = (pDst0 + Quality);
			Vertex2D* pDst2 = (pDst1 + Quality);
			Vertex2D* pDst3 = (pDst2 + Quality);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

				if (colorType == ColorFillDirection::InOut)
				{
					pDst0->set((centerX + x), (centerY + y), color1);
					pDst1->set((centerX - y), (centerY + x), color1);
					pDst2->set((centerX - x), (centerY - y), color1);
					pDst3->set((centerX + y), (centerY - x), color1);
				}
				else if(colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + ((cs.y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((cs.x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs.y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((-cs.x + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + x), (centerY + y), c0);
					pDst1->set((centerX - y), (centerY + x), c1);
					pDst2->set((centerX - x), (centerY - y), c2);
					pDst3->set((centerX + y), (centerY - x), c3);
				}
				else
				{
					const Float4 c0 = (color0 + ((cs.x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((-cs.y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs.x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((cs.y + 1.0f) * 0.5f) * colorDiff);

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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2D* pDst0 = &pVertex[0];
			Vertex2D* pDst1 = (pDst0 + Quality * 2);
			Vertex2D* pDst2 = (pDst0 + Quality * 4);
			Vertex2D* pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float ox = (cs.x * rOuter);
				const float ix = (cs.x * rInner);
				const float oy = (cs.y * rOuter);
				const float iy = (cs.y * rInner);

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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2D* pDst0 = &pVertex[1];
			Vertex2D* pDst1 = (pDst0 + Quality);
			Vertex2D* pDst2 = (pDst1 + Quality);
			Vertex2D* pDst3 = (pDst2 + Quality);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float xa = (cs.x * a);
				const float xb = (cs.x * b);
				const float ya = (cs.y * a);
				const float yb = (cs.y * b);

				if (colorType == ColorFillDirection::InOut)
				{
					pDst0->set((centerX + xa), (centerY + yb), color1);
					pDst1->set((centerX - ya), (centerY + xb), color1);
					pDst2->set((centerX - xa), (centerY - yb), color1);
					pDst3->set((centerX + ya), (centerY - xb), color1);
				}
				else if (colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + ((cs.y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((cs.x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs.y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((-cs.x + 1.0f) * 0.5f) * colorDiff);

					pDst0->set((centerX + xa), (centerY + yb), c0);
					pDst1->set((centerX - ya), (centerY + xb), c1);
					pDst2->set((centerX - xa), (centerY - yb), c2);
					pDst3->set((centerX + ya), (centerY - xb), c3);
				}
				else
				{
					const Float4 c0 = (color0 + ((cs.x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c1 = (color0 + ((-cs.y + 1.0f) * 0.5f) * colorDiff);
					const Float4 c2 = (color0 + ((-cs.x + 1.0f) * 0.5f) * colorDiff);
					const Float4 c3 = (color0 + ((cs.y + 1.0f) * 0.5f) * colorDiff);

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

		Vertex2D::IndexType BuildEllipseFrame(const BufferCreatorFunc& bufferCreator, const Float2& center, const float a, const float b, const float innerThickness,	const float outerThickness,	const Float4& innerColor, const Float4& outerColor, const float scale)
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

			Vertex2D* pDst0 = &pVertex[0];
			Vertex2D* pDst1 = (pDst0 + Quality * 2);
			Vertex2D* pDst2 = (pDst0 + Quality * 4);
			Vertex2D* pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float ux = cs.x;
				const float uy = cs.y;

				SetEllipseFrameVertexPair(
					pDst0,
					centerX,
					centerY,
					a,
					b,
					ux,
					uy,
					innerThickness,
					outerThickness);

				SetEllipseFrameVertexPair(
					pDst1,
					centerX,
					centerY,
					a,
					b,
					-uy,
					ux,
					innerThickness,
					outerThickness);

				SetEllipseFrameVertexPair(
					pDst2,
					centerX,
					centerY,
					a,
					b,
					-ux,
					-uy,
					innerThickness,
					outerThickness);

				SetEllipseFrameVertexPair(
					pDst3,
					centerX,
					centerY,
					a,
					b,
					uy,
					-ux,
					innerThickness,
					outerThickness);
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
		//	BuildEllipsePie
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildEllipsePie(const BufferCreatorFunc& bufferCreator, const Float2& center, const float rx, const float ry, const float startAngle, const float angle, const Float4& innerColor, const Float4& outerColor, const float scale)
		{
			if (angle == 0.0f)
			{
				return 0;
			}

			const float srx = (rx * scale);
			const float sry = (ry * scale);

			Vertex2D::IndexType Quality = CalculateCirclePieQuality(Max(srx, sry), Abs(angle));

			if (Quality < 2)
			{
				Quality = 2;
			}

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

			// 周（楕円）
			{
				const float start = (startAngle + ((angle < 0.0f) ? angle : 0.0f));
				const float radDelta = (Abs(angle) / (Quality - 1));
				Vertex2D* pDst = &pVertex[1];

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const float rad = (start + (radDelta * i));
					const auto [s, c] = FastMath::SinCos(rad);
					(pDst++)->pos.set((centerX + srx * s), (centerY - sry * c));
				}
			}

			// 色
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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

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
				const Float2& cs = pCS[i];
				const float cp = SignedPow(cs.x);
				const float sp = SignedPow(cs.y);

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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2D* pDst0 = pVertex;
			Vertex2D* pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2D* pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2D* pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

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
				const Float2& cs = pCS[0];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2D* pDst0 = pVertex;
			Vertex2D* pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2D* pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2D* pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

				if (colorType == ColorFillDirection::TopBottom)
				{
					const Float4 c0 = (color0 + ((cs.y + 1.0f) * colorRScale) * colorDiff);
					const Float4 c1 = (color0 + (colorROffset + (cs.x * colorRScale)) * colorDiff);
					const Float4 c2 = (color0 + (colorROffset + (-cs.y * colorRScale)) * colorDiff);
					const Float4 c3 = (color0 + ((-cs.x + 1.0f) * colorRScale) * colorDiff);

					pDst0->set((middleRightX + x), (topCenterY + y), c0);
					pDst1->set((middleRightX - y), (bottomCenterY + x), c1);
					pDst2->set((middleLeftX - x), (bottomCenterY - y), c2);
					pDst3->set((middleLeftX + y), (topCenterY - x), c3);
				}
				else // ColorFillDirection::LeftRight
				{
					const Float4 c0 = (color0 + (colorROffset + (cs.x * colorRScale)) * colorDiff);
					const Float4 c1 = (color0 + (colorROffset + (-cs.y * colorRScale)) * colorDiff);
					const Float4 c2 = (color0 + ((-cs.x + 1.0f) * colorRScale) * colorDiff);
					const Float4 c3 = (color0 + ((cs.y + 1.0f) * colorRScale) * colorDiff);

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
				const Float2& cs = pCS[0];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

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

				const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

				Vertex2D* pDst0 = pVertex;
				Vertex2D* pDst1 = (pVertex + ArcVertexCount * 1);
				Vertex2D* pDst2 = (pVertex + ArcVertexCount * 2);
				Vertex2D* pDst3 = (pVertex + ArcVertexCount * 3);

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const Float2& cs = pCS[i];
					const float ox = (cs.x * outerR);
					const float oy = (cs.y * outerR);
					const float ix = (cs.x * innerR);
					const float iy = (cs.y * innerR);

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
					const Float2& cs = pCS[0];
					const float ox = (cs.x * outerR);
					const float oy = (cs.y * outerR);
					const float ix = (cs.x * innerR);
					const float iy = (cs.y * innerR);

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

				const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

				Vertex2D* pDst0 = pVertex;
				Vertex2D* pDst1 = (pVertex + ArcVertexCount * 1);
				Vertex2D* pDst2 = (pVertex + ArcVertexCount * 2);
				Vertex2D* pDst3 = (pVertex + ArcVertexCount * 3);

				for (Vertex2D::IndexType i = 0; i < Quality; ++i)
				{
					const Float2& cs = pCS[i];
					const float ox = (cs.x * outerR);
					const float oy = (cs.y * outerR);
					const float ix = (cs.x * innerR);
					const float iy = (cs.y * innerR);

					if (colorType == ColorFillDirection::TopBottom)
					{
						const Float4 c0 = (color0 + ((cs.y + 1.0f) * ot1) * colorDiff);
						const Float4 c1 = (color0 + (ot2 + (cs.x * ot1)) * colorDiff);
						const Float4 c2 = (color0 + (ot2 + (-cs.y * ot1)) * colorDiff);
						const Float4 c3 = (color0 + ((-cs.x + 1.0f) * ot1) * colorDiff);

						pDst0->set((outerRightCenterX + ox), (outerTopCenterY + oy), c0);
						pDst1->set((outerRightCenterX - oy), (outerBottomCenterY + ox), c1);
						pDst2->set((outerLeftCenterX - ox), (outerBottomCenterY - oy), c2);
						pDst3->set((outerLeftCenterX + oy), (outerTopCenterY - ox), c3);
					}
					else // ColorFillDirection::LeftRight
					{
						const Float4 c0 = (color0 + (ot2 + (cs.x * ot1)) * colorDiff);
						const Float4 c1 = (color0 + (ot2 + (-cs.y * ot1)) * colorDiff);
						const Float4 c2 = (color0 + ((-cs.x + 1.0f) * ot1) * colorDiff);
						const Float4 c3 = (color0 + ((cs.y + 1.0f) * ot1) * colorDiff);

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
						const Float4 c0 = (color0 + (it1 + (cs.y + 1.0f) * irt) * colorDiff);
						const Float4 c1 = (color0 + (it3 + (cs.x * irt)) * colorDiff);
						const Float4 c2 = (color0 + (it3 + (-cs.y * irt)) * colorDiff);
						const Float4 c3 = (color0 + (it1 + (-cs.x + 1.0f) * irt) * colorDiff);

						pDst0->set((innerRightCenterX + ix), (innerTopCenterY + iy), c0);
						pDst1->set((innerRightCenterX - iy), (innerBottomCenterY + ix), c1);
						pDst2->set((innerLeftCenterX - ix), (innerBottomCenterY - iy), c2);
						pDst3->set((innerLeftCenterX + iy), (innerTopCenterY - ix), c3);
					}
					else // ColorFillDirection::LeftRight
					{
						const Float4 c0 = (color0 + (it3 + (cs.x * irt)) * colorDiff);
						const Float4 c1 = (color0 + (it3 + (-cs.y * irt)) * colorDiff);
						const Float4 c2 = (color0 + (it1 + (-cs.x + 1.0f) * irt) * colorDiff);
						const Float4 c3 = (color0 + (it1 + (cs.y + 1.0f) * irt) * colorDiff);

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
					const Float2& cs = pCS[0];
					const float ox = (cs.x * outerR);
					const float oy = (cs.y * outerR);
					const float ix = (cs.x * innerR);
					const float iy = (cs.y * innerR);
					
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
		//	BuildShape2DFrame
		//
		////////////////////////////////////////////////////////////////

		Vertex2D::IndexType BuildShape2DFrame(const BufferCreatorFunc& bufferCreator, const std::span<const Float2> vertices, const float thickness, const Float4& color, const float scale)
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
			const PreparedPolyline prepared = PreparePolyline<Float2>(vertices, none, scale, true, true, false, buffer);

			if (prepared.points.size() < 3)
			{
				return 0;
			}

			return EmitLineStringStrip(bufferCreator, prepared.points, (thickness * 0.5f), true,
				[&color](size_t) { return color; });
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
					return EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, true, colorAt);
				}

				// 2 点に縮退したリングは、キャップなしの開いた線分として描画する
				return EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, false, colorAt);
			}
			else
			{
				float startAngle = 0.0f, endAngle = 0.0f;

				Vertex2D::IndexType indexCount = EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, false, colorAt, &startAngle, &endAngle);

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

			PolylineBuffer& buffer = GetPolylineBuffer();
			const PreparedPolyline prepared = PreparePolyline<Vec2>(points, offset, scale, false, (not inner), false, buffer);

			if (prepared.points.size() < 2)
			{
				return BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), offset, thickness, colorStart, colorEnd, scale);
			}

			const float halfThickness = (thickness * 0.5f);
			const bool hasStartCap = (startCap != LineCap::Flat);
			const bool hasEndCap = (endCap != LineCap::Flat);

			// 色はキャップ部分も含めた全長 (allLength) に対して線形に補間する。
			const float baseLength = CalculateLength(prepared.points);
			const float startDistance = (hasStartCap ? halfThickness : 0.0f);
			const float allLength = (baseLength + startDistance + (hasEndCap ? halfThickness : 0.0f));
			const float lengthInv = ((0.0f < allLength) ? (1.0f / allLength) : 0.0f);
			const Float4 colorDiff = (colorEnd - colorStart);

			// 始端・終端の頂点色。キャップ側のグラデーションに引き継ぐ
			const Float4 stripStartColor = (colorStart + colorDiff * (startDistance * lengthInv));
			const Float4 stripEndColor = (colorStart + colorDiff * ((startDistance + baseLength) * lengthInv));

			const Float2* const pts = prepared.points.data();
			auto colorAt = [&, distance = startDistance](const size_t i) mutable
			{
				if (0 < i)
				{
					distance += pts[i].distanceFrom(pts[i - 1]);
				}

				return (colorStart + colorDiff * (distance * lengthInv));
			};

			float startAngle = 0.0f, endAngle = 0.0f;

			Vertex2D::IndexType indexCount = EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, false, colorAt, &startAngle, &endAngle);

			indexCount += BuildLineStringCaps(bufferCreator, startCap, endCap, points.front(), startAngle, points.back(), endAngle, offset, thickness, colorStart, stripStartColor, stripEndColor, colorEnd, scale);

			return indexCount;
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
					return EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, true, colorAt);
				}

				// 2 点に縮退したリングは、キャップなしの開いた線分として描画する
				return EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, false, colorAt);
			}
			else
			{
				float startAngle = 0.0f, endAngle = 0.0f;

				Vertex2D::IndexType indexCount = EmitLineStringStrip(bufferCreator, prepared.points, halfThickness, false, colorAt, &startAngle, &endAngle);

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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2D* pDst0 = pVertex;
			Vertex2D* pDst1 = (pDst0 + FanOuterVertexCount);
			Vertex2D* pDst2 = (pDst1 + FanOuterVertexCount);
			Vertex2D* pDst3 = (pDst2 + FanOuterVertexCount);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
			{
				const Float2& cs = pCS[i];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

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
				const Float2& cs = pCS[0];
				const float x = (cs.x * r);
				const float y = (cs.y * r);

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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			Vertex2D* pDst0 = &pVertex[0];
			Vertex2D* pDst1 = (pDst0 + Quality * 2);
			Vertex2D* pDst2 = (pDst0 + Quality * 4);
			Vertex2D* pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
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

			const std::span<const Float2> pCS = QuarterArcTable::GetUnitVectors(Quality);

			// 中心頂点を最初に配置
			pVertex[0].set(centerX, centerY, 0.5f, 0.5f, color);

			Vertex2D* pDst0 = &pVertex[1];
			Vertex2D* pDst1 = (pDst0 + Quality * 2);
			Vertex2D* pDst2 = (pDst0 + Quality * 4);
			Vertex2D* pDst3 = (pDst0 + Quality * 6);

			for (Vertex2D::IndexType i = 0; i < Quality; ++i)
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
