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
	constexpr float Epsilon = 1e-5f;

	enum class TangentComparison
	{
		Exact,
		Orientation,
	};

	[[nodiscard]]
	static Mesh3D MakeWeightingMesh()
	{
		return Mesh3D{
			{
				Vertex3D{ .pos = Float3{ 0.0f, 0.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 10.0f, 0.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 0.0f, 10.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 0.0f, 1.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 0.0f, 1.0f, 1.0f } },
			},
			{
				TriangleIndex32{ 0, 1, 2 },
				TriangleIndex32{ 0, 3, 4 },
			}
		};
	}

	static void CheckComputedTangents(
		const char* label,
		const Mesh3D& source,
		const TangentComparison comparison = TangentComparison::Exact)
	{
		Mesh3D mesh = source;
		for (Vertex3D& vertex : mesh.vertices)
		{
			vertex.tangent = Float4{ 0.0f, 0.0f, 0.0f, 0.0f };
		}

		CHECK_EQ(&mesh.computeTangents(), &mesh);
		REQUIRE(mesh.validate());
		CHECK_EQ(mesh.triangleCount(), source.triangleCount());
		REQUIRE(mesh.vertexCount() >= source.vertexCount());

		for (size_t i = 0; i < source.vertexCount(); ++i)
		{
			CHECK_EQ(mesh.vertices[i].pos, source.vertices[i].pos);
			CHECK_EQ(mesh.vertices[i].tex, source.vertices[i].tex);
			CHECK_EQ(mesh.vertices[i].normal, source.vertices[i].normal);
		}

		for (size_t i = 0; i < mesh.vertexCount(); ++i)
		{
			const Vertex3D& actual = mesh.vertices[i];
			Mesh3DTest::CheckVertexFrame(actual);

			bool foundMatchingAttributes = false;
			Float4 expectedTangent = Float4::Zero();
			for (const Vertex3D& expected : source.vertices)
			{
				if ((actual.pos == expected.pos)
					&& (actual.tex == expected.tex)
					&& (actual.normal == expected.normal))
				{
					foundMatchingAttributes = true;
					expectedTangent = expected.tangent;

					break;
				}
			}

			INFO("mesh = " << std::string_view{ label } << ", vertex = " << i
				<< ", tangent = (" << actual.tangent.x << ", " << actual.tangent.y
				<< ", " << actual.tangent.z << ", " << actual.tangent.w << ")");
			INFO("expected tangent = (" << expectedTangent.x << ", "
				<< expectedTangent.y << ", " << expectedTangent.z
				<< ", " << expectedTangent.w << ")");
			REQUIRE(foundMatchingAttributes);

			if (comparison == TangentComparison::Exact)
			{
				CHECK(actual.tangent.epsilonEquals(expectedTangent, Epsilon));
			}
			else
			{
				const Float3 expectedBitangent = (
					actual.normal.cross(expectedTangent.xyz()) * expectedTangent.w);
				CHECK(actual.tangent.w == doctest::Approx(expectedTangent.w).epsilon(Epsilon));
				CHECK(actual.tangent.xyz().dot(expectedTangent.xyz()) > 0.0f);
				CHECK(actual.bitangent().dot(expectedBitangent) > 0.0f);
			}
		}
	}
}

TEST_CASE("Mesh3D::computeNormals winding")
{
	Mesh3D mesh = Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 });
	for (Vertex3D& vertex : mesh.vertices)
	{
		vertex.normal = Float3{ 0.25f, 0.5f, 0.75f };
	}

	CHECK_EQ(&mesh.computeNormals(), &mesh);

	const std::array<Float3, 6> expectedNormals{
		-Float3::UnitZ(),
		Float3::UnitZ(),
		Float3::UnitX(),
		-Float3::UnitX(),
		Float3::UnitY(),
		-Float3::UnitY(),
	};

	for (size_t faceIndex = 0; faceIndex < expectedNormals.size(); ++faceIndex)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			CHECK(mesh.vertices[(faceIndex * 4) + i].normal.epsilonEquals(
				expectedNormals[faceIndex], Epsilon));
		}
	}

	for (const TriangleIndex32& triangle : mesh.indices)
	{
		const Float3 faceNormal = (
			mesh.vertices[triangle.i1].pos - mesh.vertices[triangle.i0].pos).cross(
			mesh.vertices[triangle.i2].pos - mesh.vertices[triangle.i0].pos).normalized();
		CHECK(faceNormal.epsilonEquals(mesh.vertices[triangle.i0].normal, Epsilon));
		CHECK(faceNormal.epsilonEquals(mesh.vertices[triangle.i1].normal, Epsilon));
		CHECK(faceNormal.epsilonEquals(mesh.vertices[triangle.i2].normal, Epsilon));
	}
}

TEST_CASE("Mesh3D::computeNormals shared vertices")
{
	Mesh3D mesh = Mesh3D::Grid(SizeF{ 4.0, 4.0 }, 2, 2);
	for (Vertex3D& vertex : mesh.vertices)
	{
		vertex.normal = -Float3::UnitY();
	}

	mesh.computeNormals();

	REQUIRE_EQ(mesh.vertexCount(), size_t{ 9 });
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK(vertex.normal.epsilonEquals(Float3::UnitY(), Epsilon));
	}
	CHECK(mesh.vertices[4].normal.epsilonEquals(Float3::UnitY(), Epsilon));
}

TEST_CASE("Mesh3D::computeNormals weighting")
{
	Mesh3D angle = MakeWeightingMesh();
	Mesh3D area = angle;
	Mesh3D uniform = angle;

	angle.computeNormals(VertexNormalWeighting::Angle);
	area.computeNormals(VertexNormalWeighting::Area);
	uniform.computeNormals(VertexNormalWeighting::Uniform);

	const Float3 angleNormal = angle.vertices[0].normal;
	const Float3 areaNormal = area.vertices[0].normal;
	const Float3 uniformNormal = uniform.vertices[0].normal;

	CHECK(angleNormal.length() == doctest::Approx(1.0f).epsilon(Epsilon));
	CHECK(areaNormal.length() == doctest::Approx(1.0f).epsilon(Epsilon));
	CHECK(uniformNormal.length() == doctest::Approx(1.0f).epsilon(Epsilon));
	CHECK_FALSE(angleNormal.epsilonEquals(areaNormal, Epsilon));
	CHECK_FALSE(angleNormal.epsilonEquals(uniformNormal, Epsilon));
	CHECK_FALSE(areaNormal.epsilonEquals(uniformNormal, Epsilon));
	CHECK(areaNormal.x < angleNormal.x);
	CHECK(angleNormal.x < uniformNormal.x);
	CHECK(uniformNormal.z < angleNormal.z);
	CHECK(angleNormal.z < areaNormal.z);
}

TEST_CASE("Mesh3D::computeNormals boundaries and failure")
{
	SUBCASE("Empty mesh")
	{
		Mesh3D mesh;
		CHECK_EQ(&mesh.computeNormals(), &mesh);
		CHECK(mesh.isEmpty());
	}

	SUBCASE("Vertices without triangles")
	{
		Mesh3D mesh = Mesh3D::Plane(SizeF{ 2.0, 2.0 });
		mesh.indices.clear();
		const Array<Vertex3D> expectedVertices = mesh.vertices;

		CHECK_EQ(&mesh.computeNormals(), &mesh);
		REQUIRE_EQ(mesh.vertices.size(), expectedVertices.size());
		for (size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			CHECK_EQ(mesh.vertices[i].pos, expectedVertices[i].pos);
			CHECK_EQ(mesh.vertices[i].normal, expectedVertices[i].normal);
			CHECK_EQ(mesh.vertices[i].tex, expectedVertices[i].tex);
			CHECK_EQ(mesh.vertices[i].tangent, expectedVertices[i].tangent);
		}
	}

	SUBCASE("Degenerate triangle mixed with a valid triangle")
	{
		Mesh3D mesh{
			{
				Vertex3D{ .pos = Float3{ 0.0f, 0.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 1.0f, 0.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 0.0f, 1.0f, 0.0f } },
				Vertex3D{ .pos = Float3{ 2.0f, 0.0f, 0.0f } },
			},
			{
				TriangleIndex32{ 0, 1, 2 },
				TriangleIndex32{ 0, 1, 3 },
			}
		};

		CHECK_NOTHROW(mesh.computeNormals());
		CHECK(mesh.vertices[0].normal.epsilonEquals(Float3::UnitZ(), Epsilon));
		CHECK(mesh.vertices[1].normal.epsilonEquals(Float3::UnitZ(), Epsilon));
		CHECK(mesh.vertices[2].normal.epsilonEquals(Float3::UnitZ(), Epsilon));
		CHECK_EQ(mesh.vertices[3].normal, Float3::Zero());
	}

	SUBCASE("Invalid index")
	{
		Mesh3D mesh{
			{ Vertex3D{ .pos = Float3::Zero() } },
			{ TriangleIndex32{ 0, 1, 0 } }
		};
		CHECK_THROWS_AS(mesh.computeNormals(), Error);
	}
}

TEST_CASE("Mesh3D::computeTangents generated mesh conventions")
{
	SUBCASE("Box")
	{
		CheckComputedTangents("Box", Mesh3D::Box());
	}

	SUBCASE("Grid")
	{
		CheckComputedTangents("Grid", Mesh3D::Grid(SizeF{ 2.0, 2.0 }, 2, 2));
	}

	SUBCASE("UVSphere")
	{
		CheckComputedTangents(
			"UVSphere",
			Mesh3D::UVSphere(1.0, 16, 8),
			TangentComparison::Orientation);
	}

	SUBCASE("Revolve")
	{
		const Array<Vec2> profile{ { 1.0, -1.0 }, { 1.0, 1.0 } };
		CheckComputedTangents("Revolve", Mesh3D::Revolve(profile, 16));
	}
}

TEST_CASE("Mesh3D::computeTangents boundaries and failure")
{
	SUBCASE("Empty mesh")
	{
		Mesh3D mesh;
		CHECK_EQ(&mesh.computeTangents(), &mesh);
		CHECK(mesh.isEmpty());
	}

	SUBCASE("Vertices without triangles")
	{
		Mesh3D mesh = Mesh3D::Plane(SizeF{ 2.0, 2.0 });
		mesh.indices.clear();
		const Array<Vertex3D> expectedVertices = mesh.vertices;

		CHECK_EQ(&mesh.computeTangents(), &mesh);
		REQUIRE_EQ(mesh.vertices.size(), expectedVertices.size());
		for (size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			CHECK_EQ(mesh.vertices[i].pos, expectedVertices[i].pos);
			CHECK_EQ(mesh.vertices[i].normal, expectedVertices[i].normal);
			CHECK_EQ(mesh.vertices[i].tex, expectedVertices[i].tex);
			CHECK_EQ(mesh.vertices[i].tangent, expectedVertices[i].tangent);
		}
	}

	SUBCASE("Degenerate UV basis")
	{
		Mesh3D mesh = Mesh3D::Grid(SizeF{ 2.0, 2.0 }, 2, 2);
		for (Vertex3D& vertex : mesh.vertices)
		{
			vertex.tex = Float2{ 0.5f, 0.5f };
		}

		CHECK_NOTHROW(mesh.computeTangents());
		for (const Vertex3D& vertex : mesh.vertices)
		{
			Mesh3DTest::CheckVertexFrame(vertex);
		}
	}

	SUBCASE("Invalid normal")
	{
		Mesh3D mesh = Mesh3D::Plane(SizeF{ 2.0, 2.0 });
		mesh.vertices[0].normal = Float3::Zero();
		CHECK_THROWS_AS(mesh.computeTangents(), Error);
	}
}
