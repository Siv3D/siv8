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
# include <optional>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::ForEachValidCapTriangle;
		using Mesh3DDetail::GenerationFailed;
		using Mesh3DDetail::IsFloatRepresentable;
		using RevolveCircleSample = Mesh3DDetail::CircleSample<float>;

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
			const RevolveCircleSample sample,
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
}

namespace s3d::Mesh3DDetail
{
	bool AppendRevolve(
		Mesh3D& mesh,
		const std::span<const Vec2> profile,
		const uint32 segments,
		const double smoothingAngle)
	{
		return AppendRevolve(
			mesh,
			profile,
			0.0,
			Math::TwoPi,
			segments,
			smoothingAngle,
			CloseEnds::No);
	}

	bool AppendRevolve(
		Mesh3D& mesh,
		const std::span<const Vec2> profile,
		const double startAngle,
		const double sweepAngle,
		const uint32 segments,
		const double smoothingAngle,
		const CloseEnds closeEnds)
	{
		const bool fullRevolution = (sweepAngle == Math::TwoPi);
		const bool generateEndCaps = ((closeEnds == CloseEnds::Yes) && (not fullRevolution));
		if ((profile.size() < 2)
			|| (not std::isfinite(startAngle))
			|| (not std::isfinite(sweepAngle))
			|| (sweepAngle <= 0.0)
			|| (Math::TwoPi < sweepAngle)
			|| (segments < (fullRevolution ? 3 : 1))
			|| (not std::isfinite(smoothingAngle))
			|| (smoothingAngle < 0.0)
			|| (Math::Pi < smoothingAngle))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): The profile, angles, segment count, or smoothing angle is invalid");
		}

		const bool closedProfile = (profile.front() == profile.back());
		if (closedProfile && (profile.size() < 4))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): A closed profile must contain at least three distinct points");
		}

		size_t ringStride;
		size_t fullSegmentVertexCount;
		if ((not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
			|| (not CheckedMultiply(ringStride, 2, fullSegmentVertexCount)))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): The segment count exceeds the supported range");
		}

		const size_t profileSegmentCount = (profile.size() - 1);
		size_t minimumVertexCount;
		if ((not CheckedMultiply(profileSegmentCount, (fullSegmentVertexCount - 1), minimumVertexCount))
			|| (Mesh3D::MaxVertexCount < minimumVertexCount))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): The generated vertex count exceeds the supported range");
		}

		Array<ProfileSegment> profileSegments;
		profileSegments.reserve(profileSegmentCount);

		Float2 firstPoint;
		if (not ToProfilePoint(profile.front(), firstPoint))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): Every profile point must be finite, float-representable, and have a non-negative radius");
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
				return GenerationFailed<bool>("Mesh3D::Revolve(): Every profile point must be finite, float-representable, and have a non-negative radius");
			}

			const bool startOnAxis = (current.x == 0.0f);
			const bool endOnAxis = (next.x == 0.0f);
			if (startOnAxis && endOnAxis)
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): A profile segment cannot lie entirely on the rotation axis");
			}

			const double dr = (static_cast<double>(next.x) - current.x);
			const double dy = (static_cast<double>(next.y) - current.y);
			const double length = std::hypot(dr, dy);
			if ((not std::isfinite(length))
				|| (length == 0.0))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): Profile segments must have positive finite length");
			}

			const double endDistance = (totalDistance + length);
			if (not std::isfinite(endDistance))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): The profile length exceeds the supported range");
			}

			const bool touchesAxis = (startOnAxis || endOnAxis);
			const size_t segmentVertexCount = (fullSegmentVertexCount - (touchesAxis ? 1 : 0));
			if ((not CheckedAdd(vertexCount, segmentVertexCount, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| (not CheckedAdd(trianglesPerSlice, (touchesAxis ? 1 : 2), trianglesPerSlice)))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): The generated mesh exceeds the supported size");
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

			if (closedProfile || generateEndCaps)
			{
				twiceArea += ((static_cast<double>(current.x) * next.y)
					- (static_cast<double>(next.x) * current.y));
			}
			totalDistance = endDistance;
			current = next;
		}

		if ((not closedProfile) && (current == firstPoint))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): An open profile cannot end at its first point");
		}
		else if (generateEndCaps)
		{
			twiceArea += ((static_cast<double>(current.x) * firstPoint.y)
				- (static_cast<double>(firstPoint.x) * current.y));
		}

		if (closedProfile
			&& ((not std::isfinite(twiceArea)) || (twiceArea == 0.0)))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): A closed profile must have non-zero finite area");
		}

		size_t triangleCount;
		if (not CheckedMultiply(trianglesPerSlice, static_cast<size_t>(segments), triangleCount))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): The generated triangle count exceeds the supported range");
		}

		const bool positiveProfileOrientation = (0.0 < twiceArea);
		std::optional<Polygon> capPolygon;
		size_t validCapTriangleCount = 0;
		if (generateEndCaps)
		{
			if ((not std::isfinite(twiceArea)) || (twiceArea == 0.0))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): The profile must enclose a non-zero finite area to generate end caps");
			}

			Array<Vec2> capOutline;
			capOutline.reserve(profileSegments.size() + 1);
			capOutline.push_back(profileSegments.front().start);
			for (const ProfileSegment& segment : profileSegments)
			{
				capOutline.push_back(segment.end);
			}
			if (closedProfile)
			{
				capOutline.pop_back();
			}
			if (not positiveProfileOrientation)
			{
				std::reverse(capOutline.begin(), capOutline.end());
			}

			capPolygon.emplace(std::span<const Vec2>{ capOutline });
			if (capPolygon->isEmpty()
				|| (not ValidateCapTriangles<true>(
					capPolygon->vertices(), capPolygon->indices(), validCapTriangleCount)))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): The profile cannot be triangulated for end caps");
			}

			const RectF bounds = capPolygon->boundingRect();
			if ((bounds.w <= 0.0) || (bounds.h <= 0.0))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): The end-cap bounds must have positive width and height");
			}

			size_t addedCapVertexCount;
			size_t addedCapTriangleCount;
			if ((not CheckedMultiply(capPolygon->vertices().size(), 2, addedCapVertexCount))
				|| (not CheckedAdd(vertexCount, addedCapVertexCount, vertexCount))
				|| (Mesh3D::MaxVertexCount < vertexCount)
				|| (not CheckedMultiply(validCapTriangleCount, 2, addedCapTriangleCount))
				|| (not CheckedAdd(triangleCount, addedCapTriangleCount, triangleCount)))
			{
				return GenerationFailed<bool>("Mesh3D::Revolve(): The generated mesh exceeds the supported size");
			}
		}

		Array<RevolveCircleSample> circle(ringStride);
		Array<RevolveCircleSample> middleCircle(segments);
		const bool legacyFullRevolution = (fullRevolution && (startAngle == 0.0));
		if (legacyFullRevolution)
		{
			const float angleStep = (Math::TwoPiF / static_cast<float>(segments));
			for (uint32 i = 0; i < segments; ++i)
			{
				const float angle = (angleStep * i);
				const float middleAngle = (angleStep * (i + 0.5f));
				circle[i] = RevolveCircleSample{ std::sin(angle), std::cos(angle) };
				middleCircle[i] = RevolveCircleSample{ std::sin(middleAngle), std::cos(middleAngle) };
			}
		}
		else
		{
			const double normalizedStartAngle = std::remainder(startAngle, Math::TwoPi);
			const double angleStep = (sweepAngle / segments);
			for (uint32 i = 0; i < segments; ++i)
			{
				const double angle = (normalizedStartAngle + (angleStep * i));
				const double middleAngle = (normalizedStartAngle + (angleStep * (i + 0.5)));
				circle[i] = RevolveCircleSample{
					static_cast<float>(std::sin(angle)), static_cast<float>(std::cos(angle))
				};
				middleCircle[i] = RevolveCircleSample{
					static_cast<float>(std::sin(middleAngle)), static_cast<float>(std::cos(middleAngle))
				};
			}

			if (not fullRevolution)
			{
				const double endAngle = (normalizedStartAngle + sweepAngle);
				circle[segments] = RevolveCircleSample{
					static_cast<float>(std::sin(endAngle)), static_cast<float>(std::cos(endAngle))
				};
			}
		}
		if (fullRevolution)
		{
			circle[segments] = circle[0];
		}

		const size_t vertexBase = mesh.vertices.size();
		const size_t triangleBase = mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if ((not CheckedAdd(vertexBase, vertexCount, newVertexCount))
			|| (Mesh3D::MaxVertexCount < newVertexCount)
			|| (not CheckedAdd(triangleBase, triangleCount, newTriangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::Revolve(): The generated mesh exceeds the supported size");
		}

		mesh.vertices.resize(newVertexCount);
		mesh.indices.resize(newTriangleCount);

		const bool smoothProfile = (0.0 < smoothingAngle);
		const float minNormalDot = (smoothProfile
			? std::cos(static_cast<float>(smoothingAngle))
			: 1.0f);
		const float inverseSegments = (1.0f / static_cast<float>(segments));
		const double inverseTotalDistance = (1.0 / totalDistance);
		TriangleIndex32* pTriangle = (mesh.indices.data() + triangleBase);
		size_t vertexOffset = vertexBase;

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

		if (generateEndCaps)
		{
			const auto& capVertices = capPolygon->vertices();
			const auto& capIndices = capPolygon->indices();
			const RectF bounds = capPolygon->boundingRect();
			const double inverseWidth = (1.0 / bounds.w);
			const double inverseHeight = (1.0 / bounds.h);
			const float orientationSign = (positiveProfileOrientation ? 1.0f : -1.0f);

			for (size_t capIndex = 0; capIndex < 2; ++capIndex)
			{
				const bool startCap = (capIndex == 0);
				const RevolveCircleSample sample = (startCap ? circle.front() : circle.back());
				const Float3 radial{ sample.cos, 0.0f, sample.sin };
				const Float3 angular{ -sample.sin, 0.0f, sample.cos };
				const Float3 normal = (angular
					* (startCap ? -orientationSign : orientationSign));
				const Float4 tangent{
					radial.x,
					radial.y,
					radial.z,
					(startCap ? orientationSign : -orientationSign)
				};
				const size_t capVertexBase = vertexOffset;

				for (size_t i = 0; i < capVertices.size(); ++i)
				{
					const Float2 source = capVertices[i];
					mesh.vertices[capVertexBase + i] = Vertex3D{
						.pos = Float3{
							(source.x * sample.cos), source.y, (source.x * sample.sin)
						},
						.normal = normal,
						.tex = Float2{
							static_cast<float>((source.x - bounds.x) * inverseWidth),
							static_cast<float>((source.y - bounds.y) * inverseHeight)
						},
						.tangent = tangent
					};
				}

				const bool reverseWinding = (startCap == positiveProfileOrientation);
				ForEachValidCapTriangle(capVertices, capIndices, validCapTriangleCount,
					[&](const TriangleIndex& source)
				{
					const uint32 i0 = static_cast<uint32>(capVertexBase + source.i0);
					const uint32 i1 = static_cast<uint32>(capVertexBase + source.i1);
					const uint32 i2 = static_cast<uint32>(capVertexBase + source.i2);
					*pTriangle++ = (reverseWinding
						? TriangleIndex32{ i0, i2, i1 }
						: TriangleIndex32{ i0, i1, i2 });
				});

				vertexOffset += capVertices.size();
			}
		}

		return true;
	}
}

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Revolve
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Revolve(const std::span<const Vec2> profile, const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addRevolve(profile, segments);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Revolve(const std::initializer_list<Vec2> profile, const uint32 segments)
	{
		return Revolve(std::span<const Vec2>{ profile.begin(), profile.size() }, segments);
	}

	Mesh3D Mesh3D::Revolve(
		const std::span<const Vec2> profile,
		const uint32 segments,
		const double smoothingAngle)
	{
		Mesh3DBuilder builder;
		builder.addRevolve(profile, segments, smoothingAngle);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Revolve(
		const std::initializer_list<Vec2> profile,
		const uint32 segments,
		const double smoothingAngle)
	{
		return Revolve(
			std::span<const Vec2>{ profile.begin(), profile.size() },
			segments,
			smoothingAngle);
	}

	Mesh3D Mesh3D::Revolve(
		const std::span<const Vec2> profile,
		const double startAngle,
		const double sweepAngle,
		const uint32 segments,
		const CloseEnds closeEnds)
	{
		Mesh3DBuilder builder;
		builder.addRevolve(profile, startAngle, sweepAngle, segments, closeEnds);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Revolve(
		const std::initializer_list<Vec2> profile,
		const double startAngle,
		const double sweepAngle,
		const uint32 segments,
		const CloseEnds closeEnds)
	{
		return Revolve(
			std::span<const Vec2>{ profile.begin(), profile.size() },
			startAngle,
			sweepAngle,
			segments,
			closeEnds);
	}

	Mesh3D Mesh3D::Revolve(
		const std::span<const Vec2> profile,
		const double startAngle,
		const double sweepAngle,
		const uint32 segments,
		const double smoothingAngle,
		const CloseEnds closeEnds)
	{
		Mesh3DBuilder builder;
		builder.addRevolve(
			profile, startAngle, sweepAngle, segments, smoothingAngle, closeEnds);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Revolve(
		const std::initializer_list<Vec2> profile,
		const double startAngle,
		const double sweepAngle,
		const uint32 segments,
		const double smoothingAngle,
		const CloseEnds closeEnds)
	{
		return Revolve(
			std::span<const Vec2>{ profile.begin(), profile.size() },
			startAngle,
			sweepAngle,
			segments,
			smoothingAngle,
			closeEnds);
	}
}
