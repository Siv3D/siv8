//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Mesh3DTestHelper.hpp"
# include "Mesh3DLoftExamples.hpp"

namespace
{
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;
	const std::array Square{ Vec2{ -1, -1 }, Vec2{ 1, -1 }, Vec2{ 1, 1 }, Vec2{ -1, 1 } };

	Array<Vec2> Ring(const size_t count)
	{
		Array<Vec2> result;
		for (size_t i = 0; i < count; ++i)
		{
			const double a = Math::TwoPi * i / count;
			result.emplace_back(std::cos(a), std::sin(a));
		}
		return result;
	}
}

TEST_CASE("Mesh3D::Loft borrowed sections and equivalent input forms")
{
	static_assert(requires {
		static_cast<Mesh3D (*)(std::span<const LoftSection>, const LoftOptions&)>(&Mesh3D::Loft);
		static_cast<Mesh3D (*)(std::initializer_list<LoftSection>, const LoftOptions&)>(&Mesh3D::Loft);
	});
	const std::array sections{ LoftSection{ Square, Vec3{ 0, -1, 0 } }, LoftSection{ Square, Vec3{ 0, 3, 0 } } };
	const LoftOptions options{ .uvScale = Vec2{ 2, 0.5 }, .uvOffset = Vec2{ 0.25, -0.5 } };
	const auto mesh = Mesh3D::Loft(sections, options);
	CheckMeshDataEqual(mesh, Mesh3D::Loft({ sections[0], sections[1] }, options));
	CHECK((mesh.vertexCount()) == (size_t{ 24 }));
	CHECK((mesh.triangleCount()) == (size_t{ 12 }));
	CheckMeshGeometry(mesh);
	CHECK((mesh.vertices[0].pos) == ((Float3{ -1, -1, 1 })));
	CHECK((mesh.vertices[4].pos) == ((Float3{ -1, 3, 1 })));
	CHECK((mesh.vertices[8].tex) == ((Float2{ 0.25f, -0.5f })));
	CHECK((mesh.vertices[10].tex) == ((Float2{ 0.25f, 1.5f })));
	CHECK((mesh.vertices.back().tex) == ((Float2{ 2.25f, 1.5f })));
	Mesh3DBuilder builder;
	builder.reserve(mesh.vertexCount(), mesh.triangleCount());
	REQUIRE(builder.addLoft(sections, options));
	const auto* storage = builder.getMesh().vertices.data();
	builder.clear();
	REQUIRE(builder.addLoft({ sections[0], sections[1] }, options));
	CHECK((storage) == (builder.getMesh().vertices.data()));
	CheckMeshDataEqual(builder.getMesh(), mesh);
}

TEST_CASE("Mesh3D::Loft minimum and concave contours")
{
	const std::array triangle{ Vec2{ 0, -1 }, Vec2{ 1, 1 }, Vec2{ -1, 1 } };
	const auto mesh = Mesh3D::Loft({ { triangle, Vec3::Zero() }, { triangle, Vec3{ 0, 1, 0 } } });
	CHECK((mesh.vertexCount()) == (size_t{ 18 }));
	CHECK((mesh.triangleCount()) == (size_t{ 8 }));
	CheckMeshGeometry(mesh);
	const std::array concave{ Vec2{ -2, -1 }, Vec2{ 0, -1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 }, Vec2{ 2, 1 }, Vec2{ -2, 1 } };
	CheckMeshGeometry(Mesh3D::Loft({ { concave, Vec3::Zero() }, { concave, Vec3{ 0, 2, 0 } } }));
}

TEST_CASE("Mesh3D::Loft independent end caps")
{
	for (const auto caps : { Mesh3DEndCaps::None, Mesh3DEndCaps::Start, Mesh3DEndCaps::End, Mesh3DEndCaps::Both })
	{
		const size_t count = (caps == Mesh3DEndCaps::Both ? 2 : (caps == Mesh3DEndCaps::None ? 0 : 1));
		const auto mesh = Mesh3D::Loft({ { Square, Vec3::Zero() }, { Square, Vec3{ 0, 2, 0 } } }, { .endCaps = caps });
		CHECK((mesh.vertexCount()) == (16 + count * 4));
		CHECK((mesh.triangleCount()) == (8 + count * 2));
		CheckMeshGeometry(mesh);
		if (count == 1)
		{
			CHECK((mesh.vertices[0].pos.y) == ((caps == Mesh3DEndCaps::Start ? 0.0f : 2.0f)));
			CHECK((mesh.vertices[0].normal.y) == ((caps == Mesh3DEndCaps::Start ? -1.0f : 1.0f)));
		}
	}
}

TEST_CASE("Mesh3D::Loft tilted frames nonuniform scale and shear")
{
	const auto ring = Ring(16);
	Array<LoftSection> sections;
	for (size_t i = 0; i < 4; ++i)
	{
		const auto frame = Mat4x4::AffineTransform(Float3{ 1.0f + 0.1f * i, 1, 0.8f },
			Quaternion::RotateX(i * 0.15), Float3{ 0, static_cast<float>(i), static_cast<float>(0.15 * i * i) });
		sections.push_back({ ring, frame });
	}
	const auto mesh = Mesh3D::Loft(sections, { .smoothingAngle = Math::Pi });
	CheckMeshGeometry(mesh);
	// V measures the authored origin polyline, not its Y projection.
	const double expectedV = Vec3{ 0, 1, 0.15f }.length() + Vec3{ 0, 1, 0.45f }.length() + Vec3{ 0, 1, 0.75f }.length();
	CHECK(mesh.vertices.back().tex.y == Test::Approx(expectedV).epsilon(1e-6));
	const Mat4x4 shear{ 2, 0.25f, 0, 0, 0.3f, 1, 0, 0, 0, 0.2f, 1, 0, 1, 0, 2, 1 };
	const auto sheared = Mesh3D::Loft({ { Square, shear }, { Square, shear * Mat4x4::Translate(0, 3, 0) } });
	CheckMeshGeometry(sheared);
	const Vec3 capN = Vec3{ shear.transformVector(Float3::UnitZ()) }.cross(Vec3{ shear.transformVector(Float3::UnitX()) }).normalized();
	CHECK(sheared.vertices[4].normal.epsilonEquals(Float3{ capN }, 1e-6f));
}

TEST_CASE("Mesh3D::Loft contour smoothing threshold and seam")
{
	const std::array sections{ LoftSection{ Square, Vec3::Zero() }, LoftSection{ Square, Vec3{ 0, 2, 0 } } };
	const auto hard = Mesh3D::Loft(sections, { .endCaps = Mesh3DEndCaps::None });
	const auto below = Mesh3D::Loft(sections, { .endCaps = Mesh3DEndCaps::None, .smoothingAngle = 89_deg });
	CheckMeshDataEqual(hard, below);
	const auto smooth = Mesh3D::Loft(sections, { .endCaps = Mesh3DEndCaps::None, .smoothingAngle = 90_deg });
	CHECK((smooth.vertexCount()) == (hard.vertexCount()));
	CheckMeshGeometry(smooth);
	CHECK(smooth.vertices[0].normal.epsilonEquals(smooth.vertices[13].normal, 1e-6f));
	CHECK((smooth.vertices[0].tex.x) == (0.0f));
	CHECK((smooth.vertices[13].tex.x) == (1.0f));
	CHECK_FALSE(hard.vertices[0].normal.epsilonEquals(hard.vertices[13].normal, 1e-6f));
	const auto ring = Ring(32);
	CheckMeshGeometry(Mesh3D::Loft({ { ring, Vec3::Zero() }, { ring, Vec3{ 0, 1, 0 } } }, { .smoothingAngle = 30_deg }));
}

TEST_CASE("Mesh3D::Loft tangent frames follow UV signs")
{
	const std::array sections{ LoftSection{ Square, Vec3::Zero() }, LoftSection{ Square, Vec3{ 0, 2, 0 } } };
	const auto reference = Mesh3D::Loft(sections);
	for (const Vec2 scale : { Vec2{ -2, 3 }, Vec2{ 2, -3 }, Vec2{ -2, -3 }, Vec2{ 0, 0 } })
	{
		const auto mesh = Mesh3D::Loft(sections, { .uvScale = scale });
		CheckMeshGeometry(mesh);
		for (size_t i = 0; i < mesh.vertexCount(); ++i)
		{
			CHECK(mesh.vertices[i].tangent.xyz().epsilonEquals(reference.vertices[i].tangent.xyz() * (scale.x < 0 ? -1 : 1), 1e-6f));
			CHECK((mesh.vertices[i].tangent.w) == ((scale.x < 0 ? -1 : 1) * (scale.y < 0 ? -1 : 1)));
		}
	}
}

TEST_CASE("Mesh3D::Loft invalid inputs preserve destination")
{
	const ScopedLogSilencer silence;
	Array<Vec2> first(Square.begin(), Square.end()), second = first;
	Array<LoftSection> sections{ { first, Vec3::Zero() }, { second, Vec3{ 0, 2, 0 } } };
	LoftOptions options;
	SECTION("no sections") { sections.clear(); }
	SECTION("one section") { sections.pop_back(); }
	SECTION("mismatched vertex count") { sections[1].points = std::span<const Vec2>{ second.data(), 3 }; }
	SECTION("two points") { for (auto& s : sections) { s.points = std::span<const Vec2>{ first.data(), 2 }; } }
	SECTION("same origin") { sections[1].frame = Vec3::Zero(); }
	SECTION("backwards origin") { sections[1].frame = Vec3{ 0, -1, 0 }; }
	SECTION("wrong facing frame") { sections[1].frame = Mesh3DPlacement{ Vec3{ 0, 2, 0 }, Quaternion::RotateX(Math::Pi) }; }
	SECTION("singular frame") { sections[1].frame = Mat4x4::Scale(Float3{ 0, 1, 1 }); }
	SECTION("reflected frame") { sections[1].frame = Mat4x4::Scale(Float3{ -1, 1, 1 }); }
	SECTION("projective frame") { sections[1].frame = Mat4x4{ 1,0,0,1, 0,1,0,0, 0,0,1,0, 0,2,0,1 }; }
	SECTION("nonfinite frame") { sections[1].frame = Vec3{ 0, std::numeric_limits<double>::infinity(), 0 }; }
	SECTION("nonfinite point") { first[0].x = std::numeric_limits<double>::quiet_NaN(); }
	SECTION("repeated contour vertex") { first[1] = first[0]; }
	SECTION("reversed contour") { std::reverse(first.begin(), first.end()); }
	SECTION("collapsed transformed contour") { sections[1].frame = Vec3{ 1e10, 2, 0 }; }
	SECTION("collapsed side triangle") { sections[1].frame = Mat4x4{ 1,1,0,0, -1,1,0,0, 0,0,1,0, 0,1,0,1 }; }
	SECTION("invalid caps") { options.endCaps = static_cast<Mesh3DEndCaps>(255); }
	SECTION("negative smoothing") { options.smoothingAngle = -0.1; }
	SECTION("excessive smoothing") { options.smoothingAngle = 4; }
	SECTION("nonfinite smoothing") { options.smoothingAngle = std::numeric_limits<double>::quiet_NaN(); }
	SECTION("nonfinite UV") { options.uvScale.x = std::numeric_limits<double>::infinity(); }
	SECTION("overflowing UV") { options.uvScale.y = std::numeric_limits<float>::max(); }
	Mesh3DBuilder builder;
	REQUIRE(builder.addBox());
	const auto before = builder.getMesh();
	CHECK_FALSE(builder.addLoft(sections, options));
	CheckMeshDataEqual(builder.getMesh(), before);
}


TEST_CASE("Mesh3D::Loft curved modeling recipe and shared assembly refinement")
{
	for (const double smoothing : { 0.0, Math::Pi })
	{
		auto assembly = Mesh3DLoftExamples::MakeAssembly(16, 5, smoothing);
		const auto before = assembly.bake().value();
		CheckMeshGeometry(before.mesh);
		auto refined = Mesh3DLoftExamples::MakeDuct(32, 9, smoothing);
		REQUIRE(assembly.setMesh(Mesh3DAssembly::MeshID{ 0 }, std::move(refined.mesh)));
		const auto after = assembly.bake().value();
		CheckMeshGeometry(after.mesh);
		CHECK((after.mesh.vertexCount()) > (before.mesh.vertexCount()));
		REQUIRE((after.parts.size()) == (before.parts.size()));
		for (size_t i = 0; i < before.parts.size(); ++i)
		{
			CHECK(after.parts[i].id == before.parts[i].id);
			CHECK(after.parts[i].material == before.parts[i].material);
			CHECK(after.parts[i].worldTransform == before.parts[i].worldTransform);
		}
	}
}
