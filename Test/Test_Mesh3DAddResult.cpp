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
# include <limits>

namespace
{
	void CheckRangeIndices(const Mesh3D& mesh, const Mesh3DRange& range)
	{
		const size_t vertexEnd = (range.vertexOffset + range.vertexCount);
		const size_t triangleEnd = (range.triangleOffset + range.triangleCount);
		REQUIRE(vertexEnd <= mesh.vertexCount());
		REQUIRE(triangleEnd <= mesh.triangleCount());

		for (size_t triangleIndex = range.triangleOffset; triangleIndex < triangleEnd; ++triangleIndex)
		{
			const TriangleIndex32& triangle = mesh.indices[triangleIndex];
			CHECK(range.vertexOffset <= triangle.i0);
			CHECK(range.vertexOffset <= triangle.i1);
			CHECK(range.vertexOffset <= triangle.i2);
			CHECK(triangle.i0 < vertexEnd);
			CHECK(triangle.i1 < vertexEnd);
			CHECK(triangle.i2 < vertexEnd);
		}
	}
}

TEST_CASE("Mesh3DAddResult ranges")
{
	Mesh3DBuilder builder;

	const Mesh3DAddResult boxResult = builder.addBox(Vec3{ 2.0, 4.0, 6.0 });
	REQUIRE(boxResult);
	CHECK_EQ(boxResult->vertexOffset, size_t{ 0 });
	CHECK_EQ(boxResult->vertexCount, size_t{ 24 });
	CHECK_EQ(boxResult->triangleOffset, size_t{ 0 });
	CHECK_EQ(boxResult->triangleCount, size_t{ 12 });
	CHECK_FALSE(boxResult->isEmpty());
	CheckRangeIndices(builder.getMesh(), *boxResult);

	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 } };
	const Mesh3DAddResult tubeResult = builder.addTube(
		path, 0.25, 8, Vec3{ 1.0, 2.0, 3.0 }, Quaternion::RotateX(30_degF));
	REQUIRE(tubeResult);
	CHECK_EQ(tubeResult->vertexOffset, boxResult->vertexCount);
	CHECK_EQ(tubeResult->vertexCount, size_t{ 36 });
	CHECK_EQ(tubeResult->triangleOffset, boxResult->triangleCount);
	CHECK_EQ(tubeResult->triangleCount, size_t{ 32 });
	CHECK_FALSE(tubeResult->isEmpty());
	CheckRangeIndices(builder.getMesh(), *tubeResult);

	const Mesh3DAddResult emptyResult = builder.addBox(
		Vec3{ 1.0, 1.0, 1.0 }, BoxFace::None_);
	REQUIRE(emptyResult);
	CHECK_EQ(emptyResult->vertexOffset, builder.getMesh().vertexCount());
	CHECK_EQ(emptyResult->triangleOffset, builder.getMesh().triangleCount());
	CHECK(emptyResult->isEmpty());
}

TEST_CASE("Mesh3DAddResult covers every builder shape family")
{
	Mesh3DBuilder builder;
	const Polygon rectangle{ Array<Vec2>{
		{ -0.5, -0.5 }, { 0.5, -0.5 }, { 0.5, 0.5 }, { -0.5, 0.5 }
	} };
	const Array<Vec3> path{
		Vec3::Zero(), Vec3::UnitY(), Vec3{ 1.0, 2.0, 0.0 }
	};
	const Array<Array<Vec2>> sections{
		{ { -0.5, -0.5 }, { 0.5, -0.5 }, { 0.5, 0.5 }, { -0.5, 0.5 } },
		{ { -0.25, -0.25 }, { 0.25, -0.25 }, { 0.25, 0.25 }, { -0.25, 0.25 } },
	};
	const Array<double> heights{ 0.0, 1.0 };
	const Grid<float> heightField{
		{ 0.0f, 0.25f },
		{ -0.25f, 0.0f },
	};

	const auto checkAddition = [&](auto&& add)
	{
		const size_t vertexOffset = builder.getMesh().vertexCount();
		const size_t triangleOffset = builder.getMesh().triangleCount();
		const Mesh3DAddResult result = add();
		REQUIRE(result);
		CHECK_EQ(result->vertexOffset, vertexOffset);
		CHECK_EQ(result->triangleOffset, triangleOffset);
		CHECK_EQ(result->vertexCount,
			(builder.getMesh().vertexCount() - vertexOffset));
		CHECK_EQ(result->triangleCount,
			(builder.getMesh().triangleCount() - triangleOffset));
		CHECK_FALSE(result->isEmpty());
		CheckRangeIndices(builder.getMesh(), *result);
	};

	checkAddition([&] { return builder.addMesh(Mesh3D::Box()); });
	checkAddition([&] { return builder.addBox(); });
	checkAddition([&] { return builder.addBoxShell(); });
	checkAddition([&] { return builder.addBoxFrame(); });
	checkAddition([&] { return builder.addRoundedBox(Vec3::One(), 0.1, 2); });
	checkAddition([&] { return builder.addChamferedBox(Vec3::One(), 0.1); });
	checkAddition([&] { return builder.addWedge(); });
	checkAddition([&] { return builder.addTriangularPrism(); });
	checkAddition([&] { return builder.addStairs(Vec3::One(), 3); });
	checkAddition([&] { return builder.addPyramid(1.0, 1.0); });
	checkAddition([&] {
		return builder.addRectangularFrustum(SizeF{ 1.0, 1.0 }, SizeF{ 0.5, 0.5 }, 1.0);
	});
	checkAddition([&] { return builder.addExtrude(rectangle, 1.0); });
	checkAddition([&] {
		return builder.addRevolve({ { 0.0, -0.5 }, { 0.5, -0.5 }, { 0.5, 0.5 }, { 0.0, 0.5 } }, 8);
	});
	checkAddition([&] { return builder.addTube(path, 0.1, 8); });
	checkAddition([&] { return builder.addSweep(rectangle, path); });
	checkAddition([&] { return builder.addTetrahedron(); });
	checkAddition([&] { return builder.addOctahedron(); });
	checkAddition([&] { return builder.addIcosahedron(); });
	checkAddition([&] { return builder.addDodecahedron(); });
	checkAddition([&] { return builder.addPlane(); });
	checkAddition([&] { return builder.addGrid(SizeF{ 1.0, 1.0 }, 2, 2); });
	checkAddition([&] { return builder.addHeightField(heightField, SizeF{ 1.0, 1.0 }); });
	checkAddition([&] { return builder.addLoft(sections, heights); });
	checkAddition([&] { return builder.addDisc(0.5, 8); });
	checkAddition([&] { return builder.addAnnulus(0.25, 0.5, 8); });
	checkAddition([&] { return builder.addHollowCylinder(0.25, 0.5, 1.0, 8); });
	checkAddition([&] { return builder.addConicalFrustum(0.5, 0.25, 1.0, 8); });
	checkAddition([&] { return builder.addCylinder(0.5, 1.0, 8); });
	checkAddition([&] { return builder.addCone(0.5, 1.0, 8); });
	checkAddition([&] { return builder.addTorus(0.75, 0.25, 8, 6); });
	checkAddition([&] { return builder.addSphere(0.5, 8, 4); });
	checkAddition([&] { return builder.addHemisphere(0.5, CloseBottom::Yes, 8, 2); });
	checkAddition([&] { return builder.addCapsule(0.25, 0.5, 8, 2); });

	const size_t vertexCount = builder.getMesh().vertexCount();
	const size_t triangleCount = builder.getMesh().triangleCount();
	const Mesh3DAddResult emptyResult = builder.addBoxShell(
		Vec3::One(), 0.1, BoxFace::All);
	REQUIRE(emptyResult);
	CHECK(emptyResult->isEmpty());
	CHECK_EQ(emptyResult->vertexOffset, vertexCount);
	CHECK_EQ(emptyResult->triangleOffset, triangleCount);
}

TEST_CASE("Mesh3DAddResult errors and atomicity")
{
	Mesh3DBuilder builder;
	REQUIRE(builder.addBox());
	const Mesh3D expected = builder.getMesh();
	const Polygon rectangle{ Array<Vec2>{
		{ -0.5, -0.5 }, { 0.5, -0.5 }, { 0.5, 0.5 }, { -0.5, 0.5 }
	} };
	const Array<Vec3> path{ Vec3::Zero(), Vec3::UnitY() };
	const Array<Array<Vec2>> sections{
		rectangle.outer(), rectangle.outer()
	};
	Grid<float> invalidHeightField(2, 2, 0.0f);
	invalidHeightField[0][0] = std::numeric_limits<float>::quiet_NaN();
	const Grid<float> validHeightField(2, 2, 0.0f);
	const double infinity = std::numeric_limits<double>::infinity();
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double floatCollapse = (1.0 + std::numeric_limits<double>::epsilon());
	const Polygon nonFinitePolygon{
		{ { 0.0, 0.0 }, { 1.0, 0.0 }, { nan, 1.0 } },
		Array<TriangleIndex>{ { 0, 1, 2 } }, RectF{ 0.0, 0.0, 1.0, 1.0 }, SkipValidation::Yes
	};
	const Array<Vec2> repeatedRing{
		{ 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 }, { 0.0, 0.0 }
	};
	const Array<Array<Vec2>> repeatedSections{ repeatedRing, repeatedRing };

	const auto checkFailure = [&](const Mesh3DAddResult& result, const Mesh3DErrorCode code)
	{
		REQUIRE_FALSE(result);
		CHECK_EQ(result.error().code, code);
		CHECK_FALSE(result.error().message.isEmpty());
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	};

	checkFailure(
		builder.addBox(Vec3{ std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0 }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addBox(Vec3{ 0.0, 1.0, 1.0 }),
		Mesh3DErrorCode::InvalidArgument);
	checkFailure(
		builder.addTube({}, 0.25),
		Mesh3DErrorCode::InvalidArgument);
	checkFailure(
		builder.addTube({ Vec3::Zero(), Vec3::UnitY() }, infinity),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addTube({ Vec3::Zero(), Vec3{ infinity, 0.0, 0.0 } }, 0.25),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addTube({ Vec3::Zero(), Vec3::Zero() }, 0.25),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addTube(
			{ Vec3::Zero(), Vec3::UnitX(), Vec3{ floatCollapse, 0.0, 0.0 } }, 0.25),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addTube(
			{ Vec3::Zero(), Vec3::UnitX(), Vec3::UnitY(), Vec3::Zero() },
			0.25, CloseRing::Yes),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addTube(
			{ Vec3::Zero(), Vec3{ 0.0, 100.0, 0.0 } }, 0.25, 8,
			Vec2{ 1.0, 1.0e38 }, Vec2::Zero()),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addTube(
			{ Vec3::Zero(), Vec3::UnitY() }, 0.25,
			std::numeric_limits<uint32>::max()),
		Mesh3DErrorCode::SizeLimit);
	checkFailure(
		builder.addMesh(Mesh3D{}),
		Mesh3DErrorCode::InvalidArgument);
	checkFailure(
		builder.addExtrude(Polygon{}, 1.0),
		Mesh3DErrorCode::InvalidArgument);
	checkFailure(
		builder.addExtrude(rectangle, infinity),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addExtrude(nonFinitePolygon, 1.0),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addRevolve({ Vec2::Zero(), Vec2::UnitY() }, 8),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addRevolve({ { 0.0, -1.0 }, { 1.0, -1.0 }, { 1.0, -1.0 }, { 0.0, 1.0 } }, 8),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addRevolve({ { -1.0, -1.0 }, { 1.0, 1.0 } }, 8),
		Mesh3DErrorCode::InvalidArgument);
	checkFailure(
		builder.addRevolve(
			{ { 0.0, -1.0 }, { 1.0, -1.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } },
			infinity, Math::HalfPi, 4),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addSweep(Polygon{}, path),
		Mesh3DErrorCode::InvalidArgument);
	checkFailure(
		builder.addSweep(rectangle, { Vec3::Zero(), Vec3{ infinity, 0.0, 0.0 } }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addSweep(rectangle, path, Arg::initialXAxis = Vec3{ infinity, 0.0, 0.0 }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addSweep(rectangle, path, Arg::initialXAxis = Vec3::UnitY()),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addSweep(rectangle, path, Vec2{ infinity, 1.0 }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addSweep(nonFinitePolygon, path),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addHeightField(invalidHeightField, SizeF{ 1.0, 1.0 }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addHeightField(validHeightField, SizeF{ infinity, 1.0 }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addLoft(sections, Array<double>{ 0.0, 0.0 }),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addLoft(sections, Array<double>{ 0.0, infinity }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addLoft(repeatedSections, Array<double>{ 0.0, 1.0 }),
		Mesh3DErrorCode::InvalidGeometry);
	checkFailure(
		builder.addLoft(sections, Array<double>{ 0.0, 1.0 }, Vec2{ infinity, 1.0 }),
		Mesh3DErrorCode::NumericRange);
	checkFailure(
		builder.addConicalFrustum(
			1.0, 0.5, 1.0, std::numeric_limits<uint32>::max()),
		Mesh3DErrorCode::SizeLimit);
}
