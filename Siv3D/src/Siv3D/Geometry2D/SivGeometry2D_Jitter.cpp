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

# include <Siv3D/Geometry2D/Jitter.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/PerlinNoise.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static double FBMSumAmplitude(const int32 octaves, const double persistence)
		{
			assert(0 < octaves);

			if (Abs(persistence - 1.0) < 1e-12)
			{
				return static_cast<double>(octaves);
			}

			return (1.0 - std::pow(persistence, octaves)) / (1.0 - persistence);
		}

		struct FBM2Ch
		{
			double nN;
			double nT;
		};

		[[nodiscard]]
		static FBM2Ch FBMNoise2Ch_Jitter(const PerlinNoise& perlin,
			const double xN, const double xT,
			const int32 octaves, const double persistence,
			const double invSumAmp)
		{
			double totalN = 0.0;
			double totalT = 0.0;
			double amplitude = 1.0;
			double frequency = 1.0;
			constexpr double kOctaveStride = 1000.0;

			for (int32 i = 0; i < octaves; ++i)
			{
				const double phase = (i * kOctaveStride);
				totalN += perlin.noise(xN * frequency + phase) * amplitude;
				totalT += perlin.noise(xT * frequency + phase) * amplitude;
				amplitude *= persistence;
				frequency *= 2.0;
			}

			return{ (totalN * invSumAmp), (totalT * invSumAmp) };
		}

		[[nodiscard]]
		static double Envelope01(const double t, const JitterEndpointSettings& start, const double startTaper, const JitterEndpointSettings& end, const double endTaper)
		{
			double e = 1.0;

			if (start.mode == JitterEndpointMode::Tapered && startTaper > 0.0)
			{
				e *= Min(t / startTaper, 1.0);
			}

			if (end.mode == JitterEndpointMode::Tapered && endTaper > 0.0)
			{
				e *= Min((1.0 - t) / endTaper, 1.0);
			}

			return Math::Smoothstep(e);
		}

		[[nodiscard]]
		static Vec2 JitterOffset(
			const PerlinNoise& perlin, const double s,
			double freq, int32 oct, const double pers, const double invSumAmp,
			const double normalAmp, const double tangentAmp,
			const double envelope, const Vec2& normal, const Vec2& tangent)
		{
			const double xN = (s * freq);
			const double xT = (s * freq * 1.3 + 100.0);
			const auto n = FBMNoise2Ch_Jitter(perlin, xN, xT, oct, pers, invSumAmp);
			return (normal * (n.nN * normalAmp * envelope) + tangent * (n.nT * tangentAmp * envelope));
		}

		[[nodiscard]]
		static LineString JitterImpl(const LineString& points,
			const JitterSettings& settings,
			const JitterEndpointSettings& start,
			const JitterEndpointSettings& end,
			const CloseRing closeRing)
		{
			const size_t n = points.size();
			if (n < 2)
			{
				return points;
			}

			const bool isClosed = (closeRing == CloseRing::Yes);
			const size_t segCount = (isClosed ? n : (n - 1));

			// 弧長
			Array<double> arc;
			arc.reserve(isClosed ? (n + 1) : n);
			arc.push_back(0.0);

			for (size_t i = 0; i < segCount; ++i)
			{
				const size_t j = (i + 1 < n) ? (i + 1) : 0; // closed のとき i==n-1 で 0
				arc.push_back(arc.back() + points[i].distanceFrom(points[j]));
			}

			const double totalLength = arc.back();
			if (totalLength < 0.001)
			{
				return points;
			}

			const PerlinNoise perlin{ settings.seed };
			const double startTaper = Clamp(start.taperRatio, 0.0, 0.5);
			const double endTaper = Clamp(end.taperRatio, 0.0, 0.5);
			const int32 oct = Max(settings.fbmOctaves, 1);
			const double pers = settings.fbmPersistence;
			const double invSumAmp = (1.0 / FBMSumAmplitude(oct, pers));
			const double freq = settings.noiseFrequency;
			const double normalAmp = settings.roughness * settings.normalScale;
			const double tangentAmp = settings.roughness * settings.tangentScale;
			const double segLenSetting = Max(0.001, settings.segmentLength);

			LineString result;
			result.reserve(static_cast<size_t>(totalLength / segLenSetting) + n + 8);

			for (size_t segIdx = 0; segIdx < segCount; ++segIdx)
			{
				const size_t nextIdx = (segIdx + 1 < n) ? (segIdx + 1) : 0;

				const Vec2 a = points[segIdx];
				const Vec2 b = points[nextIdx];

				const double segLength = (arc[segIdx + 1] - arc[segIdx]);
				if (segLength < 0.001)
				{
					continue;
				}

				const int32 numDivisions = Max(2, static_cast<int32>(segLength / segLenSetting));
				const bool isLastSegment = ((not isClosed) && (segIdx + 1 == segCount));
				const int32 endIndex = isClosed ? (numDivisions - 1) : (isLastSegment ? numDivisions : (numDivisions - 1));
				const Vec2 tan0 = points.tangentAtVertex(segIdx, closeRing);
				const Vec2 tan1 = points.tangentAtVertex(nextIdx, closeRing);
				Vec2 prevTangent = Math::Lerp(tan0, tan1, 0.0).normalized_or(Vec2{ 1.0, 0.0 });

				for (int32 i = 0; i <= endIndex; ++i)
				{
					const double localT = (static_cast<double>(i) / numDivisions);
					const Vec2 p = Math::Lerp(a, b, localT);

					const double s = arc[segIdx] + (segLength * localT);
					const double t = (s / totalLength);

					if (const bool isStartPoint = (segIdx == 0 && i == 0); isStartPoint)
					{
						if (start.mode == JitterEndpointMode::Locked)
						{
							// Closed の seam も含め、先頭要素をそのまま出す
							result.push_back(points.front());
							continue;
						}
					}

					if (not isClosed)
					{
						const bool isEndPoint = (segIdx + 1 == segCount && i == numDivisions);

						if (isEndPoint && (end.mode == JitterEndpointMode::Locked))
						{
							result.push_back(p);
							continue;
						}
					}

					const Vec2 tangent = Math::Lerp(tan0, tan1, localT).normalized_or(prevTangent);
					prevTangent = tangent;
					const Vec2 normal{ -tangent.y, tangent.x };

					const double envelope = Envelope01(t, start, startTaper, end, endTaper); // 端点減衰
					result.push_back(p + JitterOffset(perlin, s, freq, oct, pers, invSumAmp, normalAmp, tangentAmp, envelope, normal, tangent));
				}
			}

			return result;
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Jitter
		//
		////////////////////////////////////////////////////////////////

		LineString Jitter(const Line& g, const JitterSettings& settings, const JitterEndpointSettings& endPoint)
		{
			return Jitter(g, settings, endPoint, endPoint);
		}

		LineString Jitter(const Line& g, const JitterSettings& settings,
			const JitterEndpointSettings& start, const JitterEndpointSettings& end)
		{
			const Vec2 a = g.start;
			const Vec2 b = g.end;

			const double totalLength = a.distanceFrom(b);

			if (totalLength < 0.001)
			{
				return LineString{ a, b };
			}

			const PerlinNoise perlin{ settings.seed };
			const int32 oct = Max(settings.fbmOctaves, 1);
			const double pers = settings.fbmPersistence;
			const double invSumAmp = (1.0 / FBMSumAmplitude(oct, pers));
			const double freq = settings.noiseFrequency;
			const double normalAmp = settings.roughness * settings.normalScale;
			const double tangentAmp = settings.roughness * settings.tangentScale;
			const double segLen = Max(0.001, settings.segmentLength);
			const int32 numDivisions = Max(2, static_cast<int32>(totalLength / segLen));
			const Vec2 tangent = (b - a) / totalLength;
			const Vec2 normal{ -tangent.y, tangent.x };
			const double startTaper = Clamp(start.taperRatio, 0.0, 0.5);
			const double endTaper = Clamp(end.taperRatio, 0.0, 0.5);

			LineString result;
			result.reserve(numDivisions + 1);

			for (int32 i = 0; i <= numDivisions; ++i)
			{
				const double t = (static_cast<double>(i) / numDivisions); // 0..1
				const Vec2 p = Math::Lerp(a, b, t);

				// 端点の Locked チェック
				if ((i == 0 && start.mode == JitterEndpointMode::Locked) ||
					(i == numDivisions && end.mode == JitterEndpointMode::Locked))
				{
					result.push_back(p);
					continue;
				}

				const double envelope = Envelope01(t, start, startTaper, end, endTaper); // 端点減衰
				const double s = (t * totalLength);
				result.push_back(p + JitterOffset(perlin, s, freq, oct, pers, invSumAmp, normalAmp, tangentAmp, envelope, normal, tangent));
			}

			return result;
		}

		LineString Jitter(const RectF& g, const JitterSettings& settings, const JitterEndpointSettings& endPoint)
		{
			return JitterImpl(g.outer(), settings, endPoint, endPoint, CloseRing::Yes);
		}

		LineString Jitter(const Triangle& g, const JitterSettings& settings, const JitterEndpointSettings& endPoint)
		{
			return JitterImpl(g.outer(), settings, endPoint, endPoint, CloseRing::Yes);
		}

		LineString Jitter(const Quad& g, const JitterSettings& settings, const JitterEndpointSettings& endPoint)
		{
			return JitterImpl(g.outer(), settings, endPoint, endPoint, CloseRing::Yes);
		}

		LineString Jitter(const LineString& g, const JitterSettings& settings, const JitterEndpointSettings& endPoint)
		{
			return Jitter(g, settings, endPoint, endPoint);
		}

		LineString Jitter(const LineString& g, const JitterSettings& settings,
			const JitterEndpointSettings& start, const JitterEndpointSettings& end)
		{
			return JitterImpl(g, settings, start, end, CloseRing::No);
		}

		////////////////////////////////////////////////////////////////
		//
		//	JitterClosed
		//
		////////////////////////////////////////////////////////////////

		LineString JitterClosed(const LineString& g, const JitterSettings& settings, const JitterEndpointSettings& seam)
		{
			return JitterImpl(g, settings, seam, seam, CloseRing::Yes);
		}
	}
}
