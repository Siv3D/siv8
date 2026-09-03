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

namespace s3d
{
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
		const double radius,
		const double cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks)
	{
		Mesh3DBuilder builder;
		builder.addCapsule(radius, cylinderHeight, slices, hemisphereStacks);
		return std::move(builder).build();
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
		const double majorRadius,
		const double tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments)
	{
		Mesh3DBuilder builder;
		builder.addTorus(majorRadius, tubeRadius, ringSegments, tubeSegments);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Sphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Sphere(const double radius, const uint32 slices, const uint32 stacks)
	{
		Mesh3DBuilder builder;
		builder.addSphere(radius, slices, stacks);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Hemisphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Hemisphere(const double radius, const uint32 slices, const uint32 stacks)
	{
		Mesh3DBuilder builder;
		builder.addHemisphere(radius, slices, stacks);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Hemisphere(
		const double radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks)
	{
		Mesh3DBuilder builder;
		builder.addHemisphere(radius, closeBottom, slices, stacks);
		return std::move(builder).build();
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
		const double innerRadius,
		const double outerRadius,
		const double height,
		const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addHollowCylinder(innerRadius, outerRadius, height, segments);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ConicalFrustum
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::ConicalFrustum(
		const double bottomRadius,
		const double topRadius,
		const double height,
		const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addConicalFrustum(bottomRadius, topRadius, height, segments);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Cylinder
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Cylinder(
		const double radius,
		const double height,
		const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addCylinder(radius, height, segments);
		return std::move(builder).build();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Cone
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Cone(
		const double radius,
		const double height,
		const uint32 segments)
	{
		Mesh3DBuilder builder;
		builder.addCone(radius, height, segments);
		return std::move(builder).build();
	}
}
