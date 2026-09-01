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

# include <Siv3D/Mesh3D.hpp>
# include <Siv3D/MathConstants.hpp>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsFloatRepresentable(const double value) noexcept
		{
			constexpr double MaxFloat = std::numeric_limits<float>::max();
			return (std::isfinite(value)
				&& (-MaxFloat <= value)
				&& (value <= MaxFloat));
		}

		[[nodiscard]]
		static bool CheckedAdd(const size_t a, const size_t b, size_t& result) noexcept
		{
			if ((std::numeric_limits<size_t>::max() - a) < b)
			{
				return false;
			}

			result = (a + b);
			return true;
		}

		[[nodiscard]]
		static bool CheckedMultiply(const size_t a, const size_t b, size_t& result) noexcept
		{
			if ((a != 0)
				&& ((std::numeric_limits<size_t>::max() / a) < b))
			{
				return false;
			}

			result = (a * b);
			return true;
		}

		[[nodiscard]]
		static bool ToProfilePoint(const Vec2 source, Float2& result) noexcept
		{
			if ((source.x < 0.0)
				|| (not IsFloatRepresentable(source.x))
				|| (not IsFloatRepresentable(source.y)))
			{
				return false;
			}

			result = source;
			return ((source.x == 0.0) || (result.x != 0.0f));
		}

		struct ProfileSegment
		{
			Float2 start;
			Float2 end;
			Float2 normal;
			double startDistance;
			double endDistance;
		};

		struct CircleSample
		{
			float sin;
			float cos;
		};

		[[nodiscard]]
		static Float2 SmoothProfileNormal(
			const Float2& normal0,
			const Float2& normal1,
			const Float2& fallback,
			const float minNormalDot) noexcept
		{
			if (normal0.dot(normal1) < minNormalDot)
			{
				return fallback;
			}

			const Float2 sum = (normal0 + normal1);
			const float lengthSq = sum.lengthSq();
			if (lengthSq == 0.0f)
			{
				return fallback;
			}

			return (sum / std::sqrt(lengthSq));
		}

		[[nodiscard]]
		static Vertex3D MakeVertex(
			const Float2 point,
			const Float2 profileNormal,
			const CircleSample sample,
			const float u,
			const float v) noexcept
		{
			return Vertex3D{
				.pos = Float3{ (point.x * sample.cos), point.y, (point.x * sample.sin) },
				.normal = Float3{
					(profileNormal.x * sample.cos), profileNormal.y, (profileNormal.x * sample.sin)
				},
				.tex = Float2{ u, v },
				.tangent = Float4{ -sample.sin, 0.0f, sample.cos, -1.0f }
			};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Revolve
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Revolve(const std::span<const Vec2> profile, const uint32 segments)
	{
		return Revolve(profile, segments, 0.0);
	}

	Mesh3D Mesh3D::Revolve(
		const std::span<const Vec2> profile,
		const uint32 segments,
		const double smoothingAngle)
	{
		if ((profile.size() < 2)
			|| (segments < 3)
			|| (not std::isfinite(smoothingAngle))
			|| (smoothingAngle < 0.0)
			|| (Math::Pi < smoothingAngle))
		{
			return{};
		}

		const bool closedProfile = (profile.front() == profile.back());
		if (closedProfile && (profile.size() < 4))
		{
			return{};
		}

		size_t ringStride;
		size_t fullSegmentVertexCount;
		if ((not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
			|| (not CheckedMultiply(ringStride, 2, fullSegmentVertexCount)))
		{
			return{};
		}

		const size_t profileSegmentCount = (profile.size() - 1);
		size_t minimumVertexCount;
		if ((not CheckedMultiply(profileSegmentCount, (fullSegmentVertexCount - 1), minimumVertexCount))
			|| (Mesh3D::MaxVertexCount < minimumVertexCount))
		{
			return{};
		}

		Array<ProfileSegment> profileSegments;
		profileSegments.reserve(profileSegmentCount);

		Float2 firstPoint;
		if (not ToProfilePoint(profile.front(), firstPoint))
		{
			return{};
		}

		Float2 current = firstPoint;
		double totalDistance = 0.0;
		double twiceArea = 0.0;
		size_t vertexCount = 0;
		size_t trianglesPerSlice = 0;

		for (size_t i = 0; i < profileSegmentCount; ++i)
		{
			Float2 next;
			if (not ToProfilePoint(profile[i + 1], next))
			{
				return{};
			}

			const bool startOnAxis = (current.x == 0.0f);
			const bool endOnAxis = (next.x == 0.0f);
			if (startOnAxis && endOnAxis)
			{
				return{};
			}

			const double dr = (static_cast<double>(next.x) - current.x);
			const double dy = (static_cast<double>(next.y) - current.y);
			const double length = std::hypot(dr, dy);
			if ((not std::isfinite(length))
				|| (length == 0.0))
			{
				return{};
			}

			const double endDistance = (totalDistance + length);
			if (not std::isfinite(endDistance))
			{
				return{};
			}

			const bool touchesAxis = (startOnAxis || endOnAxis);
			const size_t segmentVertexCount = (fullSegmentVertexCount - (touchesAxis ? 1 : 0));
			if ((not CheckedAdd(vertexCount, segmentVertexCount, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| (not CheckedAdd(trianglesPerSlice, (touchesAxis ? 1 : 2), trianglesPerSlice)))
			{
				return{};
			}

			profileSegments.push_back(ProfileSegment{
				.start = current,
				.end = next,
				.normal = Float2{
					static_cast<float>(dy / length), static_cast<float>(-dr / length)
				},
				.startDistance = totalDistance,
				.endDistance = endDistance
			});

			if (closedProfile)
			{
				twiceArea += ((static_cast<double>(current.x) * next.y)
					- (static_cast<double>(next.x) * current.y));
			}
			totalDistance = endDistance;
			current = next;
		}

		if ((not closedProfile) && (current == firstPoint))
		{
			return{};
		}

		if (closedProfile
			&& ((not std::isfinite(twiceArea)) || (twiceArea == 0.0)))
		{
			return{};
		}

		size_t triangleCount;
		if (not CheckedMultiply(trianglesPerSlice, static_cast<size_t>(segments), triangleCount))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		Array<CircleSample> circle(ringStride);
		Array<CircleSample> middleCircle(segments);
		const float angleStep = (Math::TwoPiF / static_cast<float>(segments));
		for (uint32 i = 0; i < segments; ++i)
		{
			const float angle = (angleStep * i);
			const float middleAngle = (angleStep * (i + 0.5f));
			circle[i] = CircleSample{ std::sin(angle), std::cos(angle) };
			middleCircle[i] = CircleSample{ std::sin(middleAngle), std::cos(middleAngle) };
		}
		circle[segments] = circle[0];

		const bool smoothProfile = (0.0 < smoothingAngle);
		const float minNormalDot = (smoothProfile
			? std::cos(static_cast<float>(smoothingAngle))
			: 1.0f);
		const float inverseSegments = (1.0f / static_cast<float>(segments));
		const double inverseTotalDistance = (1.0 / totalDistance);
		TriangleIndex32* pTriangle = mesh.indices.data();
		size_t vertexOffset = 0;

		for (size_t profileIndex = 0; profileIndex < profileSegmentCount; ++profileIndex)
		{
			const ProfileSegment& segment = profileSegments[profileIndex];
			Float2 startNormal = segment.normal;
			Float2 endNormal = segment.normal;

			if (smoothProfile)
			{
				if ((0 < profileIndex) || closedProfile)
				{
					const size_t previousIndex = ((profileIndex == 0)
						? (profileSegmentCount - 1)
						: (profileIndex - 1));
					startNormal = SmoothProfileNormal(
						profileSegments[previousIndex].normal,
						segment.normal,
						segment.normal,
						minNormalDot);
				}

				if (((profileIndex + 1) < profileSegmentCount) || closedProfile)
				{
					const size_t nextIndex = (((profileIndex + 1) == profileSegmentCount)
						? 0
						: (profileIndex + 1));
					endNormal = SmoothProfileNormal(
						segment.normal,
						profileSegments[nextIndex].normal,
						segment.normal,
						minNormalDot);
				}
			}

			const float v0 = static_cast<float>(segment.startDistance * inverseTotalDistance);
			const float v1 = ((profileIndex + 1 == profileSegmentCount)
				? 1.0f
				: static_cast<float>(segment.endDistance * inverseTotalDistance));
			const bool startOnAxis = (segment.start.x == 0.0f);
			const bool endOnAxis = (segment.end.x == 0.0f);

			if (startOnAxis)
			{
				const size_t axisBase = vertexOffset;
				const size_t ringBase = (axisBase + segments);
				for (uint32 i = 0; i < segments; ++i)
				{
					mesh.vertices[axisBase + i] = MakeVertex(
						segment.start, startNormal, middleCircle[i], ((i + 0.5f) * inverseSegments), v0);
				}

				for (uint32 i = 0; i <= segments; ++i)
				{
					mesh.vertices[ringBase + i] = MakeVertex(
						segment.end, endNormal, circle[i], (i * inverseSegments), v1);
				}

				for (uint32 i = 0; i < segments; ++i)
				{
					*pTriangle++ = TriangleIndex32{
						static_cast<uint32>(axisBase + i),
						static_cast<uint32>(ringBase + i),
						static_cast<uint32>(ringBase + i + 1)
					};
				}
			}
			else if (endOnAxis)
			{
				const size_t ringBase = vertexOffset;
				const size_t axisBase = (ringBase + ringStride);
				for (uint32 i = 0; i <= segments; ++i)
				{
					mesh.vertices[ringBase + i] = MakeVertex(
						segment.start, startNormal, circle[i], (i * inverseSegments), v0);
				}

				for (uint32 i = 0; i < segments; ++i)
				{
					mesh.vertices[axisBase + i] = MakeVertex(
						segment.end, endNormal, middleCircle[i], ((i + 0.5f) * inverseSegments), v1);
				}

				for (uint32 i = 0; i < segments; ++i)
				{
					*pTriangle++ = TriangleIndex32{
						static_cast<uint32>(ringBase + i),
						static_cast<uint32>(axisBase + i),
						static_cast<uint32>(ringBase + i + 1)
					};
				}
			}
			else
			{
				const size_t startRingBase = vertexOffset;
				const size_t endRingBase = (startRingBase + ringStride);
				for (uint32 i = 0; i <= segments; ++i)
				{
					const float u = (i * inverseSegments);
					mesh.vertices[startRingBase + i] = MakeVertex(
						segment.start, startNormal, circle[i], u, v0);
					mesh.vertices[endRingBase + i] = MakeVertex(
						segment.end, endNormal, circle[i], u, v1);
				}

				for (uint32 i = 0; i < segments; ++i)
				{
					const uint32 startCurrent = static_cast<uint32>(startRingBase + i);
					const uint32 startNext = (startCurrent + 1);
					const uint32 endCurrent = static_cast<uint32>(endRingBase + i);
					const uint32 endNext = (endCurrent + 1);
					*pTriangle++ = TriangleIndex32{ startCurrent, endCurrent, startNext };
					*pTriangle++ = TriangleIndex32{ startNext, endCurrent, endNext };
				}
			}

			vertexOffset += (fullSegmentVertexCount - ((startOnAxis || endOnAxis) ? 1 : 0));
		}

		return mesh;
	}
}
