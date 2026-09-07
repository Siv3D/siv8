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
# include "Mesh3DGenerators.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Box
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Box(const Vec3 size, const BoxFace faces)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBox(mesh, size, faces);
		return mesh;
	}

	Mesh3D Mesh3D::Box(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const BoxFace faces)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBox(mesh, size, uvMapping, faces);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	BoxShell
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::BoxShell(
		const Vec3 outerSize,
		const double thickness,
		const BoxFace openFaces)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxShell(mesh, outerSize, thickness, openFaces);
		return mesh;
	}

	Mesh3D Mesh3D::BoxShell(
		const Vec3 outerSize,
		const Vec3 thickness,
		const BoxFace openFaces)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxShell(mesh, outerSize, thickness, openFaces);
		return mesh;
	}

	Mesh3D Mesh3D::BoxShell(
		const Vec3 outerSize,
		const double thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxShell(mesh, outerSize, thickness, uvMapping, openFaces);
		return mesh;
	}

	Mesh3D Mesh3D::BoxShell(
		const Vec3 outerSize,
		const Vec3 thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxShell(mesh, outerSize, thickness, uvMapping, openFaces);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	BoxFrame
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::BoxFrame(const Vec3 size, const double thickness)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxFrame(mesh, size, thickness);
		return mesh;
	}

	Mesh3D Mesh3D::BoxFrame(const Vec3 size, const Vec3 beamSize)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxFrame(mesh, size, beamSize);
		return mesh;
	}

	Mesh3D Mesh3D::BoxFrame(
		const Vec3 size,
		const double thickness,
		const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxFrame(mesh, size, thickness, uvMapping);
		return mesh;
	}

	Mesh3D Mesh3D::BoxFrame(
		const Vec3 size,
		const Vec3 beamSize,
		const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendBoxFrame(mesh, size, beamSize, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	RoundedBox
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::RoundedBox(const Vec3 size, const double radius, const uint32 subdivisions)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendRoundedBox(mesh, size, radius, subdivisions);
		return mesh;
	}

	Mesh3D Mesh3D::RoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendRoundedBox(mesh, size, radius, subdivisions, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	ChamferedBox
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::ChamferedBox(const Vec3 size, const double chamfer)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendChamferedBox(mesh, size, chamfer);
		return mesh;
	}

	Mesh3D Mesh3D::ChamferedBox(
		const Vec3 size,
		const double chamfer,
		const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendChamferedBox(mesh, size, chamfer, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Wedge
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Wedge(const Vec3 size)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendWedge(mesh, size);
		return mesh;
	}

	Mesh3D Mesh3D::Wedge(const Vec3 size, const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendWedge(mesh, size, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	TriangularPrism
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::TriangularPrism(const Vec3 size)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendTriangularPrism(mesh, size);
		return mesh;
	}

	Mesh3D Mesh3D::TriangularPrism(const Vec3 size, const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendTriangularPrism(mesh, size, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Stairs
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Stairs(const Vec3 size, const uint32 steps)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendStairs(mesh, size, steps);
		return mesh;
	}

	Mesh3D Mesh3D::Stairs(const Vec3 size, const uint32 steps, const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendStairs(mesh, size, steps, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Pyramid
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Pyramid(const double baseSize, const double height)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendPyramid(mesh, baseSize, height);
		return mesh;
	}

	Mesh3D Mesh3D::Pyramid(const SizeF baseSizeXZ, const double height)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendPyramid(mesh, baseSizeXZ, height);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendRectangularFrustum(mesh, bottomSizeXZ, topSizeXZ, height);
		return mesh;
	}

	Mesh3D Mesh3D::RectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const BoxUVMapping& uvMapping)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendRectangularFrustum(mesh, bottomSizeXZ, topSizeXZ, height, uvMapping);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Regular polyhedra
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Tetrahedron(const double radius)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendTetrahedron(mesh, radius);
		return mesh;
	}

	Mesh3D Mesh3D::Octahedron(const double radius)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendOctahedron(mesh, radius);
		return mesh;
	}

	Mesh3D Mesh3D::Icosahedron(const double radius)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendIcosahedron(mesh, radius);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	IcoSphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::IcoSphere(const double radius, const uint32 subdivisions)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendIcoSphere(mesh, radius, subdivisions);
		return mesh;
	}

	Mesh3D Mesh3D::Dodecahedron(const double radius)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendDodecahedron(mesh, radius);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendCapsule(mesh, radius, cylinderHeight, slices, hemisphereStacks);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Plane
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Plane(const SizeF sizeXZ, const Vec2 uvScale, const Vec2 uvOffset)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendPlane(mesh, sizeXZ, uvScale, uvOffset);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendGrid(mesh, sizeXZ, segmentsX, segmentsZ, uvScale, uvOffset);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendTorus(mesh, majorRadius, tubeRadius, ringSegments, tubeSegments);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Sphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Sphere(const double radius, const uint32 slices, const uint32 stacks)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendSphere(mesh, radius, slices, stacks);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Hemisphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Hemisphere(const double radius, const uint32 slices, const uint32 stacks)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendHemisphere(mesh, radius, slices, stacks);
		return mesh;
	}

	Mesh3D Mesh3D::Hemisphere(
		const double radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendHemisphere(mesh, radius, closeBottom, slices, stacks);
		return mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Disc
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Disc(const double radius, const uint32 segments)
	{
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendDisc(mesh, radius, segments);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendAnnulus(mesh, innerRadius, outerRadius, segments);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendHollowCylinder(mesh, innerRadius, outerRadius, height, segments);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendConicalFrustum(mesh, bottomRadius, topRadius, height, segments);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendCylinder(mesh, radius, height, segments);
		return mesh;
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
		Mesh3D mesh;
		(void)Mesh3DDetail::AppendCone(mesh, radius, height, segments);
		return mesh;
	}
}
