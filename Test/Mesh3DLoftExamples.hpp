//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D.hpp>

namespace Mesh3DLoftExamples
{
	struct Duct
	{
		s3d::Mesh3D mesh;
		s3d::Mesh3DPlacement outlet;
	};

	// Changes both the contour shape and its plane along a curved centerline.
	inline Duct MakeDuct(const s3d::uint32 samples = 32, const s3d::uint32 sectionCount = 9,
		const double smoothingAngle = s3d::Math::Pi)
	{
		using namespace s3d;
		Array<Array<Vec2>> contours(sectionCount);
		Array<LoftSection> sections;
		sections.reserve(sectionCount);
		for (uint32 i = 0; i < sectionCount; ++i)
		{
			const double t = static_cast<double>(i) / (sectionCount - 1);
			const double exponent = (0.6 + 0.4 * t);
			const double rx = (0.65 - 0.27 * t), rz = (0.45 - 0.07 * t);
			for (uint32 j = 0; j < samples; ++j)
			{
				const double a = Math::TwoPi * j / samples;
				const double c = std::cos(a), s = std::sin(a);
				contours[i].emplace_back(rx * std::copysign(std::pow(std::abs(c), exponent), c),
					rz * std::copysign(std::pow(std::abs(s), exponent), s));
			}
			const Mesh3DPlacement frame{ Vec3{ 0, 2.4 * t, t * t }, Quaternion::RotateX(std::atan(2 * t / 2.4)) };
			sections.push_back({ contours[i], frame });
		}
		return { Mesh3D::Loft(sections, { .endCaps = Mesh3DEndCaps::None, .smoothingAngle = smoothingAngle }), sections.back().frame };
	}

	inline s3d::Mesh3DAssembly MakeAssembly(const s3d::uint32 samples = 32, const s3d::uint32 sectionCount = 9,
		const double smoothingAngle = s3d::Math::Pi)
	{
		using namespace s3d;
		Mesh3DAssembly a;
		const auto blue = a.addMaterial(Material{ .name = U"blue enamel", .baseColor = ColorF{ 0.10, 0.43, 0.70 } });
		const auto dark = a.addMaterial(Material{ .name = U"base", .baseColor = ColorF{ 0.08, 0.11, 0.14 } });
		const auto orange = a.addMaterial(Material{ .name = U"outlet ring", .baseColor = ColorF{ 0.95, 0.4, 0.06 } });
		const auto grey = a.addMaterial(Material{ .name = U"fasteners", .baseColor = ColorF{ 0.5, 0.55, 0.58 } });
		auto duct = MakeDuct(samples, sectionCount, smoothingAngle);
		const auto body = a.addMesh(std::move(duct.mesh)).value();
		const auto base = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 1.55, 0.25, 1.2 }, 0.07)).value();
		const auto rim = a.addMesh(Mesh3D::Torus(0.40, 0.04, samples, 8)).value();
		const auto bolt = a.addMesh(Mesh3D::Cylinder(0.055, 0.045, 8)).value();
		const auto root = a.addPart({ .name = U"duct unit" }).value();
		(void)a.addPart({ .name = U"curved housing", .mesh = body, .material = blue, .parent = root }).value();
		(void)a.addPart({ .name = U"mounting base", .mesh = base, .material = dark, .parent = root, .placement = Vec3{ 0, -0.125, 0 } }).value();
		(void)a.addPart({ .name = U"outlet", .mesh = rim, .material = orange, .parent = root,
			.placement = Mesh3DPlacement::Align(Vec3::Zero(), duct.outlet) }).value();
		for (const double x : { -0.65, 0.65 })
		{
			for (const double z : { -0.46, 0.46 })
			{
				(void)a.addPart({ .name = U"bolt", .mesh = bolt, .material = grey, .parent = root,
					.placement = Vec3{ x, 0.0225, z } }).value();
			}
		}
		(void)a.cloneSubtree(root, Mat4x4::Scale(Float3{ -1, 1, 1 }) * Mat4x4::Translate(2.0f, 0, 0)).value();
		return a;
	}
}
