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

		for (const auto& vertex : mesh.vertices)
		{
			CheckVertexFrame(vertex);
		}

		for (const auto& triangle : mesh.indices)
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

TEST_CASE("Mesh3D::Box")
{
	const Mesh3D mesh = Mesh3D::Box(Float3{ 2.0f, 4.0f, 6.0f });

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);

	for (const auto& vertex : mesh.vertices)
	{
		CHECK(std::abs(vertex.pos.x) <= 1.0f);
		CHECK(std::abs(vertex.pos.y) <= 2.0f);
		CHECK(std::abs(vertex.pos.z) <= 3.0f);
		CHECK((vertex.tex.x == 0.0f || vertex.tex.x == 1.0f));
		CHECK((vertex.tex.y == 0.0f || vertex.tex.y == 1.0f));
	}

	CHECK(Mesh3D::Box(Float3{ 0.0f, 1.0f, 1.0f }).isEmpty());
	CHECK(Mesh3D::Box(Float3{ 1.0f, -1.0f, 1.0f }).isEmpty());
	CHECK(Mesh3D::Box(Float3{ 1.0f, 1.0f, std::numeric_limits<float>::infinity() }).isEmpty());
}

TEST_CASE("Mesh3D::Grid")
{
	const Float2 uvScale{ 2.0f, 3.0f };
	const Float2 uvOffset{ 0.25f, -0.5f };
	const Mesh3D mesh = Mesh3D::Grid(Float2{ 4.0f, 2.0f }, 2, 1, uvScale, uvOffset);

	CHECK_EQ(mesh.vertexCount(), size_t{ 6 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 });
	CheckMeshGeometry(mesh);

	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[2].pos, Float3{ 2.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[3].pos, Float3{ -2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[5].pos, Float3{ 2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].tex, uvOffset);
	CHECK_EQ(mesh.vertices[5].tex, (uvOffset + uvScale));

	CHECK(Mesh3D::Grid(Float2{ 1.0f, 1.0f }, 0, 1).isEmpty());
	CHECK(Mesh3D::Grid(Float2{ 1.0f, 1.0f }, 1, 0).isEmpty());
	CHECK(Mesh3D::Grid(Float2{ 0.0f, 1.0f }, 1, 1).isEmpty());
	CHECK(Mesh3D::Grid(Float2{ 1.0f, 1.0f }, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::UVSphere")
{
	constexpr uint32 Slices = 8;
	constexpr uint32 Stacks = 4;
	const Mesh3D mesh = Mesh3D::UVSphere(2.0f, Slices, Stacks);
	const size_t expectedVertexCount = ((Stacks - 1) * (Slices + 1) + (2 * Slices));
	const size_t expectedTriangleCount = (2 * Slices * (Stacks - 1));

	CHECK_EQ(mesh.vertexCount(), expectedVertexCount);
	CHECK_EQ(mesh.triangleCount(), expectedTriangleCount);
	CheckMeshGeometry(mesh);

	for (const auto& vertex : mesh.vertices)
	{
		CHECK(vertex.pos.length() == doctest::Approx(2.0f).epsilon(FrameEpsilon));
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	const size_t firstRingBase = Slices;
	CHECK_EQ(mesh.vertices[firstRingBase].pos, mesh.vertices[firstRingBase + Slices].pos);
	CHECK_EQ(mesh.vertices[firstRingBase].normal, mesh.vertices[firstRingBase + Slices].normal);
	CHECK_EQ(mesh.vertices[firstRingBase].tangent, mesh.vertices[firstRingBase + Slices].tangent);
	CHECK_EQ(mesh.vertices[firstRingBase].tex.x, 0.0f);
	CHECK_EQ(mesh.vertices[firstRingBase + Slices].tex.x, 1.0f);

	CHECK(Mesh3D::UVSphere(0.0f, Slices, Stacks).isEmpty());
	CHECK(Mesh3D::UVSphere(1.0f, 2, Stacks).isEmpty());
	CHECK(Mesh3D::UVSphere(1.0f, Slices, 1).isEmpty());
	CHECK(Mesh3D::UVSphere(1.0f, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}
