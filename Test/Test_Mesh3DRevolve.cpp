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

# include "Mesh3DTestHelper.hpp"

namespace
{
	using Mesh3DTest::CheckMeshGeometry;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(std::span<const Vec2>, uint32)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::span<const Vec2>, uint32, double)>(&Mesh3D::Revolve);
	});

}

TEST_CASE("Mesh3D::Revolve open cylinder side")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> profile{ { 1.0, -2.0 }, { 1.0, 3.0 } };
	const Mesh3D mesh = Mesh3D::Revolve(profile, Segments);
	const size_t ringStride = (Segments + 1);

	CHECK_EQ(mesh.vertexCount(), (ringStride * 2));
	CHECK_EQ(mesh.triangleCount(), (Segments * 2));
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 1.0f, -2.0f, 0.0f });
	CHECK_EQ(mesh.vertices[0].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[0].tangent, Float4{ 0.0f, 0.0f, 1.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].bitangent(), Float3::UnitY());
	CHECK_EQ(mesh.vertices[Segments].pos, mesh.vertices[0].pos);
	CHECK_EQ(mesh.vertices[Segments].normal, mesh.vertices[0].normal);
	CHECK_EQ(mesh.vertices[Segments].tangent, mesh.vertices[0].tangent);
	CHECK_EQ(mesh.vertices[Segments].tex, Float2{ 1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[ringStride].pos, Float3{ 1.0f, 3.0f, 0.0f });
	CHECK_EQ(mesh.vertices[ringStride].tex, Float2{ 0.0f, 1.0f });
	CheckMeshGeometry(Mesh3D::Revolve(profile));

	const Mesh3D reversed = Mesh3D::Revolve(Array<Vec2>{ { 1.0, 3.0 }, { 1.0, -2.0 } }, Segments);
	CheckMeshGeometry(reversed);
	CHECK_EQ(reversed.vertices[0].normal, -Float3::UnitX());
}

TEST_CASE("Mesh3D::Revolve solid cylinder")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> profile{
		{ 0.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { 0.0, 1.0 }
	};
	const Mesh3D mesh = Mesh3D::Revolve(profile, Segments);
	const size_t firstSegmentVertexCount = ((Segments * 2) + 1);
	const size_t secondSegmentBase = firstSegmentVertexCount;
	const size_t thirdSegmentBase = (secondSegmentBase + (2 * (Segments + 1)));

	CHECK_EQ(mesh.vertexCount(), size_t{ (6 * Segments) + 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Segments });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 0.0f, -1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[0].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[Segments].pos, Float3{ 2.0f, -1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[Segments].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[secondSegmentBase].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[thirdSegmentBase].normal, Float3::UnitY());
}

TEST_CASE("Mesh3D::Revolve cone and annulus")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> coneProfile{ { 2.0, -1.0 }, { 0.0, 2.0 } };
	const Mesh3D cone = Mesh3D::Revolve(coneProfile, Segments);
	CHECK_EQ(cone.vertexCount(), size_t{ (2 * Segments) + 1 });
	CHECK_EQ(cone.triangleCount(), size_t{ Segments });
	CheckMeshGeometry(cone);

	const Array<Vec2> annulusProfile{ { 0.5, 0.0 }, { 2.0, 0.0 } };
	const Mesh3D annulus = Mesh3D::Revolve(annulusProfile, Segments);
	CHECK_EQ(annulus.vertexCount(), size_t{ 2 * (Segments + 1) });
	CHECK_EQ(annulus.triangleCount(), size_t{ 2 * Segments });
	CheckMeshGeometry(annulus);
	CHECK_EQ(annulus.vertices[0].normal, -Float3::UnitY());
}

TEST_CASE("Mesh3D::Revolve closed hollow profile")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> profile{
		{ 2.0, -1.0 }, { 2.0, 1.0 }, { 1.0, 1.0 }, { 1.0, -1.0 }, { 2.0, -1.0 }
	};
	const Mesh3D hard = Mesh3D::Revolve(profile, Segments);
	const size_t segmentVertexCount = (2 * (Segments + 1));
	const size_t lastEndRingBase = ((3 * segmentVertexCount) + (Segments + 1));

	CHECK_EQ(hard.vertexCount(), (4 * segmentVertexCount));
	CHECK_EQ(hard.triangleCount(), size_t{ 8 * Segments });
	CheckMeshGeometry(hard);
	CHECK_EQ(hard.vertices[0].pos, hard.vertices[lastEndRingBase].pos);
	CHECK_EQ(hard.vertices[0].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(hard.vertices[lastEndRingBase].tex, Float2{ 0.0f, 1.0f });
	CHECK_NE(hard.vertices[0].normal, hard.vertices[lastEndRingBase].normal);

	const Mesh3D smooth = Mesh3D::Revolve(profile, Segments, Math::HalfPi);
	CheckMeshGeometry(smooth);
	CHECK_EQ(smooth.vertices[0].normal, smooth.vertices[lastEndRingBase].normal);
	CHECK(smooth.vertices[0].normal.x == doctest::Approx(Math::InvSqrt2));
	CHECK(smooth.vertices[0].normal.y == doctest::Approx(-Math::InvSqrt2));
}

TEST_CASE("Mesh3D::Revolve profile normal smoothing")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> profile{ { 1.0, -1.0 }, { 2.0, 0.0 }, { 1.0, 1.0 } };
	const size_t segmentVertexCount = (2 * (Segments + 1));
	const size_t firstEndRingBase = (Segments + 1);
	const size_t secondStartRingBase = segmentVertexCount;
	const Mesh3D hard = Mesh3D::Revolve(profile, Segments);
	const Mesh3D zeroAngle = Mesh3D::Revolve(profile, Segments, 0.0);
	const Mesh3D belowCornerAngle = Mesh3D::Revolve(profile, Segments, Math::QuarterPi);
	const Mesh3D smooth = Mesh3D::Revolve(profile, Segments, Math::HalfPi);

	CheckMeshGeometry(hard);
	REQUIRE_EQ(zeroAngle.vertexCount(), hard.vertexCount());
	REQUIRE_EQ(zeroAngle.triangleCount(), hard.triangleCount());
	for (size_t i = 0; i < hard.vertexCount(); ++i)
	{
		CHECK_EQ(zeroAngle.vertices[i].normal, hard.vertices[i].normal);
		CHECK_EQ(zeroAngle.vertices[i].tangent, hard.vertices[i].tangent);
	}
	CheckMeshGeometry(belowCornerAngle);
	CheckMeshGeometry(smooth);
	CHECK_NE(hard.vertices[firstEndRingBase].normal, hard.vertices[secondStartRingBase].normal);
	CHECK_EQ(belowCornerAngle.vertices[firstEndRingBase].normal, hard.vertices[firstEndRingBase].normal);
	CHECK_EQ(belowCornerAngle.vertices[secondStartRingBase].normal, hard.vertices[secondStartRingBase].normal);
	CHECK_EQ(smooth.vertices[firstEndRingBase].normal, smooth.vertices[secondStartRingBase].normal);
	CHECK(smooth.vertices[firstEndRingBase].normal.x == doctest::Approx(1.0f));
	CHECK(smooth.vertices[firstEndRingBase].normal.y == doctest::Approx(0.0f).scale(1.0));
	CHECK(smooth.vertices[firstEndRingBase].normal.z == doctest::Approx(0.0f).scale(1.0));
}

TEST_CASE("Mesh3D::Revolve interior axis point")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> profile{ { 1.0, -1.0 }, { 0.0, 0.0 }, { 1.0, 1.0 } };
	const Mesh3D mesh = Mesh3D::Revolve(profile, Segments, Math::HalfPi);

	CHECK_EQ(mesh.vertexCount(), size_t{ 2 * ((2 * Segments) + 1) });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 * Segments });
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Revolve invalid arguments")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> valid{ { 1.0, -1.0 }, { 1.0, 1.0 } };

	CHECK(Mesh3D::Revolve(std::span<const Vec2>{}, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{ { 1.0, 0.0 } }, Segments).isEmpty());
	CheckMeshGeometry(Mesh3D::Revolve(valid, 3));
	CheckMeshGeometry(Mesh3D::Revolve(valid, Segments, Math::Pi));
	CHECK(Mesh3D::Revolve(valid, 2).isEmpty());
	CHECK(Mesh3D::Revolve(valid, std::numeric_limits<uint32>::max()).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{ { -1.0, 0.0 }, { 1.0, 1.0 } }, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{
		{ 1.0, 0.0 }, { std::numeric_limits<double>::infinity(), 1.0 }
	}, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{
		{ 1.0, 0.0 }, { std::numeric_limits<double>::max(), 1.0 }
	}, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{ { 1.0, 0.0 }, { 1.0, 0.0 } }, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{ { 0.0, 0.0 }, { 0.0, 1.0 } }, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{
		{ 1.0, 0.0 }, { 2.0, 0.0 }, { 1.0, 0.0 }
	}, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(Array<Vec2>{
		{ 1.0, 0.0 }, { 2.0, 0.0 }, { 3.0, 0.0 }, { 1.0, 0.0 }
	}, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(valid, Segments, -0.001).isEmpty());
	CHECK(Mesh3D::Revolve(valid, Segments, (Math::Pi + 0.001)).isEmpty());
	CHECK(Mesh3D::Revolve(valid, Segments, std::numeric_limits<double>::quiet_NaN()).isEmpty());
	CHECK(Mesh3D::Revolve(valid, Segments, std::numeric_limits<double>::infinity()).isEmpty());
}
