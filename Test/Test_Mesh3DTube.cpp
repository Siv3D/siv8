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
	constexpr float FrameEpsilon = 1e-5f;
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(std::span<const Vec3>, double, uint32, Vec2, Vec2)>(&Mesh3D::Tube);
		static_cast<Mesh3D (*)(std::initializer_list<Vec3>, double, uint32, Vec2, Vec2)>(&Mesh3D::Tube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, Vec2, Vec2)>(&Mesh3DBuilder::addTube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, Vec3)>(&Mesh3DBuilder::addTube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, Vec3, const Quaternion&)>(&Mesh3DBuilder::addTube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, const Mat4x4&)>(&Mesh3DBuilder::addTube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, Vec2, Vec2, Vec3)>(&Mesh3DBuilder::addTube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, Vec2, Vec2, Vec3, const Quaternion&)>(&Mesh3DBuilder::addTube);
		static_cast<bool (Mesh3DBuilder::*)(std::span<const Vec3>, double, uint32, Vec2, Vec2, const Mat4x4&)>(&Mesh3DBuilder::addTube);
	});

}

TEST_CASE("Mesh3D::Tube initializer list")
{
	const Array<Vec3> path{
		{ 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 2.0, 1.0 }
	};
	CheckMeshDataEqual(
		Mesh3D::Tube({
			{ 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 2.0, 1.0 }
		}, 0.5, 8, Vec2{ 2.0, 0.25 }, Vec2{ 0.1, 0.2 }),
		Mesh3D::Tube(path, 0.5, 8, Vec2{ 2.0, 0.25 }, Vec2{ 0.1, 0.2 }));
	CHECK(Mesh3D::Tube({}, 0.5).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addTube")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	constexpr double Radius = 0.25;
	constexpr uint32 Sides = 8;
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mesh3D source = Mesh3D::Tube(path, Radius, Sides, uvScale, uvOffset);

	SUBCASE("Direct append reuses reserved storage")
	{
		Mesh3DBuilder builder;
		builder.reserve((source.vertexCount() * 2), (source.triangleCount() * 2));
		REQUIRE(builder.addTube(path, Radius, Sides, uvScale, uvOffset));
		const Vertex3D* const vertexData = builder.getMesh().vertices.data();
		const TriangleIndex32* const indexData = builder.getMesh().indices.data();

		const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
		REQUIRE(builder.addTube(path, Radius, Sides, uvScale, uvOffset, transform));
		CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
		CHECK_EQ(builder.getMesh().indices.data(), indexData);

		Mesh3D expected = source;
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });
		const Mesh3D defaultUVSource = Mesh3D::Tube(path, Radius, Sides);

		Mesh3DBuilder builder;
		REQUIRE(builder.addTube(path, Radius, Sides, offset));
		REQUIRE(builder.addTube(path, Radius, Sides, offset, rotation));
		REQUIRE(builder.addTube(path, Radius, Sides, transform));
		REQUIRE(builder.addTube(path, Radius, Sides, uvScale, uvOffset, offset));
		REQUIRE(builder.addTube(path, Radius, Sides, uvScale, uvOffset, offset, rotation));
		REQUIRE(builder.addTube(path, Radius, Sides, uvScale, uvOffset, transform));

		Mesh3D expected;
		REQUIRE(expected.append(defaultUVSource, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(defaultUVSource,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(defaultUVSource, transform));
		REQUIRE(expected.append(source, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(source,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addTube({}, Radius, Sides));
		CHECK_FALSE(builder.addTube(path, 0.0, Sides));
		CHECK_FALSE(builder.addTube(path, Radius, 2));
		CHECK_FALSE(builder.addTube(
			path, 0.0, Sides, Vec2{ 1.0, 1.0 }, Vec2::Zero(), Mat4x4::Identity()));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3D::Tube straight path and UV repeat")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> path{ { 0.0, -1.0, 0.0 }, { 0.0, 2.0, 0.0 } };
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mesh3D mesh = Mesh3D::Tube(path, 2.0, Sides, uvScale, uvOffset);
	const size_t ringStride = (Sides + 1);
	const size_t startCapBase = (path.size() * ringStride);
	const size_t endCapBase = (startCapBase + ringStride);

	CHECK_EQ(mesh.vertexCount(), size_t{ 4 * ringStride });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Sides });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 2.0f, -1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[0].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[0].tangent, Float4{ 0.0f, 0.0f, 1.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].bitangent(), Float3::UnitY());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.1f, 0.2f });
	CHECK_EQ(mesh.vertices[Sides].pos, mesh.vertices[0].pos);
	CHECK_EQ(mesh.vertices[Sides].normal, mesh.vertices[0].normal);
	CHECK_EQ(mesh.vertices[Sides].tex, Float2{ 2.1f, 0.2f });
	CHECK_EQ(mesh.vertices[ringStride].tex, Float2{ 0.1f, 0.95f });
	CHECK_EQ(mesh.vertices[startCapBase].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[endCapBase].normal, Float3::UnitY());
	CHECK_EQ(mesh.vertices[startCapBase].tex, Float2{ 1.1f, 0.325f });
	CheckMeshGeometry(Mesh3D::Tube(path, 2.0));
}

TEST_CASE("Mesh3D::Tube bent non-planar path")
{
	constexpr uint32 Sides = 12;
	constexpr double Radius = 0.2;
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 2.0, 0.0 },
		{ 1.0, 3.0, 1.0 },
		{ 2.0, 3.0, 2.0 }
	};
	const Mesh3D mesh = Mesh3D::Tube(path, Radius, Sides);
	const size_t ringStride = (Sides + 1);

	CHECK_EQ(mesh.vertexCount(), ((path.size() + 2) * ringStride));
	CHECK_EQ(mesh.triangleCount(), (2 * path.size() * Sides));
	CheckMeshGeometry(mesh);

	for (size_t pathIndex = 0; pathIndex < path.size(); ++pathIndex)
	{
		const Float3 center = path[pathIndex];
		for (uint32 sideIndex = 0; sideIndex <= Sides; ++sideIndex)
		{
			const Vertex3D& vertex = mesh.vertices[(pathIndex * ringStride) + sideIndex];
			CHECK((vertex.pos - center).length() == doctest::Approx(Radius).epsilon(FrameEpsilon));
		}
	}

	const double expectedLength = (2.0 + std::sqrt(3.0) + std::sqrt(2.0));
	CHECK(mesh.vertices[(path.size() - 1) * ringStride].tex.y
		== doctest::Approx(expectedLength).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[0].normal.dot(mesh.vertices[ringStride].normal) > 0.0f);
}

TEST_CASE("Mesh3D::Tube minimum sides and nearly straight path")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 2.0, 1e-8, 0.0 }
	};
	const Mesh3D mesh = Mesh3D::Tube(path, 0.5, 3);

	CHECK_EQ(mesh.vertexCount(), size_t{ 5 * 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 * 3 * 3 });
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Tube invalid arguments")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> valid{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 } };

	CHECK(Mesh3D::Tube(std::span<const Vec3>{}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{ { 0.0, 0.0, 0.0 } }, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(valid, 0.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(valid, -1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(valid, std::numeric_limits<double>::infinity(), Sides).isEmpty());
	CHECK(Mesh3D::Tube(valid, std::numeric_limits<double>::max(), Sides).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0, 2).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0, std::numeric_limits<uint32>::max()).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
	}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 1.0, 0.0, 0.0 }, { (1.0 + 1e-10), 0.0, 0.0 }
	}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
	}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 }
	}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { std::numeric_limits<double>::infinity(), 0.0, 0.0 }
	}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { std::numeric_limits<double>::max(), 0.0, 0.0 }
	}, 1.0, Sides).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0, Sides,
		Vec2{ std::numeric_limits<double>::quiet_NaN(), 1.0 }).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0, Sides,
		Vec2{ 1.0, std::numeric_limits<float>::max() }).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0, Sides,
		Vec2{ 1.0, 1.0 }, Vec2{ 0.0, std::numeric_limits<double>::infinity() }).isEmpty());
}
