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
		static bool MakePathData(
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
				|| (not IsFloatRepresentable(maxDistanceFromPath))
				|| (maxDistanceFromPath < 0.0)
				|| (not CheckedAdd(path.size(), (isClosed ? 1 : 0), stationCount)))
			{
				return false;
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
					return false;
				}

				if ((0 < i) && (result.points[i] == result.points[i - 1]))
				{
					return false;
				}
			}

			if (result.points.front() == result.points.back())
			{
				return false;
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
					|| (length == 0.0)
					|| (not std::isfinite(nextDistance)))
				{
					return false;
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
					return false;
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
					return false;
				}

				result.frames.front().normal = (*initialXAxis
					- (result.frames.front().tangent
						* initialXAxis->dot(result.frames.front().tangent)));
				const double normalLengthSq = result.frames.front().normal.lengthSq();
				if ((not std::isfinite(normalLengthSq))
					|| (normalLengthSq == 0.0))
				{
					return false;
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
						return false;
					}

					result.frames[i].tangent = (tangentSum / std::sqrt(tangentLengthSq));
				}

				if (not TransportNormal(
					result.frames[i - 1].tangent,
					result.frames[i].tangent,
					result.frames[i - 1].normal,
					result.frames[i].normal))
				{
					return false;
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
					return false;
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

			return true;
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
		const uint32 sides,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		return Tube(
			std::span<const Vec3>{ path.begin(), path.size() },
			radius,
			sides,
			uvScale,
			uvOffset,
			closeRing);
	}

	Mesh3D Mesh3D::Tube(
		const std::initializer_list<Vec3> path,
		const double radius,
		const CloseRing closeRing,
		const uint32 sides,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Tube(
			std::span<const Vec3>{ path.begin(), path.size() },
			radius,
			sides,
			uvScale,
			uvOffset,
			closeRing);
	}

	Mesh3DAddResult Mesh3DDetail::AppendTube(
		Mesh3D& mesh,
		const std::span<const Vec3> path,
		const double _radius,
		const uint32 sides,
		const Vec2 _uvScale,
		const Vec2 _uvOffset,
		const CloseRing closeRing)
	{
		const bool isClosed = (closeRing == CloseRing::Yes);
		if ((path.size() < (isClosed ? 3 : 2))
			|| (sides < 3))
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::Tube(): The path or side count is invalid");
		}

		if ((not IsFloatRepresentable(_radius))
			|| (not IsFloatRepresentable(_uvScale))
			|| (not IsFloatRepresentable(_uvOffset)))
		{
			return AdditionFailed(Mesh3DErrorCode::NumericRange,
				U"Mesh3D::Tube(): A numeric parameter is non-finite or outside the float range");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidArgument,
				U"Mesh3D::Tube(): radius must be positive after conversion to float");
		}

		size_t ringStride;
		size_t pathAndCapsCount;
		size_t vertexCount;
		size_t twiceSides;
		size_t triangleCount;
		if ((not CheckedAdd(static_cast<size_t>(sides), 1, ringStride))
			|| (not CheckedAdd(path.size(), (isClosed ? 1 : 2), pathAndCapsCount))
			|| (not CheckedMultiply(pathAndCapsCount, ringStride, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(sides), 2, twiceSides))
			|| (not CheckedMultiply(path.size(), twiceSides, triangleCount)))
		{
			return AdditionFailed(Mesh3DErrorCode::SizeLimit,
				U"Mesh3D::Tube(): The generated mesh exceeds the supported size");
		}

		PathData pathData;
		if (not MakePathData(path, radius, nullptr, closeRing, pathData))
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidGeometry,
				U"Mesh3D::Tube(): The path is invalid or cannot produce stable frames");
		}

		const auto& points = pathData.points;
		const auto& distances = pathData.distances;
		const auto& frames = pathData.frames;
		const size_t stationCount = points.size();
		const size_t pathSegmentCount = (isClosed ? path.size() : (path.size() - 1));

		const double sideU0 = _uvOffset.x;
		const double sideU1 = (_uvOffset.x + _uvScale.x);
		const double sideV0 = _uvOffset.y;
		const double sideV1 = (_uvOffset.y + (_uvScale.y * distances.back()));
		const double capV1 = (_uvOffset.y + _uvScale.y);
		if ((not IsFloatRepresentable(sideU0))
			|| (not IsFloatRepresentable(sideU1))
			|| (not IsFloatRepresentable(sideV0))
			|| (not IsFloatRepresentable(sideV1))
			|| ((not isClosed) && (not IsFloatRepresentable(capV1))))
		{
			return AdditionFailed(Mesh3DErrorCode::NumericRange,
				U"Mesh3D::Tube(): The generated UV coordinates exceed the float range");
		}

		const Array<CircleSample<double>> circle = MakeCircleSamples<double>(sides);

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
			const float v = static_cast<float>(
				(_uvOffset.y + (_uvScale.y * distances[pathIndex])));
			const size_t ringBase = (vertexBase + pathIndex * ringStride);

			for (uint32 sideIndex = 0; sideIndex <= sides; ++sideIndex)
			{
				const CircleSample<double> sample = circle[sideIndex];
				const Vec3 normal = ((frame.normal * sample.cos)
					+ (frame.binormal * sample.sin));
				const Vec3 tangent = ((frame.normal * -sample.sin)
					+ (frame.binormal * sample.cos));
				Float3 position;
				if (not ToFloat3((center + (normal * radius)), position))
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

		if (not isClosed)
		{
			const size_t startCapBase = (vertexBase + stationCount * ringStride);
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
					const CircleSample<double> sample = circle[sideIndex];
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

	Mesh3D Mesh3D::Tube(
		const std::span<const Vec3> path,
		const double radius,
		const uint32 sides,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		Mesh3DBuilder builder;
		(void)builder.addTube(path, radius, sides, uvScale, uvOffset, closeRing);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Tube(
		const std::span<const Vec3> path,
		const double radius,
		const CloseRing closeRing,
		const uint32 sides,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Tube(path, radius, sides, uvScale, uvOffset, closeRing);
	}

	namespace
	{
		[[nodiscard]]
		static Mesh3DAddResult AppendSweepImpl(
			Mesh3D& mesh,
			const Polygon& crossSection,
			const std::span<const Vec3> path,
			const Vec3* const initialXAxis,
			const Vec2 _uvScale,
			const Vec2 _uvOffset,
			const CloseRing closeRing)
		{
			const bool isClosed = (closeRing == CloseRing::Yes);
			if ((crossSection.isEmpty())
				|| (path.size() < (isClosed ? 3 : 2))
				|| (not IsFloatRepresentable(_uvScale))
				|| (not IsFloatRepresentable(_uvOffset)))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Sweep(): The cross section, path, or UV transform is invalid");
			}

			const auto& capVertices = crossSection.vertices();
			const auto& capIndices = crossSection.indices();
			size_t validCapTriangleCount;
			if (not ValidateCapTriangles<true>(capVertices, capIndices, validCapTriangleCount))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Sweep(): The cross-section cap triangulation is invalid");
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
				maxDistanceFromPath = std::max(maxDistanceFromPath,
					std::hypot(static_cast<double>(vertex.x), static_cast<double>(vertex.y)));
			}

			const double width = (static_cast<double>(maxX) - minX);
			const double height = (static_cast<double>(maxY) - minY);
			if ((width <= 0.0)
				|| (height <= 0.0)
				|| (not IsFloatRepresentable(maxDistanceFromPath)))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Sweep(): The cross-section bounds or distance from the path is invalid");
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
				|| ((not isClosed) && (not CheckedMultiply(capVertices.size(), 2, capVertexCount)))
				|| (not CheckedMultiply(stationCount, 2, verticesPerEdge))
				|| (not CheckedMultiply(edgeCount, verticesPerEdge, sideVertexCount))
				|| (not CheckedAdd(capVertexCount, sideVertexCount, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| ((not isClosed) && (not CheckedMultiply(validCapTriangleCount, 2, capTriangleCount)))
				|| (not CheckedMultiply(edgeCount, pathSegmentCount, sideQuadCount))
				|| (not CheckedMultiply(sideQuadCount, 2, sideTriangleCount))
				|| (not CheckedAdd(capTriangleCount, sideTriangleCount, triangleCount)))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Sweep(): The generated mesh exceeds the supported size");
			}

			PathData pathData;
			if (not MakePathData(path, maxDistanceFromPath, initialXAxis, closeRing, pathData))
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Sweep(): The path or initial X axis is invalid, or stable frames cannot be produced");
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
				|| ((not isClosed) && (not IsFloatRepresentable(capV1))))
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
			const auto generationFailed = [&](const StringView message)
			{
				mesh.vertices.resize(vertexBase);
				mesh.indices.resize(triangleBase);
				return AdditionFailed(Mesh3DErrorCode::NumericRange, message);
			};
			if (not isClosed)
			{
				const size_t startCapBase = vertexBase;
				const size_t endCapBase = (vertexBase + capVertices.size());

				for (size_t capIndex = 0; capIndex < 2; ++capIndex)
				{
					const bool startCap = (capIndex == 0);
					const size_t pathIndex = (startCap ? 0 : (path.size() - 1));
					const size_t capBase = (startCap ? startCapBase : endCapBase);
					const PathFrame& frame = pathData.frames[pathIndex];
					const Vec3 center = pathData.points[pathIndex];
					const Float3 capNormal = (startCap
						? Float3{ -frame.tangent }
						: Float3{ frame.tangent });
					const Float4 capTangent{
						static_cast<float>(frame.normal.x),
						static_cast<float>(frame.normal.y),
						static_cast<float>(frame.normal.z),
						(startCap ? 1.0f : -1.0f)
					};

					for (size_t vertexIndex = 0; vertexIndex < capVertices.size(); ++vertexIndex)
					{
						const Float2 source = capVertices[vertexIndex];
						Float3 position;
						if (not ToFloat3((center
							+ (frame.normal * source.x)
							+ (frame.binormal * source.y)), position))
						{
							return generationFailed(U"Mesh3D::Sweep(): A generated cap vertex exceeds the float range");
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
			if (not isClosed)
			{
				const size_t startCapBase = vertexBase;
				const size_t endCapBase = (vertexBase + capVertices.size());
				ForEachValidCapTriangle(capVertices, capIndices, validCapTriangleCount,
					[&](const TriangleIndex& source)
				{
					const uint32 i0 = static_cast<uint32>(startCapBase + source.i0);
					const uint32 i1 = static_cast<uint32>(startCapBase + source.i1);
					const uint32 i2 = static_cast<uint32>(startCapBase + source.i2);
					*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
					*pTriangle++ = TriangleIndex32{
						static_cast<uint32>(endCapBase + source.i0),
						static_cast<uint32>(endCapBase + source.i2),
						static_cast<uint32>(endCapBase + source.i1)
					};
				});
			}

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

					for (size_t pathIndex = 0; pathIndex < stationCount; ++pathIndex)
					{
						const PathFrame& frame = pathData.frames[pathIndex];
						const Vec3 center = pathData.points[pathIndex];
						const Vec3 normal = ((frame.normal * tangentY)
							- (frame.binormal * tangentX));
						const Vec3 tangent = ((frame.normal * tangentX)
							+ (frame.binormal * tangentY));
						const Float4 vertexTangent{
							static_cast<float>(tangent.x),
							static_cast<float>(tangent.y),
							static_cast<float>(tangent.z),
							-1.0f
						};
						const float v = static_cast<float>(_uvOffset.y
							+ (_uvScale.y * pathData.distances[pathIndex]));
						Float3 currentPosition;
						Float3 nextPosition;
						if ((not ToFloat3((center
							+ (frame.normal * current.x)
							+ (frame.binormal * current.y)), currentPosition))
							|| (not ToFloat3((center
								+ (frame.normal * next.x)
								+ (frame.binormal * next.y)), nextPosition)))
						{
							return false;
						}

						const size_t vertexBase = (edgeVertexBase + (pathIndex * 2));
						mesh.vertices[vertexBase + 0] = Vertex3D{
							.pos = currentPosition,
							.normal = normal,
							.tex = Float2{ u0, v },
							.tangent = vertexTangent
						};
						mesh.vertices[vertexBase + 1] = Vertex3D{
							.pos = nextPosition,
							.normal = normal,
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
						*pTriangle++ = TriangleIndex32{ i0, i2, i1 };
						*pTriangle++ = TriangleIndex32{ i1, i2, i3 };
					}

					sideVertexOffset += verticesPerEdge;
				}

				return true;
			};

			if (not writeRing(crossSection.outer(), ringPerimeters[0]))
			{
					return generationFailed(U"Mesh3D::Sweep(): A generated outer-ring vertex exceeds the float range");
			}

			for (size_t i = 0; i < crossSection.inners().size(); ++i)
			{
				if (not writeRing(crossSection.inners()[i], ringPerimeters[i + 1]))
				{
					return generationFailed(U"Mesh3D::Sweep(): A generated inner-ring vertex exceeds the float range");
				}
			}

			return AddedRange(mesh, vertexBase, triangleBase);
		}
	}

	Mesh3DAddResult Mesh3DDetail::AppendSweep(
		Mesh3D& mesh,
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const Vec3* const initialXAxis,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		return AppendSweepImpl(
			mesh, crossSection, path, initialXAxis, uvScale, uvOffset, closeRing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Sweep
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		Mesh3DBuilder builder;
		(void)builder.addSweep(crossSection, path, uvScale, uvOffset, closeRing);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const CloseRing closeRing,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Sweep(crossSection, path, uvScale, uvOffset, closeRing);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			uvScale,
			uvOffset,
			closeRing);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const CloseRing closeRing,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			uvScale,
			uvOffset,
			closeRing);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const Arg::initialXAxis_<Vec3> initialXAxis,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		Mesh3DBuilder builder;
		(void)builder.addSweep(crossSection, path, initialXAxis, uvScale, uvOffset, closeRing);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const Arg::initialXAxis_<Vec3> initialXAxis,
		const CloseRing closeRing,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Sweep(crossSection, path, initialXAxis, uvScale, uvOffset, closeRing);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const Arg::initialXAxis_<Vec3> initialXAxis,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const CloseRing closeRing)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			initialXAxis,
			uvScale,
			uvOffset,
			closeRing);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const Arg::initialXAxis_<Vec3> initialXAxis,
		const CloseRing closeRing,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			initialXAxis,
			uvScale,
			uvOffset,
			closeRing);
	}
}
