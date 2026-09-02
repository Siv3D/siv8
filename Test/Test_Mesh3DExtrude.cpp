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
	using Mesh3DTest::CheckMeshGeometry;
	using Mesh3DTest::CheckVertexFrame;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(const Polygon&, double)>(&Mesh3D::Extrude);
		static_cast<Mesh3D (*)(const Polygon&, double, double)>(&Mesh3D::Extrude);
	});

}

TEST_CASE("Mesh3D::Extrude rectangle")
{
	const Polygon polygon{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 3.0 }, { -2.0, 3.0 }
	} };
	const Mesh3D mesh = Mesh3D::Extrude(polygon, 6.0);

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);

	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 3.0f, 1.0f });
	CHECK_EQ(mesh.vertices[0].normal, Float3::UnitY());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[2].pos, Float3{ 2.0f, 3.0f, -3.0f });
	CHECK_EQ(mesh.vertices[2].tex, Float2{ 1.0f, 1.0f });

	CHECK_EQ(mesh.vertices[4].pos, Float3{ -2.0f, -3.0f, 1.0f });
	CHECK_EQ(mesh.vertices[4].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[4].tex, Float2{ 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[6].tex, Float2{ 1.0f, 0.0f });

	constexpr size_t SideVertexBase = 8;
	CHECK_EQ(mesh.vertices[SideVertexBase + 0].normal, Float3::UnitZ());
	CHECK_EQ(mesh.vertices[SideVertexBase + 0].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[SideVertexBase + 1].tex, Float2{ 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[SideVertexBase + 2].tex, Float2{ 0.25f, 0.0f });
	CHECK_EQ(mesh.vertices[SideVertexBase + 0].tangent, Float4{ 1.0f, 0.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[SideVertexBase + 0].bitangent(), -Float3::UnitY());
	CHECK_EQ(mesh.vertices[SideVertexBase + 4].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[SideVertexBase + 8].normal, -Float3::UnitZ());
	CHECK_EQ(mesh.vertices[SideVertexBase + 12].normal, -Float3::UnitX());

	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.x && vertex.pos.x <= 2.0f));
		CHECK((-3.0f <= vertex.pos.y && vertex.pos.y <= 3.0f));
		CHECK((-3.0f <= vertex.pos.z && vertex.pos.z <= 1.0f));
	}
}

TEST_CASE("Mesh3D::Extrude concave polygon")
{
	const Polygon polygon{ Array<Vec2>{
		{ 0.0, 0.0 }, { 3.0, 0.0 }, { 3.0, 1.0 },
		{ 1.0, 1.0 }, { 1.0, 3.0 }, { 0.0, 3.0 }
	} };
	const Mesh3D mesh = Mesh3D::Extrude(polygon, 2.0);

	CHECK_EQ(mesh.vertexCount(), ((polygon.vertices().size() * 2) + 24));
	CHECK_EQ(mesh.triangleCount(), ((polygon.indices().size() * 2) + 12));
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Extrude polygon with a hole")
{
	const Polygon polygon{
		Array<Vec2>{
			{ -3.0, -3.0 }, { 3.0, -3.0 }, { 3.0, 3.0 }, { -3.0, 3.0 }
		},
		Array<Array<Vec2>>{ Array<Vec2>{
			{ -1.0, -1.0 }, { -1.0, 1.0 }, { 1.0, 1.0 }, { 1.0, -1.0 }
		} }
	};
	const Mesh3D mesh = Mesh3D::Extrude(polygon, 4.0);

	CHECK_EQ(mesh.vertexCount(), ((polygon.vertices().size() * 2) + 32));
	CHECK_EQ(mesh.triangleCount(), ((polygon.indices().size() * 2) + 16));
	CheckMeshGeometry(mesh);

	const size_t innerSideVertexBase = ((polygon.vertices().size() * 2) + 16);
	CHECK_EQ(mesh.vertices[innerSideVertexBase].pos, Float3{ -1.0f, 2.0f, 1.0f });
	CHECK_EQ(mesh.vertices[innerSideVertexBase].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[innerSideVertexBase].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[innerSideVertexBase + 2].tex, Float2{ 0.25f, 0.0f });
	CHECK_EQ(mesh.vertices[innerSideVertexBase + 4].normal, Float3::UnitZ());
	CHECK_EQ(mesh.vertices[innerSideVertexBase + 8].normal, -Float3::UnitX());
	CHECK_EQ(mesh.vertices[innerSideVertexBase + 12].normal, -Float3::UnitZ());

	const Mesh3D smoothMesh = Mesh3D::Extrude(polygon, 4.0, Math::HalfPi);
	CheckMeshGeometry(smoothMesh);
	CHECK(smoothMesh.vertices[innerSideVertexBase].normal.x == doctest::Approx(Math::InvSqrt2));
	CHECK(smoothMesh.vertices[innerSideVertexBase].normal.z == doctest::Approx(-Math::InvSqrt2));
}

TEST_CASE("Mesh3D::Extrude polygon produced by a shape")
{
	const Polygon polygon = Circle{ 2.0 }.asPolygon(PointsPerCircle{ 12 });
	const Mesh3D mesh = Mesh3D::Extrude(polygon, 3.0);

	CHECK_EQ(mesh.vertexCount(), ((polygon.vertices().size() * 2) + (polygon.outer().size() * 4)));
	CHECK_EQ(mesh.triangleCount(), ((polygon.indices().size() * 2) + (polygon.outer().size() * 2)));
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Extrude side normal smoothing")
{
	const Polygon rectangle{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { -2.0, 1.0 }
	} };
	const Mesh3D hard = Mesh3D::Extrude(rectangle, 2.0);
	const Mesh3D zeroAngle = Mesh3D::Extrude(rectangle, 2.0, 0.0);
	const Mesh3D belowCornerAngle = Mesh3D::Extrude(rectangle, 2.0, Math::QuarterPi);
	const Mesh3D smoothCorners = Mesh3D::Extrude(rectangle, 2.0, Math::HalfPi);
	const size_t sideVertexBase = (rectangle.vertices().size() * 2);

	REQUIRE_EQ(zeroAngle.vertexCount(), hard.vertexCount());
	REQUIRE_EQ(zeroAngle.triangleCount(), hard.triangleCount());
	for (size_t i = 0; i < hard.vertexCount(); ++i)
	{
		CHECK_EQ(zeroAngle.vertices[i].normal, hard.vertices[i].normal);
		CHECK_EQ(zeroAngle.vertices[i].tangent, hard.vertices[i].tangent);
	}

	CheckMeshGeometry(belowCornerAngle);
	CHECK_EQ(belowCornerAngle.vertices[sideVertexBase].normal, Float3::UnitZ());
	CHECK_EQ(belowCornerAngle.vertices[sideVertexBase + 2].normal, Float3::UnitZ());

	CheckMeshGeometry(smoothCorners);
	const Float3 startNormal = smoothCorners.vertices[sideVertexBase].normal;
	const Float3 endNormal = smoothCorners.vertices[sideVertexBase + 2].normal;
	CHECK(startNormal.x == doctest::Approx(-Math::InvSqrt2));
	CHECK(startNormal.z == doctest::Approx(Math::InvSqrt2));
	CHECK(endNormal.x == doctest::Approx(Math::InvSqrt2));
	CHECK(endNormal.z == doctest::Approx(Math::InvSqrt2));

	const Polygon circle = Circle{ 2.0 }.asPolygon(PointsPerCircle{ 12 });
	const Mesh3D smoothCircle = Mesh3D::Extrude(circle, 2.0, Math::QuarterPi);
	const size_t circleSideVertexBase = (circle.vertices().size() * 2);
	CheckMeshGeometry(smoothCircle);
	for (size_t i = 0; i < circle.outer().size(); ++i)
	{
		const size_t currentEnd = (circleSideVertexBase + i * 4 + 2);
		const size_t nextStart = (circleSideVertexBase + ((i + 1) % circle.outer().size()) * 4);
		CHECK((smoothCircle.vertices[currentEnd].normal
			- smoothCircle.vertices[nextStart].normal).lengthSq() < FrameEpsilon);
		CHECK((smoothCircle.vertices[currentEnd].tangent
			- smoothCircle.vertices[nextStart].tangent).lengthSq() < FrameEpsilon);
	}
}

TEST_CASE("Mesh3D::Extrude large finite coordinates")
{
	constexpr float M = std::numeric_limits<float>::max();
	const Array<Vec2> outer{
		{ -M, -M }, { M, -M }, { M, M }, { -M, M }
	};
	const Polygon polygon{
		outer,
		Array<TriangleIndex>{ TriangleIndex{ 0, 1, 2 }, TriangleIndex{ 0, 2, 3 } },
		RectF{ -M, -M, (static_cast<double>(M) * 2.0), (static_cast<double>(M) * 2.0) },
		SkipValidation::Yes
	};
	const Mesh3D mesh = Mesh3D::Extrude(polygon, 1.0);

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	REQUIRE(mesh.validate());
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CheckVertexFrame(vertex);
	}
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[2].tex, Float2{ 1.0f, 1.0f });
}

TEST_CASE("Mesh3D::Extrude invalid arguments")
{
	const Array<Vec2> outer{
		{ 0.0, 0.0 }, { 2.0, 0.0 }, { 2.0, 2.0 }, { 0.0, 2.0 }
	};
	const Polygon polygon{ outer };

	CHECK(Mesh3D::Extrude(Polygon{}, 1.0).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, 0.0).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, -1.0).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, std::numeric_limits<double>::infinity()).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, std::numeric_limits<double>::max()).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, 1.0, -0.001).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, 1.0, (Math::Pi + 0.001)).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, 1.0, std::numeric_limits<double>::quiet_NaN()).isEmpty());
	CHECK(Mesh3D::Extrude(polygon, 1.0, std::numeric_limits<double>::infinity()).isEmpty());

	const Polygon invalidIndices{
		outer,
		Array<TriangleIndex>{ TriangleIndex{ 0, 1, 9 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Extrude(invalidIndices, 1.0).isEmpty());

	const Polygon reversedTriangles{
		outer,
		Array<TriangleIndex>{ TriangleIndex{ 0, 2, 1 }, TriangleIndex{ 0, 3, 2 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Extrude(reversedTriangles, 1.0).isEmpty());

	const Array<Vec2> reversedOuter{
		{ 0.0, 0.0 }, { 0.0, 2.0 }, { 2.0, 2.0 }, { 2.0, 0.0 }
	};
	const Polygon invalidOuter{
		reversedOuter,
		Array<TriangleIndex>{ TriangleIndex{ 0, 2, 1 }, TriangleIndex{ 0, 3, 2 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Extrude(invalidOuter, 1.0).isEmpty());
}
