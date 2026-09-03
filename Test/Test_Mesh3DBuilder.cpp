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

TEST_CASE("Mesh3DBuilder::addBox")
{
	SUBCASE("Single box matches Mesh3D::Box")
	{
		Mesh3DBuilder builder;
		CHECK(builder.addBox(Vec3{ 2.0, 4.0, 6.0 }));

		Mesh3DTest::CheckMeshDataEqual(
			builder.getMesh(),
			Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 }));
	}

	SUBCASE("Multiple boxes reuse reserved storage")
	{
		Mesh3DBuilder builder;
		builder.reserve(48, 24);
		REQUIRE(builder.addBox());
		const Vertex3D* const vertexData = builder.getMesh().vertices.data();
		const TriangleIndex32* const indexData = builder.getMesh().indices.data();

		const Vec3 offset{ 3.0, 4.0, 5.0 };
		REQUIRE(builder.addBox(Vec3{ 2.0, 2.0, 2.0 }, offset));
		CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
		CHECK_EQ(builder.getMesh().indices.data(), indexData);

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(
			Mesh3D::Box(Vec3{ 2.0, 2.0, 2.0 }),
			Mat4x4::Translate(Float3{ offset })));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Offset and rotation")
	{
		const Vec3 size{ 2.0, 4.0, 6.0 };
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox(size, offset, rotation));

		const Mesh3D expected = Mesh3D::Box(size).transformed(
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Matrix and custom UV mapping")
	{
		BoxUVMapping uvMapping;
		uvMapping.positiveY = FloatRect{ 0.1f, 0.2f, 0.8f, 0.7f };
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f },
			Quaternion::RotateZ(Math::QuarterPiF),
			Float3{ 5.0f, 6.0f, 7.0f });
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox(Vec3{ 2.0, 4.0, 6.0 }, uvMapping, transform));

		const Mesh3D expected = Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 }, uvMapping).transformed(transform);
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addBox(Vec3{ 0.0, 1.0, 1.0 }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder storage")
{
	Mesh3DBuilder builder;
	builder.reserve(48, 24);
	REQUIRE(builder.addBox());

	const size_t vertexCapacity = builder.getMesh().vertices.capacity();
	const size_t triangleCapacity = builder.getMesh().indices.capacity();
	builder.clear();
	CHECK(builder.getMesh().vertices.isEmpty());
	CHECK(builder.getMesh().indices.isEmpty());
	CHECK_EQ(builder.getMesh().vertices.capacity(), vertexCapacity);
	CHECK_EQ(builder.getMesh().indices.capacity(), triangleCapacity);

	REQUIRE(builder.addBox());
	const Vertex3D* const vertexData = builder.getMesh().vertices.data();
	const TriangleIndex32* const indexData = builder.getMesh().indices.data();
	const Mesh3D mesh = std::move(builder).build();
	CHECK_EQ(mesh.vertices.data(), vertexData);
	CHECK_EQ(mesh.indices.data(), indexData);
	Mesh3DTest::CheckMeshGeometry(mesh);
}
