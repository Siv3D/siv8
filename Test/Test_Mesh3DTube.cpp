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

# include "Siv3DTest.hpp"

namespace
{
	constexpr float FrameEpsilon = 1e-5f;
	constexpr float TriangleAreaEpsilon = 1e-10f;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(std::span<const Vec3>, double, uint32, Vec2, Vec2)>(&Mesh3D::Tube);
	});

	static void CheckVertexFrame(const Vertex3D& vertex)
	{
		CHECK(std::isfinite(vertex.pos.x));
		CHECK(std::isfinite(vertex.pos.y));
		CHECK(std::isfinite(vertex.pos.z));
		CHECK(std::isfinite(vertex.tex.x));
		CHECK(std::isfinite(vertex.tex.y));
		CHECK(vertex.normal.length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(vertex.tangent.xyz().length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(std::abs(vertex.normal.dot(vertex.tangent.xyz())) < FrameEpsilon);
		CHECK(std::abs(std::abs(vertex.tangent.w) - 1.0f) < FrameEpsilon);
		CHECK(vertex.bitangent().length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
	}

	static void CheckMeshGeometry(const Mesh3D& mesh)
	{
		REQUIRE_FALSE(mesh.isEmpty());
		REQUIRE(mesh.validate());

		for (const Vertex3D& vertex : mesh.vertices)
		{
			CheckVertexFrame(vertex);
		}

		for (const TriangleIndex32& triangle : mesh.indices)
		{
			const Vertex3D& v0 = mesh.vertices[triangle.i0];
			const Vertex3D& v1 = mesh.vertices[triangle.i1];
			const Vertex3D& v2 = mesh.vertices[triangle.i2];
			const Float3 faceNormal = (v1.pos - v0.pos).cross(v2.pos - v0.pos);
			const Float3 vertexNormal = (v0.normal + v1.normal + v2.normal);

			CHECK(faceNormal.lengthSq() > TriangleAreaEpsilon);
			CHECK(faceNormal.dot(vertexNormal) > 0.0f);
		}
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
