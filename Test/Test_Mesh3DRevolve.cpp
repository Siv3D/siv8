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
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(std::span<const Vec2>, uint32)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::span<const Vec2>, uint32, double)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::initializer_list<Vec2>, uint32)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::initializer_list<Vec2>, uint32, double)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::span<const Vec2>, double, double, uint32, CloseEnds)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::initializer_list<Vec2>, double, double, uint32, CloseEnds)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::span<const Vec2>, double, double, uint32, double, CloseEnds)>(&Mesh3D::Revolve);
		static_cast<Mesh3D (*)(std::initializer_list<Vec2>, double, double, uint32, double, CloseEnds)>(&Mesh3D::Revolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::initializer_list<Vec2>, uint32)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, Vec3)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, Vec3, const Quaternion&)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, const Mat4x4&)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, double)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::initializer_list<Vec2>, uint32, double)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, double, Vec3)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, double, Vec3, const Quaternion&)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, uint32, double, const Mat4x4&)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::initializer_list<Vec2>, double, double, uint32, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, Vec3, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, Vec3, const Quaternion&, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, const Mat4x4&, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, double, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::initializer_list<Vec2>, double, double, uint32, double, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, double, Vec3, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, double, Vec3, const Quaternion&, CloseEnds)>(&Mesh3DBuilder::addRevolve);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec2>, double, double, uint32, double, const Mat4x4&, CloseEnds)>(&Mesh3DBuilder::addRevolve);
	});

}

TEST_CASE("Mesh3D::Revolve initializer list")
{
	const Array<Vec2> profile{ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } };
	CheckMeshDataEqual(
		Mesh3D::Revolve({ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } }, 12),
		Mesh3D::Revolve(profile, 12));
	CheckMeshDataEqual(
		Mesh3D::Revolve({ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } }, 12, Math::QuarterPi),
		Mesh3D::Revolve(profile, 12, Math::QuarterPi));
	CHECK(Mesh3D::Revolve({}, 12).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addRevolve initializer list")
{
	const Array<Vec2> profile{ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } };
	Mesh3DBuilder builder;
	REQUIRE(builder.addRevolve(
		{ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } }, 12));
	REQUIRE(builder.addRevolve(
		{ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } }, 12, Math::QuarterPi));
	REQUIRE(builder.addRevolve(
		{ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } },
		0.25, Math::Pi, 6, CloseEnds::Yes));
	REQUIRE(builder.addRevolve(
		{ { 0.0, -1.0 }, { 2.0, -1.0 }, { 1.0, 2.0 } },
		0.25, Math::Pi, 6, Math::QuarterPi, CloseEnds::Yes));

	Mesh3D expected = Mesh3D::Revolve(profile, 12);
	REQUIRE(expected.append(Mesh3D::Revolve(profile, 12, Math::QuarterPi)));
	REQUIRE(expected.append(Mesh3D::Revolve(profile, 0.25, Math::Pi, 6, CloseEnds::Yes)));
	REQUIRE(expected.append(Mesh3D::Revolve(
		profile, 0.25, Math::Pi, 6, Math::QuarterPi, CloseEnds::Yes)));
	CheckMeshDataEqual(builder.getMesh(), expected);
	CHECK_FALSE(builder.addRevolve({}, 12));
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

TEST_CASE("Mesh3D::Revolve partial angle")
{
	constexpr uint32 Segments = 4;
	const Array<Vec2> profile{ { 1.0, -2.0 }, { 1.0, 3.0 } };
	const Mesh3D mesh = Mesh3D::Revolve(
		profile, 0.0, Math::HalfPi, Segments, CloseEnds::No);
	const size_t ringStride = (Segments + 1);

	CHECK_EQ(mesh.vertexCount(), (ringStride * 2));
	CHECK_EQ(mesh.triangleCount(), (Segments * 2));
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 1.0f, -2.0f, 0.0f });
	CHECK(mesh.vertices[Segments].pos.x == doctest::Approx(0.0f).scale(1.0));
	CHECK_EQ(mesh.vertices[Segments].pos.y, -2.0f);
	CHECK(mesh.vertices[Segments].pos.z == doctest::Approx(1.0f));
	CHECK_EQ(mesh.vertices[Segments].tex, Float2{ 1.0f, 0.0f });

	const Mesh3D oneSegment = Mesh3D::Revolve(
		profile, Math::QuarterPi, Math::HalfPi, 1, CloseEnds::No);
	CHECK_EQ(oneSegment.vertexCount(), size_t{ 4 });
	CHECK_EQ(oneSegment.triangleCount(), size_t{ 2 });
	CheckMeshGeometry(oneSegment);

	CheckMeshDataEqual(
		Mesh3D::Revolve(
			{ { 1.0, -2.0 }, { 1.0, 3.0 } },
			0.0,
			Math::HalfPi,
			Segments,
			CloseEnds::No),
		mesh);
}

TEST_CASE("Mesh3D::Revolve partial end caps")
{
	constexpr uint32 Segments = 4;
	const Array<Vec2> profile{
		{ 0.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { 0.0, 1.0 }
	};
	const Mesh3D mesh = Mesh3D::Revolve(
		profile, 0.0, Math::HalfPi, Segments, CloseEnds::Yes);
	const size_t sideVertexCount = ((6 * Segments) + 4);
	const size_t capVertexCount = 4;

	CHECK_EQ(mesh.vertexCount(), (sideVertexCount + (capVertexCount * 2)));
	CHECK_EQ(mesh.triangleCount(), size_t{ (4 * Segments) + 4 });
	CheckMeshGeometry(mesh);

	for (size_t i = 0; i < capVertexCount; ++i)
	{
		const Vertex3D& start = mesh.vertices[sideVertexCount + i];
		const Vertex3D& end = mesh.vertices[sideVertexCount + capVertexCount + i];
		CHECK_EQ(start.normal, -Float3::UnitZ());
		CHECK(end.normal.x == doctest::Approx(-1.0f));
		CHECK(end.normal.y == doctest::Approx(0.0f).scale(1.0));
		CHECK(end.normal.z == doctest::Approx(0.0f).scale(1.0));
		CHECK((0.0f <= start.tex.x && start.tex.x <= 1.0f));
		CHECK((0.0f <= start.tex.y && start.tex.y <= 1.0f));
	}

	const Array<Vec2> reversed{
		{ 0.0, 1.0 }, { 2.0, 1.0 }, { 2.0, -1.0 }, { 0.0, -1.0 }
	};
	CheckMeshGeometry(Mesh3D::Revolve(
		reversed, 0.0, Math::HalfPi, Segments, CloseEnds::Yes));

	const Array<Vec2> hollow{
		{ 2.0, -1.0 }, { 2.0, 1.0 }, { 1.0, 1.0 }, { 1.0, -1.0 }, { 2.0, -1.0 }
	};
	CheckMeshGeometry(Mesh3D::Revolve(
		hollow, -Math::QuarterPi, Math::Pi, Segments, CloseEnds::Yes));
}

TEST_CASE("Mesh3D::Revolve full-angle compatibility")
{
	constexpr uint32 Segments = 8;
	const Array<Vec2> profile{
		{ 0.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { 0.0, 1.0 }
	};

	CheckMeshDataEqual(
		Mesh3D::Revolve(profile, Segments),
		Mesh3D::Revolve(profile, 0.0, Math::TwoPi, Segments, CloseEnds::Yes));
	CheckMeshDataEqual(
		Mesh3D::Revolve(profile, Segments, Math::HalfPi),
		Mesh3D::Revolve(
			profile,
			0.0,
			Math::TwoPi,
			Segments,
			Math::HalfPi,
			CloseEnds::Yes));
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
	CHECK(Mesh3D::Revolve(valid, 0.0, 0.0, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(valid, 0.0, -Math::QuarterPi, Segments).isEmpty());
	CHECK(Mesh3D::Revolve(valid, 0.0, (Math::TwoPi + 0.001), Segments).isEmpty());
	CHECK(Mesh3D::Revolve(
		valid,
		std::numeric_limits<double>::quiet_NaN(),
		Math::HalfPi,
		Segments).isEmpty());
	CHECK(Mesh3D::Revolve(valid, 0.0, Math::HalfPi, 0).isEmpty());
	CHECK(Mesh3D::Revolve(valid, 0.0, Math::TwoPi, 2).isEmpty());
	CHECK(Mesh3D::Revolve(
		valid, 0.0, Math::HalfPi, Segments, CloseEnds::Yes).isEmpty());
	CheckMeshGeometry(Mesh3D::Revolve(valid, 0.0, Math::HalfPi, 1));
}

TEST_CASE("Mesh3DBuilder::addRevolve")
{
	constexpr uint32 Segments = 8;
	constexpr double SmoothingAngle = Math::HalfPi;
	const Array<Vec2> profile{
		{ 0.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { 0.0, 1.0 }
	};

	SUBCASE("Direct append reuses reserved storage")
	{
		const Mesh3D source = Mesh3D::Revolve(profile, Segments);
		Mesh3DBuilder builder;
		builder.reserve((source.vertexCount() * 2), (source.triangleCount() * 2));
		REQUIRE(builder.addRevolve(profile, Segments));
		const Vertex3D* const vertexData = builder.getMesh().vertices.data();
		const TriangleIndex32* const indexData = builder.getMesh().indices.data();

		const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
		REQUIRE(builder.addRevolve(profile, Segments, transform));
		CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
		CHECK_EQ(builder.getMesh().indices.data(), indexData);

		Mesh3D expected = source;
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Smoothing and transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });
		const Mesh3D hard = Mesh3D::Revolve(profile, Segments);
		const Mesh3D smooth = Mesh3D::Revolve(profile, Segments, SmoothingAngle);

		Mesh3DBuilder builder;
		REQUIRE(builder.addRevolve(profile, Segments, offset));
		REQUIRE(builder.addRevolve(profile, Segments, offset, rotation));
		REQUIRE(builder.addRevolve(profile, Segments, transform));
		REQUIRE(builder.addRevolve(profile, Segments, SmoothingAngle));
		REQUIRE(builder.addRevolve(profile, Segments, SmoothingAngle, offset));
		REQUIRE(builder.addRevolve(profile, Segments, SmoothingAngle, offset, rotation));
		REQUIRE(builder.addRevolve(profile, Segments, SmoothingAngle, transform));

		Mesh3D expected;
		REQUIRE(expected.append(hard, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(
			hard, Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(hard, transform));
		REQUIRE(expected.append(smooth));
		REQUIRE(expected.append(smooth, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(
			smooth, Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(smooth, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Partial-angle transform overloads")
	{
		constexpr double StartAngle = -Math::QuarterPi;
		constexpr double SweepAngle = Math::Pi;
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });
		const Mesh3D hard = Mesh3D::Revolve(
			profile, StartAngle, SweepAngle, Segments, CloseEnds::Yes);
		const Mesh3D smooth = Mesh3D::Revolve(
			profile,
			StartAngle,
			SweepAngle,
			Segments,
			SmoothingAngle,
			CloseEnds::Yes);

		Mesh3DBuilder builder;
		REQUIRE(builder.addRevolve(
			profile, StartAngle, SweepAngle, Segments, CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile, StartAngle, SweepAngle, Segments, offset, CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile, StartAngle, SweepAngle, Segments, offset, rotation, CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile, StartAngle, SweepAngle, Segments, transform, CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile,
			StartAngle,
			SweepAngle,
			Segments,
			SmoothingAngle,
			CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile,
			StartAngle,
			SweepAngle,
			Segments,
			SmoothingAngle,
			offset,
			CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile,
			StartAngle,
			SweepAngle,
			Segments,
			SmoothingAngle,
			offset,
			rotation,
			CloseEnds::Yes));
		REQUIRE(builder.addRevolve(
			profile,
			StartAngle,
			SweepAngle,
			Segments,
			SmoothingAngle,
			transform,
			CloseEnds::Yes));

		Mesh3D expected;
		REQUIRE(expected.append(hard));
		REQUIRE(expected.append(hard, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(
			hard, Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(hard, transform));
		REQUIRE(expected.append(smooth));
		REQUIRE(expected.append(smooth, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(
			smooth, Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(smooth, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addRevolve(std::span<const Vec2>{}, Segments));
		CHECK_FALSE(builder.addRevolve(profile, 2));
		CHECK_FALSE(builder.addRevolve(profile, Segments, -0.001));
		CHECK_FALSE(builder.addRevolve(
			profile,
			Segments,
			std::numeric_limits<double>::quiet_NaN(),
			Vec3{ 1.0, 2.0, 3.0 }));
		CHECK_FALSE(builder.addRevolve(profile, 0.0, 0.0, Segments));
		CHECK_FALSE(builder.addRevolve(profile, 0.0, Math::TwoPi, 2));
		const Array<Vec2> openProfile{ { 1.0, -1.0 }, { 1.0, 1.0 } };
		CHECK_FALSE(builder.addRevolve(
			openProfile,
			0.0,
			Math::HalfPi,
			Segments,
			Mat4x4::Identity(),
			CloseEnds::Yes));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}
}
