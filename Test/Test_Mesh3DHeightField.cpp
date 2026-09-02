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
	using Mesh3DTest::TangentHandedness;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(const Grid<float>&, SizeF, Vec2, Vec2)>(&Mesh3D::HeightField);
	});

}

TEST_CASE("Mesh3D::HeightField flat grid")
{
	const Grid<float> heights(3, 2, 0.0f);
	const SizeF sizeXZ{ 4.0, 2.0 };
	const Vec2 uvScale{ 2.0, 3.0 };
	const Vec2 uvOffset{ 0.25, -0.5 };
	const Mesh3D mesh = Mesh3D::HeightField(heights, sizeXZ, uvScale, uvOffset);
	const Mesh3D grid = Mesh3D::Grid(sizeXZ, 2, 1, uvScale, uvOffset);

	CHECK_EQ(mesh.vertexCount(), size_t{ 6 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 });
	Mesh3DTest::CheckMeshGeometry(mesh, TangentHandedness::Positive);
	REQUIRE_EQ(mesh.vertexCount(), grid.vertexCount());
	REQUIRE_EQ(mesh.triangleCount(), grid.triangleCount());
	for (size_t i = 0; i < mesh.vertexCount(); ++i)
	{
		CHECK_EQ(mesh.vertices[i].pos, grid.vertices[i].pos);
		CHECK_EQ(mesh.vertices[i].normal, grid.vertices[i].normal);
		CHECK_EQ(mesh.vertices[i].tex, grid.vertices[i].tex);
		CHECK_EQ(mesh.vertices[i].tangent, grid.vertices[i].tangent);
	}
	for (size_t i = 0; i < mesh.triangleCount(); ++i)
	{
		CHECK_EQ(mesh.indices[i].i0, grid.indices[i].i0);
		CHECK_EQ(mesh.indices[i].i1, grid.indices[i].i1);
		CHECK_EQ(mesh.indices[i].i2, grid.indices[i].i2);
	}
}

TEST_CASE("Mesh3D::HeightField sloped plane")
{
	const Grid<float> heights{
		{ 0.0f, 1.0f, 2.0f },
		{ 0.0f, 1.0f, 2.0f },
		{ 0.0f, 1.0f, 2.0f }
	};
	const Mesh3D mesh = Mesh3D::HeightField(heights, SizeF{ 2.0, 2.0 });

	CHECK_EQ(mesh.vertexCount(), size_t{ 9 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 8 });
	Mesh3DTest::CheckMeshGeometry(mesh, TangentHandedness::Positive);
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK(vertex.normal.x == doctest::Approx(-Math::InvSqrt2).epsilon(FrameEpsilon));
		CHECK(vertex.normal.y == doctest::Approx(Math::InvSqrt2).epsilon(FrameEpsilon));
		CHECK(vertex.normal.z == doctest::Approx(0.0f).scale(1.0));
		CHECK(vertex.tangent.x == doctest::Approx(Math::InvSqrt2).epsilon(FrameEpsilon));
		CHECK(vertex.tangent.y == doctest::Approx(Math::InvSqrt2).epsilon(FrameEpsilon));
		CHECK(vertex.bitangent().x == doctest::Approx(0.0f).scale(1.0));
		CHECK(vertex.bitangent().y == doctest::Approx(0.0f).scale(1.0));
		CHECK(vertex.bitangent().z == doctest::Approx(-1.0f).epsilon(FrameEpsilon));
	}
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -1.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[8].pos, Float3{ 1.0f, 2.0f, -1.0f });
}

TEST_CASE("Mesh3D::HeightField irregular heights")
{
	const Grid<float> heights{
		{ 0.0f, 0.2f, 0.0f, -0.1f },
		{ 0.1f, 0.8f, 0.4f, 0.0f },
		{ 0.0f, 0.3f, 0.1f, -0.2f }
	};
	const Mesh3D mesh = Mesh3D::HeightField(
		heights, SizeF{ 6.0, 4.0 }, Vec2{ 3.0, 2.0 }, Vec2{ -0.5, 0.25 });

	CHECK_EQ(mesh.vertexCount(), size_t{ 12 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	Mesh3DTest::CheckMeshGeometry(mesh, TangentHandedness::Positive);
	CHECK_EQ(mesh.vertices[0].tex, Float2{ -0.5f, 0.25f });
	CHECK_EQ(mesh.vertices[11].tex, Float2{ 2.5f, 2.25f });
	CHECK_EQ(mesh.vertices[5].pos, Float3{ -1.0f, 0.8f, 0.0f });
}

TEST_CASE("Mesh3D::HeightField boundary values")
{
	const Grid<float> minimum{
		{ 0.0f, 0.0f },
		{ 0.0f, 0.0f }
	};
	const Mesh3D minimumMesh = Mesh3D::HeightField(minimum, SizeF{ 1.0, 1.0 });
	CHECK_EQ(minimumMesh.vertexCount(), size_t{ 4 });
	CHECK_EQ(minimumMesh.triangleCount(), size_t{ 2 });
	Mesh3DTest::CheckMeshGeometry(minimumMesh, TangentHandedness::Positive);

	const Grid<float> maximum(2, 2, std::numeric_limits<float>::max());
	const Mesh3D maximumMesh = Mesh3D::HeightField(maximum, SizeF{ 1.0, 1.0 });
	REQUIRE(maximumMesh.validate());
	for (const Vertex3D& vertex : maximumMesh.vertices)
	{
		Mesh3DTest::CheckVertexFrame(vertex, TangentHandedness::Positive);
		CHECK_EQ(vertex.pos.y, std::numeric_limits<float>::max());
	}
}

TEST_CASE("Mesh3D::HeightField invalid arguments")
{
	const Grid<float> valid{
		{ 0.0f, 0.0f },
		{ 0.0f, 0.0f }
	};

	CHECK(Mesh3D::HeightField(Grid<float>{}, SizeF{ 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(Grid<float>{ 1, 2, 0.0f }, SizeF{ 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(Grid<float>{ 2, 1, 0.0f }, SizeF{ 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(valid, SizeF{ 0.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(valid, SizeF{ 1.0, -1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(valid,
		SizeF{ std::numeric_limits<double>::infinity(), 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(valid,
		SizeF{ std::numeric_limits<double>::max(), 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(valid,
		SizeF{ std::numeric_limits<double>::denorm_min(), 1.0 }).isEmpty());

	Grid<float> nonFinite = valid;
	nonFinite[0][0] = std::numeric_limits<float>::quiet_NaN();
	CHECK(Mesh3D::HeightField(nonFinite, SizeF{ 1.0, 1.0 }).isEmpty());
	nonFinite[0][0] = std::numeric_limits<float>::infinity();
	CHECK(Mesh3D::HeightField(nonFinite, SizeF{ 1.0, 1.0 }).isEmpty());

	CHECK(Mesh3D::HeightField(valid, SizeF{ 1.0, 1.0 },
		Vec2{ std::numeric_limits<double>::quiet_NaN(), 1.0 }).isEmpty());
	CHECK(Mesh3D::HeightField(valid, SizeF{ 1.0, 1.0 },
		Vec2{ std::numeric_limits<float>::max(), 1.0 },
		Vec2{ std::numeric_limits<float>::max(), 0.0 }).isEmpty());
}
