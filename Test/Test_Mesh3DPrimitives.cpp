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
	const Mesh3D mesh = Mesh3D::Box(Float3{ 2.0f, 4.0f, 6.0f }, uvMapping);

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
		CHECK(Mesh3D::Box(Float3{ 1.0f, 1.0f, 1.0f }, invalid).isEmpty());

		invalid = uvMapping;
		invalid.positiveY.bottom = std::numeric_limits<float>::infinity();
		CHECK(Mesh3D::Box(Float3{ 1.0f, 1.0f, 1.0f }, invalid).isEmpty());
	}

	CHECK(Mesh3D::Box(Float3{ 0.0f, 1.0f, 1.0f }, uvMapping).isEmpty());
}

TEST_CASE("Mesh3D::Box with flipped UV mapping")
{
	BoxUVMapping uvMapping;
	uvMapping.negativeZ = FloatRect{ 1.0f, 0.0f, 0.0f, 1.0f };
	uvMapping.positiveZ = FloatRect{ 0.0f, 1.0f, 1.0f, 0.0f };
	uvMapping.positiveX = FloatRect{ 1.0f, 1.0f, 0.0f, 0.0f };

	const Mesh3D defaultMesh = Mesh3D::Box();
	const Mesh3D flippedMesh = Mesh3D::Box(Float3{ 1.0f, 1.0f, 1.0f }, uvMapping);
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
	const Mesh3D collapsedMesh = Mesh3D::Box(Float3{ 1.0f, 1.0f, 1.0f }, uvMapping);
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

TEST_CASE("Mesh3D::Plane")
{
	const Float2 uvScale{ 2.0f, 3.0f };
	const Float2 uvOffset{ -0.25f, 0.5f };
	const Mesh3D mesh = Mesh3D::Plane(Float2{ 4.0f, 2.0f }, uvScale, uvOffset);

	CHECK_EQ(mesh.vertexCount(), size_t{ 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[3].pos, Float3{ 2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].tex, uvOffset);
	CHECK_EQ(mesh.vertices[3].tex, (uvOffset + uvScale));

	CHECK(Mesh3D::Plane(Float2{ -1.0f, 1.0f }).isEmpty());
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

TEST_CASE("Mesh3D::Disc")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Disc(2.0f, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ Segments + 1 });
	CHECK_EQ(mesh.triangleCount(), size_t{ Segments });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3::Zero());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.5f, 0.5f });

	for (size_t i = 1; i < mesh.vertices.size(); ++i)
	{
		CHECK(mesh.vertices[i].pos.length() == doctest::Approx(2.0f).epsilon(FrameEpsilon));
	}

	CheckMeshGeometry(Mesh3D::Disc(1.0f, 3));
	CHECK(Mesh3D::Disc(0.0f, Segments).isEmpty());
	CHECK(Mesh3D::Disc(1.0f, 2).isEmpty());
	CHECK(Mesh3D::Disc(1.0f, std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Annulus")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Annulus(1.0f, 2.0f, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ Segments * 2 });
	CHECK_EQ(mesh.triangleCount(), size_t{ Segments * 2 });
	CheckMeshGeometry(mesh);

	for (size_t i = 0; i < Segments; ++i)
	{
		CHECK(mesh.vertices[i].pos.length() == doctest::Approx(2.0f).epsilon(FrameEpsilon));
		CHECK(mesh.vertices[Segments + i].pos.length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
	}

	CheckMeshGeometry(Mesh3D::Annulus(0.5f, 1.0f, 3));
	CHECK(Mesh3D::Annulus(-1.0f, 2.0f, Segments).isEmpty());
	CHECK(Mesh3D::Annulus(2.0f, 2.0f, Segments).isEmpty());
	CHECK(Mesh3D::Annulus(2.0f, 1.0f, Segments).isEmpty());
	CHECK(Mesh3D::Annulus(1.0f, 2.0f, 2).isEmpty());
}

TEST_CASE("Mesh3D::Torus")
{
	constexpr float MajorRadius = 3.0f;
	constexpr float TubeRadius = 1.0f;
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
		const float profileDistance = std::hypot((radialDistance - MajorRadius), vertex.pos.y);
		CHECK(profileDistance == doctest::Approx(TubeRadius).epsilon(FrameEpsilon));
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	CheckMeshGeometry(Mesh3D::Torus(2.0f, 0.5f, 3, 3));
	CHECK(Mesh3D::Torus(0.0f, TubeRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, 0.0f, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, MajorRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(1.0f, 2.0f, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, TubeRadius, 2, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, TubeRadius, RingSegments, 2).isEmpty());
	CHECK(Mesh3D::Torus(std::numeric_limits<float>::infinity(), TubeRadius, RingSegments, TubeSegments).isEmpty());
	CHECK(Mesh3D::Torus(MajorRadius, TubeRadius, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Frustum")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Frustum(2.0f, 1.0f, 4.0f, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (4 * Segments) + 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Segments });
	CheckMeshGeometry(mesh);

	for (const auto& vertex : mesh.vertices)
	{
		CHECK((-2.0f <= vertex.pos.y && vertex.pos.y <= 2.0f));
		CHECK(std::sqrt((vertex.pos.x * vertex.pos.x) + (vertex.pos.z * vertex.pos.z)) <= (2.0f + FrameEpsilon));
	}

	const Mesh3D invertedFrustum = Mesh3D::Frustum(1.0f, 2.0f, 4.0f, Segments);
	CHECK_EQ(invertedFrustum.vertexCount(), mesh.vertexCount());
	CheckMeshGeometry(invertedFrustum);
	CheckMeshGeometry(Mesh3D::Frustum(1.0f, 0.5f, 1.0f, 3));

	CHECK(Mesh3D::Frustum(0.0f, 1.0f, 1.0f, Segments).isEmpty());
	CHECK(Mesh3D::Frustum(1.0f, -1.0f, 1.0f, Segments).isEmpty());
	CHECK(Mesh3D::Frustum(1.0f, 1.0f, 0.0f, Segments).isEmpty());
	CHECK(Mesh3D::Frustum(1.0f, 1.0f, 1.0f, 2).isEmpty());
	CHECK(Mesh3D::Frustum(1.0f, 1.0f, 1.0f, std::numeric_limits<uint32>::max()).isEmpty());
}

TEST_CASE("Mesh3D::Cylinder")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Cylinder(2.0f, 4.0f, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (4 * Segments) + 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Segments });
	CheckMeshGeometry(mesh);

	CHECK(Mesh3D::Cylinder(0.0f, 1.0f, Segments).isEmpty());
	CHECK(Mesh3D::Cylinder(1.0f, -1.0f, Segments).isEmpty());
}

TEST_CASE("Mesh3D::Cone")
{
	constexpr uint32 Segments = 8;
	const Mesh3D mesh = Mesh3D::Cone(2.0f, 4.0f, Segments);

	CHECK_EQ(mesh.vertexCount(), size_t{ (3 * Segments) + 2 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 * Segments });
	CheckMeshGeometry(mesh);

	for (size_t i = 1; i < Segments; ++i)
	{
		CHECK_EQ(mesh.vertices[i].pos, mesh.vertices[0].pos);
		CHECK_NE(mesh.vertices[i].tangent, mesh.vertices[0].tangent);
	}

	CheckMeshGeometry(Mesh3D::Cone(1.0f, 1.0f, 3));
	CHECK(Mesh3D::Cone(0.0f, 1.0f, Segments).isEmpty());
	CHECK(Mesh3D::Cone(1.0f, 0.0f, Segments).isEmpty());
}
