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
# include <array>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsFinite(const Float2 value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y));
		}

		[[nodiscard]]
		static bool IsFinite(const Float3 value) noexcept
		{
			return (std::isfinite(value.x)
				&& std::isfinite(value.y)
				&& std::isfinite(value.z));
		}

		[[nodiscard]]
		static bool IsFinite(const FloatRect& rect) noexcept
		{
			return (std::isfinite(rect.left)
				&& std::isfinite(rect.top)
				&& std::isfinite(rect.right)
				&& std::isfinite(rect.bottom));
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

		struct CircleSample
		{
			float sin;
			float cos;
		};

		[[nodiscard]]
		static Array<CircleSample> MakeCircleSamples(const uint32 segments)
		{
			Array<CircleSample> samples(static_cast<size_t>(segments) + 1);
			const float angleStep = (Math::TwoPiF / static_cast<float>(segments));

			for (uint32 i = 0; i < segments; ++i)
			{
				const float angle = (angleStep * i);
				samples[i] = CircleSample{ std::sin(angle), std::cos(angle) };
			}

			samples[segments] = samples[0];
			return samples;
		}

		struct BoxFace
		{
			Float3 center;
			Float3 u;
			Float3 v;
			Float3 normal;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Box
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Box(const Float3 size)
	{
		return Box(size, BoxUVMapping{});
	}

	Mesh3D Mesh3D::Box(const Float3 size, const BoxUVMapping& uvMapping)
	{
		if ((not IsFinite(size))
			|| (size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return{};
		}

		const Float3 halfSize = (size * 0.5f);
		const std::array<BoxFace, 6> faces =
		{{
			{ { 0.0f, 0.0f, -halfSize.z }, { size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ { 0.0f, 0.0f,  halfSize.z }, { -size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -size.y, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, -size.y, 0.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { 0.0f,  halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -1.0f, 0.0f } },
		}};
		const std::array<FloatRect, 6> uvRects =
		{{
			uvMapping.negativeZ,
			uvMapping.positiveZ,
			uvMapping.positiveX,
			uvMapping.negativeX,
			uvMapping.positiveY,
			uvMapping.negativeY,
		}};

		for (const auto& uvRect : uvRects)
		{
			if (not IsFinite(uvRect))
			{
				return{};
			}
		}

		Mesh3D mesh{ 24, 12 };

		for (size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
		{
			const BoxFace& face = faces[faceIndex];
			const Float3 halfU = (face.u * 0.5f);
			const Float3 halfV = (face.v * 0.5f);
			const FloatRect uvRect = uvRects[faceIndex];
			const float uSign = ((uvRect.right < uvRect.left) ? -1.0f : 1.0f);
			const float vSign = ((uvRect.bottom < uvRect.top) ? -1.0f : 1.0f);
			const Float3 tangent = (face.u.normalized() * uSign);
			const Float4 tangentFrame{ tangent, (uSign * vSign) };
			const size_t vertexOffset = (faceIndex * 4);

			mesh.vertices[vertexOffset + 0] = Vertex3D{
				.pos = (face.center - halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.left, uvRect.top },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 1] = Vertex3D{
				.pos = (face.center + halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.right, uvRect.top },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 2] = Vertex3D{
				.pos = (face.center - halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.left, uvRect.bottom },
				.tangent = tangentFrame
			};
			mesh.vertices[vertexOffset + 3] = Vertex3D{
				.pos = (face.center + halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.right, uvRect.bottom },
				.tangent = tangentFrame
			};

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			const size_t triangleOffset = (faceIndex * 2);
			mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Plane
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Plane(const Float2 sizeXZ, const Float2 uvScale, const Float2 uvOffset)
	{
		return Grid(sizeXZ, 1, 1, uvScale, uvOffset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Grid
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Grid(
		const Float2 sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Float2 uvScale,
		const Float2 uvOffset)
	{
		if ((not IsFinite(sizeXZ))
			|| (not IsFinite(uvScale))
			|| (not IsFinite(uvOffset))
			|| (sizeXZ.x <= 0.0f)
			|| (sizeXZ.y <= 0.0f)
			|| (segmentsX == 0)
			|| (segmentsZ == 0))
		{
			return{};
		}

		size_t columnCount;
		size_t rowCount;
		size_t vertexCount;
		size_t cellCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(segmentsX), 1, columnCount))
			|| (not CheckedAdd(static_cast<size_t>(segmentsZ), 1, rowCount))
			|| (not CheckedMultiply(columnCount, rowCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(segmentsX), static_cast<size_t>(segmentsZ), cellCount))
			|| (not CheckedMultiply(cellCount, 2, triangleCount)))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Float2 halfSize = (sizeXZ * 0.5f);
		const float invSegmentsX = (1.0f / static_cast<float>(segmentsX));
		const float invSegmentsZ = (1.0f / static_cast<float>(segmentsZ));
		const Float3 normal = Float3::UnitY();
		const Float4 tangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		for (uint32 z = 0; z <= segmentsZ; ++z)
		{
			const float v = (z * invSegmentsZ);
			const float positionZ = (halfSize.y - (sizeXZ.y * v));

			for (uint32 x = 0; x <= segmentsX; ++x)
			{
				const float u = (x * invSegmentsX);
				const size_t vertexIndex = (static_cast<size_t>(z) * columnCount + x);

				mesh.vertices[vertexIndex] = Vertex3D{
					.pos = Float3{ (-halfSize.x + (sizeXZ.x * u)), 0.0f, positionZ },
					.normal = normal,
					.tex = Float2{ (uvOffset.x + (uvScale.x * u)), (uvOffset.y + (uvScale.y * v)) },
					.tangent = tangent
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (uint32 z = 0; z < segmentsZ; ++z)
		{
			const size_t rowOffset = (static_cast<size_t>(z) * columnCount);

			for (uint32 x = 0; x < segmentsX; ++x)
			{
				const uint32 i0 = static_cast<uint32>(rowOffset + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(rowOffset + columnCount + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Torus
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Torus(
		const float majorRadius,
		const float tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments)
	{
		if ((not std::isfinite(majorRadius))
			|| (not std::isfinite(tubeRadius))
			|| (majorRadius <= 0.0f)
			|| (tubeRadius <= 0.0f)
			|| (majorRadius <= tubeRadius)
			|| (ringSegments < 3)
			|| (tubeSegments < 3))
		{
			return{};
		}

		size_t ringStride;
		size_t tubeRowCount;
		size_t vertexCount;
		size_t quadCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(ringSegments), 1, ringStride))
			|| (not CheckedAdd(static_cast<size_t>(tubeSegments), 1, tubeRowCount))
			|| (not CheckedMultiply(ringStride, tubeRowCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(ringSegments), static_cast<size_t>(tubeSegments), quadCount))
			|| (not CheckedMultiply(quadCount, 2, triangleCount)))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Array<CircleSample> ringSinCos = MakeCircleSamples(ringSegments);
		const Array<CircleSample> tubeSinCos = MakeCircleSamples(tubeSegments);
		const float invRingSegments = (1.0f / static_cast<float>(ringSegments));
		const float invTubeSegments = (1.0f / static_cast<float>(tubeSegments));

		for (uint32 tubeIndex = 0; tubeIndex <= tubeSegments; ++tubeIndex)
		{
			const float tubeSin = tubeSinCos[tubeIndex].sin;
			const float tubeCos = tubeSinCos[tubeIndex].cos;
			const float ringRadius = (majorRadius + (tubeRadius * tubeSin));
			const float positionY = (tubeRadius * tubeCos);
			const float v = (static_cast<float>(tubeIndex) * invTubeSegments);
			const size_t rowBase = (static_cast<size_t>(tubeIndex) * ringStride);

			for (uint32 ringIndex = 0; ringIndex <= ringSegments; ++ringIndex)
			{
				const float ringSin = ringSinCos[ringIndex].sin;
				const float ringCos = ringSinCos[ringIndex].cos;
				const Float3 normal{ (ringCos * tubeSin), tubeCos, (ringSin * tubeSin) };

				mesh.vertices[rowBase + ringIndex] = Vertex3D{
					.pos = Float3{ (ringRadius * ringCos), positionY, (ringRadius * ringSin) },
					.normal = normal,
					.tex = Float2{ (static_cast<float>(ringIndex) * invRingSegments), v },
					.tangent = Float4{ -ringSin, 0.0f, ringCos, 1.0f }
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (uint32 tubeIndex = 0; tubeIndex < tubeSegments; ++tubeIndex)
		{
			const size_t rowBase = (static_cast<size_t>(tubeIndex) * ringStride);
			const size_t nextRowBase = (rowBase + ringStride);

			for (uint32 ringIndex = 0; ringIndex < ringSegments; ++ringIndex)
			{
				const uint32 i0 = static_cast<uint32>(rowBase + ringIndex);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(nextRowBase + ringIndex);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	UVSphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::UVSphere(const float radius, const uint32 slices, const uint32 stacks)
	{
		if ((not std::isfinite(radius))
			|| (radius <= 0.0f)
			|| (slices < 3)
			|| (stacks < 2))
		{
			return{};
		}

		const size_t interiorRingCount = (static_cast<size_t>(stacks) - 1);
		size_t ringStride;
		size_t ringVertexCount;
		size_t poleVertexCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(interiorRingCount, ringStride, ringVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(slices), 2, poleVertexCount))
			|| (not CheckedAdd(ringVertexCount, poleVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(slices), interiorRingCount, triangleCount))
			|| (not CheckedMultiply(triangleCount, 2, triangleCount)))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const size_t firstRingBase = slices;
		const size_t bottomPoleBase = (firstRingBase + ringVertexCount);
		const float longitudeStep = (Math::TwoPiF / static_cast<float>(slices));
		const float latitudeStep = (Math::PiF / static_cast<float>(stacks));
		const Array<CircleSample> longitudeSinCos = MakeCircleSamples(slices);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float longitude = (longitudeStep * x);

			const float middleLongitude = (longitude + (longitudeStep * 0.5f));
			const float middleSin = std::sin(middleLongitude);
			const float middleCos = std::cos(middleLongitude);
			const float u = ((x + 0.5f) / static_cast<float>(slices));
			const Float4 tangent{ -middleSin, 0.0f, middleCos, 1.0f };

			mesh.vertices[x] = Vertex3D{
				.pos = Float3{ 0.0f, radius, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ u, 0.0f },
				.tangent = tangent
			};
			mesh.vertices[bottomPoleBase + x] = Vertex3D{
				.pos = Float3{ 0.0f, -radius, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ u, 1.0f },
				.tangent = tangent
			};
		}

		for (uint32 y = 1; y < stacks; ++y)
		{
			const float v = (static_cast<float>(y) / static_cast<float>(stacks));
			const float latitude = (latitudeStep * y);
			const float latitudeSin = std::sin(latitude);
			const float latitudeCos = std::cos(latitude);
			const size_t ringBase = (firstRingBase + (static_cast<size_t>(y - 1) * ringStride));

			for (uint32 x = 0; x <= slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;
				const Float3 normal{
					(latitudeSin * longitudeCos),
					latitudeCos,
					(latitudeSin * longitudeSin)
				};

				mesh.vertices[ringBase + x] = Vertex3D{
					.pos = (normal * radius),
					.normal = normal,
					.tex = Float2{ (static_cast<float>(x) / static_cast<float>(slices)), v },
					.tangent = Float4{ -longitudeSin, 0.0f, longitudeCos, 1.0f }
				};
			}
		}

		TriangleIndex32* pTriangle = mesh.indices.data();

		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 pole = x;
			const uint32 ringLeft = static_cast<uint32>(firstRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			*pTriangle++ = TriangleIndex32{ pole, ringRight, ringLeft };
		}

		for (size_t ring = 0; (ring + 1) < interiorRingCount; ++ring)
		{
			const size_t upperRingBase = (firstRingBase + (ring * ringStride));
			const size_t lowerRingBase = (upperRingBase + ringStride);

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 i0 = static_cast<uint32>(upperRingBase + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(lowerRingBase + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		const size_t lastRingBase = (firstRingBase + ((interiorRingCount - 1) * ringStride));
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 ringLeft = static_cast<uint32>(lastRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			const uint32 pole = static_cast<uint32>(bottomPoleBase + x);
			*pTriangle++ = TriangleIndex32{ ringLeft, ringRight, pole };
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Disc
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Disc(const float radius, const uint32 segments)
	{
		return Annulus(0.0f, radius, segments);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Annulus
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Annulus(const float innerRadius, const float outerRadius, const uint32 segments)
	{
		if ((not std::isfinite(innerRadius))
			|| (not std::isfinite(outerRadius))
			|| (innerRadius < 0.0f)
			|| (outerRadius <= innerRadius)
			|| (segments < 3))
		{
			return{};
		}

		const bool isDisc = (innerRadius == 0.0f);
		size_t vertexCount;
		size_t triangleCount;

		if (isDisc)
		{
			if (not CheckedAdd(static_cast<size_t>(segments), 1, vertexCount))
			{
				return{};
			}

			triangleCount = segments;
		}
		else
		{
			if ((not CheckedMultiply(static_cast<size_t>(segments), 2, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 2, triangleCount)))
			{
				return{};
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Array<CircleSample> circle = MakeCircleSamples(segments);
		const Float3 normal = Float3::UnitY();
		const Float4 tangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		if (isDisc)
		{
			mesh.vertices[0] = Vertex3D{
				.pos = Float3::Zero(),
				.normal = normal,
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = tangent
			};

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				mesh.vertices[static_cast<size_t>(i) + 1] = Vertex3D{
					.pos = Float3{ (outerRadius * sample.cos), 0.0f, (outerRadius * sample.sin) },
					.normal = normal,
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) },
					.tangent = tangent
				};

				const uint32 current = (i + 1);
				const uint32 next = (((i + 1) % segments) + 1);
				mesh.indices[i] = TriangleIndex32{ 0, next, current };
			}
		}
		else
		{
			const size_t innerRingBase = segments;
			const float innerUVScale = (0.5f * innerRadius / outerRadius);

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				mesh.vertices[i] = Vertex3D{
					.pos = Float3{ (outerRadius * sample.cos), 0.0f, (outerRadius * sample.sin) },
					.normal = normal,
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) },
					.tangent = tangent
				};
				mesh.vertices[innerRingBase + i] = Vertex3D{
					.pos = Float3{ (innerRadius * sample.cos), 0.0f, (innerRadius * sample.sin) },
					.normal = normal,
					.tex = Float2{ (0.5f + (innerUVScale * sample.cos)), (0.5f - (innerUVScale * sample.sin)) },
					.tangent = tangent
				};
			}

			TriangleIndex32* pTriangle = mesh.indices.data();
			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 next = ((i + 1) % segments);
				const uint32 outerCurrent = i;
				const uint32 outerNext = next;
				const uint32 innerCurrent = static_cast<uint32>(innerRingBase + i);
				const uint32 innerNext = static_cast<uint32>(innerRingBase + next);

				*pTriangle++ = TriangleIndex32{ outerCurrent, innerCurrent, outerNext };
				*pTriangle++ = TriangleIndex32{ innerCurrent, innerNext, outerNext };
			}
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Frustum
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Frustum(
		const float bottomRadius,
		const float topRadius,
		const float height,
		const uint32 segments)
	{
		if ((not std::isfinite(bottomRadius))
			|| (not std::isfinite(topRadius))
			|| (not std::isfinite(height))
			|| (bottomRadius <= 0.0f)
			|| (topRadius < 0.0f)
			|| (height <= 0.0f)
			|| (segments < 3))
		{
			return{};
		}

		const bool isCone = (topRadius == 0.0f);
		size_t ringStride;
		size_t vertexCount;
		size_t triangleCount;

		if (not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
		{
			return{};
		}

		if (isCone)
		{
			size_t scaledSegments;
			if ((not CheckedMultiply(static_cast<size_t>(segments), 3, scaledSegments))
				|| (not CheckedAdd(scaledSegments, 2, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 2, triangleCount)))
			{
				return{};
			}
		}
		else
		{
			if ((not CheckedMultiply(ringStride, 4, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 4, triangleCount)))
			{
				return{};
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Array<CircleSample> circle = MakeCircleSamples(segments);
		const float halfHeight = (height * 0.5f);
		const float radiusDelta = (bottomRadius - topRadius);
		const float inverseSideLength = (1.0f / std::sqrt((height * height) + (radiusDelta * radiusDelta)));
		const float angleStep = (Math::TwoPiF / static_cast<float>(segments));
		TriangleIndex32* pTriangle = mesh.indices.data();

		if (isCone)
		{
			const size_t bottomSideBase = segments;
			const size_t bottomCapCenter = (bottomSideBase + ringStride);
			const size_t bottomCapRingBase = (bottomCapCenter + 1);

			for (uint32 i = 0; i < segments; ++i)
			{
				const float middleAngle = ((i + 0.5f) * angleStep);
				const float middleSin = std::sin(middleAngle);
				const float middleCos = std::cos(middleAngle);
				const Float3 sideNormal = Float3{
					(height * middleCos),
					radiusDelta,
					(height * middleSin)
				} * inverseSideLength;

				mesh.vertices[i] = Vertex3D{
					.pos = Float3{ 0.0f, halfHeight, 0.0f },
					.normal = sideNormal,
					.tex = Float2{ ((i + 0.5f) / static_cast<float>(segments)), 0.0f },
					.tangent = Float4{ -middleSin, 0.0f, middleCos, 1.0f }
				};
			}

			for (uint32 i = 0; i <= segments; ++i)
			{
				const CircleSample sample = circle[i];
				const Float3 sideNormal = Float3{
					(height * sample.cos),
					radiusDelta,
					(height * sample.sin)
				} * inverseSideLength;

				mesh.vertices[bottomSideBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = sideNormal,
					.tex = Float2{ (static_cast<float>(i) / static_cast<float>(segments)), 1.0f },
					.tangent = Float4{ -sample.sin, 0.0f, sample.cos, 1.0f }
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 apex = i;
				const uint32 bottomLeft = static_cast<uint32>(bottomSideBase + i);
				const uint32 bottomRight = (bottomLeft + 1);
				*pTriangle++ = TriangleIndex32{ apex, bottomRight, bottomLeft };
			}

			mesh.vertices[bottomCapCenter] = Vertex3D{
				.pos = Float3{ 0.0f, -halfHeight, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				mesh.vertices[bottomCapRingBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = -Float3::UnitY(),
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f + (0.5f * sample.sin)) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 current = static_cast<uint32>(bottomCapRingBase + i);
				const uint32 next = static_cast<uint32>(bottomCapRingBase + ((i + 1) % segments));
				*pTriangle++ = TriangleIndex32{ static_cast<uint32>(bottomCapCenter), current, next };
			}
		}
		else
		{
			const size_t topSideBase = 0;
			const size_t bottomSideBase = ringStride;
			const size_t bottomCapCenter = (ringStride * 2);
			const size_t bottomCapRingBase = (bottomCapCenter + 1);
			const size_t topCapCenter = (bottomCapRingBase + segments);
			const size_t topCapRingBase = (topCapCenter + 1);

			for (uint32 i = 0; i <= segments; ++i)
			{
				const CircleSample sample = circle[i];
				const Float3 sideNormal = Float3{
					(height * sample.cos),
					radiusDelta,
					(height * sample.sin)
				} * inverseSideLength;
				const Float4 sideTangent{ -sample.sin, 0.0f, sample.cos, 1.0f };
				const float u = (static_cast<float>(i) / static_cast<float>(segments));

				mesh.vertices[topSideBase + i] = Vertex3D{
					.pos = Float3{ (topRadius * sample.cos), halfHeight, (topRadius * sample.sin) },
					.normal = sideNormal,
					.tex = Float2{ u, 0.0f },
					.tangent = sideTangent
				};
				mesh.vertices[bottomSideBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = sideNormal,
					.tex = Float2{ u, 1.0f },
					.tangent = sideTangent
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 topLeft = static_cast<uint32>(topSideBase + i);
				const uint32 topRight = (topLeft + 1);
				const uint32 bottomLeft = static_cast<uint32>(bottomSideBase + i);
				const uint32 bottomRight = (bottomLeft + 1);

				*pTriangle++ = TriangleIndex32{ topLeft, topRight, bottomLeft };
				*pTriangle++ = TriangleIndex32{ bottomLeft, topRight, bottomRight };
			}

			mesh.vertices[bottomCapCenter] = Vertex3D{
				.pos = Float3{ 0.0f, -halfHeight, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			mesh.vertices[topCapCenter] = Vertex3D{
				.pos = Float3{ 0.0f, halfHeight, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				mesh.vertices[bottomCapRingBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = -Float3::UnitY(),
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f + (0.5f * sample.sin)) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
				mesh.vertices[topCapRingBase + i] = Vertex3D{
					.pos = Float3{ (topRadius * sample.cos), halfHeight, (topRadius * sample.sin) },
					.normal = Float3::UnitY(),
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 bottomCurrent = static_cast<uint32>(bottomCapRingBase + i);
				const uint32 bottomNext = static_cast<uint32>(bottomCapRingBase + ((i + 1) % segments));
				const uint32 topCurrent = static_cast<uint32>(topCapRingBase + i);
				const uint32 topNext = static_cast<uint32>(topCapRingBase + ((i + 1) % segments));

				*pTriangle++ = TriangleIndex32{ static_cast<uint32>(bottomCapCenter), bottomCurrent, bottomNext };
				*pTriangle++ = TriangleIndex32{ static_cast<uint32>(topCapCenter), topNext, topCurrent };
			}
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Cylinder
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Cylinder(const float radius, const float height, const uint32 segments)
	{
		return Frustum(radius, radius, height, segments);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Cone
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Cone(const float radius, const float height, const uint32 segments)
	{
		return Frustum(radius, 0.0f, height, segments);
	}
}
