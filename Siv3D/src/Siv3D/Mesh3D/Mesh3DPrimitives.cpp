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
# include "Mesh3DCommon.hpp"
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::GenerationFailed;
		using Mesh3DDetail::IsFloatRepresentable;
		using CircleSample = Mesh3DDetail::CircleSample<float>;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Box
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Box(const Vec3 size)
	{
		Mesh3DBuilder builder;
		builder.addBox(size);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Box(const Vec3 size, const BoxUVMapping& uvMapping)
	{
		Mesh3DBuilder builder;
		builder.addBox(size, uvMapping);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	RoundedBox
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::RoundedBox(const Vec3 size, const double radius, const uint32 subdivisions)
	{
		Mesh3DBuilder builder;
		builder.addRoundedBox(size, radius, subdivisions);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::RoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping)
	{
		Mesh3DBuilder builder;
		builder.addRoundedBox(size, radius, subdivisions, uvMapping);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Wedge
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Wedge(const Vec3 size)
	{
		Mesh3DBuilder builder;
		builder.addWedge(size);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Wedge(const Vec3 size, const BoxUVMapping& uvMapping)
	{
		Mesh3DBuilder builder;
		builder.addWedge(size, uvMapping);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	TriangularPrism
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::TriangularPrism(const Vec3 size)
	{
		Mesh3DBuilder builder;
		builder.addTriangularPrism(size);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::TriangularPrism(const Vec3 size, const BoxUVMapping& uvMapping)
	{
		Mesh3DBuilder builder;
		builder.addTriangularPrism(size, uvMapping);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Stairs
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Stairs(const Vec3 size, const uint32 steps)
	{
		Mesh3DBuilder builder;
		builder.addStairs(size, steps);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Stairs(const Vec3 size, const uint32 steps, const BoxUVMapping& uvMapping)
	{
		Mesh3DBuilder builder;
		builder.addStairs(size, steps, uvMapping);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Pyramid
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Pyramid(const double baseSize, const double height)
	{
		Mesh3DBuilder builder;
		builder.addPyramid(baseSize, height);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Pyramid(const SizeF baseSizeXZ, const double height)
	{
		Mesh3DBuilder builder;
		builder.addPyramid(baseSizeXZ, height);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	RectangularFrustum
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::RectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height)
	{
		Mesh3DBuilder builder;
		builder.addRectangularFrustum(bottomSizeXZ, topSizeXZ, height);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::RectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const BoxUVMapping& uvMapping)
	{
		Mesh3DBuilder builder;
		builder.addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, uvMapping);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Regular polyhedra
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Tetrahedron(const double radius)
	{
		Mesh3DBuilder builder;
		builder.addTetrahedron(radius);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Octahedron(const double radius)
	{
		Mesh3DBuilder builder;
		builder.addOctahedron(radius);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Icosahedron(const double radius)
	{
		Mesh3DBuilder builder;
		builder.addIcosahedron(radius);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Dodecahedron(const double radius)
	{
		Mesh3DBuilder builder;
		builder.addDodecahedron(radius);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Capsule
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Capsule(
		const double _radius,
		const double _cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks)
	{
		if ((not IsFloatRepresentable(_radius))
			|| (not IsFloatRepresentable(_cylinderHeight)))
		{
			return GenerationFailed("Mesh3D::Capsule(): radius and cylinderHeight must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		const float cylinderHeight = static_cast<float>(_cylinderHeight);
		if ((radius <= 0.0f)
			|| (cylinderHeight < 0.0f)
			|| (slices < 3)
			|| (hemisphereStacks < 1))
		{
			return GenerationFailed("Mesh3D::Capsule(): The radius, cylinder height, slice count, or hemisphere stack count is invalid");
		}

		if (cylinderHeight == 0.0f)
		{
			if ((std::numeric_limits<uint32>::max() / 2) < hemisphereStacks)
			{
				return GenerationFailed("Mesh3D::Capsule(): hemisphereStacks exceed the supported range for a zero-length cylinder");
			}

			return UVSphere(radius, slices, (hemisphereStacks * 2));
		}

		size_t interiorRingCount;
		size_t ringStride;
		size_t ringVertexCount;
		size_t poleVertexCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedMultiply(static_cast<size_t>(hemisphereStacks), 2, interiorRingCount))
			|| (not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(interiorRingCount, ringStride, ringVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(slices), 2, poleVertexCount))
			|| (not CheckedAdd(ringVertexCount, poleVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(slices), interiorRingCount, triangleCount))
			|| (not CheckedMultiply(triangleCount, 2, triangleCount)))
		{
			return GenerationFailed("Mesh3D::Capsule(): The generated mesh exceeds the supported size");
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const size_t firstRingBase = slices;
		const size_t bottomPoleBase = (firstRingBase + ringVertexCount);
		const float halfCylinderHeight = (cylinderHeight * 0.5f);
		const float hemisphereAngleStep = (Math::HalfPiF / static_cast<float>(hemisphereStacks));
		const float invSlices = (1.0f / static_cast<float>(slices));
		const float longitudeStep = (Math::TwoPiF * invSlices);
		const float profileLength = ((Math::PiF * radius) + cylinderHeight);
		const float invProfileLength = (1.0f / profileLength);
		const Array<CircleSample> longitudeSinCos = Mesh3DDetail::MakeCircleSamples<float>(slices);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float middleLongitude = ((longitudeStep * x) + (longitudeStep * 0.5f));
			const float middleSin = std::sin(middleLongitude);
			const float middleCos = std::cos(middleLongitude);
			const float u = ((x + 0.5f) * invSlices);
			const Float4 tangent{ -middleSin, 0.0f, middleCos, 1.0f };

			mesh.vertices[x] = Vertex3D{
				.pos = Float3{ 0.0f, (halfCylinderHeight + radius), 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ u, 0.0f },
				.tangent = tangent
			};
			mesh.vertices[bottomPoleBase + x] = Vertex3D{
				.pos = Float3{ 0.0f, -(halfCylinderHeight + radius), 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ u, 1.0f },
				.tangent = tangent
			};
		}

		const auto writeRing = [&](const size_t ringIndex, const float ringRadius, const float positionY,
			const float normalRadial, const float normalY, const float v)
		{
			const size_t ringBase = (firstRingBase + (ringIndex * ringStride));

			for (uint32 x = 0; x <= slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;

				mesh.vertices[ringBase + x] = Vertex3D{
					.pos = Float3{ (ringRadius * longitudeCos), positionY, (ringRadius * longitudeSin) },
					.normal = Float3{ (normalRadial * longitudeCos), normalY, (normalRadial * longitudeSin) },
					.tex = Float2{ (static_cast<float>(x) * invSlices), v },
					.tangent = Float4{ -longitudeSin, 0.0f, longitudeCos, 1.0f }
				};
			}
		};

		for (uint32 stack = 1; stack <= hemisphereStacks; ++stack)
		{
			const float angle = (hemisphereAngleStep * stack);
			const float angleSin = std::sin(angle);
			const float angleCos = std::cos(angle);
			writeRing(
				(stack - 1),
				(radius * angleSin),
				(halfCylinderHeight + (radius * angleCos)),
				angleSin,
				angleCos,
				((radius * angle) * invProfileLength));
		}

		writeRing(
			hemisphereStacks,
			radius,
			-halfCylinderHeight,
			1.0f,
			0.0f,
			(((Math::HalfPiF * radius) + cylinderHeight) * invProfileLength));

		for (uint32 stack = 1; stack < hemisphereStacks; ++stack)
		{
			const float angle = (hemisphereAngleStep * stack);
			const float angleSin = std::sin(angle);
			const float angleCos = std::cos(angle);
			writeRing(
				(static_cast<size_t>(hemisphereStacks) + stack),
				(radius * angleCos),
				(-halfCylinderHeight - (radius * angleSin)),
				angleCos,
				-angleSin,
				(((Math::HalfPiF * radius) + cylinderHeight + (radius * angle)) * invProfileLength));
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
	//	Plane
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Plane(const SizeF sizeXZ, const Vec2 uvScale, const Vec2 uvOffset)
	{
		Mesh3DBuilder builder;
		builder.addPlane(sizeXZ, uvScale, uvOffset);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Grid
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Grid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		Mesh3DBuilder builder;
		builder.addGrid(sizeXZ, segmentsX, segmentsZ, uvScale, uvOffset);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Torus
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Torus(
		const double _majorRadius,
		const double _tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments)
	{
		if ((not IsFloatRepresentable(_majorRadius))
			|| (not IsFloatRepresentable(_tubeRadius)))
		{
			return GenerationFailed("Mesh3D::Torus(): majorRadius and tubeRadius must be finite and float-representable");
		}

		const float majorRadius = static_cast<float>(_majorRadius);
		const float tubeRadius = static_cast<float>(_tubeRadius);
		if ((majorRadius <= 0.0f)
			|| (tubeRadius <= 0.0f)
			|| (majorRadius <= tubeRadius)
			|| (ringSegments < 3)
			|| (tubeSegments < 3))
		{
			return GenerationFailed("Mesh3D::Torus(): The radii or segment counts are invalid");
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
			return GenerationFailed("Mesh3D::Torus(): The generated mesh exceeds the supported size");
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Array<CircleSample> ringSinCos = Mesh3DDetail::MakeCircleSamples<float>(ringSegments);
		const Array<CircleSample> tubeSinCos = Mesh3DDetail::MakeCircleSamples<float>(tubeSegments);
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

	Mesh3D Mesh3D::UVSphere(const double _radius, const uint32 slices, const uint32 stacks)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed("Mesh3D::UVSphere(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if ((radius <= 0.0f)
			|| (slices < 3)
			|| (stacks < 2))
		{
			return GenerationFailed("Mesh3D::UVSphere(): The radius, slice count, or stack count is invalid");
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
			return GenerationFailed("Mesh3D::UVSphere(): The generated mesh exceeds the supported size");
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const size_t firstRingBase = slices;
		const size_t bottomPoleBase = (firstRingBase + ringVertexCount);
		const float longitudeStep = (Math::TwoPiF / static_cast<float>(slices));
		const float latitudeStep = (Math::PiF / static_cast<float>(stacks));
		const Array<CircleSample> longitudeSinCos = Mesh3DDetail::MakeCircleSamples<float>(slices);

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
	//	Hemisphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Hemisphere(const double radius, const uint32 slices, const uint32 stacks)
	{
		return Hemisphere(radius, CloseBottom::No, slices, stacks);
	}

	Mesh3D Mesh3D::Hemisphere(const double _radius, const CloseBottom closeBottom, const uint32 slices, const uint32 stacks)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed("Mesh3D::Hemisphere(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if ((radius <= 0.0f)
			|| (slices < 3)
			|| (stacks < 1))
		{
			return GenerationFailed("Mesh3D::Hemisphere(): The radius, slice count, or stack count is invalid");
		}

		size_t ringStride;
		size_t ringVertexCount;
		size_t surfaceVertexCount;
		size_t twiceStackCount;
		size_t triangleFactor;
		size_t surfaceTriangleCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(static_cast<size_t>(stacks), ringStride, ringVertexCount))
			|| (not CheckedAdd(static_cast<size_t>(slices), ringVertexCount, surfaceVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(stacks), 2, twiceStackCount)))
		{
			return GenerationFailed("Mesh3D::Hemisphere(): The surface dimensions exceed the supported range");
		}

		triangleFactor = (twiceStackCount - 1);
		if (not CheckedMultiply(static_cast<size_t>(slices), triangleFactor, surfaceTriangleCount))
		{
			return GenerationFailed("Mesh3D::Hemisphere(): The surface triangle count exceeds the supported range");
		}

		vertexCount = surfaceVertexCount;
		triangleCount = surfaceTriangleCount;
		if (closeBottom)
		{
			size_t bottomVertexCount;
			if ((not CheckedAdd(static_cast<size_t>(slices), 1, bottomVertexCount))
				|| (not CheckedAdd(vertexCount, bottomVertexCount, vertexCount))
				|| (not CheckedAdd(triangleCount, static_cast<size_t>(slices), triangleCount)))
			{
				return GenerationFailed("Mesh3D::Hemisphere(): The closed-bottom mesh exceeds the supported size");
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return GenerationFailed("Mesh3D::Hemisphere(): The generated vertex count exceeds the supported range");
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const size_t firstRingBase = slices;
		const float invSlices = (1.0f / static_cast<float>(slices));
		const float invStacks = (1.0f / static_cast<float>(stacks));
		const float longitudeStep = (Math::TwoPiF * invSlices);
		const float latitudeStep = (Math::HalfPiF * invStacks);
		const Array<CircleSample> longitudeSinCos = Mesh3DDetail::MakeCircleSamples<float>(slices);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float middleLongitude = (longitudeStep * (static_cast<float>(x) + 0.5f));
			const Float4 tangent{ -std::sin(middleLongitude), 0.0f, std::cos(middleLongitude), 1.0f };

			mesh.vertices[x] = Vertex3D{
				.pos = Float3{ 0.0f, radius, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ ((static_cast<float>(x) + 0.5f) * invSlices), 0.0f },
				.tangent = tangent
			};
		}

		for (uint32 stack = 1; stack <= stacks; ++stack)
		{
			const bool isEquator = (stack == stacks);
			const float latitude = (latitudeStep * static_cast<float>(stack));
			const float latitudeSin = (isEquator ? 1.0f : std::sin(latitude));
			const float latitudeCos = (isEquator ? 0.0f : std::cos(latitude));
			const float v = (static_cast<float>(stack) * invStacks);
			const size_t ringBase = (firstRingBase + (static_cast<size_t>(stack - 1) * ringStride));

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
					.tex = Float2{ (static_cast<float>(x) * invSlices), v },
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

		for (size_t ring = 0; (ring + 1) < stacks; ++ring)
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

		if (closeBottom)
		{
			const size_t bottomCenterIndex = surfaceVertexCount;
			const size_t bottomRingBase = (bottomCenterIndex + 1);
			mesh.vertices[bottomCenterIndex] = Vertex3D{
				.pos = Float3::Zero(),
				.normal = -Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};

			for (uint32 x = 0; x < slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;
				mesh.vertices[bottomRingBase + x] = Vertex3D{
					.pos = Float3{ (radius * longitudeCos), 0.0f, (radius * longitudeSin) },
					.normal = -Float3::UnitY(),
					.tex = Float2{ (0.5f + 0.5f * longitudeCos), (0.5f + 0.5f * longitudeSin) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
			}

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 center = static_cast<uint32>(bottomCenterIndex);
				const uint32 current = static_cast<uint32>(bottomRingBase + x);
				const uint32 next = static_cast<uint32>(bottomRingBase + ((x + 1) % slices));
				*pTriangle++ = TriangleIndex32{ center, current, next };
			}
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Disc
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Disc(const double radius, const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addDisc(radius, segments);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Annulus
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Annulus(
		const double innerRadius,
		const double outerRadius,
		const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addAnnulus(innerRadius, outerRadius, segments);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	HollowCylinder
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::HollowCylinder(
		const double _innerRadius,
		const double _outerRadius,
		const double _height,
		const uint32 segments)
	{
		if ((not IsFloatRepresentable(_innerRadius))
			|| (not IsFloatRepresentable(_outerRadius))
			|| (not IsFloatRepresentable(_height)))
		{
			return GenerationFailed("Mesh3D::HollowCylinder(): The radii and height must be finite and float-representable");
		}

		const float innerRadius = static_cast<float>(_innerRadius);
		const float outerRadius = static_cast<float>(_outerRadius);
		const float height = static_cast<float>(_height);
		if ((innerRadius <= 0.0f)
			|| (outerRadius <= innerRadius)
			|| (height <= 0.0f)
			|| (segments < 3))
		{
			return GenerationFailed("Mesh3D::HollowCylinder(): The radii, height, or segment count is invalid");
		}

		size_t ringStride;
		size_t vertexCount;
		size_t triangleCount;
		if ((not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
			|| (not CheckedMultiply(static_cast<size_t>(segments), 8, vertexCount))
			|| (not CheckedAdd(vertexCount, 4, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(segments), 8, triangleCount)))
		{
			return GenerationFailed("Mesh3D::HollowCylinder(): The generated mesh exceeds the supported size");
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Array<CircleSample> circle = Mesh3DDetail::MakeCircleSamples<float>(segments);
		const float halfHeight = (height * 0.5f);
		const float inverseSegments = (1.0f / static_cast<float>(segments));
		const float innerUVScale = (0.5f * innerRadius / outerRadius);
		const size_t outerTopBase = 0;
		const size_t outerBottomBase = ringStride;
		const size_t innerTopBase = (ringStride * 2);
		const size_t innerBottomBase = (ringStride * 3);
		const size_t topOuterBase = (ringStride * 4);
		const size_t topInnerBase = (topOuterBase + segments);
		const size_t bottomOuterBase = (topInnerBase + segments);
		const size_t bottomInnerBase = (bottomOuterBase + segments);

		for (uint32 i = 0; i <= segments; ++i)
		{
			const float u = (i * inverseSegments);
			const CircleSample outerSample = circle[i];
			const Float3 outerNormal{ outerSample.cos, 0.0f, outerSample.sin };
			const Float4 outerTangent{ -outerSample.sin, 0.0f, outerSample.cos, 1.0f };
			mesh.vertices[outerTopBase + i] = Vertex3D{
				.pos = Float3{ (outerRadius * outerSample.cos), halfHeight, (outerRadius * outerSample.sin) },
				.normal = outerNormal,
				.tex = Float2{ u, 0.0f },
				.tangent = outerTangent
			};
			mesh.vertices[outerBottomBase + i] = Vertex3D{
				.pos = Float3{ (outerRadius * outerSample.cos), -halfHeight, (outerRadius * outerSample.sin) },
				.normal = outerNormal,
				.tex = Float2{ u, 1.0f },
				.tangent = outerTangent
			};

			const CircleSample innerSample = circle[segments - i];
			const Float3 innerNormal{ -innerSample.cos, 0.0f, -innerSample.sin };
			const Float4 innerTangent{ innerSample.sin, 0.0f, -innerSample.cos, 1.0f };
			mesh.vertices[innerTopBase + i] = Vertex3D{
				.pos = Float3{ (innerRadius * innerSample.cos), halfHeight, (innerRadius * innerSample.sin) },
				.normal = innerNormal,
				.tex = Float2{ u, 0.0f },
				.tangent = innerTangent
			};
			mesh.vertices[innerBottomBase + i] = Vertex3D{
				.pos = Float3{ (innerRadius * innerSample.cos), -halfHeight, (innerRadius * innerSample.sin) },
				.normal = innerNormal,
				.tex = Float2{ u, 1.0f },
				.tangent = innerTangent
			};
		}

		for (uint32 i = 0; i < segments; ++i)
		{
			const CircleSample sample = circle[i];
			const Float3 outerPosition{ (outerRadius * sample.cos), 0.0f, (outerRadius * sample.sin) };
			const Float3 innerPosition{ (innerRadius * sample.cos), 0.0f, (innerRadius * sample.sin) };
			const Float2 topOuterUV{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) };
			const Float2 topInnerUV{ (0.5f + (innerUVScale * sample.cos)), (0.5f - (innerUVScale * sample.sin)) };
			const Float2 bottomOuterUV{ (0.5f + (0.5f * sample.cos)), (0.5f + (0.5f * sample.sin)) };
			const Float2 bottomInnerUV{ (0.5f + (innerUVScale * sample.cos)), (0.5f + (innerUVScale * sample.sin)) };

			mesh.vertices[topOuterBase + i] = Vertex3D{
				.pos = Float3{ outerPosition.x, halfHeight, outerPosition.z },
				.normal = Float3::UnitY(),
				.tex = topOuterUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			mesh.vertices[topInnerBase + i] = Vertex3D{
				.pos = Float3{ innerPosition.x, halfHeight, innerPosition.z },
				.normal = Float3::UnitY(),
				.tex = topInnerUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			mesh.vertices[bottomOuterBase + i] = Vertex3D{
				.pos = Float3{ outerPosition.x, -halfHeight, outerPosition.z },
				.normal = -Float3::UnitY(),
				.tex = bottomOuterUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			mesh.vertices[bottomInnerBase + i] = Vertex3D{
				.pos = Float3{ innerPosition.x, -halfHeight, innerPosition.z },
				.normal = -Float3::UnitY(),
				.tex = bottomInnerUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (uint32 i = 0; i < segments; ++i)
		{
			const uint32 outerTopLeft = static_cast<uint32>(outerTopBase + i);
			const uint32 outerTopRight = (outerTopLeft + 1);
			const uint32 outerBottomLeft = static_cast<uint32>(outerBottomBase + i);
			const uint32 outerBottomRight = (outerBottomLeft + 1);
			*pTriangle++ = TriangleIndex32{ outerTopLeft, outerTopRight, outerBottomLeft };
			*pTriangle++ = TriangleIndex32{ outerBottomLeft, outerTopRight, outerBottomRight };

			const uint32 innerTopLeft = static_cast<uint32>(innerTopBase + i);
			const uint32 innerTopRight = (innerTopLeft + 1);
			const uint32 innerBottomLeft = static_cast<uint32>(innerBottomBase + i);
			const uint32 innerBottomRight = (innerBottomLeft + 1);
			*pTriangle++ = TriangleIndex32{ innerTopLeft, innerTopRight, innerBottomLeft };
			*pTriangle++ = TriangleIndex32{ innerBottomLeft, innerTopRight, innerBottomRight };

			const uint32 next = ((i + 1) % segments);
			const uint32 topOuterCurrent = static_cast<uint32>(topOuterBase + i);
			const uint32 topOuterNext = static_cast<uint32>(topOuterBase + next);
			const uint32 topInnerCurrent = static_cast<uint32>(topInnerBase + i);
			const uint32 topInnerNext = static_cast<uint32>(topInnerBase + next);
			*pTriangle++ = TriangleIndex32{ topOuterCurrent, topInnerCurrent, topOuterNext };
			*pTriangle++ = TriangleIndex32{ topInnerCurrent, topInnerNext, topOuterNext };

			const uint32 bottomOuterCurrent = static_cast<uint32>(bottomOuterBase + i);
			const uint32 bottomOuterNext = static_cast<uint32>(bottomOuterBase + next);
			const uint32 bottomInnerCurrent = static_cast<uint32>(bottomInnerBase + i);
			const uint32 bottomInnerNext = static_cast<uint32>(bottomInnerBase + next);
			*pTriangle++ = TriangleIndex32{ bottomOuterCurrent, bottomOuterNext, bottomInnerCurrent };
			*pTriangle++ = TriangleIndex32{ bottomInnerCurrent, bottomOuterNext, bottomInnerNext };
		}

		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	ConicalFrustum
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::ConicalFrustum(
		const double _bottomRadius,
		const double _topRadius,
		const double _height,
		const uint32 segments)
	{
		if ((not IsFloatRepresentable(_bottomRadius))
			|| (not IsFloatRepresentable(_topRadius))
			|| (not IsFloatRepresentable(_height)))
		{
			return GenerationFailed("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The radii and height must be finite and float-representable");
		}

		const float bottomRadius = static_cast<float>(_bottomRadius);
		const float topRadius = static_cast<float>(_topRadius);
		const float height = static_cast<float>(_height);
		if ((bottomRadius <= 0.0f)
			|| (topRadius < 0.0f)
			|| (height <= 0.0f)
			|| (segments < 3))
		{
			return GenerationFailed("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The radii, height, or segment count is invalid");
		}

		const bool isCone = (topRadius == 0.0f);
		size_t ringStride;
		size_t vertexCount;
		size_t triangleCount;

		if (not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
		{
			return GenerationFailed("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): segments exceed the supported range");
		}

		if (isCone)
		{
			size_t scaledSegments;
			if ((not CheckedMultiply(static_cast<size_t>(segments), 3, scaledSegments))
				|| (not CheckedAdd(scaledSegments, 2, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 2, triangleCount)))
			{
				return GenerationFailed("Mesh3D::Cone(): The generated mesh exceeds the supported size");
			}
		}
		else
		{
			if ((not CheckedMultiply(ringStride, 4, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 4, triangleCount)))
			{
				return GenerationFailed("Mesh3D::ConicalFrustum()/Cylinder(): The generated mesh exceeds the supported size");
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return GenerationFailed("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The generated vertex count exceeds the supported range");
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const Array<CircleSample> circle = Mesh3DDetail::MakeCircleSamples<float>(segments);
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

	Mesh3D Mesh3D::Cylinder(const double radius, const double height, const uint32 segments)
	{
		return ConicalFrustum(radius, radius, height, segments);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Cone
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Cone(const double radius, const double height, const uint32 segments)
	{
		return ConicalFrustum(radius, 0.0f, height, segments);
	}
}
