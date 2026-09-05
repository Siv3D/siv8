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
# include <Siv3D/Mesh3DBuilder.hpp>
# include <Siv3D/MathConstants.hpp>
# include <Siv3D/Polygon.hpp>
# include "Mesh3DCommon.hpp"
# include <algorithm>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::AddedRange;
		using Mesh3DDetail::AdditionFailed;
		using Mesh3DDetail::CapValidationResult;
		using Mesh3DDetail::ForEachValidCapTriangle;
		using Mesh3DDetail::IsFloatRepresentable;
		using Mesh3DDetail::RingValidationResult;
		using Mesh3DDetail::ValidateCapTriangles;
		using Mesh3DDetail::ValidateRing;

		struct PathFrame
		{
			Vec3 tangent;
			Vec3 normal;
			Vec3 binormal;
		};

		struct PathData
		{
			Array<Float3> points;
			Array<double> distances;
			Array<PathFrame> frames;
		};

		enum class PathDataStatus
		{
			Success,
			InvalidArgument,
			InvalidGeometry,
			NumericRange,
			SizeLimit,
		};

		[[nodiscard]]
		static constexpr Mesh3DErrorCode ToErrorCode(const PathDataStatus status) noexcept
		{
			switch (status)
			{
			case PathDataStatus::InvalidArgument:
				return Mesh3DErrorCode::InvalidArgument;
			case PathDataStatus::InvalidGeometry:
				return Mesh3DErrorCode::InvalidGeometry;
			case PathDataStatus::NumericRange:
				return Mesh3DErrorCode::NumericRange;
			case PathDataStatus::SizeLimit:
				return Mesh3DErrorCode::SizeLimit;
			default:
				return Mesh3DErrorCode::InvalidGeometry;
			}
		}

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

		[[nodiscard]]
		static PathDataStatus MakePathData(
			const std::span<const Vec3> path,
			const double maxDistanceFromPath,
			const Vec3* const initialXAxis,
			const CloseRing closeRing,
			PathData& result)
		{
			const bool isClosed = (closeRing == CloseRing::Yes);
			const size_t minimumPointCount = (isClosed ? 3 : 2);
			size_t stationCount;
			if ((path.size() < minimumPointCount)
				|| (maxDistanceFromPath < 0.0))
			{
				return PathDataStatus::InvalidArgument;
			}

			if (not IsFloatRepresentable(maxDistanceFromPath))
			{
				return PathDataStatus::NumericRange;
			}

			if (not CheckedAdd(path.size(), (isClosed ? 1 : 0), stationCount))
			{
				return PathDataStatus::SizeLimit;
			}

			result.points.reserve(stationCount);
			result.points.resize(path.size());
			constexpr double MaxFloat = std::numeric_limits<float>::max();
			for (size_t i = 0; i < path.size(); ++i)
			{
				if ((not ToFloat3(path[i], result.points[i]))
					|| ((MaxFloat - std::abs(path[i].x)) < maxDistanceFromPath)
					|| ((MaxFloat - std::abs(path[i].y)) < maxDistanceFromPath)
					|| ((MaxFloat - std::abs(path[i].z)) < maxDistanceFromPath))
				{
					return PathDataStatus::NumericRange;
				}

				if ((0 < i) && (result.points[i] == result.points[i - 1]))
				{
					return PathDataStatus::InvalidGeometry;
				}
			}

			if (result.points.front() == result.points.back())
			{
				return PathDataStatus::InvalidGeometry;
			}

			const size_t pathSegmentCount = (isClosed ? path.size() : (path.size() - 1));
			Array<Vec3> segmentTangents(pathSegmentCount);
			result.distances.assign(stationCount, 0.0);
			for (size_t i = 0; i < pathSegmentCount; ++i)
			{
				const size_t nextIndex = ((i + 1) % path.size());
				const Vec3 delta = (Vec3{ result.points[nextIndex] } - Vec3{ result.points[i] });
				const double length = delta.length();
				const double nextDistance = (result.distances[i] + length);
				if ((not std::isfinite(length))
					|| (not std::isfinite(nextDistance)))
				{
					return PathDataStatus::NumericRange;
				}

				if (length == 0.0)
				{
					return PathDataStatus::InvalidGeometry;
				}

				segmentTangents[i] = (delta / length);
				result.distances[i + 1] = nextDistance;
			}

			result.frames.resize(stationCount);
			if (isClosed)
			{
				const Vec3 tangentSum = (segmentTangents.back() + segmentTangents.front());
				const double tangentLengthSq = tangentSum.lengthSq();
				if ((not std::isfinite(tangentLengthSq))
					|| (tangentLengthSq == 0.0))
				{
					return PathDataStatus::InvalidGeometry;
				}

				result.frames.front().tangent = (tangentSum / std::sqrt(tangentLengthSq));
			}
			else
			{
				result.frames.front().tangent = segmentTangents.front();
			}

			if (initialXAxis)
			{
				if (not IsFloatRepresentable(*initialXAxis))
				{
					return PathDataStatus::NumericRange;
				}

				result.frames.front().normal = (*initialXAxis
					- (result.frames.front().tangent
						* initialXAxis->dot(result.frames.front().tangent)));
				const double normalLengthSq = result.frames.front().normal.lengthSq();
				if ((not std::isfinite(normalLengthSq))
					|| (normalLengthSq == 0.0))
				{
					return PathDataStatus::InvalidGeometry;
				}

				result.frames.front().normal /= std::sqrt(normalLengthSq);
			}
			else
			{
				result.frames.front().normal = MakeInitialNormal(result.frames.front().tangent);
			}
			result.frames.front().binormal = (
				result.frames.front().normal.cross(result.frames.front().tangent));

			for (size_t i = 1; i < path.size(); ++i)
			{
				if ((not isClosed) && ((i + 1) == path.size()))
				{
					result.frames[i].tangent = segmentTangents.back();
				}
				else
				{
					const Vec3 tangentSum = (segmentTangents[i - 1] + segmentTangents[i]);
					const double tangentLengthSq = tangentSum.lengthSq();
					if ((not std::isfinite(tangentLengthSq))
						|| (tangentLengthSq == 0.0))
					{
						return PathDataStatus::InvalidGeometry;
					}

					result.frames[i].tangent = (tangentSum / std::sqrt(tangentLengthSq));
				}

				if (not TransportNormal(
					result.frames[i - 1].tangent,
					result.frames[i].tangent,
					result.frames[i - 1].normal,
					result.frames[i].normal))
				{
					return PathDataStatus::InvalidGeometry;
				}

				result.frames[i].binormal = (
					result.frames[i].normal.cross(result.frames[i].tangent));
			}

			if (isClosed)
			{
				// Parallel transport around a loop generally leaves a residual rotation.
				Vec3 closureNormal;
				if (not TransportNormal(
					result.frames[path.size() - 1].tangent,
					result.frames.front().tangent,
					result.frames[path.size() - 1].normal,
					closureNormal))
				{
					return PathDataStatus::InvalidGeometry;
				}

				const Vec3& initialNormal = result.frames.front().normal;
				const Vec3& initialTangent = result.frames.front().tangent;
				const double correctionAngle = std::atan2(
					initialTangent.dot(closureNormal.cross(initialNormal)),
					std::clamp(closureNormal.dot(initialNormal), -1.0, 1.0));
				const double totalLength = result.distances.back();

				// Distribute the inverse holonomy by arc length and keep the seam frame exact.
				for (size_t i = 1; i < path.size(); ++i)
				{
					PathFrame& frame = result.frames[i];
					const double angle = (correctionAngle * (result.distances[i] / totalLength));
					const double cosine = std::cos(angle);
					const double sine = std::sin(angle);
					frame.normal = ((frame.normal * cosine) - (frame.binormal * sine));
					frame.binormal = frame.normal.cross(frame.tangent);
				}

				result.points.push_back(result.points.front());
				result.frames.back() = result.frames.front();
			}

			return PathDataStatus::Success;
		}

	}

	////////////////////////////////////////////////////////////////
	//
	//	Tube
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Tube(
		const std::initializer_list<Vec3> path,
		const double radius,
		const TubeOptions& options)
	{
		return Tube(std::span<const Vec3>{ path.begin(), path.size() }, radius, options);
	}

	Mesh3D Mesh3D::Tube(
		const std::initializer_list<Vec3> path,
		const std::initializer_list<double> radii,
		const TubeOptions& options)
	{
		return Tube(
			std::span<const Vec3>{ path.begin(), path.size() },
			std::span<const double>{ radii.begin(), radii.size() },
			options);
	}

	namespace
	{
		struct ConstantTubeRadii
		{
			static constexpr bool PerPoint = false;
			double value;

			[[nodiscard]]
			double operator [](const size_t) const noexcept
			{
				return value;
			}
		};

		struct PerPointTubeRadii
		{
			static constexpr bool PerPoint = true;
			std::span<const double> values;

			[[nodiscard]]
			double operator [](const size_t index) const noexcept
			{
				return values[index];
			}
		};

		template <class Radii>
		[[nodiscard]]
		static Mesh3DAddResult AppendTubeImpl(
			Mesh3D& mesh,
			const std::span<const Vec3> path,
			const Radii& radii,
			const uint32 sides,
			const Vec2 _uvScale,
			const Vec2 _uvOffset,
			const CloseRing closeRing,
			const Optional<Mesh3DEndCaps> requestedEndCaps)
		{
			if constexpr (Radii::PerPoint)
			{
				if (radii.values.size() != path.size())
				{
					return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
						U"Mesh3D::Tube(): The radius count must match the path point count");
				}
			}

			const bool isClosed = (closeRing == CloseRing::Yes);
			const Mesh3DEndCaps endCaps = requestedEndCaps.value_or(
				isClosed ? Mesh3DEndCaps::None : Mesh3DEndCaps::Both);
			if (isClosed && (endCaps != Mesh3DEndCaps::None))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
					U"Mesh3D::Tube(): A closed path cannot have end caps");
			}
			const bool generateStartCap = ((endCaps == Mesh3DEndCaps::Start)
				|| (endCaps == Mesh3DEndCaps::Both));
			const bool generateEndCap = ((endCaps == Mesh3DEndCaps::End)
				|| (endCaps == Mesh3DEndCaps::Both));
			const size_t capCount = (static_cast<size_t>(generateStartCap)
				+ static_cast<size_t>(generateEndCap));
			if ((path.size() < (isClosed ? 3 : 2))
				|| (sides < 3))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
					U"Mesh3D::Tube(): The path or side count is invalid");
			}

			if ((not IsFloatRepresentable(_uvScale))
				|| (not IsFloatRepresentable(_uvOffset)))
			{
				return AdditionFailed(Mesh3DErrorCode::NumericRange,
					U"Mesh3D::Tube(): A numeric parameter is non-finite or outside the float range");
			}

			float maxRadius = 0.0f;
			if constexpr (Radii::PerPoint)
			{
				for (const double sourceRadius : radii.values)
				{
					if (not IsFloatRepresentable(sourceRadius))
					{
						return AdditionFailed(Mesh3DErrorCode::NumericRange,
							U"Mesh3D::Tube(): A radius is non-finite or outside the float range");
					}

					const float radius = static_cast<float>(sourceRadius);
					if (radius <= 0.0f)
					{
						return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
							U"Mesh3D::Tube(): Every radius must be positive after conversion to float");
					}

					maxRadius = std::max(maxRadius, radius);
				}
			}
			else
			{
				if (not IsFloatRepresentable(radii.value))
				{
					return AdditionFailed(Mesh3DErrorCode::NumericRange,
						U"Mesh3D::Tube(): A radius is non-finite or outside the float range");
				}

				maxRadius = static_cast<float>(radii.value);
				if (maxRadius <= 0.0f)
				{
					return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
						U"Mesh3D::Tube(): radius must be positive after conversion to float");
				}
			}

			size_t ringStride;
			size_t pathAndCapsCount;
			size_t vertexCount;
			size_t twiceSides;
			size_t sideTriangleCount;
			size_t capTriangleCount;
			size_t triangleCount;
			size_t stationCount;
			const size_t pathSegmentCount = (isClosed ? path.size() : (path.size() - 1));
			if ((not CheckedAdd(path.size(), (isClosed ? 1 : 0), stationCount))
				|| (not CheckedAdd(static_cast<size_t>(sides), 1, ringStride))
				|| (not CheckedAdd(stationCount, capCount, pathAndCapsCount))
				|| (not CheckedMultiply(pathAndCapsCount, ringStride, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| (not CheckedMultiply(static_cast<size_t>(sides), 2, twiceSides))
				|| (not CheckedMultiply(pathSegmentCount, twiceSides, sideTriangleCount))
				|| (not CheckedMultiply(capCount, static_cast<size_t>(sides), capTriangleCount))
				|| (not CheckedAdd(sideTriangleCount, capTriangleCount, triangleCount)))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit,
					U"Mesh3D::Tube(): The generated mesh exceeds the supported size");
			}

			PathData pathData;
			const PathDataStatus pathDataStatus = MakePathData(
				path, maxRadius, nullptr, closeRing, pathData);
			if (pathDataStatus != PathDataStatus::Success)
			{
				return AdditionFailed(ToErrorCode(pathDataStatus),
					U"Mesh3D::Tube(): The path is invalid or cannot produce stable frames");
			}

			const auto& points = pathData.points;
			const auto& distances = pathData.distances;
			const auto& frames = pathData.frames;

			const double sideU0 = _uvOffset.x;
			const double sideU1 = (_uvOffset.x + _uvScale.x);
			const double sideV0 = _uvOffset.y;
			const double sideV1 = (_uvOffset.y + (_uvScale.y * distances.back()));
			const double capV1 = (_uvOffset.y + _uvScale.y);
			if ((not IsFloatRepresentable(sideU0))
				|| (not IsFloatRepresentable(sideU1))
				|| (not IsFloatRepresentable(sideV0))
				|| (not IsFloatRepresentable(sideV1))
				|| ((0 < capCount) && (not IsFloatRepresentable(capV1))))
			{
				return AdditionFailed(Mesh3DErrorCode::NumericRange,
					U"Mesh3D::Tube(): The generated UV coordinates exceed the float range");
			}

			const Array<Mesh3DDetail::CircleSample<double>> circle =
				Mesh3DDetail::MakeCircleSamples<double>(sides);

			const Float2 uvScale = _uvScale;
			const Float2 uvOffset = _uvOffset;
			const float inverseSides = (1.0f / sides);
			const size_t vertexBase = mesh.vertices.size();
			const size_t triangleBase = mesh.indices.size();
			size_t newVertexCount;
			size_t newTriangleCount;
			if ((not CheckedAdd(vertexBase, vertexCount, newVertexCount))
				|| (Mesh3D::MaxVertexCount < newVertexCount)
				|| (not CheckedAdd(triangleBase, triangleCount, newTriangleCount)))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit,
					U"Mesh3D::Tube(): The generated mesh exceeds the supported size");
			}

			mesh.vertices.resize(newVertexCount);
			mesh.indices.resize(newTriangleCount);
			const auto generationFailed = [&](const StringView message)
			{
				mesh.vertices.resize(vertexBase);
				mesh.indices.resize(triangleBase);
				return AdditionFailed(Mesh3DErrorCode::NumericRange, message);
			};

			for (size_t pathIndex = 0; pathIndex < stationCount; ++pathIndex)
			{
				const PathFrame& frame = frames[pathIndex];
				const Vec3 center = points[pathIndex];
				float radius;
				double radiusSlope = 0.0;
				double sideNormalScale = 1.0;
				if constexpr (Radii::PerPoint)
				{
					const size_t sourceIndex = (pathIndex % path.size());
					radius = static_cast<float>(radii[sourceIndex]);
					if (isClosed)
					{
						const size_t previousIndex = ((sourceIndex + path.size() - 1) % path.size());
						const size_t nextIndex = ((sourceIndex + 1) % path.size());
						const size_t previousSegmentIndex = previousIndex;
						const size_t nextSegmentIndex = sourceIndex;
						const double adjacentLength = (
							(distances[previousSegmentIndex + 1] - distances[previousSegmentIndex])
							+ (distances[nextSegmentIndex + 1] - distances[nextSegmentIndex]));
						radiusSlope = (
							(static_cast<float>(radii[nextIndex]) - static_cast<float>(radii[previousIndex]))
							/ adjacentLength);
					}
					else if (sourceIndex == 0)
					{
						radiusSlope = (
							(static_cast<float>(radii[1]) - static_cast<float>(radii[0]))
							/ (distances[1] - distances[0]));
					}
					else if ((sourceIndex + 1) == path.size())
					{
						radiusSlope = (
							(static_cast<float>(radii[sourceIndex]) - static_cast<float>(radii[sourceIndex - 1]))
							/ (distances[sourceIndex] - distances[sourceIndex - 1]));
					}
					else
					{
						radiusSlope = (
							(static_cast<float>(radii[sourceIndex + 1]) - static_cast<float>(radii[sourceIndex - 1]))
							/ (distances[sourceIndex + 1] - distances[sourceIndex - 1]));
					}

					sideNormalScale = (1.0 / std::sqrt(1.0 + (radiusSlope * radiusSlope)));
				}
				else
				{
					radius = static_cast<float>(radii[pathIndex]);
				}
				const float v = static_cast<float>(
					(_uvOffset.y + (_uvScale.y * distances[pathIndex])));
				const size_t ringBase = (vertexBase + pathIndex * ringStride);

				for (uint32 sideIndex = 0; sideIndex <= sides; ++sideIndex)
				{
					const Mesh3DDetail::CircleSample<double> sample = circle[sideIndex];
					const Vec3 radial = ((frame.normal * sample.cos)
						+ (frame.binormal * sample.sin));
					Vec3 normal = radial;
					if constexpr (Radii::PerPoint)
					{
						normal = ((radial - (frame.tangent * radiusSlope)) * sideNormalScale);
					}
					const Vec3 tangent = ((frame.normal * -sample.sin)
						+ (frame.binormal * sample.cos));
					Float3 position;
					if (not ToFloat3((center + (radial * radius)), position))
					{
						return generationFailed(U"Mesh3D::Tube(): A generated side vertex exceeds the float range");
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

			TriangleIndex32* pTriangle = (mesh.indices.data() + triangleBase);
			for (size_t pathIndex = 0; pathIndex < pathSegmentCount; ++pathIndex)
			{
				const size_t ringBase = (vertexBase + pathIndex * ringStride);
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

			if (0 < capCount)
			{
				const size_t firstCapBase = (vertexBase + stationCount * ringStride);
				const Float2 capCenterUV = (uvOffset + (uvScale * 0.5f));
				size_t writtenCapCount = 0;
				for (size_t capIndex = 0; capIndex < 2; ++capIndex)
				{
					const bool startCap = (capIndex == 0);
					if ((startCap && (not generateStartCap))
						|| ((not startCap) && (not generateEndCap)))
					{
						continue;
					}
					const size_t pathIndex = (startCap ? 0 : (path.size() - 1));
					const float radius = static_cast<float>(radii[pathIndex]);
					const size_t capBase = (firstCapBase + (writtenCapCount++ * ringStride));
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
						const Mesh3DDetail::CircleSample<double> sample = circle[sideIndex];
						const Vec3 radial = ((frame.normal * sample.cos)
							+ (frame.binormal * sample.sin));
						Float3 position;
						if (not ToFloat3((Vec3{ points[pathIndex] } + (radial * radius)), position))
						{
							return generationFailed(U"Mesh3D::Tube(): A generated cap vertex exceeds the float range");
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
			}

			return AddedRange(mesh, vertexBase, triangleBase);
		}
	}

	Mesh3DAddResult Mesh3DDetail::AppendTube(
		Mesh3D& mesh,
		const std::span<const Vec3> path,
		const double radius,
		const TubeOptions& options)
	{
		return AppendTubeImpl(
			mesh, path, ConstantTubeRadii{ radius }, options.sides,
			options.uvScale, options.uvOffset, options.closeRing, options.endCaps);
	}

	Mesh3DAddResult Mesh3DDetail::AppendTube(
		Mesh3D& mesh,
		const std::span<const Vec3> path,
		const std::span<const double> radii,
		const TubeOptions& options)
	{
		return AppendTubeImpl(
			mesh, path, PerPointTubeRadii{ radii }, options.sides,
			options.uvScale, options.uvOffset, options.closeRing, options.endCaps);
	}

	Mesh3D Mesh3D::Tube(
		const std::span<const Vec3> path,
		const double radius,
		const TubeOptions& options)
	{
		Mesh3DBuilder builder;
		(void)builder.addTube(path, radius, options);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Tube(
		const std::span<const Vec3> path,
		const std::span<const double> radii,
		const TubeOptions& options)
	{
		Mesh3DBuilder builder;
		(void)builder.addTube(path, radii, options);
		return std::move(builder).build();
	}

	namespace
	{
		struct ConstantSweepSectionTransforms
		{
			static constexpr bool PerPoint = false;
		};

		struct PerPointSweepSectionTransforms
		{
			static constexpr bool PerPoint = true;
			std::span<const SweepSectionTransform> values;
		};

		struct TransformedSweepFrame
		{
			Vec3 xAxis;
			Vec3 yAxis;
		};

		template <class SectionTransforms>
		[[nodiscard]]
		static Mesh3DAddResult AppendSweepImpl(
			Mesh3D& mesh,
			const Polygon& crossSection,
			const std::span<const Vec3> path,
			const SectionTransforms& sectionTransforms,
			const Vec3* const initialXAxis,
			const Vec2 _uvScale,
			const Vec2 _uvOffset,
			const CloseRing closeRing,
			const Optional<Mesh3DEndCaps> requestedEndCaps)
		{
			if constexpr (SectionTransforms::PerPoint)
			{
				if (sectionTransforms.values.size() != path.size())
				{
					return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
						U"Mesh3D::Sweep(): The section-transform count must match the path point count");
				}

				bool allIdentity = true;
				for (const SweepSectionTransform& transform : sectionTransforms.values)
				{
					if ((not IsFloatRepresentable(transform.scale))
						|| (not std::isfinite(transform.twist)))
					{
						return AdditionFailed(Mesh3DErrorCode::NumericRange,
							U"Mesh3D::Sweep(): A section transform is non-finite or outside the float range");
					}

					const Float2 scale = transform.scale;
					if ((scale.x <= 0.0f) || (scale.y <= 0.0f))
					{
						return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
							U"Mesh3D::Sweep(): Every section scale component must be positive after conversion to float");
					}

					allIdentity = (allIdentity
						&& (scale == Float2::One())
						&& (transform.twist == 0.0));
				}

				if (allIdentity)
				{
					return AppendSweepImpl(
						mesh, crossSection, path, ConstantSweepSectionTransforms{},
						initialXAxis, _uvScale, _uvOffset, closeRing, requestedEndCaps);
				}
			}

			const bool isClosed = (closeRing == CloseRing::Yes);
			const Mesh3DEndCaps endCaps = requestedEndCaps.value_or(
				isClosed ? Mesh3DEndCaps::None : Mesh3DEndCaps::Both);
			if (isClosed && (endCaps != Mesh3DEndCaps::None))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
					U"Mesh3D::Sweep(): A closed path cannot have end caps");
			}
			const bool generateStartCap = ((endCaps == Mesh3DEndCaps::Start)
				|| (endCaps == Mesh3DEndCaps::Both));
			const bool generateEndCap = ((endCaps == Mesh3DEndCaps::End)
				|| (endCaps == Mesh3DEndCaps::Both));
			const size_t capCount = (static_cast<size_t>(generateStartCap)
				+ static_cast<size_t>(generateEndCap));
			if ((crossSection.isEmpty())
				|| (path.size() < (isClosed ? 3 : 2)))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Sweep(): The cross section or path is invalid");
			}

			if ((not IsFloatRepresentable(_uvScale))
				|| (not IsFloatRepresentable(_uvOffset)))
			{
				return AdditionFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Sweep(): The UV transform is non-finite or outside the float range");
			}

			const auto& capVertices = crossSection.vertices();
			const auto& capIndices = crossSection.indices();
			size_t validCapTriangleCount;
			const CapValidationResult capValidation = ValidateCapTriangles<true>(
				capVertices, capIndices, validCapTriangleCount);
			if (capValidation != CapValidationResult::Valid)
			{
				return AdditionFailed(
					(capValidation == CapValidationResult::NumericRange
						? Mesh3DErrorCode::NumericRange
						: Mesh3DErrorCode::InvalidGeometry),
					U"Mesh3D::Sweep(): The cross-section cap triangulation is invalid");
			}

			size_t edgeCount = crossSection.outer().size();
			for (const auto& inner : crossSection.inners())
			{
				if (not CheckedAdd(edgeCount, inner.size(), edgeCount))
				{
					return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Sweep(): The cross-section edge count exceeds the supported range");
				}
			}

			size_t ringCount;
			if (not CheckedAdd(crossSection.inners().size(), 1, ringCount))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Sweep(): The cross-section ring count exceeds the supported range");
			}

			Array<double> ringPerimeters(ringCount);
			if (ValidateRing(std::span<const Vec2>{ crossSection.outer() }, true, ringPerimeters[0])
				!= RingValidationResult::Valid)
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Sweep(): The cross-section outer ring is invalid");
			}

			for (size_t i = 0; i < crossSection.inners().size(); ++i)
			{
				if (ValidateRing(std::span<const Vec2>{ crossSection.inners()[i] }, false, ringPerimeters[i + 1])
					!= RingValidationResult::Valid)
				{
					return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Sweep(): A cross-section inner ring is invalid");
				}
			}

			float minX = capVertices.front().x;
			float maxX = minX;
			float minY = capVertices.front().y;
			float maxY = minY;
			double maxDistanceFromPath = 0.0;
			for (const Float2 vertex : capVertices)
			{
				minX = std::min(minX, vertex.x);
				maxX = std::max(maxX, vertex.x);
				minY = std::min(minY, vertex.y);
				maxY = std::max(maxY, vertex.y);
				if constexpr (not SectionTransforms::PerPoint)
				{
					maxDistanceFromPath = std::max(maxDistanceFromPath,
						std::hypot(static_cast<double>(vertex.x), static_cast<double>(vertex.y)));
				}
			}

			if constexpr (SectionTransforms::PerPoint)
			{
				for (const SweepSectionTransform& transform : sectionTransforms.values)
				{
					const Float2 scale = transform.scale;
					for (const Float2 vertex : capVertices)
					{
						maxDistanceFromPath = std::max(maxDistanceFromPath,
							std::hypot(
								(static_cast<double>(vertex.x) * scale.x),
								(static_cast<double>(vertex.y) * scale.y)));
					}
				}
			}

			const double width = (static_cast<double>(maxX) - minX);
			const double height = (static_cast<double>(maxY) - minY);
			if ((width <= 0.0)
				|| (height <= 0.0))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Sweep(): The cross-section bounds must have positive width and height");
			}

			if (not IsFloatRepresentable(maxDistanceFromPath))
			{
				return AdditionFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Sweep(): The cross-section distance from the path exceeds the float range");
			}

			size_t capVertexCount = 0;
			size_t verticesPerEdge;
			size_t sideVertexCount;
			size_t vertexCount;
			size_t capTriangleCount = 0;
			const size_t pathSegmentCount = (isClosed ? path.size() : (path.size() - 1));
			size_t sideQuadCount;
			size_t sideTriangleCount;
			size_t triangleCount;
			size_t stationCount;
			if ((not CheckedAdd(path.size(), (isClosed ? 1 : 0), stationCount))
				|| (not CheckedMultiply(capVertices.size(), capCount, capVertexCount))
				|| (not CheckedMultiply(stationCount, 2, verticesPerEdge))
				|| (not CheckedMultiply(edgeCount, verticesPerEdge, sideVertexCount))
				|| (not CheckedAdd(capVertexCount, sideVertexCount, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| (not CheckedMultiply(validCapTriangleCount, capCount, capTriangleCount))
				|| (not CheckedMultiply(edgeCount, pathSegmentCount, sideQuadCount))
				|| (not CheckedMultiply(sideQuadCount, 2, sideTriangleCount))
				|| (not CheckedAdd(capTriangleCount, sideTriangleCount, triangleCount)))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Sweep(): The generated mesh exceeds the supported size");
			}

			PathData pathData;
			const PathDataStatus pathDataStatus = MakePathData(
				path, maxDistanceFromPath, initialXAxis, closeRing, pathData);
			if (pathDataStatus != PathDataStatus::Success)
			{
				return AdditionFailed(ToErrorCode(pathDataStatus), U"Mesh3D::Sweep(): The path or initial X axis is invalid, or stable frames cannot be produced");
			}

			Array<TransformedSweepFrame> transformedFrames;
			if constexpr (SectionTransforms::PerPoint)
			{
				transformedFrames.resize(stationCount);
				for (size_t pathIndex = 0; pathIndex < stationCount; ++pathIndex)
				{
					const PathFrame& frame = pathData.frames[pathIndex];
					const SweepSectionTransform& transform =
						sectionTransforms.values[pathIndex % path.size()];
					const Float2 scale = transform.scale;
					const double twist = std::remainder(transform.twist, Math::TwoPi);
					const double cosine = std::cos(twist);
					const double sine = std::sin(twist);
					transformedFrames[pathIndex] = TransformedSweepFrame{
						.xAxis = ((frame.normal * cosine) - (frame.binormal * sine)) * scale.x,
						.yAxis = ((frame.normal * sine) + (frame.binormal * cosine)) * scale.y,
					};
				}
			}

			const double sideU0 = _uvOffset.x;
			const double sideU1 = (_uvOffset.x + _uvScale.x);
			const double sideV0 = _uvOffset.y;
			const double sideV1 = (_uvOffset.y
				+ (_uvScale.y * pathData.distances.back()));
			const double capV1 = (_uvOffset.y + _uvScale.y);
			if ((not IsFloatRepresentable(sideU0))
				|| (not IsFloatRepresentable(sideU1))
				|| (not IsFloatRepresentable(sideV0))
				|| (not IsFloatRepresentable(sideV1))
				|| ((0 < capCount) && (not IsFloatRepresentable(capV1))))
			{
				return AdditionFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Sweep(): The generated UV coordinates exceed the float range");
			}

			const Float2 uvScale = _uvScale;
			const Float2 uvOffset = _uvOffset;
			const size_t vertexBase = mesh.vertices.size();
			const size_t triangleBase = mesh.indices.size();
			size_t newVertexCount;
			size_t newTriangleCount;
			if ((not CheckedAdd(vertexBase, vertexCount, newVertexCount))
				|| (Mesh3D::MaxVertexCount < newVertexCount)
				|| (not CheckedAdd(triangleBase, triangleCount, newTriangleCount)))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Sweep(): The generated mesh exceeds the supported size");
			}

			mesh.vertices.resize(newVertexCount);
			mesh.indices.resize(newTriangleCount);
			const auto generationFailed = [&](const Mesh3DErrorCode code, const StringView message)
			{
				mesh.vertices.resize(vertexBase);
				mesh.indices.resize(triangleBase);
				return AdditionFailed(code, message);
			};
			const auto getPosition = [&](const size_t pathIndex, const Float2 source)
			{
				const Vec3 center = pathData.points[pathIndex];
				if constexpr (SectionTransforms::PerPoint)
				{
					const TransformedSweepFrame& frame = transformedFrames[pathIndex];
					return (center + (frame.xAxis * source.x) + (frame.yAxis * source.y));
				}
				else
				{
					const PathFrame& frame = pathData.frames[pathIndex];
					return (center + (frame.normal * source.x) + (frame.binormal * source.y));
				}
			};
			if (0 < capCount)
			{
				size_t writtenCapCount = 0;
				for (size_t capIndex = 0; capIndex < 2; ++capIndex)
				{
					const bool startCap = (capIndex == 0);
					if ((startCap && (not generateStartCap))
						|| ((not startCap) && (not generateEndCap)))
					{
						continue;
					}
					const size_t pathIndex = (startCap ? 0 : (path.size() - 1));
					const size_t capBase = (vertexBase + (writtenCapCount++ * capVertices.size()));
					const PathFrame& frame = pathData.frames[pathIndex];
					const Float3 capNormal = (startCap
						? Float3{ -frame.tangent }
						: Float3{ frame.tangent });
					Vec3 capTangentDirection = frame.normal;
					if constexpr (SectionTransforms::PerPoint)
					{
						capTangentDirection = transformedFrames[pathIndex].xAxis.normalized();
					}
					const Float4 capTangent{
						static_cast<float>(capTangentDirection.x),
						static_cast<float>(capTangentDirection.y),
						static_cast<float>(capTangentDirection.z),
						(startCap ? 1.0f : -1.0f)
					};

					for (size_t vertexIndex = 0; vertexIndex < capVertices.size(); ++vertexIndex)
					{
						const Float2 source = capVertices[vertexIndex];
						Float3 position;
						if (not ToFloat3(getPosition(pathIndex, source), position))
						{
							return generationFailed(Mesh3DErrorCode::NumericRange,
								U"Mesh3D::Sweep(): A generated cap vertex exceeds the float range");
						}

						const float u = static_cast<float>(
							(static_cast<double>(source.x) - minX) / width);
						const float v = static_cast<float>(
							(static_cast<double>(source.y) - minY) / height);
						mesh.vertices[capBase + vertexIndex] = Vertex3D{
							.pos = position,
							.normal = capNormal,
							.tex = Float2{
								(uvOffset.x + (uvScale.x * u)),
								(uvOffset.y + (uvScale.y * v))
							},
							.tangent = capTangent
						};
					}
				}
			}

			TriangleIndex32* pTriangle = (mesh.indices.data() + triangleBase);
			if (0 < capCount)
			{
				ForEachValidCapTriangle(capVertices, capIndices, validCapTriangleCount,
					[&](const TriangleIndex& source)
				{
					size_t capBase = vertexBase;
					if (generateStartCap)
					{
						const uint32 i0 = static_cast<uint32>(capBase + source.i0);
						const uint32 i1 = static_cast<uint32>(capBase + source.i1);
						const uint32 i2 = static_cast<uint32>(capBase + source.i2);
						*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
						capBase += capVertices.size();
					}
					if (generateEndCap)
					{
						*pTriangle++ = TriangleIndex32{
							static_cast<uint32>(capBase + source.i0),
							static_cast<uint32>(capBase + source.i2),
							static_cast<uint32>(capBase + source.i1)
						};
					}
				});
			}

			Array<Float3> currentPositions;
			Array<Float3> nextPositions;
			if constexpr (SectionTransforms::PerPoint)
			{
				currentPositions.resize(stationCount);
				nextPositions.resize(stationCount);
			}
			Mesh3DErrorCode writeRingError = Mesh3DErrorCode::NumericRange;
			size_t sideVertexOffset = (vertexBase + capVertexCount);
			const auto writeRing = [&](const std::span<const Vec2> ring, const double perimeter)
			{
				double accumulatedLength = 0.0;
				for (size_t edgeIndex = 0; edgeIndex < ring.size(); ++edgeIndex)
				{
					const Float2 current = ring[edgeIndex];
					const Float2 next = ring[(edgeIndex + 1) % ring.size()];
					const double dx = (static_cast<double>(next.x) - current.x);
					const double dy = (static_cast<double>(next.y) - current.y);
					const double edgeLength = std::hypot(dx, dy);
					const double tangentX = (dx / edgeLength);
					const double tangentY = (dy / edgeLength);
					const float u0 = static_cast<float>(
						(_uvOffset.x + (_uvScale.x * (accumulatedLength / perimeter))));
					accumulatedLength += edgeLength;
					const float u1 = static_cast<float>(
						(_uvOffset.x + (_uvScale.x * (((edgeIndex + 1) == ring.size())
							? 1.0
							: (accumulatedLength / perimeter)))));
					const size_t edgeVertexBase = sideVertexOffset;

					if constexpr (SectionTransforms::PerPoint)
					{
						for (size_t pathIndex = 0; pathIndex < stationCount; ++pathIndex)
						{
							if ((not ToFloat3(getPosition(pathIndex, current), currentPositions[pathIndex]))
								|| (not ToFloat3(getPosition(pathIndex, next), nextPositions[pathIndex])))
							{
								writeRingError = Mesh3DErrorCode::NumericRange;
								return false;
							}
						}
					}

					for (size_t pathIndex = 0; pathIndex < stationCount; ++pathIndex)
					{
						const float v = static_cast<float>(_uvOffset.y
							+ (_uvScale.y * pathData.distances[pathIndex]));
						Float3 currentPosition;
						Float3 nextPosition;
						Vec3 currentNormal;
						Vec3 nextNormal;
						Vec3 tangent;
						if constexpr (SectionTransforms::PerPoint)
						{
							currentPosition = currentPositions[pathIndex];
							nextPosition = nextPositions[pathIndex];
							const size_t sourceIndex = (isClosed
								? (pathIndex % path.size())
								: pathIndex);
							const size_t previousIndex = (isClosed
								? ((sourceIndex + path.size() - 1) % path.size())
								: ((sourceIndex == 0) ? 0 : (sourceIndex - 1)));
							const size_t followingIndex = (isClosed
								? ((sourceIndex + 1) % path.size())
								: std::min((sourceIndex + 1), (path.size() - 1)));
							const Vec3 edge = (Vec3{ nextPosition } - Vec3{ currentPosition });
							const Vec3 currentLongitudinal =
								(Vec3{ currentPositions[followingIndex] }
									- Vec3{ currentPositions[previousIndex] });
							const Vec3 nextLongitudinal =
								(Vec3{ nextPositions[followingIndex] }
									- Vec3{ nextPositions[previousIndex] });
							currentNormal = currentLongitudinal.cross(edge);
							nextNormal = nextLongitudinal.cross(edge);
							const double edgeLengthSq = edge.lengthSq();
							const double currentNormalLengthSq = currentNormal.lengthSq();
							const double nextNormalLengthSq = nextNormal.lengthSq();
							if ((not std::isfinite(edgeLengthSq))
								|| (not std::isfinite(currentNormalLengthSq))
								|| (not std::isfinite(nextNormalLengthSq))
								|| (edgeLengthSq == 0.0)
								|| (currentNormalLengthSq == 0.0)
								|| (nextNormalLengthSq == 0.0))
							{
								writeRingError = Mesh3DErrorCode::InvalidGeometry;
								return false;
							}

							tangent = (edge / std::sqrt(edgeLengthSq));
							currentNormal /= std::sqrt(currentNormalLengthSq);
							nextNormal /= std::sqrt(nextNormalLengthSq);
						}
						else
						{
							const PathFrame& frame = pathData.frames[pathIndex];
							currentNormal = ((frame.normal * tangentY)
								- (frame.binormal * tangentX));
							nextNormal = currentNormal;
							tangent = ((frame.normal * tangentX)
								+ (frame.binormal * tangentY));
							if ((not ToFloat3(getPosition(pathIndex, current), currentPosition))
								|| (not ToFloat3(getPosition(pathIndex, next), nextPosition)))
							{
								writeRingError = Mesh3DErrorCode::NumericRange;
								return false;
							}
						}
						const Float4 vertexTangent{
							static_cast<float>(tangent.x),
							static_cast<float>(tangent.y),
							static_cast<float>(tangent.z),
							-1.0f
						};

						const size_t vertexBase = (edgeVertexBase + (pathIndex * 2));
						mesh.vertices[vertexBase + 0] = Vertex3D{
							.pos = currentPosition,
							.normal = currentNormal,
							.tex = Float2{ u0, v },
							.tangent = vertexTangent
						};
						mesh.vertices[vertexBase + 1] = Vertex3D{
							.pos = nextPosition,
							.normal = nextNormal,
							.tex = Float2{ u1, v },
							.tangent = vertexTangent
						};
					}

					for (size_t pathIndex = 0; pathIndex < pathSegmentCount; ++pathIndex)
					{
						const uint32 i0 = static_cast<uint32>(edgeVertexBase + (pathIndex * 2));
						const uint32 i1 = (i0 + 1);
						const uint32 i2 = (i0 + 2);
						const uint32 i3 = (i0 + 3);
						if constexpr (SectionTransforms::PerPoint)
						{
							const auto triangleIsOutward = [&](const uint32 a, const uint32 b, const uint32 c)
							{
								const Vertex3D& v0 = mesh.vertices[a];
								const Vertex3D& v1 = mesh.vertices[b];
								const Vertex3D& v2 = mesh.vertices[c];
								const Vec3 faceNormal = (Vec3{ v1.pos } - Vec3{ v0.pos }).cross(
									Vec3{ v2.pos } - Vec3{ v0.pos });
								const Vec3 vertexNormal = (Vec3{ v0.normal }
									+ Vec3{ v1.normal } + Vec3{ v2.normal });
								return ((0.0 < faceNormal.lengthSq())
									&& (0.0 < faceNormal.dot(vertexNormal)));
							};
							if ((not triangleIsOutward(i0, i2, i1))
								|| (not triangleIsOutward(i1, i2, i3)))
							{
								writeRingError = Mesh3DErrorCode::InvalidGeometry;
								return false;
							}
						}
						*pTriangle++ = TriangleIndex32{ i0, i2, i1 };
						*pTriangle++ = TriangleIndex32{ i1, i2, i3 };
					}

					sideVertexOffset += verticesPerEdge;
				}

				return true;
			};

			if (not writeRing(crossSection.outer(), ringPerimeters[0]))
			{
				return generationFailed(writeRingError,
					U"Mesh3D::Sweep(): A transformed outer ring is non-finite, degenerate, or inverted");
			}

			for (size_t i = 0; i < crossSection.inners().size(); ++i)
			{
				if (not writeRing(crossSection.inners()[i], ringPerimeters[i + 1]))
				{
					return generationFailed(writeRingError,
						U"Mesh3D::Sweep(): A transformed inner ring is non-finite, degenerate, or inverted");
				}
			}

			return AddedRange(mesh, vertexBase, triangleBase);
		}
	}

	Mesh3DAddResult Mesh3DDetail::AppendSweep(
		Mesh3D& mesh,
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const SweepOptions& options)
	{
		const Vec3* const initialXAxis = (options.initialXAxis
			? &options.initialXAxis.value()
			: nullptr);
		return AppendSweepImpl(
			mesh, crossSection, path, ConstantSweepSectionTransforms{},
			initialXAxis, options.uvScale, options.uvOffset,
			options.closeRing, options.endCaps);
	}

	Mesh3DAddResult Mesh3DDetail::AppendSweep(
		Mesh3D& mesh,
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const std::span<const SweepSectionTransform> sectionTransforms,
		const SweepOptions& options)
	{
		const Vec3* const initialXAxis = (options.initialXAxis
			? &options.initialXAxis.value()
			: nullptr);
		return AppendSweepImpl(
			mesh, crossSection, path, PerPointSweepSectionTransforms{ sectionTransforms },
			initialXAxis, options.uvScale, options.uvOffset,
			options.closeRing, options.endCaps);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Sweep
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const SweepOptions& options)
	{
		Mesh3DBuilder builder;
		(void)builder.addSweep(crossSection, path, options);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const SweepOptions& options)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			options);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const std::span<const SweepSectionTransform> sectionTransforms,
		const SweepOptions& options)
	{
		Mesh3DBuilder builder;
		(void)builder.addSweep(crossSection, path, sectionTransforms, options);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const std::initializer_list<SweepSectionTransform> sectionTransforms,
		const SweepOptions& options)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			std::span<const SweepSectionTransform>{
				sectionTransforms.begin(), sectionTransforms.size() },
			options);
	}
}
