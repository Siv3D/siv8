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
		static_cast<Mesh3D (*)(Vec3, BoxFace)>(&Mesh3D::Box);
		static_cast<Mesh3D (*)(Vec3, const BoxUVMapping&, BoxFace)>(&Mesh3D::Box);
		static_cast<Mesh3D (*)(Vec3, double, uint32)>(&Mesh3D::RoundedBox);
		static_cast<Mesh3D (*)(Vec3, double, uint32, const BoxUVMapping&)>(&Mesh3D::RoundedBox);
		static_cast<Mesh3D (*)(Vec3, double)>(&Mesh3D::ChamferedBox);
		static_cast<Mesh3D (*)(Vec3, double, const BoxUVMapping&)>(&Mesh3D::ChamferedBox);
		static_cast<Mesh3D (*)(Vec3)>(&Mesh3D::Wedge);
		static_cast<Mesh3D (*)(Vec3, const BoxUVMapping&)>(&Mesh3D::Wedge);
		static_cast<Mesh3D (*)(Vec3)>(&Mesh3D::TriangularPrism);
		static_cast<Mesh3D (*)(Vec3, const BoxUVMapping&)>(&Mesh3D::TriangularPrism);
		static_cast<Mesh3D (*)(Vec3, uint32)>(&Mesh3D::Stairs);
		static_cast<Mesh3D (*)(Vec3, uint32, const BoxUVMapping&)>(&Mesh3D::Stairs);
		static_cast<Mesh3D (*)(double, double)>(&Mesh3D::Pyramid);
		static_cast<Mesh3D (*)(SizeF, double)>(&Mesh3D::Pyramid);
		static_cast<Mesh3D (*)(SizeF, SizeF, double)>(&Mesh3D::RectangularFrustum);
		static_cast<Mesh3D (*)(SizeF, SizeF, double, const BoxUVMapping&)>(&Mesh3D::RectangularFrustum);
		static_cast<Mesh3D (*)(double)>(&Mesh3D::Tetrahedron);
		static_cast<Mesh3D (*)(double)>(&Mesh3D::Octahedron);
		static_cast<Mesh3D (*)(double)>(&Mesh3D::Icosahedron);
		static_cast<Mesh3D (*)(double, uint32)>(&Mesh3D::IcoSphere);
		static_cast<Mesh3D (*)(double)>(&Mesh3D::Dodecahedron);
		static_cast<Mesh3D (*)(SizeF, Vec2, Vec2)>(&Mesh3D::Plane);
		static_cast<Mesh3D (*)(SizeF, uint32, uint32, Vec2, Vec2)>(&Mesh3D::Grid);
		static_cast<Mesh3D (*)(double, double, uint32, uint32)>(&Mesh3D::Torus);
		static_cast<Mesh3D (*)(double, double, uint32, uint32)>(&Mesh3D::Capsule);
		static_cast<Mesh3D (*)(double, uint32, uint32)>(&Mesh3D::Sphere);
		static_cast<Mesh3D (*)(double, uint32, uint32)>(&Mesh3D::Hemisphere);
		static_cast<Mesh3D (*)(double, CloseBottom, uint32, uint32)>(&Mesh3D::Hemisphere);
		static_cast<Mesh3D (*)(double, uint32)>(&Mesh3D::Disc);
		static_cast<Mesh3D (*)(double, double, uint32)>(&Mesh3D::Annulus);
		static_cast<Mesh3D (*)(double, double, double, uint32)>(&Mesh3D::HollowCylinder);
		static_cast<Mesh3D (*)(double, double, double, uint32)>(&Mesh3D::ConicalFrustum);
		static_cast<Mesh3D (*)(double, double, uint32)>(&Mesh3D::Cylinder);
		static_cast<Mesh3D (*)(double, double, uint32)>(&Mesh3D::Cone);
	});

	static Float2 MapUV(const FloatRect& rect, const float u, const float v)
	{
		return{
			(rect.left + ((rect.right - rect.left) * u)),
			(rect.top + ((rect.bottom - rect.top) * v))
		};
	}

	static void CheckUV(const Float2 actual, const Float2 expected)
	{
		CHECK(actual.x == doctest::Approx(expected.x).epsilon(FrameEpsilon));
		CHECK(actual.y == doctest::Approx(expected.y).epsilon(FrameEpsilon));
	}

	static void CheckRegularPolyhedron(
		const Mesh3D& mesh,
		const double radius,
		const size_t faceCount,
		const size_t verticesPerFace)
	{
		REQUIRE_EQ(mesh.vertexCount(), (faceCount * verticesPerFace));
		REQUIRE_EQ(mesh.triangleCount(), (faceCount * (verticesPerFace - 2)));
		CheckMeshGeometry(mesh);
		const float expectedEdgeLength = mesh.vertices[0].pos.distanceFrom(mesh.vertices[1].pos);

		for (size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex)
		{
			const size_t vertexBase = (faceIndex * verticesPerFace);
			Float3 faceCenter = Float3::Zero();

			for (size_t i = 0; i < verticesPerFace; ++i)
			{
				const Vertex3D& vertex = mesh.vertices[vertexBase + i];
				const Vertex3D& nextVertex = mesh.vertices[vertexBase + ((i + 1) % verticesPerFace)];
				CHECK(vertex.pos.length() == doctest::Approx(radius).epsilon(FrameEpsilon));
				CHECK(vertex.pos.distanceFrom(nextVertex.pos) == doctest::Approx(expectedEdgeLength).epsilon(FrameEpsilon));
				CHECK_EQ(vertex.normal, mesh.vertices[vertexBase].normal);
				CHECK_EQ(vertex.tangent, mesh.vertices[vertexBase].tangent);
				CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
				CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
				faceCenter += vertex.pos;
			}

			CHECK(mesh.vertices[vertexBase].normal.dot(faceCenter) > 0.0f);
		}
	}

}

TEST_CASE("Mesh3D::Box")
{
	const Mesh3D mesh = Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 });

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

	CHECK(Mesh3D::Box(Vec3{ 0.0, 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::Box(Vec3{ 1.0, -1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::Box(Vec3{ 1.0, 1.0, std::numeric_limits<double>::infinity() }).isEmpty());
	CHECK(Mesh3D::Box(Vec3{ 1.0, 1.0, std::numeric_limits<double>::max() }).isEmpty());
}

TEST_CASE("Mesh3D::Box face mask")
{
	struct FaceCase
	{
		BoxFace face;
		Float3 normal;
	};

	constexpr std::array<FaceCase, 6> FaceCases
	{{
		{ BoxFace::NegativeX, Float3{ -1.0f, 0.0f, 0.0f } },
		{ BoxFace::PositiveX, Float3{ 1.0f, 0.0f, 0.0f } },
		{ BoxFace::NegativeY, Float3{ 0.0f, -1.0f, 0.0f } },
		{ BoxFace::PositiveY, Float3{ 0.0f, 1.0f, 0.0f } },
		{ BoxFace::NegativeZ, Float3{ 0.0f, 0.0f, -1.0f } },
		{ BoxFace::PositiveZ, Float3{ 0.0f, 0.0f, 1.0f } },
	}};

	for (const auto& [face, normal] : FaceCases)
	{
		const Mesh3D mesh = Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 }, face);
		REQUIRE_EQ(mesh.vertexCount(), size_t{ 4 });
		REQUIRE_EQ(mesh.triangleCount(), size_t{ 2 });
		CheckMeshGeometry(mesh);

		for (const auto& vertex : mesh.vertices)
		{
			CHECK_EQ(vertex.normal, normal);
		}
	}

	SUBCASE("Multiple faces")
	{
		const BoxFace faces = (BoxFace::NegativeX | BoxFace::PositiveY | BoxFace::PositiveZ);
		const Mesh3D mesh = Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 }, faces);
		CHECK_EQ(mesh.vertexCount(), size_t{ 12 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 6 });
		CheckMeshGeometry(mesh);
	}

	SUBCASE("No faces")
	{
		CHECK(Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 }, BoxFace::None_).isEmpty());
		CHECK(Mesh3D::Box(Vec3{ 0.0, 4.0, 6.0 }, BoxFace::None_).isEmpty());
	}

	SUBCASE("Only selected UV rectangles are validated")
	{
		BoxUVMapping uvMapping;
		uvMapping.negativeX.left = std::numeric_limits<float>::quiet_NaN();

		const Mesh3D mesh = Mesh3D::Box(
			Vec3{ 2.0, 4.0, 6.0 }, uvMapping, BoxFace::PositiveX);
		CHECK_EQ(mesh.vertexCount(), size_t{ 4 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 2 });
		CHECK(Mesh3D::Box(
			Vec3{ 2.0, 4.0, 6.0 }, uvMapping, BoxFace::NegativeX).isEmpty());
		CHECK(Mesh3D::Box(
			Vec3{ 2.0, 4.0, 6.0 }, uvMapping, BoxFace::None_).isEmpty());
	}
}

TEST_CASE("Mesh3D::Box with UV mapping")
{
	const BoxUVMapping uvMapping{
		.negativeZ = FloatRect{ -1.0f, -2.0f, 0.0f, -1.0f },
		.positiveZ = FloatRect{ 0.0f, 0.0f, 0.25f, 0.5f },
		.positiveX = FloatRect{ 0.25f, 0.0f, 0.5f, 0.5f },
		.negativeX = FloatRect{ 0.5f, 0.0f, 0.75f, 0.5f },
		.positiveY = FloatRect{ 0.75f, 0.0f, 1.0f, 0.5f },
		.negativeY = FloatRect{ 1.0f, 0.0f, 1.25f, 0.5f },
	};
	const std::array<FloatRect, 6> uvRects{
		uvMapping.negativeZ,
		uvMapping.positiveZ,
		uvMapping.positiveX,
		uvMapping.negativeX,
		uvMapping.positiveY,
		uvMapping.negativeY,
	};
	const Mesh3D mesh = Mesh3D::Box(Vec3{ 2.0, 4.0, 6.0 }, uvMapping);

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);

	for (size_t faceIndex = 0; faceIndex < uvRects.size(); ++faceIndex)
	{
		const FloatRect rect = uvRects[faceIndex];
		const size_t vertexOffset = (faceIndex * 4);
		CHECK_EQ(mesh.vertices[vertexOffset + 0].tex, Float2{ rect.left, rect.top });
		CHECK_EQ(mesh.vertices[vertexOffset + 1].tex, Float2{ rect.right, rect.top });
		CHECK_EQ(mesh.vertices[vertexOffset + 2].tex, Float2{ rect.left, rect.bottom });
		CHECK_EQ(mesh.vertices[vertexOffset + 3].tex, Float2{ rect.right, rect.bottom });
	}

	SUBCASE("Invalid UV rectangles")
	{
		BoxUVMapping invalid = uvMapping;
		invalid.negativeX.left = std::numeric_limits<float>::quiet_NaN();
		CHECK(Mesh3D::Box(Vec3{ 1.0, 1.0, 1.0 }, invalid).isEmpty());

		invalid = uvMapping;
		invalid.positiveY.bottom = std::numeric_limits<float>::infinity();
		CHECK(Mesh3D::Box(Vec3{ 1.0, 1.0, 1.0 }, invalid).isEmpty());
	}

	CHECK(Mesh3D::Box(Vec3{ 0.0, 1.0, 1.0 }, uvMapping).isEmpty());
}

TEST_CASE("Mesh3D::Box with flipped UV mapping")
{
	BoxUVMapping uvMapping;
	uvMapping.negativeZ = FloatRect{ 1.0f, 0.0f, 0.0f, 1.0f };
	uvMapping.positiveZ = FloatRect{ 0.0f, 1.0f, 1.0f, 0.0f };
	uvMapping.positiveX = FloatRect{ 1.0f, 1.0f, 0.0f, 0.0f };

	const Mesh3D defaultMesh = Mesh3D::Box();
	const Mesh3D flippedMesh = Mesh3D::Box(Vec3{ 1.0, 1.0, 1.0 }, uvMapping);
	CheckMeshGeometry(flippedMesh);

	SUBCASE("U flip")
	{
		constexpr size_t VertexOffset = 0;
		CHECK_EQ(flippedMesh.vertices[VertexOffset + 0].tex, Float2{ 1.0f, 0.0f });
		CHECK_EQ(flippedMesh.vertices[VertexOffset + 3].tex, Float2{ 0.0f, 1.0f });

		for (size_t i = 0; i < 4; ++i)
		{
			CHECK_EQ(flippedMesh.vertices[VertexOffset + i].tangent.xyz(), -defaultMesh.vertices[VertexOffset + i].tangent.xyz());
			CHECK_EQ(flippedMesh.vertices[VertexOffset + i].tangent.w, -defaultMesh.vertices[VertexOffset + i].tangent.w);
		}
	}

	SUBCASE("V flip")
	{
		constexpr size_t VertexOffset = 4;
		CHECK_EQ(flippedMesh.vertices[VertexOffset + 0].tex, Float2{ 0.0f, 1.0f });
		CHECK_EQ(flippedMesh.vertices[VertexOffset + 3].tex, Float2{ 1.0f, 0.0f });

		for (size_t i = 0; i < 4; ++i)
		{
			CHECK_EQ(flippedMesh.vertices[VertexOffset + i].tangent.xyz(), defaultMesh.vertices[VertexOffset + i].tangent.xyz());
			CHECK_EQ(flippedMesh.vertices[VertexOffset + i].tangent.w, -defaultMesh.vertices[VertexOffset + i].tangent.w);
		}
	}

	SUBCASE("U and V flip")
	{
		constexpr size_t VertexOffset = 8;
		CHECK_EQ(flippedMesh.vertices[VertexOffset + 0].tex, Float2{ 1.0f, 1.0f });
		CHECK_EQ(flippedMesh.vertices[VertexOffset + 3].tex, Float2{ 0.0f, 0.0f });

		for (size_t i = 0; i < 4; ++i)
		{
			CHECK_EQ(flippedMesh.vertices[VertexOffset + i].tangent.xyz(), -defaultMesh.vertices[VertexOffset + i].tangent.xyz());
			CHECK_EQ(flippedMesh.vertices[VertexOffset + i].tangent.w, defaultMesh.vertices[VertexOffset + i].tangent.w);
		}
	}
}

TEST_CASE("Mesh3D::Box with collapsed UV mapping")
{
	BoxUVMapping uvMapping;
	uvMapping.negativeZ = FloatRect{ 0.25f, 0.0f, 0.25f, 1.0f };
	uvMapping.positiveZ = FloatRect{ 0.0f, 0.75f, 1.0f, 0.75f };
	uvMapping.positiveX = FloatRect{ 0.25f, 0.75f, 0.25f, 0.75f };

	const Mesh3D defaultMesh = Mesh3D::Box();
	const Mesh3D collapsedMesh = Mesh3D::Box(Vec3{ 1.0, 1.0, 1.0 }, uvMapping);
	CheckMeshGeometry(collapsedMesh);

	SUBCASE("Collapsed U")
	{
		constexpr size_t VertexOffset = 0;
		for (size_t i = 0; i < 4; ++i)
		{
			CHECK_EQ(collapsedMesh.vertices[VertexOffset + i].tex.x, 0.25f);
			CHECK_EQ(collapsedMesh.vertices[VertexOffset + i].tangent, defaultMesh.vertices[VertexOffset + i].tangent);
		}
	}

	SUBCASE("Collapsed V")
	{
		constexpr size_t VertexOffset = 4;
		for (size_t i = 0; i < 4; ++i)
		{
			CHECK_EQ(collapsedMesh.vertices[VertexOffset + i].tex.y, 0.75f);
			CHECK_EQ(collapsedMesh.vertices[VertexOffset + i].tangent, defaultMesh.vertices[VertexOffset + i].tangent);
		}
	}

	SUBCASE("Collapsed U and V")
	{
		constexpr size_t VertexOffset = 8;
		for (size_t i = 0; i < 4; ++i)
		{
			CHECK_EQ(collapsedMesh.vertices[VertexOffset + i].tex, Float2{ 0.25f, 0.75f });
			CHECK_EQ(collapsedMesh.vertices[VertexOffset + i].tangent, defaultMesh.vertices[VertexOffset + i].tangent);
		}
	}
}

TEST_CASE("Mesh3D::RoundedBox")
{
	constexpr Vec3 Size{ 2.0, 4.0, 6.0 };
	constexpr double Radius = 0.5;
	constexpr uint32 Subdivisions = 2;
	const Mesh3D mesh = Mesh3D::RoundedBox(Size, Radius, Subdivisions);

	CHECK_EQ(mesh.vertexCount(), size_t{ 216 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 300 });
	CheckMeshGeometry(mesh);

	const Float3 innerHalfSize{ 0.5f, 1.5f, 2.5f };
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK(std::abs(vertex.pos.x) <= 1.0f);
		CHECK(std::abs(vertex.pos.y) <= 2.0f);
		CHECK(std::abs(vertex.pos.z) <= 3.0f);

		const Float3 innerPoint{
			std::clamp(vertex.pos.x, -innerHalfSize.x, innerHalfSize.x),
			std::clamp(vertex.pos.y, -innerHalfSize.y, innerHalfSize.y),
			std::clamp(vertex.pos.z, -innerHalfSize.z, innerHalfSize.z)
		};
		const Float3 offset = (vertex.pos - innerPoint);
		CHECK(offset.length() == doctest::Approx(Radius).epsilon(FrameEpsilon));
		CHECK(vertex.normal.dot(offset.normalized()) == doctest::Approx(1.0f).epsilon(FrameEpsilon));
	}

	SUBCASE("Box projection and flipped UV")
	{
		BoxUVMapping uvMapping;
		uvMapping.negativeZ = FloatRect{ 2.0f, 3.0f, -2.0f, -1.0f };
		const Mesh3D mappedMesh = Mesh3D::RoundedBox(Size, Radius, Subdivisions, uvMapping);
		CheckMeshGeometry(mappedMesh);

		constexpr size_t NegativeZVertexCount = 36;
		for (size_t i = 0; i < NegativeZVertexCount; ++i)
		{
			const Vertex3D& vertex = mappedMesh.vertices[i];
			const Float2 projectedUV{
				(0.5f + (vertex.pos.x / static_cast<float>(Size.x))),
				(0.5f - (vertex.pos.y / static_cast<float>(Size.y)))
			};
			CheckUV(vertex.tex, MapUV(uvMapping.negativeZ, projectedUV.x, projectedUV.y));
			CHECK(vertex.tangent.xyz().dot(mesh.vertices[i].tangent.xyz()) == doctest::Approx(-1.0f).epsilon(FrameEpsilon));
			CHECK_EQ(vertex.tangent.w, mesh.vertices[i].tangent.w);
		}
	}

	SUBCASE("Zero radius equals Box")
	{
		const Mesh3D box = Mesh3D::Box(Size);
		const Mesh3D zeroRadius = Mesh3D::RoundedBox(Size, 0.0, Subdivisions);
		REQUIRE_EQ(zeroRadius.vertexCount(), box.vertexCount());
		REQUIRE_EQ(zeroRadius.triangleCount(), box.triangleCount());

		for (size_t i = 0; i < box.vertices.size(); ++i)
		{
			CHECK_EQ(zeroRadius.vertices[i].pos, box.vertices[i].pos);
			CHECK_EQ(zeroRadius.vertices[i].normal, box.vertices[i].normal);
			CHECK_EQ(zeroRadius.vertices[i].tex, box.vertices[i].tex);
			CHECK_EQ(zeroRadius.vertices[i].tangent, box.vertices[i].tangent);
		}

		for (size_t i = 0; i < box.indices.size(); ++i)
		{
			CHECK_EQ(zeroRadius.indices[i].i0, box.indices[i].i0);
			CHECK_EQ(zeroRadius.indices[i].i1, box.indices[i].i1);
			CHECK_EQ(zeroRadius.indices[i].i2, box.indices[i].i2);
		}
	}

	SUBCASE("Maximum radius on a cube")
	{
		const Mesh3D sphere = Mesh3D::RoundedBox(Vec3{ 2.0, 2.0, 2.0 }, 1.0, Subdivisions);
		CHECK_EQ(sphere.vertexCount(), size_t{ 150 });
		CHECK_EQ(sphere.triangleCount(), size_t{ 192 });
		CheckMeshGeometry(sphere);

		for (const Vertex3D& vertex : sphere.vertices)
		{
			CHECK(vertex.pos.length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
			CHECK(vertex.normal.dot(vertex.pos) == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		}

		const Mesh3D roundedCapsule = Mesh3D::RoundedBox(Size, 1.0, Subdivisions);
		CHECK_EQ(roundedCapsule.vertexCount(), size_t{ 192 });
		CHECK_EQ(roundedCapsule.triangleCount(), size_t{ 260 });
		CheckMeshGeometry(roundedCapsule);
	}

	SUBCASE("Minimum subdivisions")
	{
		CheckMeshGeometry(Mesh3D::RoundedBox(Size, Radius, 1));
	}

	SUBCASE("Invalid arguments")
	{
		CHECK(Mesh3D::RoundedBox(Vec3{ 0.0, 1.0, 1.0 }, 0.1, 1).isEmpty());
		CHECK(Mesh3D::RoundedBox(Vec3{ 1.0, -1.0, 1.0 }, 0.1, 1).isEmpty());
		CHECK(Mesh3D::RoundedBox(Size, -0.1, 1).isEmpty());
		CHECK(Mesh3D::RoundedBox(Size, 1.1, 1).isEmpty());
		CHECK(Mesh3D::RoundedBox(Size, std::numeric_limits<double>::quiet_NaN(), 1).isEmpty());
		CHECK(Mesh3D::RoundedBox(Size, std::numeric_limits<double>::infinity(), 1).isEmpty());
		CHECK(Mesh3D::RoundedBox(Size, Radius, 0).isEmpty());
		CHECK(Mesh3D::RoundedBox(Size, Radius, std::numeric_limits<uint32>::max()).isEmpty());
		CHECK(Mesh3D::RoundedBox(
			Vec3{ std::numeric_limits<double>::max(), 1.0, 1.0 }, Radius, 1).isEmpty());

		BoxUVMapping invalidMapping;
		invalidMapping.positiveY.left = std::numeric_limits<float>::quiet_NaN();
		CHECK(Mesh3D::RoundedBox(Size, Radius, 1, invalidMapping).isEmpty());
	}
}

TEST_CASE("Mesh3D::ChamferedBox")
{
	constexpr Vec3 Size{ 2.0, 4.0, 6.0 };
	constexpr double Chamfer = 0.5;
	const Mesh3D mesh = Mesh3D::ChamferedBox(Size, Chamfer);

	CHECK_EQ(mesh.vertexCount(), size_t{ 96 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 44 });
	CheckMeshGeometry(mesh);

	size_t mainVertexCount = 0;
	size_t edgeVertexCount = 0;
	size_t cornerVertexCount = 0;
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK(std::abs(vertex.pos.x) <= 1.0f);
		CHECK(std::abs(vertex.pos.y) <= 2.0f);
		CHECK(std::abs(vertex.pos.z) <= 3.0f);
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));

		const size_t nonZeroNormalComponents =
			(static_cast<size_t>(vertex.normal.x != 0.0f)
				+ static_cast<size_t>(vertex.normal.y != 0.0f)
				+ static_cast<size_t>(vertex.normal.z != 0.0f));
		if (nonZeroNormalComponents == 1)
		{
			++mainVertexCount;
		}
		else if (nonZeroNormalComponents == 2)
		{
			++edgeVertexCount;
		}
		else
		{
			++cornerVertexCount;
		}
	}

	CHECK_EQ(mainVertexCount, size_t{ 24 });
	CHECK_EQ(edgeVertexCount, size_t{ 48 });
	CHECK_EQ(cornerVertexCount, size_t{ 24 });

	SUBCASE("Zero chamfer equals Box")
	{
		Mesh3DTest::CheckMeshDataEqual(
			Mesh3D::ChamferedBox(Size, 0.0),
			Mesh3D::Box(Size));
	}

	SUBCASE("Projection-axis priority")
	{
		const BoxUVMapping uvMapping{
			.negativeZ = FloatRect{ 10.0f, 0.0f, 10.0f, 1.0f },
			.positiveZ = FloatRect{ 20.0f, 0.0f, 20.0f, 1.0f },
			.positiveX = FloatRect{ 30.0f, 0.0f, 30.0f, 1.0f },
			.negativeX = FloatRect{ 40.0f, 0.0f, 40.0f, 1.0f },
			.positiveY = FloatRect{ 50.0f, 0.0f, 50.0f, 1.0f },
			.negativeY = FloatRect{ 60.0f, 0.0f, 60.0f, 1.0f },
		};
		const Mesh3D mappedMesh = Mesh3D::ChamferedBox(Size, Chamfer, uvMapping);
		CheckMeshGeometry(mappedMesh);

		std::array<size_t, 6> projectionCounts{};
		for (const Vertex3D& vertex : mappedMesh.vertices)
		{
			if (vertex.tex.x == 10.0f)
			{
				++projectionCounts[0];
			}
			else if (vertex.tex.x == 20.0f)
			{
				++projectionCounts[1];
			}
			else if (vertex.tex.x == 30.0f)
			{
				++projectionCounts[2];
			}
			else if (vertex.tex.x == 40.0f)
			{
				++projectionCounts[3];
			}
			else if (vertex.tex.x == 50.0f)
			{
				++projectionCounts[4];
			}
			else if (vertex.tex.x == 60.0f)
			{
				++projectionCounts[5];
			}
		}

		constexpr std::array<size_t, 6> ExpectedCounts{ 4, 4, 32, 32, 12, 12 };
		CHECK_EQ(projectionCounts, ExpectedCounts);
	}

	SUBCASE("Boundary and invalid arguments")
	{
		CheckMeshGeometry(Mesh3D::ChamferedBox(Size, 0.99));
		CHECK(Mesh3D::ChamferedBox(Size, 1.0).isEmpty());
		CHECK(Mesh3D::ChamferedBox(Size, 1.01).isEmpty());
		CHECK(Mesh3D::ChamferedBox(Size, -0.1).isEmpty());
		CHECK(Mesh3D::ChamferedBox(
			Size, std::numeric_limits<double>::quiet_NaN()).isEmpty());
		CHECK(Mesh3D::ChamferedBox(
			Size, std::numeric_limits<double>::infinity()).isEmpty());
		CHECK(Mesh3D::ChamferedBox(Vec3{ 0.0, 1.0, 1.0 }, 0.1).isEmpty());
		CHECK(Mesh3D::ChamferedBox(
			Vec3{ std::numeric_limits<double>::max(), 1.0, 1.0 }, 0.1).isEmpty());

		BoxUVMapping invalidMapping;
		invalidMapping.positiveY.left = std::numeric_limits<float>::quiet_NaN();
		CHECK(Mesh3D::ChamferedBox(Size, Chamfer, invalidMapping).isEmpty());
	}
}

TEST_CASE("Mesh3D::Wedge")
{
	const Vec3 size{ 4.0, 2.0, 6.0 };
	const Mesh3D mesh = Mesh3D::Wedge(size);

	CHECK_EQ(mesh.vertexCount(), size_t{ 18 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 8 });
	CheckMeshGeometry(mesh);

	const Float3 expectedSlopeNormal = Float3{ 0.0f, 6.0f, -2.0f }.normalized();
	size_t slopeVertexCount = 0;
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.x && vertex.pos.x <= 2.0f));
		CHECK((-1.0f <= vertex.pos.y && vertex.pos.y <= 1.0f));
		CHECK((-3.0f <= vertex.pos.z && vertex.pos.z <= 3.0f));
		if (vertex.normal.dot(expectedSlopeNormal) > (1.0f - FrameEpsilon))
		{
			++slopeVertexCount;
			CHECK(vertex.pos.y == doctest::Approx(vertex.pos.z / 3.0f).epsilon(FrameEpsilon));
		}
	}
	CHECK_EQ(slopeVertexCount, size_t{ 4 });

	const Mesh3D defaultMappingMesh = Mesh3D::Wedge(size, BoxUVMapping{});
	REQUIRE_EQ(defaultMappingMesh.vertexCount(), mesh.vertexCount());
	REQUIRE_EQ(defaultMappingMesh.triangleCount(), mesh.triangleCount());
	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK_EQ(defaultMappingMesh.vertices[i].pos, mesh.vertices[i].pos);
		CHECK_EQ(defaultMappingMesh.vertices[i].normal, mesh.vertices[i].normal);
		CHECK_EQ(defaultMappingMesh.vertices[i].tex, mesh.vertices[i].tex);
		CHECK_EQ(defaultMappingMesh.vertices[i].tangent, mesh.vertices[i].tangent);
	}

	CHECK(Mesh3D::Wedge(Vec3{ 0.0, 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::Wedge(Vec3{ 1.0, -1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::Wedge(Vec3{ 1.0, 1.0, std::numeric_limits<double>::infinity() }).isEmpty());
	CHECK(Mesh3D::Wedge(Vec3{ std::numeric_limits<double>::max(), 1.0, 1.0 }).isEmpty());

	const double maxFloat = std::numeric_limits<float>::max();
	const Mesh3D maxSizeMesh = Mesh3D::Wedge(Vec3{ 1.0, maxFloat, maxFloat });
	REQUIRE_FALSE(maxSizeMesh.isEmpty());
	for (const Vertex3D& vertex : maxSizeMesh.vertices)
	{
		CheckVertexFrame(vertex);
	}
}

TEST_CASE("Mesh3D::Wedge with projected UV mapping")
{
	const Vec3 size{ 4.0, 2.0, 6.0 };
	const BoxUVMapping uvMapping{
		.negativeZ = FloatRect{ 0.0f, 0.0f, 0.1f, 0.1f },
		.positiveZ = FloatRect{ 0.8f, 0.1f, 0.2f, 0.7f },
		.positiveX = FloatRect{ 0.1f, 0.2f, 0.4f, 0.8f },
		.negativeX = FloatRect{ 0.9f, 0.8f, 0.5f, 0.2f },
		.positiveY = FloatRect{ -1.0f, 0.25f, 1.0f, 0.75f },
		.negativeY = FloatRect{ 0.2f, 0.9f, 0.8f, 0.3f },
	};
	const Mesh3D mesh = Mesh3D::Wedge(size, uvMapping);
	CheckMeshGeometry(mesh);

	for (const Vertex3D& vertex : mesh.vertices)
	{
		const float x = (0.5f + (vertex.pos.x / static_cast<float>(size.x)));
		const float y = (0.5f - (vertex.pos.y / static_cast<float>(size.y)));
		const float z = (0.5f + (vertex.pos.z / static_cast<float>(size.z)));

		if (vertex.normal == Float3::UnitZ())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveZ, (1.0f - x), y));
		}
		else if (vertex.normal == Float3::UnitX())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveX, z, y));
		}
		else if (vertex.normal == -Float3::UnitX())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeX, (1.0f - z), y));
		}
		else if (vertex.normal == -Float3::UnitY())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeY, x, z));
		}
		else
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveY, x, (1.0f - z)));
		}
	}

	BoxUVMapping invalid = uvMapping;
	invalid.positiveY.right = std::numeric_limits<float>::quiet_NaN();
	CHECK(Mesh3D::Wedge(size, invalid).isEmpty());
}

TEST_CASE("Mesh3D::TriangularPrism")
{
	const Vec3 size{ 4.0, 2.0, 6.0 };
	const Mesh3D mesh = Mesh3D::TriangularPrism(size);

	CHECK_EQ(mesh.vertexCount(), size_t{ 18 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 8 });
	CheckMeshGeometry(mesh);

	const Float3 expectedFrontNormal = Float3{ 0.0f, 3.0f, -2.0f }.normalized();
	const Float3 expectedBackNormal = Float3{ 0.0f, 3.0f, 2.0f }.normalized();
	std::array<size_t, 5> normalCounts{};

	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.x && vertex.pos.x <= 2.0f));
		CHECK((-1.0f <= vertex.pos.y && vertex.pos.y <= 1.0f));
		CHECK((-3.0f <= vertex.pos.z && vertex.pos.z <= 3.0f));

		if (vertex.normal == Float3::UnitX()) { ++normalCounts[0]; }
		else if (vertex.normal == -Float3::UnitX()) { ++normalCounts[1]; }
		else if (vertex.normal == -Float3::UnitY()) { ++normalCounts[2]; }
		else if (vertex.normal.dot(expectedFrontNormal) > (1.0f - FrameEpsilon)) { ++normalCounts[3]; }
		else if (vertex.normal.dot(expectedBackNormal) > (1.0f - FrameEpsilon)) { ++normalCounts[4]; }
	}

	CHECK_EQ(normalCounts, (std::array<size_t, 5>{ 3, 3, 4, 4, 4 }));
	CHECK_EQ(mesh.vertices[2].pos, Float3{ 2.0f, 1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[5].pos, Float3{ -2.0f, 1.0f, 0.0f });

	const Mesh3D defaultMappingMesh = Mesh3D::TriangularPrism(size, BoxUVMapping{});
	REQUIRE_EQ(defaultMappingMesh.vertexCount(), mesh.vertexCount());
	for (size_t i = 0; i < mesh.vertexCount(); ++i)
	{
		CHECK_EQ(defaultMappingMesh.vertices[i].pos, mesh.vertices[i].pos);
		CHECK_EQ(defaultMappingMesh.vertices[i].normal, mesh.vertices[i].normal);
		CHECK_EQ(defaultMappingMesh.vertices[i].tex, mesh.vertices[i].tex);
		CHECK_EQ(defaultMappingMesh.vertices[i].tangent, mesh.vertices[i].tangent);
	}

	CHECK(Mesh3D::TriangularPrism(Vec3{ 0.0, 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::TriangularPrism(Vec3{ 1.0, -1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::TriangularPrism(Vec3{ 1.0, 1.0, std::numeric_limits<double>::infinity() }).isEmpty());
	CHECK(Mesh3D::TriangularPrism(Vec3{ std::numeric_limits<double>::max(), 1.0, 1.0 }).isEmpty());

	BoxUVMapping invalidMapping;
	invalidMapping.positiveY.left = std::numeric_limits<float>::quiet_NaN();
	CHECK(Mesh3D::TriangularPrism(size, invalidMapping).isEmpty());
}

TEST_CASE("Mesh3D::TriangularPrism with projected UV mapping")
{
	const Vec3 size{ 4.0, 2.0, 6.0 };
	const BoxUVMapping uvMapping{
		.positiveX = FloatRect{ 0.1f, 0.2f, 0.4f, 0.8f },
		.negativeX = FloatRect{ 0.9f, 0.8f, 0.5f, 0.2f },
		.positiveY = FloatRect{ -1.0f, 0.25f, 1.0f, 0.75f },
		.negativeY = FloatRect{ 0.2f, 0.9f, 0.8f, 0.3f },
	};
	const Mesh3D mesh = Mesh3D::TriangularPrism(size, uvMapping);
	CheckMeshGeometry(mesh);

	for (const Vertex3D& vertex : mesh.vertices)
	{
		const float x = (0.5f + (vertex.pos.x / static_cast<float>(size.x)));
		const float y = (0.5f - (vertex.pos.y / static_cast<float>(size.y)));
		const float z = (0.5f + (vertex.pos.z / static_cast<float>(size.z)));

		if (vertex.normal == Float3::UnitX())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveX, z, y));
		}
		else if (vertex.normal == -Float3::UnitX())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeX, (1.0f - z), y));
		}
		else if (vertex.normal == -Float3::UnitY())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeY, x, z));
		}
		else
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveY, x, (1.0f - z)));
		}
	}
}

TEST_CASE("Mesh3D::Stairs")
{
	constexpr uint32 Steps = 4;
	const Vec3 size{ 4.0, 2.0, 8.0 };
	const Mesh3D mesh = Mesh3D::Stairs(size, Steps);

	CHECK_EQ(mesh.vertexCount(), size_t{ (16 * Steps) + 8 });
	CHECK_EQ(mesh.triangleCount(), size_t{ (8 * Steps) + 4 });
	CheckMeshGeometry(mesh);

	std::array<size_t, 6> normalCounts{};
	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.x && vertex.pos.x <= 2.0f));
		CHECK((-1.0f <= vertex.pos.y && vertex.pos.y <= 1.0f));
		CHECK((-4.0f <= vertex.pos.z && vertex.pos.z <= 4.0f));

		if (vertex.normal == -Float3::UnitZ()) { ++normalCounts[0]; }
		else if (vertex.normal == Float3::UnitZ()) { ++normalCounts[1]; }
		else if (vertex.normal == Float3::UnitX()) { ++normalCounts[2]; }
		else if (vertex.normal == -Float3::UnitX()) { ++normalCounts[3]; }
		else if (vertex.normal == Float3::UnitY()) { ++normalCounts[4]; }
		else if (vertex.normal == -Float3::UnitY()) { ++normalCounts[5]; }
	}
	CHECK_EQ(normalCounts[0], size_t{ 4 * Steps });
	CHECK_EQ(normalCounts[1], size_t{ 4 });
	CHECK_EQ(normalCounts[2], size_t{ 4 * Steps });
	CHECK_EQ(normalCounts[3], size_t{ 4 * Steps });
	CHECK_EQ(normalCounts[4], size_t{ 4 * Steps });
	CHECK_EQ(normalCounts[5], size_t{ 4 });

	const Mesh3D oneStep = Mesh3D::Stairs(size, 1);
	CHECK_EQ(oneStep.vertexCount(), size_t{ 24 });
	CHECK_EQ(oneStep.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(oneStep);

	CHECK(Mesh3D::Stairs(size, 0).isEmpty());
	CHECK(Mesh3D::Stairs(Vec3{ 0.0, 1.0, 1.0 }, Steps).isEmpty());
	CHECK(Mesh3D::Stairs(Vec3{ 1.0, 1.0, -1.0 }, Steps).isEmpty());
	CHECK(Mesh3D::Stairs(Vec3{ 1.0, std::numeric_limits<double>::infinity(), 1.0 }, Steps).isEmpty());
	CHECK(Mesh3D::Stairs(size, std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Stairs with projected UV mapping")
{
	constexpr uint32 Steps = 3;
	const Vec3 size{ 4.0, 3.0, 6.0 };
	const BoxUVMapping uvMapping{
		.negativeZ = FloatRect{ 0.8f, 0.1f, 0.2f, 0.9f },
		.positiveZ = FloatRect{ 0.1f, 0.2f, 0.4f, 0.8f },
		.positiveX = FloatRect{ -1.0f, 0.0f, 1.0f, 0.5f },
		.negativeX = FloatRect{ 1.0f, 0.75f, 0.0f, 0.25f },
		.positiveY = FloatRect{ 0.2f, 0.9f, 0.8f, 0.3f },
		.negativeY = FloatRect{ 0.0f, 0.0f, 0.5f, 1.0f },
	};
	const Mesh3D mesh = Mesh3D::Stairs(size, Steps, uvMapping);
	CheckMeshGeometry(mesh);

	for (const Vertex3D& vertex : mesh.vertices)
	{
		const float x = (0.5f + (vertex.pos.x / static_cast<float>(size.x)));
		const float y = (0.5f - (vertex.pos.y / static_cast<float>(size.y)));
		const float z = (0.5f + (vertex.pos.z / static_cast<float>(size.z)));

		if (vertex.normal == -Float3::UnitZ())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeZ, x, y));
		}
		else if (vertex.normal == Float3::UnitZ())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveZ, (1.0f - x), y));
		}
		else if (vertex.normal == Float3::UnitX())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveX, z, y));
		}
		else if (vertex.normal == -Float3::UnitX())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeX, (1.0f - z), y));
		}
		else if (vertex.normal == Float3::UnitY())
		{
			CheckUV(vertex.tex, MapUV(uvMapping.positiveY, x, (1.0f - z)));
		}
		else
		{
			CheckUV(vertex.tex, MapUV(uvMapping.negativeY, x, z));
		}
	}

	const Mesh3D defaultMesh = Mesh3D::Stairs(size, Steps);
	const Mesh3D defaultMappingMesh = Mesh3D::Stairs(size, Steps, BoxUVMapping{});
	REQUIRE_EQ(defaultMappingMesh.vertexCount(), defaultMesh.vertexCount());
	for (size_t i = 0; i < defaultMesh.vertices.size(); ++i)
	{
		CHECK_EQ(defaultMappingMesh.vertices[i].tex, defaultMesh.vertices[i].tex);
		CHECK_EQ(defaultMappingMesh.vertices[i].tangent, defaultMesh.vertices[i].tangent);
	}

	BoxUVMapping invalid = uvMapping;
	invalid.negativeZ.top = std::numeric_limits<float>::infinity();
	CHECK(Mesh3D::Stairs(size, Steps, invalid).isEmpty());
}

TEST_CASE("Mesh3D::Pyramid")
{
	const SizeF baseSizeXZ{ 4.0, 2.0 };
	constexpr double Height = 3.0;
	const Mesh3D mesh = Mesh3D::Pyramid(baseSizeXZ, Height);

	CHECK_EQ(mesh.vertexCount(), size_t{ 16 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 6 });
	CheckMeshGeometry(mesh);

	for (const auto& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.x && vertex.pos.x <= 2.0f));
		CHECK((-1.5f <= vertex.pos.y && vertex.pos.y <= 1.5f));
		CHECK((-1.0f <= vertex.pos.z && vertex.pos.z <= 1.0f));
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	const Float3 expectedSideNormals[4] =
	{
		Float3{ 0.0f, 1.0f, -Height }.normalized(),
		Float3{ 0.0f, 1.0f, Height }.normalized(),
		Float3{ Height, 2.0f, 0.0f }.normalized(),
		Float3{ -Height, 2.0f, 0.0f }.normalized(),
	};
	const Float3 expectedSideTangents[4] =
	{
		Float3::UnitX(),
		-Float3::UnitX(),
		Float3::UnitZ(),
		-Float3::UnitZ(),
	};

	for (size_t faceIndex = 0; faceIndex < 4; ++faceIndex)
	{
		const size_t vertexOffset = (faceIndex * 3);
		CHECK_EQ(mesh.vertices[vertexOffset + 0].tex, Float2{ 1.0f, 1.0f });
		CHECK_EQ(mesh.vertices[vertexOffset + 1].tex, Float2{ 0.0f, 1.0f });
		CHECK_EQ(mesh.vertices[vertexOffset + 2].pos, Float3{ 0.0f, 1.5f, 0.0f });
		CHECK_EQ(mesh.vertices[vertexOffset + 2].tex, Float2{ 0.5f, 0.0f });

		for (size_t i = 0; i < 3; ++i)
		{
			const Vertex3D& vertex = mesh.vertices[vertexOffset + i];
			CHECK(vertex.normal.x == doctest::Approx(expectedSideNormals[faceIndex].x).epsilon(FrameEpsilon));
			CHECK(vertex.normal.y == doctest::Approx(expectedSideNormals[faceIndex].y).epsilon(FrameEpsilon));
			CHECK(vertex.normal.z == doctest::Approx(expectedSideNormals[faceIndex].z).epsilon(FrameEpsilon));
			CHECK_EQ(vertex.tangent, Float4{ expectedSideTangents[faceIndex], 1.0f });
		}
	}

	constexpr size_t BottomVertexBase = 12;
	CHECK_EQ(mesh.vertices[BottomVertexBase + 0].pos, Float3{ -2.0f, -1.5f, -1.0f });
	CHECK_EQ(mesh.vertices[BottomVertexBase + 1].pos, Float3{ 2.0f, -1.5f, -1.0f });
	CHECK_EQ(mesh.vertices[BottomVertexBase + 2].pos, Float3{ -2.0f, -1.5f, 1.0f });
	CHECK_EQ(mesh.vertices[BottomVertexBase + 3].pos, Float3{ 2.0f, -1.5f, 1.0f });
	CHECK_EQ(mesh.vertices[BottomVertexBase + 0].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[BottomVertexBase + 3].tex, Float2{ 1.0f, 1.0f });

	for (size_t i = BottomVertexBase; i < mesh.vertices.size(); ++i)
	{
		CHECK_EQ(mesh.vertices[i].normal, -Float3::UnitY());
		CHECK_EQ(mesh.vertices[i].tangent, Float4{ 1.0f, 0.0f, 0.0f, 1.0f });
	}

	CheckMeshGeometry(Mesh3D::Pyramid());
	const Mesh3D squareMesh = Mesh3D::Pyramid(2.0, Height);
	const Mesh3D squareMeshFromSize = Mesh3D::Pyramid(SizeF{ 2.0, 2.0 }, Height);
	CHECK_EQ(squareMesh.vertexCount(), squareMeshFromSize.vertexCount());
	CHECK_EQ(squareMesh.triangleCount(), squareMeshFromSize.triangleCount());
	for (size_t i = 0; i < squareMesh.vertices.size(); ++i)
	{
		CHECK_EQ(squareMesh.vertices[i].pos, squareMeshFromSize.vertices[i].pos);
		CHECK_EQ(squareMesh.vertices[i].normal, squareMeshFromSize.vertices[i].normal);
		CHECK_EQ(squareMesh.vertices[i].tex, squareMeshFromSize.vertices[i].tex);
		CHECK_EQ(squareMesh.vertices[i].tangent, squareMeshFromSize.vertices[i].tangent);
	}
	CHECK(Mesh3D::Pyramid(-1.0, 1.0).isEmpty());
	CHECK(Mesh3D::Pyramid(SizeF{ 0.0, 1.0 }, 1.0).isEmpty());
	CHECK(Mesh3D::Pyramid(SizeF{ 1.0, -1.0 }, 1.0).isEmpty());
	CHECK(Mesh3D::Pyramid(SizeF{ 1.0, 1.0 }, 0.0).isEmpty());
	CHECK(Mesh3D::Pyramid(SizeF{ std::numeric_limits<double>::infinity(), 1.0 }, 1.0).isEmpty());
	CHECK(Mesh3D::Pyramid(SizeF{ 1.0, 1.0 }, std::numeric_limits<double>::quiet_NaN()).isEmpty());
}

TEST_CASE("Mesh3D::RectangularFrustum")
{
	const SizeF bottomSizeXZ{ 4.0, 6.0 };
	const SizeF topSizeXZ{ 2.0, 2.0 };
	constexpr double Height = 4.0;
	const Mesh3D mesh = Mesh3D::RectangularFrustum(bottomSizeXZ, topSizeXZ, Height);

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);

	const std::array<Float3, 6> expectedNormals{
		Float3{ 0.0f, 2.0f, -4.0f }.normalized(),
		Float3{ 0.0f, 2.0f, 4.0f }.normalized(),
		Float3{ 4.0f, 1.0f, 0.0f }.normalized(),
		Float3{ -4.0f, 1.0f, 0.0f }.normalized(),
		Float3::UnitY(),
		-Float3::UnitY(),
	};

	for (size_t faceIndex = 0; faceIndex < expectedNormals.size(); ++faceIndex)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			const Vertex3D& vertex = mesh.vertices[(faceIndex * 4) + i];
			CHECK(vertex.normal.dot(expectedNormals[faceIndex]) > (1.0f - FrameEpsilon));
		}
	}

	CHECK_EQ(mesh.vertices[0].pos, Float3{ -1.0f, 2.0f, -1.0f });
	CHECK_EQ(mesh.vertices[2].pos, Float3{ -2.0f, -2.0f, -3.0f });
	CheckUV(mesh.vertices[16].tex, Float2{ 0.25f, (1.0f / 3.0f) });
	CheckUV(mesh.vertices[19].tex, Float2{ 0.75f, (2.0f / 3.0f) });

	const Mesh3D inverted = Mesh3D::RectangularFrustum(topSizeXZ, bottomSizeXZ, Height);
	CHECK_EQ(inverted.vertexCount(), mesh.vertexCount());
	CheckMeshGeometry(inverted);

	const BoxUVMapping uvMapping{
		.negativeZ = FloatRect{ 0.1f, 0.2f, 0.4f, 0.8f },
		.positiveZ = FloatRect{ 0.9f, 0.8f, 0.5f, 0.2f },
		.positiveX = FloatRect{ -1.0f, 0.25f, 1.0f, 0.75f },
		.negativeX = FloatRect{ 0.2f, 0.9f, 0.8f, 0.3f },
		.positiveY = FloatRect{ 0.0f, 0.0f, 0.5f, 0.5f },
		.negativeY = FloatRect{ 0.5f, 0.5f, 1.0f, 1.0f },
	};
	const Mesh3D mapped = Mesh3D::RectangularFrustum(bottomSizeXZ, topSizeXZ, Height, uvMapping);
	CheckMeshGeometry(mapped);
	CheckUV(mapped.vertices[0].tex, MapUV(uvMapping.negativeZ, 0.25f, 0.0f));
	CheckUV(mapped.vertices[7].tex, MapUV(uvMapping.positiveZ, 1.0f, 1.0f));
	CheckUV(mapped.vertices[16].tex, MapUV(uvMapping.positiveY, 0.25f, (1.0f / 3.0f)));
	CheckUV(mapped.vertices[23].tex, MapUV(uvMapping.negativeY, 1.0f, 1.0f));

	const Mesh3D box = Mesh3D::Box(Vec3{ 4.0, Height, 6.0 }, uvMapping);
	const Mesh3D equalEnds = Mesh3D::RectangularFrustum(bottomSizeXZ, bottomSizeXZ, Height, uvMapping);
	REQUIRE_EQ(equalEnds.vertexCount(), box.vertexCount());
	REQUIRE_EQ(equalEnds.triangleCount(), box.triangleCount());
	for (size_t i = 0; i < box.vertexCount(); ++i)
	{
		CHECK_EQ(equalEnds.vertices[i].pos, box.vertices[i].pos);
		CHECK_EQ(equalEnds.vertices[i].normal, box.vertices[i].normal);
		CHECK_EQ(equalEnds.vertices[i].tex, box.vertices[i].tex);
		CHECK_EQ(equalEnds.vertices[i].tangent, box.vertices[i].tangent);
	}

	CHECK(Mesh3D::RectangularFrustum(SizeF{ 0.0, 1.0 }, topSizeXZ, Height).isEmpty());
	CHECK(Mesh3D::RectangularFrustum(bottomSizeXZ, SizeF{ 1.0, -1.0 }, Height).isEmpty());
	CHECK(Mesh3D::RectangularFrustum(bottomSizeXZ, topSizeXZ, 0.0).isEmpty());
	CHECK(Mesh3D::RectangularFrustum(
		SizeF{ std::numeric_limits<double>::infinity(), 1.0 }, topSizeXZ, Height).isEmpty());

	BoxUVMapping invalidMapping;
	invalidMapping.negativeZ.bottom = std::numeric_limits<float>::quiet_NaN();
	CHECK(Mesh3D::RectangularFrustum(bottomSizeXZ, topSizeXZ, Height, invalidMapping).isEmpty());
}

TEST_CASE("Mesh3D::Tetrahedron")
{
	const Mesh3D mesh = Mesh3D::Tetrahedron(2.0);
	CheckRegularPolyhedron(mesh, 2.0, 4, 3);
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 1.0f, 1.0f });
	CHECK_EQ(mesh.vertices[1].tex, Float2{ 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[2].tex, Float2{ 0.5f, 0.0f });
	CheckRegularPolyhedron(Mesh3D::Tetrahedron(), 1.0, 4, 3);
	CHECK(Mesh3D::Tetrahedron(0.0).isEmpty());
	CHECK(Mesh3D::Tetrahedron(std::numeric_limits<double>::infinity()).isEmpty());
}

TEST_CASE("Mesh3D::Octahedron")
{
	CheckRegularPolyhedron(Mesh3D::Octahedron(2.0), 2.0, 8, 3);
	CheckRegularPolyhedron(Mesh3D::Octahedron(), 1.0, 8, 3);
	CHECK(Mesh3D::Octahedron(-1.0).isEmpty());
	CHECK(Mesh3D::Octahedron(std::numeric_limits<double>::quiet_NaN()).isEmpty());
}

TEST_CASE("Mesh3D::Icosahedron")
{
	CheckRegularPolyhedron(Mesh3D::Icosahedron(2.0), 2.0, 20, 3);
	CheckRegularPolyhedron(Mesh3D::Icosahedron(), 1.0, 20, 3);
	CHECK(Mesh3D::Icosahedron(0.0).isEmpty());
	CHECK(Mesh3D::Icosahedron(std::numeric_limits<double>::max()).isEmpty());
}

TEST_CASE("Mesh3D::IcoSphere")
{
	constexpr std::array<size_t, 4> ExpectedVertexCounts{ 12, 42, 162, 642 };
	constexpr std::array<size_t, 4> ExpectedTriangleCounts{ 20, 80, 320, 1280 };

	for (uint32 subdivisions = 0; subdivisions < ExpectedVertexCounts.size(); ++subdivisions)
	{
		const Mesh3D mesh = Mesh3D::IcoSphere(2.0, subdivisions);
		REQUIRE_EQ(mesh.vertexCount(), ExpectedVertexCounts[subdivisions]);
		REQUIRE_EQ(mesh.triangleCount(), ExpectedTriangleCounts[subdivisions]);
		CheckMeshGeometry(mesh, Mesh3DTest::TangentHandedness::Positive);

		Array<bool> usedVertices(mesh.vertexCount(), false);
		for (const TriangleIndex32& triangle : mesh.indices)
		{
			usedVertices[triangle.i0] = true;
			usedVertices[triangle.i1] = true;
			usedVertices[triangle.i2] = true;
		}

		for (size_t i = 0; i < mesh.vertexCount(); ++i)
		{
			const Vertex3D& vertex = mesh.vertices[i];
			CHECK(vertex.pos.length() == doctest::Approx(2.0f).epsilon(FrameEpsilon));
			CHECK_EQ(vertex.normal, (vertex.pos / 2.0f));
			CHECK_EQ(vertex.tex, Float2{ 0.0f, 0.0f });
			CHECK(usedVertices[i]);
		}
	}

	const Mesh3D defaultMesh = Mesh3D::IcoSphere();
	CHECK_EQ(defaultMesh.vertexCount(), size_t{ 162 });
	CHECK_EQ(defaultMesh.triangleCount(), size_t{ 320 });

	const Mesh3D maximumMesh = Mesh3D::IcoSphere(1.0, 8);
	CHECK_EQ(maximumMesh.vertexCount(), size_t{ 655362 });
	CHECK_EQ(maximumMesh.triangleCount(), size_t{ 1310720 });
	CHECK(maximumMesh.validate());

	CHECK(Mesh3D::IcoSphere(0.0, 0).isEmpty());
	CHECK(Mesh3D::IcoSphere(-1.0, 1).isEmpty());
	CHECK(Mesh3D::IcoSphere(std::numeric_limits<double>::infinity(), 2).isEmpty());
	CHECK(Mesh3D::IcoSphere(std::numeric_limits<double>::max(), 2).isEmpty());
	CHECK(Mesh3D::IcoSphere(1.0, 9).isEmpty());
}

TEST_CASE("Mesh3D::Dodecahedron")
{
	CheckRegularPolyhedron(Mesh3D::Dodecahedron(2.0), 2.0, 12, 5);
	CheckRegularPolyhedron(Mesh3D::Dodecahedron(), 1.0, 12, 5);
	CHECK(Mesh3D::Dodecahedron(0.0).isEmpty());
	CHECK(Mesh3D::Dodecahedron(std::numeric_limits<double>::infinity()).isEmpty());
}

TEST_CASE("Mesh3D::Plane")
{
	const Vec2 uvScale{ 2.0, 3.0 };
	const Vec2 uvOffset{ -0.25, 0.5 };
	const Mesh3D mesh = Mesh3D::Plane(SizeF{ 4.0, 2.0 }, uvScale, uvOffset);

	CHECK_EQ(mesh.vertexCount(), size_t{ 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[3].pos, Float3{ 2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].tex, Float2{ uvOffset });
	CHECK_EQ(mesh.vertices[3].tex, Float2{ (uvOffset + uvScale) });

	CHECK(Mesh3D::Plane(SizeF{ -1.0, 1.0 }).isEmpty());
}

TEST_CASE("Mesh3D::Grid")
{
	const Vec2 uvScale{ 2.0, 3.0 };
	const Vec2 uvOffset{ 0.25, -0.5 };
	const Mesh3D mesh = Mesh3D::Grid(SizeF{ 4.0, 2.0 }, 2, 1, uvScale, uvOffset);

	CHECK_EQ(mesh.vertexCount(), size_t{ 6 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 });
	CheckMeshGeometry(mesh);

	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[2].pos, Float3{ 2.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[3].pos, Float3{ -2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[5].pos, Float3{ 2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].tex, Float2{ uvOffset });
	CHECK_EQ(mesh.vertices[5].tex, Float2{ (uvOffset + uvScale) });

	CHECK(Mesh3D::Grid(SizeF{ 1.0, 1.0 }, 0, 1).isEmpty());
	CHECK(Mesh3D::Grid(SizeF{ 1.0, 1.0 }, 1, 0).isEmpty());
	CHECK(Mesh3D::Grid(SizeF{ 0.0, 1.0 }, 1, 1).isEmpty());
	CHECK(Mesh3D::Grid(SizeF{ 1.0, 1.0 }, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
	CHECK(Mesh3D::Grid(SizeF{ 1.0, 1.0 }, 1, 1, Vec2{ std::numeric_limits<double>::max(), 1.0 }).isEmpty());
}

TEST_CASE("Mesh3D::Sphere")
{
	constexpr uint32 Slices = 8;
	constexpr uint32 Stacks = 4;
	const Mesh3D mesh = Mesh3D::Sphere(2.0, Slices, Stacks);
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
	for (uint32 x = 0; x < Slices; ++x)
	{
		CHECK_EQ(mesh.vertices[x].pos, Float3{ 0.0f, 2.0f, 0.0f });
		CHECK_EQ(mesh.vertices[x].tex.y, 0.0f);
	}

	const size_t equatorBase = (firstRingBase + (Stacks / 2 - 1) * (Slices + 1));
	CHECK(mesh.vertices[equatorBase + 0].pos.x == doctest::Approx(2.0f).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[equatorBase + 0].pos.y == doctest::Approx(0.0f).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[equatorBase + 0].pos.z == doctest::Approx(0.0f).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[equatorBase + 0].tex, Float2{ 0.0f, 0.5f });
	CHECK(mesh.vertices[equatorBase + 1].pos.z > 0.0f);
	CHECK(mesh.vertices[equatorBase + 1].tex.x > mesh.vertices[equatorBase + 0].tex.x);
	CHECK(mesh.vertices[equatorBase + Slices / 4].pos.x == doctest::Approx(0.0f).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[equatorBase + Slices / 4].pos.y == doctest::Approx(0.0f).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[equatorBase + Slices / 4].pos.z == doctest::Approx(2.0f).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[equatorBase + Slices / 4].tex, Float2{ 0.25f, 0.5f });

	const size_t bottomPoleBase = (firstRingBase + (Stacks - 1) * (Slices + 1));
	for (uint32 x = 0; x < Slices; ++x)
	{
		CHECK_EQ(mesh.vertices[bottomPoleBase + x].pos, Float3{ 0.0f, -2.0f, 0.0f });
		CHECK_EQ(mesh.vertices[bottomPoleBase + x].tex.y, 1.0f);
	}

	CHECK_EQ(mesh.vertices[firstRingBase].pos, mesh.vertices[firstRingBase + Slices].pos);
	CHECK_EQ(mesh.vertices[firstRingBase].normal, mesh.vertices[firstRingBase + Slices].normal);
	CHECK_EQ(mesh.vertices[firstRingBase].tangent, mesh.vertices[firstRingBase + Slices].tangent);
	CHECK_EQ(mesh.vertices[firstRingBase].tex.x, 0.0f);
	CHECK_EQ(mesh.vertices[firstRingBase + Slices].tex.x, 1.0f);

	CHECK(Mesh3D::Sphere(0.0, Slices, Stacks).isEmpty());
	CHECK(Mesh3D::Sphere(1.0, 2, Stacks).isEmpty());
	CHECK(Mesh3D::Sphere(1.0, Slices, 1).isEmpty());
	CHECK(Mesh3D::Sphere(1.0, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Hemisphere")
{
	constexpr double Radius = 2.0;
	constexpr uint32 Slices = 8;
	constexpr uint32 Stacks = 2;
	const Mesh3D mesh = Mesh3D::Hemisphere(Radius, Slices, Stacks);
	const size_t expectedVertexCount = (Slices + Stacks * (Slices + 1));
	const size_t expectedTriangleCount = (Slices * (2 * Stacks - 1));

	CHECK_EQ(mesh.vertexCount(), expectedVertexCount);
	CHECK_EQ(mesh.triangleCount(), expectedTriangleCount);
	CheckMeshGeometry(mesh);

	for (const auto& vertex : mesh.vertices)
	{
		CHECK(vertex.pos.length() == doctest::Approx(Radius).epsilon(FrameEpsilon));
		CHECK(vertex.pos.y >= 0.0f);
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	CHECK_EQ(mesh.vertices[0].pos, Float3{ 0.0f, Radius, 0.0f });
	CHECK_EQ(mesh.vertices[0].normal, Float3::UnitY());
	CHECK_EQ(mesh.vertices[0].tex.y, 0.0f);

	const size_t equatorBase = (Slices + (Stacks - 1) * (Slices + 1));
	CHECK_EQ(mesh.vertices[equatorBase].pos, mesh.vertices[equatorBase + Slices].pos);
	CHECK_EQ(mesh.vertices[equatorBase].normal, mesh.vertices[equatorBase + Slices].normal);
	CHECK_EQ(mesh.vertices[equatorBase].tangent, mesh.vertices[equatorBase + Slices].tangent);
	CHECK_EQ(mesh.vertices[equatorBase].pos.y, 0.0f);
	CHECK_EQ(mesh.vertices[equatorBase].normal.y, 0.0f);
	CHECK_EQ(mesh.vertices[equatorBase].tex, Float2{ 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[equatorBase + Slices].tex, Float2{ 1.0f, 1.0f });
	CHECK(mesh.vertices[equatorBase + 1].pos.z > 0.0f);
	CHECK(mesh.vertices[equatorBase + 1].tex.x > mesh.vertices[equatorBase].tex.x);
	CHECK(mesh.vertices[equatorBase + Slices / 4].pos.x == doctest::Approx(0.0f).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[equatorBase + Slices / 4].pos.y, 0.0f);
	CHECK(mesh.vertices[equatorBase + Slices / 4].pos.z == doctest::Approx(Radius).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[equatorBase + Slices / 4].tex, Float2{ 0.25f, 1.0f });

	const Mesh3D explicitOpenMesh = Mesh3D::Hemisphere(Radius, CloseBottom::No, Slices, Stacks);
	CHECK_EQ(explicitOpenMesh.vertexCount(), mesh.vertexCount());
	CHECK_EQ(explicitOpenMesh.triangleCount(), mesh.triangleCount());

	const Mesh3D closedMesh = Mesh3D::Hemisphere(Radius, CloseBottom::Yes, Slices, Stacks);
	CHECK_EQ(closedMesh.vertexCount(), (expectedVertexCount + Slices + 1));
	CHECK_EQ(closedMesh.triangleCount(), (expectedTriangleCount + Slices));
	CheckMeshGeometry(closedMesh);

	for (size_t i = 0; i < mesh.vertices.size(); ++i)
	{
		CHECK_EQ(closedMesh.vertices[i].pos, mesh.vertices[i].pos);
		CHECK_EQ(closedMesh.vertices[i].normal, mesh.vertices[i].normal);
		CHECK_EQ(closedMesh.vertices[i].tex, mesh.vertices[i].tex);
		CHECK_EQ(closedMesh.vertices[i].tangent, mesh.vertices[i].tangent);
	}

	const size_t bottomCenterIndex = expectedVertexCount;
	const size_t bottomRingBase = (bottomCenterIndex + 1);
	CHECK_EQ(closedMesh.vertices[bottomCenterIndex].pos, Float3::Zero());
	CHECK_EQ(closedMesh.vertices[bottomCenterIndex].normal, -Float3::UnitY());
	CHECK_EQ(closedMesh.vertices[bottomCenterIndex].tex, Float2{ 0.5f, 0.5f });
	CHECK_EQ(closedMesh.vertices[bottomCenterIndex].tangent, Float4{ 1.0f, 0.0f, 0.0f, 1.0f });
	CHECK_EQ(closedMesh.vertices[bottomRingBase].pos, Float3{ Radius, 0.0f, 0.0f });
	CHECK_EQ(closedMesh.vertices[bottomRingBase].tex, Float2{ 1.0f, 0.5f });

	for (size_t i = bottomRingBase; i < closedMesh.vertices.size(); ++i)
	{
		CHECK_EQ(closedMesh.vertices[i].pos.y, 0.0f);
		CHECK_EQ(closedMesh.vertices[i].normal, -Float3::UnitY());
		CHECK_EQ(closedMesh.vertices[i].tangent, Float4{ 1.0f, 0.0f, 0.0f, 1.0f });
		CHECK((0.0f <= closedMesh.vertices[i].tex.x && closedMesh.vertices[i].tex.x <= 1.0f));
		CHECK((0.0f <= closedMesh.vertices[i].tex.y && closedMesh.vertices[i].tex.y <= 1.0f));
	}

	CheckMeshGeometry(Mesh3D::Hemisphere(1.0, 3, 1));
	CheckMeshGeometry(Mesh3D::Hemisphere(1.0, CloseBottom::Yes, 3, 1));
	CHECK(Mesh3D::Hemisphere(0.0, Slices, Stacks).isEmpty());
	CHECK(Mesh3D::Hemisphere(std::numeric_limits<double>::infinity(), Slices, Stacks).isEmpty());
	CHECK(Mesh3D::Hemisphere(1.0, 2, Stacks).isEmpty());
	CHECK(Mesh3D::Hemisphere(1.0, Slices, 0).isEmpty());
	CHECK(Mesh3D::Hemisphere(1.0, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
	CHECK(Mesh3D::Hemisphere(1.0, CloseBottom::Yes, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Disc")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Disc(2.0, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ Segments + 1 });
	CHECK_EQ(mesh.triangleCount(), size_t{ Segments });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3::Zero());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.5f, 0.5f });

	for (size_t i = 1; i < mesh.vertices.size(); ++i)
	{
		CHECK(mesh.vertices[i].pos.length() == doctest::Approx(2.0f).epsilon(FrameEpsilon));
	}

	CheckMeshGeometry(Mesh3D::Disc(1.0, 3));
	CHECK(Mesh3D::Disc(0.0, Segments).isEmpty());
	CHECK(Mesh3D::Disc(1.0, 2).isEmpty());
	CHECK(Mesh3D::Disc(1.0, std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Annulus")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Annulus(1.0, 2.0, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ Segments * 2 });
	CHECK_EQ(mesh.triangleCount(), size_t{ Segments * 2 });
	CheckMeshGeometry(mesh);

	for (size_t i = 0; i < Segments; ++i)
	{
		CHECK(mesh.vertices[i].pos.length() == doctest::Approx(2.0f).epsilon(FrameEpsilon));
		CHECK(mesh.vertices[Segments + i].pos.length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
	}

	CheckMeshGeometry(Mesh3D::Annulus(0.5, 1.0, 3));
	CHECK(Mesh3D::Annulus(-1.0, 2.0, Segments).isEmpty());
	CHECK(Mesh3D::Annulus(2.0, 2.0, Segments).isEmpty());
	CHECK(Mesh3D::Annulus(2.0, 1.0, Segments).isEmpty());
	CHECK(Mesh3D::Annulus(1.0, 2.0, 2).isEmpty());
}

TEST_CASE("Mesh3D::HollowCylinder")
{
	constexpr double InnerRadius = 1.0;
	constexpr double OuterRadius = 2.0;
	constexpr double Height = 4.0;
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::HollowCylinder(InnerRadius, OuterRadius, Height, Segments);
	const size_t ringStride = (Segments + 1);
	const size_t outerTopBase = 0;
	const size_t outerBottomBase = ringStride;
	const size_t innerTopBase = (ringStride * 2);
	const size_t innerBottomBase = (ringStride * 3);
	const size_t topOuterBase = (ringStride * 4);
	const size_t topInnerBase = (topOuterBase + Segments);
	const size_t bottomOuterBase = (topInnerBase + Segments);
	const size_t bottomInnerBase = (bottomOuterBase + Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (8 * Segments) + 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 8 * Segments });
	CheckMeshGeometry(mesh);

	CHECK_EQ(mesh.vertices[outerTopBase].pos, Float3{ 2.0f, 2.0f, 0.0f });
	CHECK_EQ(mesh.vertices[outerTopBase].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[outerTopBase].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[outerTopBase].tangent, Float4{ 0.0f, 0.0f, 1.0f, 1.0f });
	CHECK_EQ(mesh.vertices[outerTopBase + Segments].pos, mesh.vertices[outerTopBase].pos);
	CHECK_EQ(mesh.vertices[outerTopBase + Segments].normal, mesh.vertices[outerTopBase].normal);
	CHECK_EQ(mesh.vertices[outerTopBase + Segments].tangent, mesh.vertices[outerTopBase].tangent);
	CHECK_EQ(mesh.vertices[outerTopBase + Segments].tex, Float2{ 1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[outerBottomBase].tex, Float2{ 0.0f, 1.0f });

	CHECK_EQ(mesh.vertices[innerTopBase].pos, Float3{ 1.0f, 2.0f, 0.0f });
	CHECK_EQ(mesh.vertices[innerTopBase].normal, -Float3::UnitX());
	CHECK_EQ(mesh.vertices[innerTopBase].tex, Float2{ 0.0f, 0.0f });
	CHECK_EQ(mesh.vertices[innerTopBase].tangent, Float4{ 0.0f, 0.0f, -1.0f, 1.0f });
	CHECK_EQ(mesh.vertices[innerTopBase + Segments].pos, mesh.vertices[innerTopBase].pos);
	CHECK_EQ(mesh.vertices[innerTopBase + Segments].normal, mesh.vertices[innerTopBase].normal);
	CHECK_EQ(mesh.vertices[innerTopBase + Segments].tangent, mesh.vertices[innerTopBase].tangent);
	CHECK_EQ(mesh.vertices[innerTopBase + Segments].tex, Float2{ 1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[innerBottomBase].tex, Float2{ 0.0f, 1.0f });

	for (size_t i = 0; i <= Segments; ++i)
	{
		const Vertex3D& outer = mesh.vertices[outerTopBase + i];
		const Vertex3D& inner = mesh.vertices[innerTopBase + i];
		CHECK(inner.pos.x == doctest::Approx(outer.pos.x * 0.5f).epsilon(FrameEpsilon));
		CHECK(inner.pos.z == doctest::Approx(outer.pos.z * -0.5f).epsilon(FrameEpsilon));
		CHECK_EQ(inner.tex.x, outer.tex.x);
	}

	CHECK_EQ(mesh.vertices[topOuterBase].tex, Float2{ 1.0f, 0.5f });
	CHECK_EQ(mesh.vertices[topInnerBase].tex, Float2{ 0.75f, 0.5f });
	CHECK_EQ(mesh.vertices[bottomOuterBase].tex, Float2{ 1.0f, 0.5f });
	CHECK_EQ(mesh.vertices[bottomInnerBase].tex, Float2{ 0.75f, 0.5f });

	CheckMeshGeometry(Mesh3D::HollowCylinder(0.5, 1.0, 1.0, 3));
	CHECK(Mesh3D::HollowCylinder(0.0, OuterRadius, Height, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(-1.0, OuterRadius, Height, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(OuterRadius, OuterRadius, Height, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(3.0, OuterRadius, Height, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(InnerRadius, OuterRadius, 0.0, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(InnerRadius, OuterRadius, Height, 2).isEmpty());
	CHECK(Mesh3D::HollowCylinder(InnerRadius, std::numeric_limits<double>::infinity(), Height, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(InnerRadius, std::numeric_limits<double>::max(), Height, Segments).isEmpty());
	CHECK(Mesh3D::HollowCylinder(InnerRadius, OuterRadius, Height, std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Torus")
{
	constexpr double MajorRadius = 3.0;
	constexpr double TubeRadius = 1.0;
	constexpr uint32 RingSegments = 8;
	constexpr uint32 TubeSegments = 4;
	const Mesh3D mesh = Mesh3D::Torus(MajorRadius, TubeRadius, RingSegments, TubeSegments);
	const size_t ringStride = (RingSegments + 1);

	CHECK_EQ(mesh.vertexCount(), size_t{ (RingSegments + 1) * (TubeSegments + 1) });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 * RingSegments * TubeSegments });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ MajorRadius, TubeRadius, 0.0f });
	CHECK_EQ(mesh.vertices[RingSegments].pos, mesh.vertices[0].pos);
	CHECK_EQ(mesh.vertices[RingSegments].normal, mesh.vertices[0].normal);
	CHECK_EQ(mesh.vertices[RingSegments].tangent, mesh.vertices[0].tangent);
	CHECK_EQ(mesh.vertices[RingSegments].tex, Float2{ 1.0f, 0.0f });

	const size_t lastTubeRow = (TubeSegments * ringStride);
	CHECK_EQ(mesh.vertices[lastTubeRow].pos, mesh.vertices[0].pos);
	CHECK_EQ(mesh.vertices[lastTubeRow].normal, mesh.vertices[0].normal);
	CHECK_EQ(mesh.vertices[lastTubeRow].tangent, mesh.vertices[0].tangent);
	CHECK_EQ(mesh.vertices[lastTubeRow].tex, Float2{ 0.0f, 1.0f });

	for (const auto& vertex : mesh.vertices)
	{
		const float radialDistance = std::sqrt((vertex.pos.x * vertex.pos.x) + (vertex.pos.z * vertex.pos.z));
		const double profileDistance = std::hypot((radialDistance - MajorRadius), vertex.pos.y);
		CHECK(profileDistance == doctest::Approx(TubeRadius).epsilon(FrameEpsilon));
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	CheckMeshGeometry(Mesh3D::Torus(2.0, 0.5, 3, 3));
	CHECK(Mesh3D::Torus(0.0, TubeRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, 0.0, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, MajorRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(1.0, 2.0, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, TubeRadius, 2, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, TubeRadius, RingSegments, 2).isEmpty());
	CHECK(Mesh3D::Torus(std::numeric_limits<double>::infinity(), TubeRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(std::numeric_limits<double>::max(), TubeRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, TubeRadius, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Capsule")
{
	constexpr double Radius = 1.0;
	constexpr double CylinderHeight = 2.0;
	constexpr uint32 Slices = 8;
	constexpr uint32 HemisphereStacks = 2;
	const Mesh3D mesh = Mesh3D::Capsule(Radius, CylinderHeight, Slices, HemisphereStacks);
	const size_t interiorRingCount = (HemisphereStacks * 2);
	const size_t ringStride = (Slices + 1);
	const size_t firstRingBase = Slices;

	CHECK_EQ(mesh.vertexCount(), ((interiorRingCount * ringStride) + (2 * Slices)));
	CHECK_EQ(mesh.triangleCount(), (2 * Slices * interiorRingCount));
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 0.0f, 2.0f, 0.0f });

	for (size_t ring = 0; ring < interiorRingCount; ++ring)
	{
		const size_t ringBase = (firstRingBase + (ring * ringStride));
		CHECK_EQ(mesh.vertices[ringBase].pos, mesh.vertices[ringBase + Slices].pos);
		CHECK_EQ(mesh.vertices[ringBase].normal, mesh.vertices[ringBase + Slices].normal);
		CHECK_EQ(mesh.vertices[ringBase].tangent, mesh.vertices[ringBase + Slices].tangent);
		CHECK_EQ(mesh.vertices[ringBase].tex.x, 0.0f);
		CHECK_EQ(mesh.vertices[ringBase + Slices].tex.x, 1.0f);
	}

	const float halfCylinderHeight = static_cast<float>(CylinderHeight * 0.5);
	for (const auto& vertex : mesh.vertices)
	{
		const float closestY = ((vertex.pos.y < -halfCylinderHeight) ? -halfCylinderHeight
			: ((halfCylinderHeight < vertex.pos.y) ? halfCylinderHeight : vertex.pos.y));
		const Float3 closestAxisPoint{ 0.0f, closestY, 0.0f };
		CHECK(vertex.pos.distanceFrom(closestAxisPoint) == doctest::Approx(Radius).epsilon(FrameEpsilon));
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	SUBCASE("Zero cylinder height")
	{
		const Mesh3D capsule = Mesh3D::Capsule(Radius, 0.0, Slices, HemisphereStacks);
		const Mesh3D sphere = Mesh3D::Sphere(Radius, Slices, (HemisphereStacks * 2));
		REQUIRE_EQ(capsule.vertexCount(), sphere.vertexCount());
		REQUIRE_EQ(capsule.triangleCount(), sphere.triangleCount());

		for (size_t i = 0; i < capsule.vertexCount(); ++i)
		{
			CHECK_EQ(capsule.vertices[i].pos, sphere.vertices[i].pos);
			CHECK_EQ(capsule.vertices[i].normal, sphere.vertices[i].normal);
			CHECK_EQ(capsule.vertices[i].tex, sphere.vertices[i].tex);
			CHECK_EQ(capsule.vertices[i].tangent, sphere.vertices[i].tangent);
		}

		for (size_t i = 0; i < capsule.triangleCount(); ++i)
		{
			CHECK_EQ(capsule.indices[i].i0, sphere.indices[i].i0);
			CHECK_EQ(capsule.indices[i].i1, sphere.indices[i].i1);
			CHECK_EQ(capsule.indices[i].i2, sphere.indices[i].i2);
		}
	}

	CheckMeshGeometry(Mesh3D::Capsule(1.0, 1.0, 3, 1));
	CHECK(Mesh3D::Capsule(0.0, CylinderHeight, Slices, HemisphereStacks).isEmpty());
	CHECK(Mesh3D::Capsule(Radius, -1.0, Slices, HemisphereStacks).isEmpty());
	CHECK(Mesh3D::Capsule(Radius, CylinderHeight, 2, HemisphereStacks).isEmpty());
	CHECK(Mesh3D::Capsule(Radius, CylinderHeight, Slices, 0).isEmpty());
	CHECK(Mesh3D::Capsule(Radius, std::numeric_limits<double>::infinity(), Slices, HemisphereStacks).isEmpty());
	CHECK(Mesh3D::Capsule(Radius, CylinderHeight, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::ConicalFrustum")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::ConicalFrustum(2.0, 1.0, 4.0, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (4 * Segments) + 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Segments });
	CheckMeshGeometry(mesh);

	for (const auto& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.y && vertex.pos.y <= 2.0f));
		CHECK(std::sqrt((vertex.pos.x * vertex.pos.x) + (vertex.pos.z * vertex.pos.z)) <= (2.0f + FrameEpsilon));
	}

	const Mesh3D invertedFrustum = Mesh3D::ConicalFrustum(1.0, 2.0, 4.0, Segments);
	CHECK_EQ(invertedFrustum.vertexCount(), mesh.vertexCount());
	CheckMeshGeometry(invertedFrustum);
	CheckMeshGeometry(Mesh3D::ConicalFrustum(1.0, 0.5, 1.0, 3));

	CHECK(Mesh3D::ConicalFrustum(0.0, 1.0, 1.0, Segments).isEmpty());
	CHECK(Mesh3D::ConicalFrustum(1.0, -1.0, 1.0, Segments).isEmpty());
	CHECK(Mesh3D::ConicalFrustum(1.0, 1.0, 0.0, Segments).isEmpty());
	CHECK(Mesh3D::ConicalFrustum(1.0, 1.0, 1.0, 2).isEmpty());
	CHECK(Mesh3D::ConicalFrustum(1.0, 1.0, 1.0, std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Cylinder")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Cylinder(2.0, 4.0, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (4 * Segments) + 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Segments });
	CheckMeshGeometry(mesh);

	CHECK(Mesh3D::Cylinder(0.0, 1.0, Segments).isEmpty());
	CHECK(Mesh3D::Cylinder(1.0, -1.0, Segments).isEmpty());
}

TEST_CASE("Mesh3D::Cone")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Cone(2.0, 4.0, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (3 * Segments) + 2 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 * Segments });
	CheckMeshGeometry(mesh);

	for (size_t i = 0; i < Segments; ++i)
	{
		CHECK_EQ(mesh.vertices[i].pos, mesh.vertices[0].pos);
		CHECK_EQ(mesh.vertices[i].tex,
			Float2{ ((static_cast<float>(i) + 0.5f) / static_cast<float>(Segments)), 0.0f });
		CHECK_EQ(mesh.vertices[i].tangent.w, 1.0f);
	}

	constexpr size_t BottomSideBase = Segments;
	for (size_t i = 0; i <= Segments; ++i)
	{
		CHECK_EQ(mesh.vertices[BottomSideBase + i].tex,
			Float2{ (static_cast<float>(i) / static_cast<float>(Segments)), 1.0f });
		CHECK_EQ(mesh.vertices[BottomSideBase + i].tangent.w, 1.0f);
	}
	CHECK_EQ(mesh.vertices[BottomSideBase].pos, mesh.vertices[BottomSideBase + Segments].pos);
	CHECK_EQ(mesh.vertices[BottomSideBase].tex, Float2{ 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[BottomSideBase + Segments].tex, Float2{ 1.0f, 1.0f });

	CheckMeshGeometry(Mesh3D::Cone(1.0, 1.0, 3));
	CHECK(Mesh3D::Cone(0.0, 1.0, Segments).isEmpty());
	CHECK(Mesh3D::Cone(1.0, 0.0, Segments).isEmpty());
}
