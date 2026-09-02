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
		static_cast<Mesh3D (*)(const Polygon&, std::span<const Vec3>, Vec2, Vec2)>(&Mesh3D::Sweep);
		static_cast<Mesh3D (*)(const Polygon&, std::span<const Vec3>, Vec3, Vec2, Vec2)>(&Mesh3D::Sweep);
		static_cast<Mesh3D (*)(const Polygon&, std::initializer_list<Vec3>, Vec2, Vec2)>(&Mesh3D::Sweep);
		static_cast<Mesh3D (*)(const Polygon&, std::initializer_list<Vec3>, Vec3, Vec2, Vec2)>(&Mesh3D::Sweep);
	});

}

TEST_CASE("Mesh3D::Sweep initializer list")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };

	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, {
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
		}, uvScale, uvOffset),
		Mesh3D::Sweep(crossSection, path, uvScale, uvOffset));
	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, {
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
		}, Vec3::UnitX(), uvScale, uvOffset),
		Mesh3D::Sweep(crossSection, path, Vec3::UnitX(), uvScale, uvOffset));
	CHECK(Mesh3D::Sweep(crossSection, {}).isEmpty());
}

TEST_CASE("Mesh3D::Sweep rectangle and UV repeat")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { -2.0, 1.0 }
	} };
	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 0.0, 3.0, 0.0 } };
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mesh3D mesh = Mesh3D::Sweep(
		crossSection, path, Vec3::UnitX(), uvScale, uvOffset);
	const size_t capVertexCount = (crossSection.vertices().size() * 2);

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.1f, 0.2f });
	CHECK_EQ(mesh.vertices[0].tangent, Float4{ 1.0f, 0.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[0].bitangent(), Float3::UnitZ());

	CHECK_EQ(mesh.vertices[capVertexCount + 0].pos, Float3{ -2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[capVertexCount + 0].normal, -Float3::UnitZ());
	CHECK_EQ(mesh.vertices[capVertexCount + 0].tex, Float2{ 0.1f, 0.2f });
	CHECK(mesh.vertices[capVertexCount + 1].tex.x
		== doctest::Approx(0.1 + (2.0 / 3.0)).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[capVertexCount + 2].tex, Float2{ 0.1f, 0.95f });
	CHECK_EQ(mesh.vertices[capVertexCount + 0].bitangent(), Float3::UnitY());

	const Mesh3D automatic = Mesh3D::Sweep(crossSection, path, uvScale, uvOffset);
	CheckMeshGeometry(automatic);
	CHECK_EQ(automatic.vertices[0].pos, mesh.vertices[0].pos);
}

TEST_CASE("Mesh3D::Sweep bent non-planar path")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 2.0, 0.0 },
		{ 1.0, 3.0, 1.0 },
		{ 2.0, 3.0, 2.0 }
	};
	const Mesh3D mesh = Mesh3D::Sweep(crossSection, path, Vec3::UnitX());

	CHECK_EQ(mesh.vertexCount(), size_t{ 40 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 28 });
	CheckMeshGeometry(mesh);
	const size_t sideVertexBase = (crossSection.vertices().size() * 2);
	const size_t verticesPerEdge = (path.size() * 2);
	CHECK(mesh.vertices[sideVertexBase].normal.dot(
		mesh.vertices[sideVertexBase + 2].normal) > 0.0f);
	CHECK_NE(mesh.vertices[sideVertexBase].normal,
		mesh.vertices[sideVertexBase + verticesPerEdge].normal);
}

TEST_CASE("Mesh3D::Sweep polygon with a hole")
{
	const Polygon crossSection{
		Array<Vec2>{
			{ -3.0, -3.0 }, { 3.0, -3.0 }, { 3.0, 3.0 }, { -3.0, 3.0 }
		},
		Array<Array<Vec2>>{ Array<Vec2>{
			{ -1.0, -1.0 }, { -1.0, 1.0 }, { 1.0, 1.0 }, { 1.0, -1.0 }
		} }
	};
	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 4.0, 0.0, 0.0 } };
	const Mesh3D mesh = Mesh3D::Sweep(crossSection, path, Vec3::UnitY());
	const size_t edgeCount = (crossSection.outer().size() + crossSection.inners()[0].size());

	CHECK_EQ(mesh.vertexCount(), ((crossSection.vertices().size() * 2) + (edgeCount * 4)));
	CHECK_EQ(mesh.triangleCount(), ((crossSection.indices().size() * 2) + (edgeCount * 2)));
	CheckMeshGeometry(mesh);

	const size_t innerSideBase = ((crossSection.vertices().size() * 2)
		+ (crossSection.outer().size() * 4));
	CHECK_EQ(mesh.vertices[innerSideBase].normal, Float3::UnitY());
}

TEST_CASE("Mesh3D::Sweep initial orientation")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { -2.0, 1.0 }
	} };
	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 3.0 } };
	const Mesh3D xOriented = Mesh3D::Sweep(crossSection, path, Vec3::UnitX());
	const Mesh3D yOriented = Mesh3D::Sweep(crossSection, path, Vec3::UnitY());

	CheckMeshGeometry(xOriented);
	CheckMeshGeometry(yOriented);
	CHECK_EQ(xOriented.vertices[0].pos, Float3{ -2.0f, 1.0f, 0.0f });
	CHECK_EQ(yOriented.vertices[0].pos, Float3{ -1.0f, -2.0f, 0.0f });
}

TEST_CASE("Mesh3D::Sweep invalid arguments")
{
	const Array<Vec2> outer{
		{ 0.0, 0.0 }, { 2.0, 0.0 }, { 2.0, 2.0 }, { 0.0, 2.0 }
	};
	const Polygon crossSection{ outer };
	const Array<Vec3> validPath{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 } };

	CHECK(Mesh3D::Sweep(Polygon{}, validPath).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, std::span<const Vec3>{}).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 0.0, 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath, Vec3::Zero()).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath, Vec3::UnitY()).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Vec3{ std::numeric_limits<double>::infinity(), 0.0, 0.0 }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Vec2{ std::numeric_limits<double>::quiet_NaN(), 1.0 }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Vec2{ 1.0, std::numeric_limits<float>::max() }).isEmpty());

	const Polygon invalidIndices{
		outer,
		Array<TriangleIndex>{ TriangleIndex{ 0, 1, 9 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Sweep(invalidIndices, validPath).isEmpty());

	const Array<Vec2> reversedOuter{
		{ 0.0, 0.0 }, { 0.0, 2.0 }, { 2.0, 2.0 }, { 2.0, 0.0 }
	};
	const Polygon invalidOuter{
		reversedOuter,
		Array<TriangleIndex>{ TriangleIndex{ 0, 2, 1 }, TriangleIndex{ 0, 3, 2 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Sweep(invalidOuter, validPath).isEmpty());
}
