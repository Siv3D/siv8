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

# include <Siv3D/Mesh3DBuilder.hpp>
# include "../Mesh3D/Mesh3DCommon.hpp"

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::OperationFailed;

		[[nodiscard]]
		static Mesh3DAddResult TransformAddedRange(
			Mesh3D& mesh,
			Mesh3DAddResult result,
			const Mat4x4& transform) noexcept
		{
			if (result && (not result->isEmpty()))
			{
				Mesh3DDetail::TransformMeshRange(mesh, *result, transform);
			}

			return result;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addMesh
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addMesh(const Mesh3D& mesh)
	{
		if (mesh.isEmpty())
		{
			return OperationFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3DBuilder::addMesh(): mesh must not be empty");
		}
		const auto result = Mesh3DDetail::AppendMesh(m_mesh, mesh);
		if (not result)
		{
			return OperationFailed(result.error(), U"Mesh3DBuilder::addMesh(): mesh is invalid or the combined mesh exceeds the supported size");
		}
		return *result;
	}

	Mesh3DAddResult Mesh3DBuilder::addMesh(
		const Mesh3D& mesh,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(m_mesh, addMesh(mesh), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBox
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addBox(const Vec3 size, const BoxFace faces)
	{
		return Mesh3DDetail::AppendBox(m_mesh, size, faces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBox(const Vec3 size, const Mesh3DPlacement& placement, const BoxFace faces)
	{
		return addBox(size, BoxUVMapping{}, placement, faces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBox(
		const Vec3 _size,
		const BoxUVMapping& uvMapping,
		const BoxFace selectedFaces)
	{
		return Mesh3DDetail::AppendBox(m_mesh, _size, uvMapping, selectedFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBox(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement,
		const BoxFace faces)
	{
		return TransformAddedRange(
			m_mesh, addBox(size, uvMapping, faces), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBoxShell
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const double thickness,
		const BoxFace openFaces)
	{
		return Mesh3DDetail::AppendBoxShell(m_mesh, outerSize, thickness, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const double thickness,
		const Mesh3DPlacement& placement,
		const BoxFace openFaces)
	{
		return addBoxShell(outerSize, Vec3::All(thickness), BoxUVMapping{}, placement, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const Vec3 thickness,
		const BoxFace openFaces)
	{
		return Mesh3DDetail::AppendBoxShell(m_mesh, outerSize, thickness, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const Vec3 thickness,
		const Mesh3DPlacement& placement,
		const BoxFace openFaces)
	{
		return addBoxShell(outerSize, thickness, BoxUVMapping{}, placement, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const double thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces)
	{
		return Mesh3DDetail::AppendBoxShell(m_mesh, outerSize, thickness, uvMapping, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const double thickness,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement,
		const BoxFace openFaces)
	{
		return addBoxShell(outerSize, Vec3::All(thickness), uvMapping, placement, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 _outerSize,
		const Vec3 _thickness,
		const BoxUVMapping& uvMapping,
		const BoxFace openFaces)
	{
		return Mesh3DDetail::AppendBoxShell(m_mesh, _outerSize, _thickness, uvMapping, openFaces);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxShell(
		const Vec3 outerSize,
		const Vec3 thickness,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement,
		const BoxFace openFaces)
	{
		return TransformAddedRange(
			m_mesh,
			addBoxShell(outerSize, thickness, uvMapping, openFaces),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBoxFrame
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(const Vec3 size, const double thickness)
	{
		return Mesh3DDetail::AppendBoxFrame(m_mesh, size, thickness);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(
		const Vec3 size,
		const double thickness,
		const Mesh3DPlacement& placement)
	{
		return addBoxFrame(size, Vec3::All(thickness), BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(const Vec3 size, const Vec3 beamSize)
	{
		return Mesh3DDetail::AppendBoxFrame(m_mesh, size, beamSize);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(
		const Vec3 size,
		const Vec3 beamSize,
		const Mesh3DPlacement& placement)
	{
		return addBoxFrame(size, beamSize, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(
		const Vec3 size,
		const double thickness,
		const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendBoxFrame(m_mesh, size, thickness, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(
		const Vec3 size,
		const double thickness,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return addBoxFrame(size, Vec3::All(thickness), uvMapping, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(
		const Vec3 _size,
		const Vec3 _beamSize,
		const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendBoxFrame(m_mesh, _size, _beamSize, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addBoxFrame(
		const Vec3 size,
		const Vec3 beamSize,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addBoxFrame(size, beamSize, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundedBox
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions)
	{
		return Mesh3DDetail::AppendRoundedBox(m_mesh, size, radius, subdivisions);
	}

	Mesh3DAddResult Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const Mesh3DPlacement& placement)
	{
		return addRoundedBox(size, radius, subdivisions, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addRoundedBox(
		const Vec3 _size,
		const double _radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendRoundedBox(m_mesh, _size, _radius, subdivisions, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addRoundedBox(size, radius, subdivisions, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addChamferedBox
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addChamferedBox(const Vec3 size, const double chamfer)
	{
		return Mesh3DDetail::AppendChamferedBox(m_mesh, size, chamfer);
	}

	Mesh3DAddResult Mesh3DBuilder::addChamferedBox(
		const Vec3 size,
		const double chamfer,
		const Mesh3DPlacement& placement)
	{
		return addChamferedBox(size, chamfer, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addChamferedBox(
		const Vec3 _size,
		const double _chamfer,
		const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendChamferedBox(m_mesh, _size, _chamfer, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addChamferedBox(
		const Vec3 size,
		const double chamfer,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addChamferedBox(size, chamfer, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addWedge
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addWedge(const Vec3 size)
	{
		return Mesh3DDetail::AppendWedge(m_mesh, size);
	}

	Mesh3DAddResult Mesh3DBuilder::addWedge(const Vec3 size, const Mesh3DPlacement& placement)
	{
		return addWedge(size, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addWedge(const Vec3 _size, const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendWedge(m_mesh, _size, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addWedge(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addWedge(size, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addExtrude
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const ExtrudeOptions& options)
	{
		return Mesh3DDetail::AppendExtrude(m_mesh, polygon, height, options.smoothingAngle);
	}

	Mesh3DAddResult Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const Mesh3DPlacement& placement,
		const ExtrudeOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendExtrude(m_mesh, polygon, height, options.smoothingAngle),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRevolve
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addRevolve(
		const std::span<const Vec2> profile,
		const RevolveOptions& options)
	{
		return Mesh3DDetail::AppendRevolve(m_mesh, profile, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addRevolve(
		const std::initializer_list<Vec2> profile,
		const RevolveOptions& options)
	{
		return addRevolve(std::span<const Vec2>{ profile.begin(), profile.size() }, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addRevolve(
		const std::span<const Vec2> profile,
		const Mesh3DPlacement& placement,
		const RevolveOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendRevolve(m_mesh, profile, options),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTube
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addTube(
		const std::span<const Vec3> path,
		const double radius,
		const TubeOptions& options)
	{
		return Mesh3DDetail::AppendTube(m_mesh, path, radius, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addTube(
		const std::initializer_list<Vec3> path,
		const double radius,
		const TubeOptions& options)
	{
		return addTube(
			std::span<const Vec3>{ path.begin(), path.size() }, radius, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addTube(
		const std::span<const Vec3> path,
		const std::span<const double> radii,
		const TubeOptions& options)
	{
		return Mesh3DDetail::AppendTube(m_mesh, path, radii, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addTube(
		const std::initializer_list<Vec3> path,
		const std::initializer_list<double> radii,
		const TubeOptions& options)
	{
		return addTube(
			std::span<const Vec3>{ path.begin(), path.size() },
			std::span<const double>{ radii.begin(), radii.size() },
			options);
	}

	Mesh3DAddResult Mesh3DBuilder::addTube(
		const std::span<const Vec3> path,
		const double radius,
		const Mesh3DPlacement& placement,
		const TubeOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendTube(m_mesh, path, radius, options),
			placement.getTransform());
	}

	Mesh3DAddResult Mesh3DBuilder::addTube(
		const std::span<const Vec3> path,
		const std::span<const double> radii,
		const Mesh3DPlacement& placement,
		const TubeOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendTube(m_mesh, path, radii, options),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSweep
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addSweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const SweepOptions& options)
	{
		return Mesh3DDetail::AppendSweep(m_mesh, crossSection, path, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addSweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const SweepOptions& options)
	{
		return addSweep(
			crossSection, std::span<const Vec3>{ path.begin(), path.size() }, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addSweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const Mesh3DPlacement& placement,
		const SweepOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendSweep(m_mesh, crossSection, path, options),
			placement.getTransform());
	}

	Mesh3DAddResult Mesh3DBuilder::addSweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const std::span<const SweepSectionTransform> sectionTransforms,
		const SweepOptions& options)
	{
		return Mesh3DDetail::AppendSweep(
			m_mesh, crossSection, path, sectionTransforms, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addSweep(
		const Polygon& crossSection,
		const std::initializer_list<Vec3> path,
		const std::initializer_list<SweepSectionTransform> sectionTransforms,
		const SweepOptions& options)
	{
		return addSweep(
			crossSection,
			std::span<const Vec3>{ path.begin(), path.size() },
			std::span<const SweepSectionTransform>{
				sectionTransforms.begin(), sectionTransforms.size() },
			options);
	}

	Mesh3DAddResult Mesh3DBuilder::addSweep(
		const Polygon& crossSection,
		const std::span<const Vec3> path,
		const std::span<const SweepSectionTransform> sectionTransforms,
		const Mesh3DPlacement& placement,
		const SweepOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendSweep(
				m_mesh, crossSection, path, sectionTransforms, options),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTriangularPrism
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addTriangularPrism(const Vec3 size)
	{
		return Mesh3DDetail::AppendTriangularPrism(m_mesh, size);
	}

	Mesh3DAddResult Mesh3DBuilder::addTriangularPrism(const Vec3 size, const Mesh3DPlacement& placement)
	{
		return addTriangularPrism(size, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addTriangularPrism(const Vec3 _size, const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendTriangularPrism(m_mesh, _size, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addTriangularPrism(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addTriangularPrism(size, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addStairs
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addStairs(const Vec3 size, const uint32 steps)
	{
		return Mesh3DDetail::AppendStairs(m_mesh, size, steps);
	}

	Mesh3DAddResult Mesh3DBuilder::addStairs(const Vec3 size, const uint32 steps, const Mesh3DPlacement& placement)
	{
		return addStairs(size, steps, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addStairs(
		const Vec3 _size,
		const uint32 steps,
		const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendStairs(m_mesh, _size, steps, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addStairs(
		const Vec3 size,
		const uint32 steps,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addStairs(size, steps, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPyramid
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addPyramid(const double baseSize, const double height)
	{
		return Mesh3DDetail::AppendPyramid(m_mesh, baseSize, height);
	}

	Mesh3DAddResult Mesh3DBuilder::addPyramid(
		const double baseSize,
		const double height,
		const Mesh3DPlacement& placement)
	{
		return addPyramid(SizeF{ baseSize, baseSize }, height, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addPyramid(const SizeF _baseSizeXZ, const double _height)
	{
		return Mesh3DDetail::AppendPyramid(m_mesh, _baseSizeXZ, _height);
	}

	Mesh3DAddResult Mesh3DBuilder::addPyramid(
		const SizeF baseSizeXZ,
		const double height,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addPyramid(baseSizeXZ, height),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectangularFrustum
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height)
	{
		return Mesh3DDetail::AppendRectangularFrustum(m_mesh, bottomSizeXZ, topSizeXZ, height);
	}

	Mesh3DAddResult Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const Mesh3DPlacement& placement)
	{
		return addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, BoxUVMapping{}, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addRectangularFrustum(
		const SizeF _bottomSizeXZ,
		const SizeF _topSizeXZ,
		const double _height,
		const BoxUVMapping& uvMapping)
	{
		return Mesh3DDetail::AppendRectangularFrustum(m_mesh, _bottomSizeXZ, _topSizeXZ, _height, uvMapping);
	}

	Mesh3DAddResult Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const BoxUVMapping& uvMapping,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, uvMapping),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTetrahedron
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addTetrahedron(const double _radius)
	{
		return Mesh3DDetail::AppendTetrahedron(m_mesh, _radius);
	}

	Mesh3DAddResult Mesh3DBuilder::addTetrahedron(const double radius, const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(m_mesh, addTetrahedron(radius), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addOctahedron
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addOctahedron(const double _radius)
	{
		return Mesh3DDetail::AppendOctahedron(m_mesh, _radius);
	}

	Mesh3DAddResult Mesh3DBuilder::addOctahedron(const double radius, const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(m_mesh, addOctahedron(radius), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addIcosahedron
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addIcosahedron(const double _radius)
	{
		return Mesh3DDetail::AppendIcosahedron(m_mesh, _radius);
	}

	Mesh3DAddResult Mesh3DBuilder::addIcosahedron(const double radius, const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(m_mesh, addIcosahedron(radius), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addIcoSphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addIcoSphere(
		const double _radius,
		const uint32 subdivisions)
	{
		return Mesh3DDetail::AppendIcoSphere(m_mesh, _radius, subdivisions);
	}

	Mesh3DAddResult Mesh3DBuilder::addIcoSphere(
		const double radius,
		const uint32 subdivisions,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh, addIcoSphere(radius, subdivisions), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addDodecahedron
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addDodecahedron(const double _radius)
	{
		return Mesh3DDetail::AppendDodecahedron(m_mesh, _radius);
	}

	Mesh3DAddResult Mesh3DBuilder::addDodecahedron(const double radius, const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(m_mesh, addDodecahedron(radius), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPlane
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return Mesh3DDetail::AppendPlane(m_mesh, sizeXZ, uvScale, uvOffset);
	}

	Mesh3DAddResult Mesh3DBuilder::addPlane(const SizeF sizeXZ, const Mesh3DPlacement& placement)
	{
		return addPlane(sizeXZ, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Mesh3DPlacement& placement)
	{
		return addGrid(sizeXZ, 1, 1, uvScale, uvOffset, placement);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addGrid
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addGrid(
		const SizeF _sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 _uvScale,
		const Vec2 _uvOffset)
	{
		return Mesh3DDetail::AppendGrid(m_mesh, _sizeXZ, segmentsX, segmentsZ, _uvScale, _uvOffset);
	}

	Mesh3DAddResult Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Mesh3DPlacement& placement)
	{
		return addGrid(sizeXZ, segmentsX, segmentsZ,
			Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addGrid(sizeXZ, segmentsX, segmentsZ, uvScale, uvOffset),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addHeightField
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addHeightField(
		const Grid<float>& heights,
		const SizeF sizeXZ,
		const HeightFieldOptions& options)
	{
		return Mesh3DDetail::AppendHeightField(m_mesh, heights, sizeXZ, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addHeightField(
		const Grid<float>& heights,
		const SizeF sizeXZ,
		const Mesh3DPlacement& placement,
		const HeightFieldOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendHeightField(m_mesh, heights, sizeXZ, options),
			placement.getTransform());
	}

	Mesh3DAddResult Mesh3DBuilder::addHeightField(
		const Size gridSize,
		const SizeF sizeXZ,
		const FunctionRef<double(Point)> heightFunction,
		const HeightFieldOptions& options)
	{
		return Mesh3DDetail::AppendHeightField(
			m_mesh, gridSize, sizeXZ, heightFunction, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addHeightField(
		const Size gridSize,
		const SizeF sizeXZ,
		const FunctionRef<double(Point)> heightFunction,
		const Mesh3DPlacement& placement,
		const HeightFieldOptions& options)
	{
		return TransformAddedRange(
			m_mesh,
			Mesh3DDetail::AppendHeightField(
				m_mesh, gridSize, sizeXZ, heightFunction, options),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addLoft
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addLoft(const std::span<const LoftSection> sections, const LoftOptions& options)
	{
		return Mesh3DDetail::AppendLoft(m_mesh, sections, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addLoft(const std::initializer_list<LoftSection> sections, const LoftOptions& options)
	{
		return addLoft(std::span<const LoftSection>{ sections.begin(), sections.size() }, options);
	}

	Mesh3DAddResult Mesh3DBuilder::addLoft(const std::span<const LoftSection> sections,
		const Mesh3DPlacement& placement, const LoftOptions& options)
	{
		return TransformAddedRange(m_mesh, addLoft(sections, options), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addDisc
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addDisc(const double radius, const uint32 segments)
	{
		return Mesh3DDetail::AppendDisc(m_mesh, radius, segments);
	}

	Mesh3DAddResult Mesh3DBuilder::addDisc(
		const double radius,
		const uint32 segments,
		const Mesh3DPlacement& placement)
	{
		return addAnnulus(0.0, radius, segments, placement);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addAnnulus
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addAnnulus(
		const double _innerRadius,
		const double _outerRadius,
		const uint32 segments)
	{
		return Mesh3DDetail::AppendAnnulus(m_mesh, _innerRadius, _outerRadius, segments);
	}

	Mesh3DAddResult Mesh3DBuilder::addAnnulus(
		const double innerRadius,
		const double outerRadius,
		const uint32 segments,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addAnnulus(innerRadius, outerRadius, segments),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addHollowCylinder
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addHollowCylinder(
		const double _innerRadius,
		const double _outerRadius,
		const double _height,
		const uint32 segments)
	{
		return Mesh3DDetail::AppendHollowCylinder(m_mesh, _innerRadius, _outerRadius, _height, segments);
	}

	Mesh3DAddResult Mesh3DBuilder::addHollowCylinder(
		const double innerRadius,
		const double outerRadius,
		const double height,
		const uint32 segments,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addHollowCylinder(innerRadius, outerRadius, height, segments),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addConicalFrustum
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addConicalFrustum(
		const double _bottomRadius,
		const double _topRadius,
		const double _height,
		const uint32 segments)
	{
		return Mesh3DDetail::AppendConicalFrustum(m_mesh, _bottomRadius, _topRadius, _height, segments);
	}

	Mesh3DAddResult Mesh3DBuilder::addConicalFrustum(
		const double bottomRadius,
		const double topRadius,
		const double height,
		const uint32 segments,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addConicalFrustum(bottomRadius, topRadius, height, segments),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCylinder
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addCylinder(
		const double radius,
		const double height,
		const uint32 segments)
	{
		return Mesh3DDetail::AppendCylinder(m_mesh, radius, height, segments);
	}

	Mesh3DAddResult Mesh3DBuilder::addCylinder(
		const double radius,
		const double height,
		const uint32 segments,
		const Mesh3DPlacement& placement)
	{
		return addConicalFrustum(radius, radius, height, segments, placement);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCone
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addCone(
		const double radius,
		const double height,
		const uint32 segments)
	{
		return Mesh3DDetail::AppendCone(m_mesh, radius, height, segments);
	}

	Mesh3DAddResult Mesh3DBuilder::addCone(
		const double radius,
		const double height,
		const uint32 segments,
		const Mesh3DPlacement& placement)
	{
		return addConicalFrustum(radius, 0.0, height, segments, placement);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTorus
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addTorus(
		const double _majorRadius,
		const double _tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments)
	{
		return Mesh3DDetail::AppendTorus(m_mesh, _majorRadius, _tubeRadius, ringSegments, tubeSegments);
	}

	Mesh3DAddResult Mesh3DBuilder::addTorus(
		const double majorRadius,
		const double tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addTorus(majorRadius, tubeRadius, ringSegments, tubeSegments),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addSphere(
		const double _radius,
		const uint32 slices,
		const uint32 stacks)
	{
		return Mesh3DDetail::AppendSphere(m_mesh, _radius, slices, stacks);
	}

	Mesh3DAddResult Mesh3DBuilder::addSphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(m_mesh, addSphere(radius, slices, stacks), placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addHemisphere
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addHemisphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks)
	{
		return Mesh3DDetail::AppendHemisphere(m_mesh, radius, slices, stacks);
	}

	Mesh3DAddResult Mesh3DBuilder::addHemisphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Mesh3DPlacement& placement)
	{
		return addHemisphere(radius, CloseBottom::No, slices, stacks, placement);
	}

	Mesh3DAddResult Mesh3DBuilder::addHemisphere(
		const double _radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks)
	{
		return Mesh3DDetail::AppendHemisphere(m_mesh, _radius, closeBottom, slices, stacks);
	}

	Mesh3DAddResult Mesh3DBuilder::addHemisphere(
		const double radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addHemisphere(radius, closeBottom, slices, stacks),
			placement.getTransform());
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCapsule
	//
	////////////////////////////////////////////////////////////////

	Mesh3DAddResult Mesh3DBuilder::addCapsule(
		const double _radius,
		const double _cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks)
	{
		return Mesh3DDetail::AppendCapsule(m_mesh, _radius, _cylinderHeight, slices, hemisphereStacks);
	}

	Mesh3DAddResult Mesh3DBuilder::addCapsule(
		const double radius,
		const double cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks,
		const Mesh3DPlacement& placement)
	{
		return TransformAddedRange(
			m_mesh,
			addCapsule(radius, cylinderHeight, slices, hemisphereStacks),
			placement.getTransform());
	}
}
