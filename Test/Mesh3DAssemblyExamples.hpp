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
# include <Siv3D.hpp>

// These recipes are also reproduced as a self-contained manual test.
namespace Mesh3DAssemblyExamples
{
	using Assembly = s3d::Mesh3DAssembly;

	struct Chest
	{
		Assembly assembly;
		Assembly::PartID hinge;
		Assembly::PartID lid;
		Assembly::MaterialID metal;
	};

	inline Chest MakeChest()
	{
		Chest result;
		auto& a = result.assembly;
		const auto wood = a.addMaterial(s3d::Material{
			.name = U"wood", .baseColor = s3d::ColorF{ 0.40, 0.18, 0.06 }, .roughness = 0.85 });
		result.metal = a.addMaterial(s3d::Material{
			.name = U"iron", .baseColor = s3d::ColorF{ 0.25, 0.28, 0.32 }, .metallic = 0.9, .roughness = 0.3 });
		const auto shell = a.addMesh(s3d::Mesh3D::BoxShell(s3d::Vec3{ 2.4, 1.2, 1.4 }, 0.12, s3d::BoxFace::PositiveY)).value();
		const auto lid = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ 2.4, 0.16, 1.4 })).value();
		const auto strap = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ 0.12, 0.035, 1.3 })).value();
		const auto lock = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ 0.25, 0.3, 0.08 })).value();
		(void)a.addPart({ .name = U"body", .mesh = shell, .material = wood,
			.placement = s3d::Vec3{ 0, 0.6, 0 } }).value();

		// The rear top edge is the hinge. The lid's front extends in local -Z.
		result.hinge = a.addPart({ .name = U"hinge", .placement = s3d::Vec3{ 0, 1.2, 0.7 } }).value();
		result.lid = a.addPart({ .name = U"lid", .mesh = lid, .material = wood, .parent = result.hinge,
			.placement = s3d::Vec3{ 0, 0.08, -0.7 } }).value();
		for (const double x : { -0.8, 0.8 })
		{
			(void)a.addPart({ .name = U"lid strap", .mesh = strap, .material = result.metal, .parent = result.lid,
				.placement = s3d::Vec3{ x, 0.085, 0 } }).value();
		}
		(void)a.addPart({ .name = U"lock", .mesh = lock, .material = result.metal, .parent = result.lid,
			.placement = s3d::Vec3{ 0, -0.04, -0.72 } }).value();
		return result;
	}

	inline void OpenChest(Chest& chest, const double angle)
	{
		chest.assembly.setPlacement(chest.hinge,
			{ s3d::Vec3{ 0, 1.2, 0.7 }, s3d::Quaternion::RotateX(angle) }).value();
	}

	struct Cart
	{
		Assembly assembly;
		Assembly::MeshID wheelMesh;
		s3d::Array<Assembly::PartID> axles;
		s3d::Array<Assembly::PartID> wheels;
		double track = 1.8;
		double radius = 0.4;
	};

	inline Cart MakeCart(const double track = 1.8, const double radius = 0.4, const s3d::uint32 axleCount = 2)
	{
		Cart result;
		result.track = track;
		result.radius = radius;
		auto& a = result.assembly;
		const auto paint = a.addMaterial(s3d::Material{
			.name = U"paint", .baseColor = s3d::ColorF{ 0.1, 0.45, 0.65 }, .roughness = 0.5 });
		const auto rubber = a.addMaterial(s3d::Material{
			.name = U"rubber", .baseColor = s3d::ColorF{ 0.06 }, .roughness = 0.95 });
		const auto metal = a.addMaterial(s3d::Material{
			.name = U"steel", .baseColor = s3d::ColorF{ 0.5 }, .metallic = 0.9, .roughness = 0.3 });
		result.wheelMesh = a.addMesh(s3d::Mesh3D::Cylinder(radius, 0.22, 16)).value();
		const auto axleMesh = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ track, 0.12, 0.12 })).value();
		const auto deckMesh = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ (track - 0.32), 0.18, 2.8 })).value();
		(void)a.addPart({ .name = U"deck", .mesh = deckMesh, .material = paint,
			.placement = s3d::Vec3{ 0, (radius + 0.14), 0 } }).value();
		for (s3d::uint32 i = 0; i < axleCount; ++i)
		{
			const double z = ((axleCount == 1) ? 0.0 : (-0.9 + (1.8 * i / (axleCount - 1))));
			const auto axle = a.addPart({ .name = U"axle", .mesh = axleMesh, .material = metal,
				.placement = s3d::Vec3{ 0, radius, z } }).value();
			result.axles << axle;
			for (const double side : { -1.0, 1.0 })
			{
				result.wheels << a.addPart({ .name = ((side < 0) ? U"left wheel" : U"right wheel"),
					.mesh = result.wheelMesh, .material = rubber, .parent = axle,
					.placement = s3d::Mesh3DPlacement{
						s3d::Vec3{ (side * track / 2), 0, 0 }, s3d::Quaternion::RotateZ(s3d::Math::HalfPi) } }).value();
			}
		}
		return result;
	}

	inline s3d::Mesh3D ExtractPart(const Assembly::BakedMesh& baked, const Assembly::BakedPart& part)
	{
		const auto& r = part.range;
		if (r.isEmpty())
		{
			return{};
		}
		s3d::Mesh3D result{
			std::span<const s3d::Vertex3D>{ (baked.mesh.vertices.data() + r.vertexOffset), r.vertexCount },
			std::span<const s3d::TriangleIndex32>{ (baked.mesh.indices.data() + r.triangleOffset), r.triangleCount } };
		const auto base = static_cast<s3d::uint32>(r.vertexOffset);
		for (auto& triangle : result.indices)
		{
			triangle.i0 -= base;
			triangle.i1 -= base;
			triangle.i2 -= base;
		}
		return result;
	}

	// Export each part at its baked world position. Import all OBJ files in the
	// directory together to inspect the complete model with per-part materials.
	inline bool SaveParts(const Assembly::BakedMesh& baked, const s3d::FilePath& directory)
	{
		if (not s3d::FileSystem::CreateDirectories(directory))
		{
			return false;
		}
		for (const auto& part : baked.parts)
		{
			if (part.range.isEmpty())
			{
				continue;
			}
			const s3d::Mesh3D mesh = ExtractPart(baked, part);
			const s3d::Material material = (part.material
				? baked.materials[static_cast<size_t>(*part.material)] : s3d::Material{});
			// IDs make filenames independent of duplicate or filesystem-unsafe names.
			const auto path = (directory + U"/part_" + s3d::Format(static_cast<size_t>(part.id)) + U".obj");
			if (not mesh.saveOBJ(path, material))
			{
				return false;
			}
		}
		return true;
	}
}
