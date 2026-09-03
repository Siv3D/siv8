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

TEST_CASE("Mesh3DBuilder::addRoundedBox")
{
	constexpr Vec3 Size{ 2.0, 4.0, 6.0 };
	constexpr double Radius = 0.5;
	constexpr uint32 Subdivisions = 2;

	SUBCASE("Single rounded box matches Mesh3D::RoundedBox")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addRoundedBox(Size, Radius, Subdivisions));
		Mesh3DTest::CheckMeshDataEqual(
			builder.getMesh(),
			Mesh3D::RoundedBox(Size, Radius, Subdivisions));
	}

	SUBCASE("Appends indices with an offset")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addRoundedBox(Size, Radius, Subdivisions, offset));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(
			Mesh3D::RoundedBox(Size, Radius, Subdivisions),
			Mat4x4::Translate(Float3{ offset })));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Offset and rotation")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		Mesh3DBuilder builder;
		REQUIRE(builder.addRoundedBox(Size, Radius, Subdivisions, offset, rotation));

		const Mesh3D expected = Mesh3D::RoundedBox(Size, Radius, Subdivisions).transformed(
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
		REQUIRE(builder.addRoundedBox(Size, Radius, Subdivisions, uvMapping, transform));

		const Mesh3D expected = Mesh3D::RoundedBox(Size, Radius, Subdivisions, uvMapping).transformed(transform);
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Zero radius delegates to addBox")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addRoundedBox(Size, 0.0, Subdivisions));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), Mesh3D::Box(Size));
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addRoundedBox(Size, -0.1, Subdivisions));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addWedge")
{
	const Vec3 size{ 4.0, 2.0, 6.0 };

	SUBCASE("Appends indices with an offset")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addWedge(size, offset));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(Mesh3D::Wedge(size), Mat4x4::Translate(Float3{ offset })));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Offset, rotation, and custom UV mapping")
	{
		BoxUVMapping uvMapping;
		uvMapping.positiveY = FloatRect{ 0.1f, 0.2f, 0.8f, 0.7f };
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		Mesh3DBuilder builder;
		REQUIRE(builder.addWedge(size, uvMapping, offset, rotation));

		const Mesh3D expected = Mesh3D::Wedge(size, uvMapping).transformed(
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addWedge(Vec3{ 0.0, 1.0, 1.0 }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addTriangularPrism")
{
	const Vec3 size{ 4.0, 2.0, 6.0 };

	SUBCASE("Appends indices with an offset")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addTriangularPrism(size));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(Mesh3D::TriangularPrism(size)));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Matrix and custom UV mapping")
	{
		BoxUVMapping uvMapping;
		uvMapping.negativeX = FloatRect{ 0.9f, 0.8f, 0.5f, 0.2f };
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f },
			Quaternion::RotateZ(Math::QuarterPiF),
			Float3{ 5.0f, 6.0f, 7.0f });
		Mesh3DBuilder builder;
		REQUIRE(builder.addTriangularPrism(size, uvMapping, transform));

		const Mesh3D expected = Mesh3D::TriangularPrism(size, uvMapping).transformed(transform);
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addTriangularPrism(Vec3{ 1.0, -1.0, 1.0 }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addStairs")
{
	const Vec3 size{ 4.0, 3.0, 6.0 };
	constexpr uint32 Steps = 3;

	SUBCASE("Appends indices with an offset")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addStairs(size, Steps, offset));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(
			Mesh3D::Stairs(size, Steps),
			Mat4x4::Translate(Float3{ offset })));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Offset, rotation, and custom UV mapping")
	{
		BoxUVMapping uvMapping;
		uvMapping.negativeZ = FloatRect{ 0.1f, 0.2f, 0.8f, 0.7f };
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateX(Math::QuarterPiF);
		Mesh3DBuilder builder;
		REQUIRE(builder.addStairs(size, Steps, uvMapping, offset, rotation));

		const Mesh3D expected = Mesh3D::Stairs(size, Steps, uvMapping).transformed(
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addStairs(size, 0));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addRectangularFrustum")
{
	const SizeF bottomSizeXZ{ 4.0, 6.0 };
	const SizeF topSizeXZ{ 2.0, 3.0 };
	constexpr double Height = 5.0;

	SUBCASE("Appends indices with an offset")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addRectangularFrustum(bottomSizeXZ, topSizeXZ, Height));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(Mesh3D::RectangularFrustum(bottomSizeXZ, topSizeXZ, Height)));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Matrix and custom UV mapping")
	{
		BoxUVMapping uvMapping;
		uvMapping.positiveX = FloatRect{ 0.9f, 0.8f, 0.5f, 0.2f };
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f },
			Quaternion::RotateZ(Math::QuarterPiF),
			Float3{ 5.0f, 6.0f, 7.0f });
		Mesh3DBuilder builder;
		REQUIRE(builder.addRectangularFrustum(bottomSizeXZ, topSizeXZ, Height, uvMapping, transform));

		const Mesh3D expected = Mesh3D::RectangularFrustum(
			bottomSizeXZ, topSizeXZ, Height, uvMapping).transformed(transform);
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Equal ends delegate to addBox")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addRectangularFrustum(bottomSizeXZ, bottomSizeXZ, Height));
		Mesh3DTest::CheckMeshDataEqual(
			builder.getMesh(),
			Mesh3D::Box(Vec3{ bottomSizeXZ.x, Height, bottomSizeXZ.y }));
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addRectangularFrustum(bottomSizeXZ, topSizeXZ, 0.0));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addPyramid")
{
	const SizeF baseSizeXZ{ 4.0, 6.0 };
	constexpr double Height = 5.0;

	SUBCASE("Appends indices with an offset")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addPyramid(baseSizeXZ, Height));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(Mesh3D::Pyramid(baseSizeXZ, Height)));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Square base with offset and rotation")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		Mesh3DBuilder builder;
		REQUIRE(builder.addPyramid(4.0, Height, offset, rotation));

		const Mesh3D expected = Mesh3D::Pyramid(4.0, Height).transformed(
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addPyramid(baseSizeXZ, 0.0));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder regular polyhedra")
{
	SUBCASE("Appends all regular polyhedra with vertex offsets")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addTetrahedron(1.0));
		REQUIRE(builder.addOctahedron(1.5));
		REQUIRE(builder.addIcosahedron(2.0));
		REQUIRE(builder.addDodecahedron(2.5));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(Mesh3D::Tetrahedron(1.0)));
		REQUIRE(expected.append(Mesh3D::Octahedron(1.5)));
		REQUIRE(expected.append(Mesh3D::Icosahedron(2.0)));
		REQUIRE(expected.append(Mesh3D::Dodecahedron(2.5)));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateZ(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });

		Mesh3DBuilder builder;
		REQUIRE(builder.addTetrahedron(1.0, offset));
		REQUIRE(builder.addOctahedron(1.5, offset, rotation));
		REQUIRE(builder.addIcosahedron(2.0, transform));
		REQUIRE(builder.addDodecahedron(2.5, transform));

		Mesh3D expected;
		REQUIRE(expected.append(Mesh3D::Tetrahedron(1.0), Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(
			Mesh3D::Octahedron(1.5),
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(Mesh3D::Icosahedron(2.0), transform));
		REQUIRE(expected.append(Mesh3D::Dodecahedron(2.5), transform));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addTetrahedron(0.0));
		CHECK_FALSE(builder.addOctahedron(-1.0));
		CHECK_FALSE(builder.addIcosahedron(std::numeric_limits<double>::infinity()));
		CHECK_FALSE(builder.addDodecahedron(std::numeric_limits<double>::quiet_NaN()));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addPlane")
{
	const SizeF sizeXZ{ 4.0, 6.0 };

	SUBCASE("Appends indices with an offset")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addPlane(sizeXZ));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(Mesh3D::Plane(sizeXZ)));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("UV transform, offset, and rotation")
	{
		const Vec2 uvScale{ 2.0, 3.0 };
		const Vec2 uvOffset{ -0.25, 0.5 };
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateX(Math::QuarterPiF);
		Mesh3DBuilder builder;
		REQUIRE(builder.addPlane(sizeXZ, uvScale, uvOffset, offset, rotation));

		const Mesh3D expected = Mesh3D::Plane(sizeXZ, uvScale, uvOffset).transformed(
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addPlane(SizeF{ 0.0, 1.0 }));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3DBuilder::addGrid")
{
	const SizeF sizeXZ{ 4.0, 6.0 };
	constexpr uint32 SegmentsX = 3;
	constexpr uint32 SegmentsZ = 2;

	SUBCASE("Appends indices with an offset")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		REQUIRE(builder.addGrid(sizeXZ, SegmentsX, SegmentsZ, offset));

		Mesh3D expected = Mesh3D::Box();
		REQUIRE(expected.append(
			Mesh3D::Grid(sizeXZ, SegmentsX, SegmentsZ),
			Mat4x4::Translate(Float3{ offset })));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("UV transform and matrix")
	{
		const Vec2 uvScale{ 2.0, 3.0 };
		const Vec2 uvOffset{ 0.25, -0.5 };
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f },
			Quaternion::RotateZ(Math::QuarterPiF),
			Float3{ 5.0f, 6.0f, 7.0f });
		Mesh3DBuilder builder;
		REQUIRE(builder.addGrid(
			sizeXZ, SegmentsX, SegmentsZ, uvScale, uvOffset, transform));

		const Mesh3D expected = Mesh3D::Grid(
			sizeXZ, SegmentsX, SegmentsZ, uvScale, uvOffset).transformed(transform);
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addGrid(sizeXZ, 0, SegmentsZ));
		CHECK_FALSE(builder.addGrid(
			sizeXZ,
			std::numeric_limits<uint32>::max(),
			std::numeric_limits<uint32>::max()));
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
