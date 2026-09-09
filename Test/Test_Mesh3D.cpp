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
	constexpr float Mesh3DEpsilon = 1e-5f;

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

	[[nodiscard]]
	static Vertex3D MakeFrameVertex()
	{
		return Vertex3D{
			.pos = Float3{ 1.0f, 2.0f, 3.0f },
			.normal = Float3::UnitY(),
			.tex = Float2{ 0.25f, 0.75f },
			.tangent = Float4{ 1.0f, 0.0f, 0.0f, -1.0f }
		};
	}

	[[nodiscard]]
	static Mesh3D MakeFrameMesh()
	{
		return Mesh3D{
			{ MakeFrameVertex() },
			{ TriangleIndex32{ 0, 0, 0 } }
		};
	}

	static void CheckFloat3(const Float3 actual, const Float3 expected)
	{
		CHECK(actual.epsilonEquals(expected, Mesh3DEpsilon));
	}

	static void CheckFloat4(const Float4 actual, const Float4 expected)
	{
		CHECK(actual.epsilonEquals(expected, Mesh3DEpsilon));
	}

	static void CheckMesh(const Mesh3D& actual, const Mesh3D& expected)
	{
		REQUIRE((actual.vertices.size()) == (expected.vertices.size()));
		REQUIRE((actual.indices.size()) == (expected.indices.size()));

		for (size_t i = 0; i < actual.vertices.size(); ++i)
		{
			CheckFloat3(actual.vertices[i].pos, expected.vertices[i].pos);
			CheckFloat3(actual.vertices[i].normal, expected.vertices[i].normal);
			CHECK((actual.vertices[i].tex) == (expected.vertices[i].tex));
			CheckFloat4(actual.vertices[i].tangent, expected.vertices[i].tangent);
		}

		for (size_t i = 0; i < actual.indices.size(); ++i)
		{
			CHECK((actual.indices[i].i0) == (expected.indices[i].i0));
			CHECK((actual.indices[i].i1) == (expected.indices[i].i1));
			CHECK((actual.indices[i].i2) == (expected.indices[i].i2));
		}
	}

	static void CheckTriangle(const TriangleIndex32& triangle, const uint32 i0, const uint32 i1, const uint32 i2)
	{
		CHECK((triangle.i0) == (i0));
		CHECK((triangle.i1) == (i1));
		CHECK((triangle.i2) == (i2));
	}
}

TEST_CASE("Mesh3D constructors")
{
	SECTION("Counts")
	{
		const Mesh3D mesh{ 3, 2 };
		CHECK((mesh.vertexCount()) == (size_t{ 3 }));
		CHECK((mesh.triangleCount()) == (size_t{ 2 }));
	}

	SECTION("Arrays")
	{
		const Array<Vertex3D> vertices{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) };
		const Array<TriangleIndex32> indices{ TriangleIndex32{ 0, 1, 2 } };
		const Mesh3D mesh{ vertices, indices };

		REQUIRE((mesh.vertexCount()) == (vertices.size()));
		REQUIRE((mesh.triangleCount()) == (indices.size()));
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			CHECK((mesh.vertices[i].pos) == (vertices[i].pos));
			CHECK((mesh.vertices[i].normal) == (vertices[i].normal));
			CHECK((mesh.vertices[i].tex) == (vertices[i].tex));
			CHECK((mesh.vertices[i].tangent) == (vertices[i].tangent));
		}
		CheckTriangle(mesh.indices[0], 0, 1, 2);
	}

	SECTION("Spans")
	{
		const std::array vertices{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) };
		const std::array indices{ TriangleIndex32{ 0, 1, 2 } };
		const Mesh3D mesh{ std::span<const Vertex3D>{ vertices }, std::span<const TriangleIndex32>{ indices } };

		CHECK((mesh.vertexCount()) == (vertices.size()));
		CHECK((mesh.triangleCount()) == (indices.size()));
		CHECK((mesh.vertices[2].pos) == (vertices[2].pos));
		CHECK((mesh.vertices[2].normal) == (vertices[2].normal));
		CHECK((mesh.vertices[2].tex) == (vertices[2].tex));
		CHECK((mesh.vertices[2].tangent) == (vertices[2].tangent));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
	}

	SECTION("Too many vertices")
	{
		if constexpr (Mesh3D::MaxVertexCount < std::numeric_limits<size_t>::max())
		{
			const Mesh3D mesh{ (Mesh3D::MaxVertexCount + 1), 1 };
			CHECK(mesh.vertices.isEmpty());
			CHECK(mesh.indices.isEmpty());
		}
	}
}

TEST_CASE("Mesh3D::reserve and clear")
{
	Mesh3D mesh{
		{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
		{ TriangleIndex32{ 0, 1, 2 } }
	};

	mesh.reserve(16, 8);
	CHECK(mesh.vertices.capacity() >= 16);
	CHECK(mesh.indices.capacity() >= 8);
	CHECK((mesh.vertexCount()) == (size_t{ 3 }));
	CHECK((mesh.triangleCount()) == (size_t{ 1 }));

	const size_t vertexCapacity = mesh.vertices.capacity();
	const size_t triangleCapacity = mesh.indices.capacity();
	mesh.clear();
	CHECK(mesh.vertices.isEmpty());
	CHECK(mesh.indices.isEmpty());
	CHECK((mesh.vertices.capacity()) == (vertexCapacity));
	CHECK((mesh.indices.capacity()) == (triangleCapacity));

	if constexpr (Mesh3D::MaxVertexCount < std::numeric_limits<size_t>::max())
	{
		mesh.reserve((Mesh3D::MaxVertexCount + 1), 32);
		CHECK((mesh.vertices.capacity()) == (vertexCapacity));
		CHECK((mesh.indices.capacity()) == (triangleCapacity));
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
	SECTION("Another mesh")
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
		CHECK((mesh.vertexCount()) == (size_t{ 7 }));
		CHECK((mesh.triangleCount()) == (size_t{ 3 }));
		CHECK((mesh.vertices[3].pos == source.vertices[0].pos));
		CHECK((mesh.vertices[6].tangent == source.vertices[3].tangent));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
		CheckTriangle(mesh.indices[1], 3, 5, 6);
		CheckTriangle(mesh.indices[2], 3, 4, 5);
	}

	SECTION("Self append")
	{
		Mesh3D mesh{
			{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
			{ TriangleIndex32{ 0, 1, 2 } }
		};

		CHECK(mesh.append(mesh));
		CHECK((mesh.vertexCount()) == (size_t{ 6 }));
		CHECK((mesh.triangleCount()) == (size_t{ 2 }));
		CHECK((mesh.vertices[3].pos == mesh.vertices[0].pos));
		CHECK((mesh.vertices[5].tangent == mesh.vertices[2].tangent));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
		CheckTriangle(mesh.indices[1], 3, 4, 5);
	}

	SECTION("Invalid source leaves the destination unchanged")
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
		CHECK((mesh.vertexCount()) == (size_t{ 3 }));
		CHECK((mesh.triangleCount()) == (size_t{ 1 }));
		CHECK((mesh.vertices[2].pos == MakeVertex(2.0f).pos));
		CheckTriangle(mesh.indices[0], 0, 1, 2);
	}
}

TEST_CASE("Mesh3D::append with transform")
{
	const Mat4x4 matrix{
		1, 1, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		4, 5, 6, 1
	};

	SECTION("Another mesh")
	{
		const Mesh3D source = MakeFrameMesh();
		const Mesh3D destination{
			{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
			{ TriangleIndex32{ 0, 1, 2 } }
		};
		Mesh3D expected = destination;
		REQUIRE(expected.append(source.transformed(matrix)));

		Mesh3D actual = destination;
		CHECK(actual.append(source, matrix));
		CheckMesh(actual, expected);
		CheckMesh(source, MakeFrameMesh());
	}

	SECTION("Self append with negative determinant")
	{
		const Mat4x4 negativeScale = Mat4x4::Scale(Float3{ -2.0f, 3.0f, 4.0f });
		const Mesh3D original = MakeFrameMesh();
		Mesh3D expected = original;
		REQUIRE(expected.append(original.transformed(negativeScale)));

		Mesh3D actual = original;
		CHECK(actual.append(actual, negativeScale));
		CheckMesh(actual, expected);
		CHECK((actual.vertices[1].tangent.w) == (-original.vertices[0].tangent.w));
	}

	SECTION("Singular transform")
	{
		const Mat4x4 singular = Mat4x4::Scale(Float3{ 2.0f, 0.0f, 4.0f });
		const Mesh3D source = MakeFrameMesh();
		Mesh3D expected;
		REQUIRE(expected.append(source.transformed(singular)));

		Mesh3D actual;
		CHECK(actual.append(source, singular));
		CheckMesh(actual, expected);
		CHECK((actual.vertices[0].normal) == (source.vertices[0].normal));
		CHECK((actual.vertices[0].tangent) == (source.vertices[0].tangent));
	}

	SECTION("Invalid source leaves the destination unchanged")
	{
		const Mesh3D source{
			{ MakeVertex(3.0f), MakeVertex(4.0f), MakeVertex(5.0f) },
			{ TriangleIndex32{ 0, 1, 3 } }
		};
		const Mesh3D original = MakeFrameMesh();
		Mesh3D actual = original;

		CHECK_FALSE(actual.append(source, matrix));
		CheckMesh(actual, original);
	}

	SECTION("Empty source")
	{
		const Mesh3D original = MakeFrameMesh();
		Mesh3D actual = original;

		CHECK(actual.append(Mesh3D{}, matrix));
		CheckMesh(actual, original);
	}
}

TEST_CASE("Mesh3D::transformUV")
{
	const Mesh3D source{
		{ MakeVertex(0.0f), MakeVertex(1.0f), MakeVertex(2.0f) },
		{ TriangleIndex32{ 0, 1, 2 } }
	};
	Mesh3D mesh = source;
	const Mat3x2 transform{ 2.0f, 0.0f, 0.0f, 3.0f, 4.0f, -5.0f };

	CHECK((&mesh.transformUV(transform)) == (&mesh));
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK((mesh.vertices[i].pos) == (source.vertices[i].pos));
		CHECK((mesh.vertices[i].normal) == (source.vertices[i].normal));
		CHECK((mesh.vertices[i].tex) == (transform.transformPoint(source.vertices[i].tex)));
		CHECK((mesh.vertices[i].tangent) == (source.vertices[i].tangent));
	}
	REQUIRE((mesh.indices.size()) == (source.indices.size()));
	CheckTriangle(mesh.indices[0], source.indices[0].i0, source.indices[0].i1, source.indices[0].i2);

	Mesh3D empty;
	CHECK((&empty.transformUV(transform)) == (&empty));
	CHECK(empty.vertices.isEmpty());
	CHECK(empty.indices.isEmpty());
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
		CHECK((mesh.vertices[i].tangent.x) == (original[i].tangent.x));
		CHECK((mesh.vertices[i].tangent.y) == (original[i].tangent.y));
		CHECK((mesh.vertices[i].tangent.z) == (original[i].tangent.z));
		CHECK((mesh.vertices[i].tangent.w) == (-original[i].tangent.w));
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
		CHECK((mesh.vertices[i].tangent.w) == (-originalVertices[i].tangent.w));
	}

	mesh.invert();
	CheckTriangle(mesh.indices[0], 0, 1, 2);
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK((mesh.vertices[i].normal == originalVertices[i].normal));
		CHECK((mesh.vertices[i].tangent == originalVertices[i].tangent));
	}
}

TEST_CASE("Mesh3D::transform")
{
	const Mesh3D source = MakeFrameMesh();

	SECTION("Non-singular affine transform")
	{
		const Mat4x4 matrix{
			1, 1, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			4, 5, 6, 1
		};
		Mesh3D mesh = source;

		CHECK((&mesh.transform(matrix)) == (&mesh));
		CheckFloat3(mesh.vertices[0].pos, Float3{ 5.0f, 8.0f, 9.0f });
		CheckFloat3(mesh.vertices[0].normal, Float3{ -1.0f, 1.0f, 0.0f }.normalized());
		CheckFloat3(mesh.vertices[0].tangent.xyz(), Float3{ 1.0f, 1.0f, 0.0f }.normalized());
		CHECK((mesh.vertices[0].tangent.w) == (-1.0f));
		CHECK((mesh.vertices[0].tex) == (source.vertices[0].tex));
		CheckTriangle(mesh.indices[0], 0, 0, 0);

		const Mesh3D copiedResult = source.transformed(matrix);
		const Mesh3D movedResult = Mesh3D{ source }.transformed(matrix);
		CheckMesh(copiedResult, mesh);
		CheckMesh(movedResult, mesh);
		CheckMesh(source, MakeFrameMesh());
	}

	SECTION("Negative determinant")
	{
		Mesh3D mesh = source;
		mesh.transform(Mat4x4::Scale(Float3{ -2.0f, 3.0f, 4.0f }));

		CheckFloat3(mesh.vertices[0].pos, Float3{ -2.0f, 6.0f, 12.0f });
		CheckFloat3(mesh.vertices[0].normal, Float3::UnitY());
		CheckFloat3(mesh.vertices[0].tangent.xyz(), -Float3::UnitX());
		CHECK((mesh.vertices[0].tangent.w) == (1.0f));
		CheckTriangle(mesh.indices[0], 0, 0, 0);
	}

	SECTION("Singular linear transform preserves the tangent frame")
	{
		Mesh3D mesh = source;
		mesh.transform(Mat4x4::Scale(Float3{ 2.0f, 0.0f, 4.0f }));

		CheckFloat3(mesh.vertices[0].pos, Float3{ 2.0f, 0.0f, 12.0f });
		CHECK((mesh.vertices[0].normal) == (source.vertices[0].normal));
		CHECK((mesh.vertices[0].tangent) == (source.vertices[0].tangent));
		CHECK((mesh.vertices[0].tex) == (source.vertices[0].tex));
	}

	SECTION("Empty mesh")
	{
		Mesh3D mesh;
		CHECK((&mesh.transform(Mat4x4::Identity())) == (&mesh));
		CHECK(mesh.vertices.isEmpty());
		CHECK(mesh.indices.isEmpty());
	}
}

TEST_CASE("Mesh3D::translate")
{
	const Mesh3D source = MakeFrameMesh();
	const Float3 offset{ 4.0f, -5.0f, 6.0f };
	Mesh3D expected = source;
	CHECK((&expected.translate(offset)) == (&expected));
	CheckFloat3(expected.vertices[0].pos, Float3{ 5.0f, -3.0f, 9.0f });
	CHECK((expected.vertices[0].normal) == (source.vertices[0].normal));
	CHECK((expected.vertices[0].tangent) == (source.vertices[0].tangent));
	CHECK((expected.vertices[0].tex) == (source.vertices[0].tex));

	const Mesh3D copiedResult = source.translated(offset);
	CheckMesh(copiedResult, expected);
	CheckMesh(source, MakeFrameMesh());

	Mesh3D reusable = source;
	const Vertex3D* const vertexData = reusable.vertices.data();
	const TriangleIndex32* const indexData = reusable.indices.data();
	const Mesh3D movedResult = std::move(reusable).translated(offset);
	CHECK((movedResult.vertices.data()) == (vertexData));
	CHECK((movedResult.indices.data()) == (indexData));
	CheckMesh(movedResult, expected);
}

TEST_CASE("Mesh3D::rotate")
{
	const Mesh3D source = MakeFrameMesh();
	const Quaternion rotation = Quaternion::RotateZ(Math::HalfPiF);
	Mesh3D expected = source;

	CHECK((&expected.rotate(rotation)) == (&expected));
	CheckFloat3(expected.vertices[0].pos, Float3{ -2.0f, 1.0f, 3.0f });
	CheckFloat3(expected.vertices[0].normal, -Float3::UnitX());
	CheckFloat3(expected.vertices[0].tangent.xyz(), Float3::UnitY());
	CHECK((expected.vertices[0].tangent.w) == (-1.0f));
	CHECK((expected.vertices[0].tex) == (source.vertices[0].tex));

	const Mesh3D copiedResult = source.rotated(rotation);
	const Mesh3D movedResult = Mesh3D{ source }.rotated(rotation);
	CheckMesh(copiedResult, expected);
	CheckMesh(movedResult, expected);
	CheckMesh(source, MakeFrameMesh());
}

TEST_CASE("Mesh3D::scale_uniform")
{
	const Mesh3D source = MakeFrameMesh();

	SECTION("Positive")
	{
		Mesh3D expected = source;
		CHECK((&expected.scale(2.0f)) == (&expected));
		CheckFloat3(expected.vertices[0].pos, Float3{ 2.0f, 4.0f, 6.0f });
		CHECK((expected.vertices[0].normal) == (source.vertices[0].normal));
		CHECK((expected.vertices[0].tangent) == (source.vertices[0].tangent));

		CheckMesh(source.scaled(2.0f), expected);
		CheckMesh(Mesh3D{ source }.scaled(2.0f), expected);
	}

	SECTION("Negative")
	{
		Mesh3D mesh = source;
		mesh.scale(-2.0f);
		CheckFloat3(mesh.vertices[0].pos, Float3{ -2.0f, -4.0f, -6.0f });
		CHECK((mesh.vertices[0].normal) == (-source.vertices[0].normal));
		CHECK((mesh.vertices[0].tangent) == (-source.vertices[0].tangent));
		CheckTriangle(mesh.indices[0], 0, 0, 0);
	}

	SECTION("Zero")
	{
		Mesh3D mesh = source;
		mesh.scale(0.0f);
		CHECK((mesh.vertices[0].pos) == (Float3::Zero()));
		CHECK((mesh.vertices[0].normal) == (source.vertices[0].normal));
		CHECK((mesh.vertices[0].tangent) == (source.vertices[0].tangent));
	}
}

TEST_CASE("Mesh3D::scale_non_uniform")
{
	Mesh3D source = MakeFrameMesh();
	source.vertices[0].normal = Float3{ 1.0f, 1.0f, 0.0f }.normalized();
	source.vertices[0].tangent = Float4{ Float3{ 1.0f, -1.0f, 0.0f }.normalized(), 1.0f };

	SECTION("Non-singular")
	{
		const Float3 scale{ 2.0f, 3.0f, 4.0f };
		Mesh3D expected = source;
		CHECK((&expected.scale(scale)) == (&expected));
		CheckFloat3(expected.vertices[0].pos, Float3{ 2.0f, 6.0f, 12.0f });
		CheckFloat3(expected.vertices[0].normal,
			Float3{ (source.vertices[0].normal.x / scale.x), (source.vertices[0].normal.y / scale.y), 0.0f }.normalized());
		CheckFloat3(expected.vertices[0].tangent.xyz(),
			Float3{ (source.vertices[0].tangent.x * scale.x), (source.vertices[0].tangent.y * scale.y), 0.0f }.normalized());
		CHECK((expected.vertices[0].tangent.w) == (1.0f));
		CHECK(std::abs(expected.vertices[0].normal.dot(expected.vertices[0].tangent.xyz())) < Mesh3DEpsilon);

		CheckMesh(source.scaled(scale), expected);
		CheckMesh(Mesh3D{ source }.scaled(scale), expected);
	}

	SECTION("Negative determinant")
	{
		Mesh3D mesh = source;
		mesh.scale(Float3{ -2.0f, 3.0f, 4.0f });
		CHECK((mesh.vertices[0].tangent.w) == (-1.0f));
		CheckTriangle(mesh.indices[0], 0, 0, 0);
	}

	SECTION("Zero component preserves the tangent frame")
	{
		Mesh3D mesh = source;
		mesh.scale(Float3{ 2.0f, 0.0f, 4.0f });
		CheckFloat3(mesh.vertices[0].pos, Float3{ 2.0f, 0.0f, 12.0f });
		CHECK((mesh.vertices[0].normal) == (source.vertices[0].normal));
		CHECK((mesh.vertices[0].tangent) == (source.vertices[0].tangent));
	}
}

TEST_CASE("Mesh3D emptiness and index validation have distinct scopes")
{
	const Mesh3DRange empty{ 7, 0, 11, 0 };
	CHECK(empty.isEmpty());
	CHECK_FALSE((Mesh3DRange{ 7, 1, 11, 0 }.isEmpty()));
	CHECK_FALSE((Mesh3DRange{ 7, 0, 11, 1 }.isEmpty()));
	Mesh3D verticesOnly{ Array<Vertex3D>(1), {} };
	CHECK(verticesOnly.isEmpty());
	CHECK(verticesOnly.validate());
	Mesh3D triangleOnly{ {}, { TriangleIndex32{ 0, 0, 0 } } };
	CHECK(triangleOnly.isEmpty());
	CHECK_FALSE(triangleOnly.validate());
	verticesOnly.indices.push_back({ 0, 0, 0 });
	verticesOnly.vertices[0].pos.x = std::numeric_limits<float>::quiet_NaN();
	CHECK_FALSE(verticesOnly.isEmpty());
	CHECK(verticesOnly.validate()); // Neither non-finite attributes nor degeneracy are index errors.
}
