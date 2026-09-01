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
# include <algorithm>
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
		static bool IsFloatRepresentable(const Vec2 value) noexcept
		{
			return (IsFloatRepresentable(value.x)
				&& IsFloatRepresentable(value.y));
		}

		[[nodiscard]]
		static bool IsFloatRepresentable(const Vec3 value) noexcept
		{
			return (IsFloatRepresentable(value.x)
				&& IsFloatRepresentable(value.y)
				&& IsFloatRepresentable(value.z));
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

		struct PathFrame
		{
			Vec3 tangent;
			Vec3 normal;
			Vec3 binormal;
		};

		struct CircleSample
		{
			double sin;
			double cos;
		};

		[[nodiscard]]
		static Vec3 MakeInitialNormal(const Vec3 tangent) noexcept
		{
			const Vec3 absTangent{ std::abs(tangent.x), std::abs(tangent.y), std::abs(tangent.z) };
			Vec3 reference;

			if ((absTangent.x <= absTangent.y)
				&& (absTangent.x <= absTangent.z))
			{
				reference = Vec3{ 1.0, 0.0, 0.0 };
			}
			else if (absTangent.y <= absTangent.z)
			{
				reference = Vec3{ 0.0, 1.0, 0.0 };
			}
			else
			{
				reference = Vec3{ 0.0, 0.0, 1.0 };
			}

			return (reference - (tangent * reference.dot(tangent))).normalized();
		}

		[[nodiscard]]
		static bool TransportNormal(
			const Vec3 previousTangent,
			const Vec3 tangent,
			const Vec3 previousNormal,
			Vec3& normal) noexcept
		{
			const Vec3 rotationAxis = previousTangent.cross(tangent);
			const double axisLengthSq = rotationAxis.lengthSq();
			const double cosine = std::clamp(previousTangent.dot(tangent), -1.0, 1.0);

			if (axisLengthSq == 0.0)
			{
				if (cosine <= 0.0)
				{
					return false;
				}

				normal = previousNormal;
				return true;
			}

			const double sine = std::sqrt(axisLengthSq);
			const Vec3 axis = (rotationAxis / sine);
			normal = ((previousNormal * cosine)
				+ (axis.cross(previousNormal) * sine)
				+ (axis * (axis.dot(previousNormal) * (1.0 - cosine))));
			normal -= (tangent * normal.dot(tangent));

			const double normalLengthSq = normal.lengthSq();
			if ((not std::isfinite(normalLengthSq))
				|| (normalLengthSq == 0.0))
			{
				return false;
			}

			normal /= std::sqrt(normalLengthSq);
			return true;
		}

		[[nodiscard]]
		static bool ToFloat3(const Vec3 value, Float3& result) noexcept
		{
			if (not IsFloatRepresentable(value))
			{
				return false;
			}

			result = value;
			return (std::isfinite(result.x)
				&& std::isfinite(result.y)
				&& std::isfinite(result.z));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Tube
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Tube(
		const std::span<const Vec3> path,
		const double _radius,
		const uint32 sides,
		const Vec2 _uvScale,
		const Vec2 _uvOffset)
	{
		if ((path.size() < 2)
			|| (sides < 3)
			|| (not IsFloatRepresentable(_radius))
			|| (not IsFloatRepresentable(_uvScale))
			|| (not IsFloatRepresentable(_uvOffset)))
		{
			return{};
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return{};
		}

		size_t ringStride;
		size_t pathAndCapsCount;
		size_t vertexCount;
		size_t twiceSides;
		size_t triangleCount;
		if ((not CheckedAdd(static_cast<size_t>(sides), 1, ringStride))
			|| (not CheckedAdd(path.size(), 2, pathAndCapsCount))
			|| (not CheckedMultiply(pathAndCapsCount, ringStride, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(sides), 2, twiceSides))
			|| (not CheckedMultiply(path.size(), twiceSides, triangleCount)))
		{
			return{};
		}

		Array<Float3> points(path.size());
		constexpr double MaxFloat = std::numeric_limits<float>::max();
		for (size_t i = 0; i < path.size(); ++i)
		{
			if ((not ToFloat3(path[i], points[i]))
				|| ((MaxFloat - std::abs(path[i].x)) < radius)
				|| ((MaxFloat - std::abs(path[i].y)) < radius)
				|| ((MaxFloat - std::abs(path[i].z)) < radius))
			{
				return{};
			}

			if ((0 < i) && (points[i] == points[i - 1]))
			{
				return{};
			}
		}

		if (points.front() == points.back())
		{
			return{};
		}

		const size_t pathSegmentCount = (path.size() - 1);
		Array<Vec3> segmentTangents(pathSegmentCount);
		Array<double> distances(path.size(), 0.0);
		for (size_t i = 0; i < pathSegmentCount; ++i)
		{
			const Vec3 delta = (Vec3{ points[i + 1] } - Vec3{ points[i] });
			const double length = delta.length();
			const double nextDistance = (distances[i] + length);
			if ((not std::isfinite(length))
				|| (length == 0.0)
				|| (not std::isfinite(nextDistance)))
			{
				return{};
			}

			segmentTangents[i] = (delta / length);
			distances[i + 1] = nextDistance;
		}

		const double sideU0 = _uvOffset.x;
		const double sideU1 = (_uvOffset.x + _uvScale.x);
		const double sideV0 = _uvOffset.y;
		const double sideV1 = (_uvOffset.y + (_uvScale.y * distances.back()));
		const double capV1 = (_uvOffset.y + _uvScale.y);
		if ((not IsFloatRepresentable(sideU0))
			|| (not IsFloatRepresentable(sideU1))
			|| (not IsFloatRepresentable(sideV0))
			|| (not IsFloatRepresentable(sideV1))
			|| (not IsFloatRepresentable(capV1)))
		{
			return{};
		}

		Array<PathFrame> frames(path.size());
		frames.front().tangent = segmentTangents.front();
		frames.front().normal = MakeInitialNormal(frames.front().tangent);
		frames.front().binormal = frames.front().normal.cross(frames.front().tangent);

		for (size_t i = 1; i < path.size(); ++i)
		{
			if ((i + 1) == path.size())
			{
				frames[i].tangent = segmentTangents.back();
			}
			else
			{
				const Vec3 tangentSum = (segmentTangents[i - 1] + segmentTangents[i]);
				const double tangentLengthSq = tangentSum.lengthSq();
				if ((not std::isfinite(tangentLengthSq))
					|| (tangentLengthSq == 0.0))
				{
					return{};
				}

				frames[i].tangent = (tangentSum / std::sqrt(tangentLengthSq));
			}

			if (not TransportNormal(
				frames[i - 1].tangent,
				frames[i].tangent,
				frames[i - 1].normal,
				frames[i].normal))
			{
				return{};
			}

			frames[i].binormal = frames[i].normal.cross(frames[i].tangent);
		}

		Array<CircleSample> circle(ringStride);
		const double angleStep = (Math::TwoPi / sides);
		for (uint32 i = 0; i < sides; ++i)
		{
			const double angle = (angleStep * i);
			circle[i] = CircleSample{ std::sin(angle), std::cos(angle) };
		}
		circle[sides] = circle[0];

		const Float2 uvScale = _uvScale;
		const Float2 uvOffset = _uvOffset;
		const float inverseSides = (1.0f / sides);
		Mesh3D mesh{ vertexCount, triangleCount };

		for (size_t pathIndex = 0; pathIndex < path.size(); ++pathIndex)
		{
			const PathFrame& frame = frames[pathIndex];
			const Vec3 center = points[pathIndex];
			const float v = static_cast<float>(
				(_uvOffset.y + (_uvScale.y * distances[pathIndex])));
			const size_t ringBase = (pathIndex * ringStride);

			for (uint32 sideIndex = 0; sideIndex <= sides; ++sideIndex)
			{
				const CircleSample sample = circle[sideIndex];
				const Vec3 normal = ((frame.normal * sample.cos)
					+ (frame.binormal * sample.sin));
				const Vec3 tangent = ((frame.normal * -sample.sin)
					+ (frame.binormal * sample.cos));
				Float3 position;
				if (not ToFloat3((center + (normal * radius)), position))
				{
					return{};
				}

				mesh.vertices[ringBase + sideIndex] = Vertex3D{
					.pos = position,
					.normal = normal,
					.tex = Float2{
						(uvOffset.x + (uvScale.x * (sideIndex * inverseSides))), v
					},
					.tangent = Float4{
						static_cast<float>(tangent.x),
						static_cast<float>(tangent.y),
						static_cast<float>(tangent.z),
						-1.0f
					}
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (size_t pathIndex = 0; pathIndex < pathSegmentCount; ++pathIndex)
		{
			const size_t ringBase = (pathIndex * ringStride);
			const size_t nextRingBase = (ringBase + ringStride);
			for (uint32 sideIndex = 0; sideIndex < sides; ++sideIndex)
			{
				const uint32 i0 = static_cast<uint32>(ringBase + sideIndex);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(nextRingBase + sideIndex);
				const uint32 i3 = (i2 + 1);
				*pTriangle++ = TriangleIndex32{ i0, i2, i1 };
				*pTriangle++ = TriangleIndex32{ i1, i2, i3 };
			}
		}

		const size_t startCapBase = (path.size() * ringStride);
		const size_t endCapBase = (startCapBase + ringStride);
		const Float2 capCenterUV = (uvOffset + (uvScale * 0.5f));
		for (size_t capIndex = 0; capIndex < 2; ++capIndex)
		{
			const bool startCap = (capIndex == 0);
			const size_t pathIndex = (startCap ? 0 : (path.size() - 1));
			const size_t capBase = (startCap ? startCapBase : endCapBase);
			const PathFrame& frame = frames[pathIndex];
			const Float3 capNormal = (startCap
				? Float3{ -frame.tangent }
				: Float3{ frame.tangent });
			const Float4 capTangent{
				static_cast<float>(frame.normal.x),
				static_cast<float>(frame.normal.y),
				static_cast<float>(frame.normal.z),
				(startCap ? 1.0f : -1.0f)
			};

			mesh.vertices[capBase] = Vertex3D{
				.pos = points[pathIndex],
				.normal = capNormal,
				.tex = capCenterUV,
				.tangent = capTangent
			};

			for (uint32 sideIndex = 0; sideIndex < sides; ++sideIndex)
			{
				const CircleSample sample = circle[sideIndex];
				const Vec3 radial = ((frame.normal * sample.cos)
					+ (frame.binormal * sample.sin));
				Float3 position;
				if (not ToFloat3((Vec3{ points[pathIndex] } + (radial * radius)), position))
				{
					return{};
				}

				mesh.vertices[capBase + 1 + sideIndex] = Vertex3D{
					.pos = position,
					.normal = capNormal,
					.tex = Float2{
						(uvOffset.x + (uvScale.x * static_cast<float>(0.5 + (0.5 * sample.cos)))),
						(uvOffset.y + (uvScale.y * static_cast<float>(0.5 + (0.5 * sample.sin))))
					},
					.tangent = capTangent
				};
			}

			for (uint32 sideIndex = 0; sideIndex < sides; ++sideIndex)
			{
				const uint32 current = static_cast<uint32>(capBase + 1 + sideIndex);
				const uint32 next = static_cast<uint32>(capBase + 1 + ((sideIndex + 1) % sides));
				if (startCap)
				{
					*pTriangle++ = TriangleIndex32{ static_cast<uint32>(capBase), current, next };
				}
				else
				{
					*pTriangle++ = TriangleIndex32{ static_cast<uint32>(capBase), next, current };
				}
			}
		}

		return mesh;
	}
}
