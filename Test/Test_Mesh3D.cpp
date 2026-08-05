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
	[[nodiscard]]
	static Vertex3D MakeVertex(const float x)
	{
		return Vertex3D{
			.pos = Float3{ x, (x + 1.0f), (x + 2.0f) },
			.normal = Float3{ (x + 3.0f), (x + 4.0f), (x + 5.0f) },
			.tex = Float2{ (x + 6.0f), (x + 7.0f) },
			.tangent = Float4{ (x + 8.0f), (x + 9.0f), (x + 10.0f), ((x == 0.0f) ? -1.0f : 1.0f) }
		};
	}

	static void CheckTriangle(const TriangleIndex32& triangle, const uint32 i0, const uint32 i1, const uint32 i2)
	{
		CHECK_EQ(triangle.i0, i0);
		CHECK_EQ(triangle.i1, i1);
		CHECK_EQ(triangle.i2, i2);
	}
}

TEST_CASE("Mesh3D::validate")
{
	CHECK(Mesh3D{}.validate());
	CHECK(Mesh3D{ Array<Vertex3D>(3), {} }.validate());
	CHECK(Mesh3D{ Array<Vertex3D>(3), { TriangleIndex32{ 0, 1, 2 } } }.validate());

	CHECK_FALSE(Mesh3D{ {}, { TriangleIndex32{ 0, 0, 0 } } }.validate());
	CHECK_FALSE(Mesh3D{ Array<Vertex3D>(3), { TriangleIndex32{ 3, 1, 2 } } }.validate());
	CHECK_FALSE(Mesh3D{ Array<Vertex3D>(3), { TriangleIndex32{ 0, 3, 2 } } }.validate());
	CHECK_FALSE(Mesh3D{ Array<Vertex3D>(3), { TriangleIndex32{ 0, 1, 3 } } }.validate());
	CHECK_FALSE(Mesh3D{ Array<Vertex3D>(3), { TriangleIndex32{ 0, 1, std::numeric_limits<uint32>::max() } } }.validate());
}

TEST_CASE("Mesh3D::append")
{
	SUBCASE("Another mesh")
	{
		Mesh3D mesh{
			{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
			{ TriangleIndex32{ 0, 1, 2 } }
		};
		const Mesh3D source{
			{ MakeVertex(3.0f), MakeVertex(4.0f), MakeVertex(5.0f), MakeVertex(6.0f) },
			{ TriangleIndex32{ 0, 2, 3 }, TriangleIndex32{ 0, 1, 2 } }
		};

		CHECK(mesh.append(source));
		CHECK_EQ(mesh.vertexCount(), size_t{ 7 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 3 });
		CHECK((mesh.vertices[3].pos == source.vertices[0].pos));
		CHECK((mesh.vertices[6].tangent == source.vertices[3].tangent));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
		CheckTriangle(mesh.indices[1], 3, 5, 6);
		CheckTriangle(mesh.indices[2], 3, 4, 5);
	}

	SUBCASE("Self append")
	{
		Mesh3D mesh{
			{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
			{ TriangleIndex32{ 0, 1, 2 } }
		};

		CHECK(mesh.append(mesh));
		CHECK_EQ(mesh.vertexCount(), size_t{ 6 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 2 });
		CHECK((mesh.vertices[3].pos == mesh.vertices[0].pos));
		CHECK((mesh.vertices[5].tangent == mesh.vertices[2].tangent));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
		CheckTriangle(mesh.indices[1], 3, 4, 5);
	}

	SUBCASE("Invalid source leaves the destination unchanged")
	{
		Mesh3D mesh{
			{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
			{ TriangleIndex32{ 0, 1, 2 } }
		};
		const Mesh3D source{
			{ MakeVertex(3.0f), MakeVertex(4.0f), MakeVertex(5.0f) },
			{ TriangleIndex32{ 0, 1, 3 } }
		};

		CHECK_FALSE(mesh.append(source));
		CHECK_EQ(mesh.vertexCount(), size_t{ 3 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 1 });
		CHECK((mesh.vertices[2].pos == MakeVertex(2.0f).pos));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
	}
}

TEST_CASE("Mesh3D::reverseWinding")
{
	Mesh3D mesh{
		Array<Vertex3D>(4),
		{ TriangleIndex32{ 0, 1, 2 }, TriangleIndex32{ 1, 3, 2 } }
	};

	CHECK(&mesh.reverseWinding() == &mesh);
	CheckTriangle(mesh.indices[0], 0, 2, 1);
	CheckTriangle(mesh.indices[1], 1, 2, 3);

	mesh.reverseWinding();
	CheckTriangle(mesh.indices[0], 0, 1, 2);
	CheckTriangle(mesh.indices[1], 1, 3, 2);
}

TEST_CASE("Mesh3D::invertNormals")
{
	Mesh3D mesh{
		{ MakeVertex(0.0f), MakeVertex(1.0f) },
		{}
	};
	const Array<Vertex3D> original = mesh.vertices;
	const Float3 originalBitangent = mesh.vertices[0].bitangent();

	CHECK(&mesh.invertNormals() == &mesh);
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK((mesh.vertices[i].pos == original[i].pos));
		CHECK((mesh.vertices[i].normal == -original[i].normal));
		CHECK((mesh.vertices[i].tex == original[i].tex));
		CHECK_EQ(mesh.vertices[i].tangent.x, original[i].tangent.x);
		CHECK_EQ(mesh.vertices[i].tangent.y, original[i].tangent.y);
		CHECK_EQ(mesh.vertices[i].tangent.z, original[i].tangent.z);
		CHECK_EQ(mesh.vertices[i].tangent.w, -original[i].tangent.w);
	}
	CHECK((mesh.vertices[0].bitangent() == originalBitangent));

	mesh.invertNormals();
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK((mesh.vertices[i].normal == original[i].normal));
		CHECK((mesh.vertices[i].tangent == original[i].tangent));
	}
}

TEST_CASE("Mesh3D::invert")
{
	Mesh3D mesh{
		{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
		{ TriangleIndex32{ 0, 1, 2 } }
	};
	const Array<Vertex3D> originalVertices = mesh.vertices;

	CHECK(&mesh.invert() == &mesh);
	CheckTriangle(mesh.indices[0], 0, 2, 1);
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK((mesh.vertices[i].normal == -originalVertices[i].normal));
		CHECK_EQ(mesh.vertices[i].tangent.w, -originalVertices[i].tangent.w);
	}

	mesh.invert();
	CheckTriangle(mesh.indices[0], 0, 1, 2);
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK((mesh.vertices[i].normal == originalVertices[i].normal));
		CHECK((mesh.vertices[i].tangent == originalVertices[i].tangent));
	}
}
