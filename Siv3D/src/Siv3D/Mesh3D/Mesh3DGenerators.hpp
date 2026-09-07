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

# pragma once
# include <Siv3D/Mesh3D.hpp>

// Destination-writing implementations shared by factories and Mesh3DBuilder.
namespace s3d::Mesh3DDetail
{
	// Validates and copies once; public callers choose logging and empty-input policy.
	[[nodiscard]]
	Result<Mesh3DRange, Mesh3DErrorCode> AppendMesh(Mesh3D& destination, const Mesh3D& source);

	[[nodiscard]]
	Mesh3DAddResult AppendBox(
		Mesh3D& mesh,
		const Vec3 size,
		const BoxFace faces);
	[[nodiscard]]
	Mesh3DAddResult AppendBox(
		Mesh3D& mesh,
		const Vec3 _size,
		const BoxUVMapping& uvMapping,
		const BoxFace selectedFaces);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 outerSize,
		const double thickness,
		const BoxFace openFaces);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 outerSize,
		const Vec3 thickness,
		const BoxFace openFaces);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 outerSize,
		const double thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxShell(
		Mesh3D& mesh,
		const Vec3 _outerSize,
		const Vec3 _thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 size,
		const double thickness);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 size,
		const Vec3 beamSize);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 size,
		const double thickness,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendBoxFrame(
		Mesh3D& mesh,
		const Vec3 _size,
		const Vec3 _beamSize,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendRoundedBox(
		Mesh3D& mesh,
		const Vec3 size,
		const double radius,
		const uint32 subdivisions);
	[[nodiscard]]
	Mesh3DAddResult AppendRoundedBox(
		Mesh3D& mesh,
		const Vec3 _size,
		const double _radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendChamferedBox(
		Mesh3D& mesh,
		const Vec3 size,
		const double chamfer);
	[[nodiscard]]
	Mesh3DAddResult AppendChamferedBox(
		Mesh3D& mesh,
		const Vec3 _size,
		const double _chamfer,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendWedge(
		Mesh3D& mesh,
		const Vec3 size);
	[[nodiscard]]
	Mesh3DAddResult AppendWedge(
		Mesh3D& mesh,
		const Vec3 _size,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendTriangularPrism(
		Mesh3D& mesh,
		const Vec3 size);
	[[nodiscard]]
	Mesh3DAddResult AppendTriangularPrism(
		Mesh3D& mesh,
		const Vec3 _size,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendStairs(
		Mesh3D& mesh,
		const Vec3 size,
		const uint32 steps);
	[[nodiscard]]
	Mesh3DAddResult AppendStairs(
		Mesh3D& mesh,
		const Vec3 _size,
		const uint32 steps,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendPyramid(
		Mesh3D& mesh,
		const double baseSize,
		const double height);
	[[nodiscard]]
	Mesh3DAddResult AppendPyramid(
		Mesh3D& mesh,
		const SizeF _baseSizeXZ,
		const double _height);
	[[nodiscard]]
	Mesh3DAddResult AppendRectangularFrustum(
		Mesh3D& mesh,
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height);
	[[nodiscard]]
	Mesh3DAddResult AppendRectangularFrustum(
		Mesh3D& mesh,
		const SizeF _bottomSizeXZ,
		const SizeF _topSizeXZ,
		const double _height,
		const BoxUVMapping& uvMapping);
	[[nodiscard]]
	Mesh3DAddResult AppendTetrahedron(
		Mesh3D& mesh,
		const double _radius);
	[[nodiscard]]
	Mesh3DAddResult AppendOctahedron(
		Mesh3D& mesh,
		const double _radius);
	[[nodiscard]]
	Mesh3DAddResult AppendIcosahedron(
		Mesh3D& mesh,
		const double _radius);
	[[nodiscard]]
	Mesh3DAddResult AppendIcoSphere(
		Mesh3D& mesh,
		const double _radius,
		const uint32 subdivisions);
	[[nodiscard]]
	Mesh3DAddResult AppendDodecahedron(
		Mesh3D& mesh,
		const double _radius);
	[[nodiscard]]
	Mesh3DAddResult AppendPlane(
		Mesh3D& mesh,
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset);
	[[nodiscard]]
	Mesh3DAddResult AppendGrid(
		Mesh3D& mesh,
		const SizeF _sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 _uvScale,
		const Vec2 _uvOffset);
	[[nodiscard]]
	Mesh3DAddResult AppendDisc(
		Mesh3D& mesh,
		const double radius,
		const uint32 segments);
	[[nodiscard]]
	Mesh3DAddResult AppendAnnulus(
		Mesh3D& mesh,
		const double _innerRadius,
		const double _outerRadius,
		const uint32 segments);
	[[nodiscard]]
	Mesh3DAddResult AppendHollowCylinder(
		Mesh3D& mesh,
		const double _innerRadius,
		const double _outerRadius,
		const double _height,
		const uint32 segments);
	[[nodiscard]]
	Mesh3DAddResult AppendConicalFrustum(
		Mesh3D& mesh,
		const double _bottomRadius,
		const double _topRadius,
		const double _height,
		const uint32 segments);
	[[nodiscard]]
	Mesh3DAddResult AppendCylinder(
		Mesh3D& mesh,
		const double radius,
		const double height,
		const uint32 segments);
	[[nodiscard]]
	Mesh3DAddResult AppendCone(
		Mesh3D& mesh,
		const double radius,
		const double height,
		const uint32 segments);
	[[nodiscard]]
	Mesh3DAddResult AppendTorus(
		Mesh3D& mesh,
		const double _majorRadius,
		const double _tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments);
	[[nodiscard]]
	Mesh3DAddResult AppendSphere(
		Mesh3D& mesh,
		const double _radius,
		const uint32 slices,
		const uint32 stacks);
	[[nodiscard]]
	Mesh3DAddResult AppendHemisphere(
		Mesh3D& mesh,
		const double radius,
		const uint32 slices,
		const uint32 stacks);
	[[nodiscard]]
	Mesh3DAddResult AppendHemisphere(
		Mesh3D& mesh,
		const double _radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks);
	[[nodiscard]]
	Mesh3DAddResult AppendCapsule(
		Mesh3D& mesh,
		const double _radius,
		const double _cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks);

	[[nodiscard]]
	Mesh3DAddResult AppendExtrude(
		Mesh3D& mesh,
		const Polygon& polygon,
		double height,
		double smoothingAngle);

	[[nodiscard]]
	Mesh3DAddResult AppendRevolve(
		Mesh3D& mesh,
		std::span<const Vec2> profile,
		const RevolveOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendTube(
		Mesh3D& mesh,
		std::span<const Vec3> path,
		double radius,
		const TubeOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendTube(
		Mesh3D& mesh,
		std::span<const Vec3> path,
		std::span<const double> radii,
		const TubeOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendSweep(
		Mesh3D& mesh,
		const Polygon& crossSection,
		std::span<const Vec3> path,
		const SweepOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendSweep(
		Mesh3D& mesh,
		const Polygon& crossSection,
		std::span<const Vec3> path,
		std::span<const SweepSectionTransform> sectionTransforms,
		const SweepOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendHeightField(
		Mesh3D& mesh,
		const Grid<float>& heights,
		SizeF sizeXZ,
		const HeightFieldOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendHeightField(
		Mesh3D& mesh,
		Size gridSize,
		SizeF sizeXZ,
		FunctionRef<double(Point)> heightFunction,
		const HeightFieldOptions& options);

	[[nodiscard]]
	Mesh3DAddResult AppendLoft(
		Mesh3D& mesh,
		std::span<const LoftSection> sections,
		const LoftOptions& options);
}
