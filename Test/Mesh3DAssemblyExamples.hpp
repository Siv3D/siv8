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

	// A texture-free stylized service robot. Every visible accent is geometry
	// with a base-color material; repeated fittings reuse registered meshes.
	inline Assembly MakeRobot(const s3d::ColorF bodyColor = s3d::ColorF{ 0.95, 0.38, 0.08 })
	{
		using namespace s3d;
		Assembly a;
		const auto paint = a.addMaterial(Material{ .name = U"enamel", .baseColor = bodyColor, .roughness = 0.5 });
		const auto ivory = a.addMaterial(Material{ .name = U"ivory", .baseColor = ColorF{ 0.88, 0.90, 0.82 } });
		const auto dark = a.addMaterial(Material{ .name = U"rubber", .baseColor = ColorF{ 0.055, 0.075, 0.10 } });
		const auto grey = a.addMaterial(Material{ .name = U"joints", .baseColor = ColorF{ 0.24, 0.31, 0.35 } });
		const auto glass = a.addMaterial(Material{ .name = U"visor", .baseColor = ColorF{ 0.02, 0.13, 0.20 }, .roughness = 0.25 });
		const auto cyan = a.addMaterial(Material{ .name = U"status cyan", .baseColor = ColorF{ 0.16, 0.86, 0.96 } });
		const auto yellow = a.addMaterial(Material{ .name = U"warning yellow", .baseColor = ColorF{ 1.0, 0.76, 0.10 } });
		const auto red = a.addMaterial(Material{ .name = U"signal red", .baseColor = ColorF{ 0.86, 0.12, 0.09 } });
		const auto body = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 1.1, 1.05, 0.7 }, 0.10)).value();
		const auto head = a.addMesh(Mesh3D::RoundedBox(Vec3{ 1.16, 0.68, 0.76 }, 0.14, 4)).value();
		const auto visor = a.addMesh(Mesh3D::RoundedBox(Vec3{ 0.98, 0.39, 0.12 }, 0.055, 3)).value();
		const auto eye = a.addMesh(Mesh3D::RoundedBox(Vec3{ 0.12, 0.19, 0.045 }, 0.02, 3)).value();
		const auto joint = a.addMesh(Mesh3D::Cylinder(0.20, 0.18, 16)).value();
		const auto upperArm = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.28, 0.38, 0.30 }, 0.045)).value();
		const auto forearm = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.34, 0.42, 0.36 }, 0.05)).value();
		const auto finger = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.095, 0.21, 0.18 }, 0.02)).value();
		const auto shin = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.32, 0.38, 0.36 }, 0.04)).value();
		const Vec3 footSize{ 0.48, 0.24, 0.72 };
		const auto foot = a.addMesh(Mesh3D::RoundedBox(footSize, 0.07, 3)).value();
		const auto sole = a.addMesh(Mesh3D::Box(Vec3{ 0.49, 0.07, 0.73 })).value();
		const auto vent = a.addMesh(Mesh3D::Box(Vec3{ 0.48, 0.055, 0.045 })).value();
		const auto panel = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.78, 0.40, 0.10 }, 0.04)).value();
		const auto button = a.addMesh(Mesh3D::Cylinder(0.042, 0.045, 12)).value();
		const auto backpack = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.74, 0.75, 0.36 }, 0.08)).value();
		const auto rim = a.addMesh(Mesh3D::Box(Vec3{ 0.62, 0.065, 0.025 })).value();
		const auto belt = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.86, 0.18, 0.59 }, 0.045)).value();
		const auto antenna = a.addMesh(Mesh3D::Cylinder(0.028, 0.32, 10)).value();
		const auto tip = a.addMesh(Mesh3D::Sphere(0.075, 12, 8)).value();

		const auto add = [&](String name, Assembly::MeshID mesh, Assembly::MaterialID material,
			Vec3 offset, Optional<Assembly::PartID> parent = none, Quaternion rotation = Quaternion{})
		{
			return a.addPart({ .name = std::move(name), .mesh = mesh, .material = material, .parent = parent,
				.placement = Mesh3DPlacement{ offset, rotation } }).value();
		};
		const auto torso = add(U"torso", body, paint, Vec3{ 0, 1.25, 0 });
		add(U"waist", belt, grey, Vec3{ 0, 0.69, 0 });
		const auto face = add(U"head", head, ivory, Vec3{ 0, 2.15, 0 });
		// The neck connects the head to the torso; cylinders start along +Y.
		add(U"neck", joint, grey, Vec3{ 0, 1.80, 0 });
		add(U"visor", visor, glass, Vec3{ 0, 0.025, -0.385 }, face);
		for (const double side : { -1.0, 1.0 })
		{
			add(U"eye", eye, cyan, Vec3{ (side * 0.24), 0.025, -0.455 }, face);
		}
		add(U"antenna", antenna, grey, Vec3{ 0.35, 0.47, 0.12 }, face);
		add(U"antenna tip", tip, red, Vec3{ 0.35, 0.66, 0.12 }, face);
		const auto chest = add(U"chest panel", panel, dark, Vec3{ 0, 0.21, -0.35 }, torso);
		for (uint32 i = 0; i < 3; ++i)
		{
			add(U"status light", button, (i == 0 ? red : (i == 1 ? yellow : cyan)),
				Vec3{ (-0.23 + i * 0.23), 0.02, -0.055 }, chest, Quaternion::RotateX(Math::HalfPi));
			add(U"vent", vent, grey, Vec3{ 0, (-0.10 - i * 0.12), -0.35 }, torso);
		}
		const auto pack = add(U"battery pack", backpack, grey, Vec3{ 0, 0.03, 0.47 }, torso);
		for (uint32 i = 0; i < 3; ++i)
		{
			add(U"battery stripe", rim, yellow, Vec3{ 0, (0.20 - i * 0.20), 0.18 }, pack);
		}
		// Build one arm, then copy its hierarchy with shared mesh/material IDs.
		const auto arm = a.addPart({ .name = U"arm frame",
			.placement = Mesh3DPlacement{ Vec3{ 0.62, 1.62, 0 }, Quaternion::RotateZ(0.10) } }).value();
		add(U"shoulder", joint, grey, Vec3::Zero(), arm, Quaternion::RotateZ(Math::HalfPi));
		add(U"upper arm", upperArm, paint, Vec3{ 0.12, -0.23, 0 }, arm);
		add(U"elbow", joint, dark, Vec3{ 0.12, -0.45, 0 }, arm, Quaternion::RotateZ(Math::HalfPi));
		add(U"forearm", forearm, ivory, Vec3{ 0.12, -0.68, 0 }, arm);
		add(U"claw", finger, grey, Vec3{ 0.02, -0.94, -0.04 }, arm);
		add(U"claw", finger, grey, Vec3{ 0.22, -0.94, -0.04 }, arm);
		(void)a.cloneSubtree(arm, Mat4x4::AffineTransform(Float3{ -1, 1, 1 },
			Quaternion::RotateZ(-0.10), Float3{ -0.62f, 1.62f, 0 })).value();
		for (const double side : { -1.0, 1.0 })
		{
			const auto leg = a.addPart({ .name = U"leg frame", .placement = Vec3{ (side * 0.27), 0, 0 } }).value();
			add(U"knee", joint, dark, Vec3{ 0, 0.56, 0 }, leg, Quaternion::RotateZ(Math::HalfPi));
			add(U"shin", shin, paint, Vec3{ 0, 0.40, 0 }, leg);
			// Align the housing's bottom to a mounting plane inside the sole.
			(void)a.addPart({ .name = U"foot", .mesh = foot, .material = ivory, .parent = leg,
				.placement = Mesh3DPlacement::Align(Vec3{ 0, (-footSize.y * 0.5), 0 }, Vec3{ 0, 0.05, -0.13 }) }).value();
			add(U"sole", sole, dark, Vec3{ 0, 0.035, -0.13 }, leg);
		}
		return a;
	}
}
