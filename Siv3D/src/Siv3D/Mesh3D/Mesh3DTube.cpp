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
		using Mesh3DDetail::ForEachValidCapTriangle;
		using Mesh3DDetail::GenerationFailed;
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
			const Vec3* const initialNormal,
			PathData& result)
		{
			if ((path.size() < 2)
				|| (not IsFloatRepresentable(maxDistanceFromPath))
				|| (maxDistanceFromPath < 0.0))
			{
				return false;
			}

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

			const size_t pathSegmentCount = (path.size() - 1);
			Array<Vec3> segmentTangents(pathSegmentCount);
			result.distances.assign(path.size(), 0.0);
			for (size_t i = 0; i < pathSegmentCount; ++i)
			{
				const Vec3 delta = (Vec3{ result.points[i + 1] } - Vec3{ result.points[i] });
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

			result.frames.resize(path.size());
			result.frames.front().tangent = segmentTangents.front();
			if (initialNormal)
			{
				if (not IsFloatRepresentable(*initialNormal))
				{
					return false;
				}

				result.frames.front().normal = (*initialNormal
					- (result.frames.front().tangent
						* initialNormal->dot(result.frames.front().tangent)));
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
				if ((i + 1) == path.size())
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
		const Vec2 uvOffset)
	{
		return Tube(
			std::span<const Vec3>{ path.begin(), path.size() },
			radius,
			sides,
			uvScale,
			uvOffset);
	}

	bool Mesh3DDetail::AppendTube(
		Mesh3D& mesh,
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
			return GenerationFailed<bool>("Mesh3D::Tube(): The path, radius, side count, or UV transform is invalid");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return GenerationFailed<bool>("Mesh3D::Tube(): radius must be positive after conversion to float");
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
			return GenerationFailed<bool>("Mesh3D::Tube(): The generated mesh exceeds the supported size");
		}

		PathData pathData;
		if (not MakePathData(path, radius, nullptr, pathData))
		{
			return GenerationFailed<bool>("Mesh3D::Tube(): The path is invalid or cannot produce stable frames");
		}

		const auto& points = pathData.points;
		const auto& distances = pathData.distances;
		const auto& frames = pathData.frames;
		const size_t pathSegmentCount = (path.size() - 1);

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
			return GenerationFailed<bool>("Mesh3D::Tube(): The generated UV coordinates exceed the float range");
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
			return GenerationFailed<bool>("Mesh3D::Tube(): The generated mesh exceeds the supported size");
		}

		mesh.vertices.resize(newVertexCount);
		mesh.indices.resize(newTriangleCount);
		const auto generationFailed = [&](const char* const message)
		{
			mesh.vertices.resize(vertexBase);
			mesh.indices.resize(triangleBase);
			return GenerationFailed<bool>(message);
		};

		for (size_t pathIndex = 0; pathIndex < path.size(); ++pathIndex)
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
					return generationFailed("Mesh3D::Tube(): A generated side vertex exceeds the float range");
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

		const size_t startCapBase = (vertexBase + path.size() * ringStride);
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
					return generationFailed("Mesh3D::Tube(): A generated cap vertex exceeds the float range");
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

		return true;
	}

	Mesh3D Mesh3D::Tube(
		const std::span<const Vec3> path,
		const double radius,
		const uint32 sides,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		Mesh3DBuilder builder;
		builder.addTube(path, radius, sides, uvScale, uvOffset);
		return std::move(builder).build();
	}

	namespace
	{
		[[nodiscard]]
		static Mesh3D MakeSweep(
			const Polygon& crossSection,
			const std::span<const Vec3> path,
			const Vec3* const initialNormal,
			const Vec2 _uvScale,
			const Vec2 _uvOffset)
		{
			if ((crossSection.isEmpty())
				|| (path.size() < 2)
				|| (not IsFloatRepresentable(_uvScale))
				|| (not IsFloatRepresentable(_uvOffset)))
			{
				return GenerationFailed("Mesh3D::Sweep(): The cross section, path, or UV transform is invalid");
			}

			const auto& capVertices = crossSection.vertices();
			const auto& capIndices = crossSection.indices();
			size_t validCapTriangleCount;
			if (not ValidateCapTriangles<true>(capVertices, capIndices, validCapTriangleCount))
			{
				return GenerationFailed("Mesh3D::Sweep(): The cross-section cap triangulation is invalid");
			}

			size_t edgeCount = crossSection.outer().size();
			for (const auto& inner : crossSection.inners())
			{
				if (not CheckedAdd(edgeCount, inner.size(), edgeCount))
				{
					return GenerationFailed("Mesh3D::Sweep(): The cross-section edge count exceeds the supported range");
				}
			}

			size_t ringCount;
			if (not CheckedAdd(crossSection.inners().size(), 1, ringCount))
			{
				return GenerationFailed("Mesh3D::Sweep(): The cross-section ring count exceeds the supported range");
			}

			Array<double> ringPerimeters(ringCount);
			if (ValidateRing(std::span<const Vec2>{ crossSection.outer() }, true, ringPerimeters[0])
				!= RingValidationResult::Valid)
			{
				return GenerationFailed("Mesh3D::Sweep(): The cross-section outer ring is invalid");
			}

			for (size_t i = 0; i < crossSection.inners().size(); ++i)
			{
				if (ValidateRing(std::span<const Vec2>{ crossSection.inners()[i] }, false, ringPerimeters[i + 1])
					!= RingValidationResult::Valid)
				{
					return GenerationFailed("Mesh3D::Sweep(): A cross-section inner ring is invalid");
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
				return GenerationFailed("Mesh3D::Sweep(): The cross-section bounds or distance from the path is invalid");
			}

			size_t capVertexCount;
			size_t verticesPerEdge;
			size_t sideVertexCount;
			size_t vertexCount;
			size_t capTriangleCount;
			const size_t pathSegmentCount = (path.size() - 1);
			size_t sideQuadCount;
			size_t sideTriangleCount;
			size_t triangleCount;
			if ((not CheckedMultiply(capVertices.size(), 2, capVertexCount))
				|| (not CheckedMultiply(path.size(), 2, verticesPerEdge))
				|| (not CheckedMultiply(edgeCount, verticesPerEdge, sideVertexCount))
				|| (not CheckedAdd(capVertexCount, sideVertexCount, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| (not CheckedMultiply(validCapTriangleCount, 2, capTriangleCount))
				|| (not CheckedMultiply(edgeCount, pathSegmentCount, sideQuadCount))
				|| (not CheckedMultiply(sideQuadCount, 2, sideTriangleCount))
				|| (not CheckedAdd(capTriangleCount, sideTriangleCount, triangleCount)))
			{
				return GenerationFailed("Mesh3D::Sweep(): The generated mesh exceeds the supported size");
			}

			PathData pathData;
			if (not MakePathData(path, maxDistanceFromPath, initialNormal, pathData))
			{
				return GenerationFailed("Mesh3D::Sweep(): The path or initial normal is invalid, or stable frames cannot be produced");
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
				|| (not IsFloatRepresentable(capV1)))
			{
				return GenerationFailed("Mesh3D::Sweep(): The generated UV coordinates exceed the float range");
			}

			const Float2 uvScale = _uvScale;
			const Float2 uvOffset = _uvOffset;
			Mesh3D mesh{ vertexCount, triangleCount };
			const size_t startCapBase = 0;
			const size_t endCapBase = capVertices.size();

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
						return GenerationFailed("Mesh3D::Sweep(): A generated cap vertex exceeds the float range");
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

			TriangleIndex32* pTriangle = mesh.indices.data();
			ForEachValidCapTriangle(capVertices, capIndices, validCapTriangleCount,
				[&](const TriangleIndex& source)
			{
				const uint32 i0 = source.i0;
				const uint32 i1 = source.i1;
				const uint32 i2 = source.i2;
				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{
					static_cast<uint32>(endCapBase + i0),
					static_cast<uint32>(endCapBase + i2),
					static_cast<uint32>(endCapBase + i1)
				};
			});

			size_t sideVertexOffset = capVertexCount;
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

					for (size_t pathIndex = 0; pathIndex < path.size(); ++pathIndex)
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
				return GenerationFailed("Mesh3D::Sweep(): A generated outer-ring vertex exceeds the float range");
			}

			for (size_t i = 0; i < crossSection.inners().size(); ++i)
			{
				if (not writeRing(crossSection.inners()[i], ringPerimeters[i + 1]))
				{
					return GenerationFailed("Mesh3D::Sweep(): A generated inner-ring vertex exceeds the float range");
				}
			}

			return mesh;
		}
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
		const Vec2 uvOffset)
	{
		return MakeSweep(crossSection, path, nullptr, uvScale, uvOffset);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			uvScale,
			uvOffset);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const Vec3 initialNormal,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return MakeSweep(crossSection, path, &initialNormal, uvScale, uvOffset);
	}

	Mesh3D Mesh3D::Sweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const Vec3 initialNormal,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Sweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			initialNormal,
			uvScale,
			uvOffset);
	}
}
