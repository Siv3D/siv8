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
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(Vec3, double, BoxFace)>(&Mesh3D::BoxShell);
		static_cast<Mesh3D (*)(Vec3, Vec3, BoxFace)>(&Mesh3D::BoxShell);
		static_cast<Mesh3D (*)(Vec3, double, const BoxUVMapping&, BoxFace)>(&Mesh3D::BoxShell);
		static_cast<Mesh3D (*)(Vec3, Vec3, const BoxUVMapping&, BoxFace)>(&Mesh3D::BoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, Vec3, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, Vec3, const Quaternion&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const Mat4x4&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, Vec3, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, Vec3, const Quaternion&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const Mat4x4&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, Vec3, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, Vec3, const Quaternion&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, const Mat4x4&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, Vec3, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, Vec3, const Quaternion&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, const Mat4x4&, BoxFace)>(&Mesh3DBuilder::addBoxShell);
	});
}

TEST_CASE("Mesh3D::BoxShell closed")
{
	const Mesh3D mesh = Mesh3D::BoxShell(Vec3{ 2.0, 4.0, 6.0 }, 0.25);

	CHECK_EQ(mesh.vertexCount(), size_t{ 48 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 24 });
	CheckMeshGeometry(mesh);

	for (const Vertex3D& vertex : mesh.vertices)
	{
		CHECK(std::abs(vertex.pos.x) <= 1.0f);
		CHECK(std::abs(vertex.pos.y) <= 2.0f);
		CHECK(std::abs(vertex.pos.z) <= 3.0f);
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}
}

TEST_CASE("Mesh3D::BoxShell one opening")
{
	constexpr Vec3 OuterSize{ 2.0, 4.0, 6.0 };
	constexpr Vec3 Thickness{ 0.25, 0.5, 0.75 };
	const Mesh3D mesh = Mesh3D::BoxShell(OuterSize, Thickness, BoxFace::PositiveZ);

	CHECK_EQ(mesh.vertexCount(), size_t{ 56 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 28 });
	CheckMeshGeometry(mesh);

	size_t openingRimVertexCount = 0;
	bool innerTopReachesOpening = false;
	for (const Vertex3D& vertex : mesh.vertices)
	{
		if ((vertex.normal == Float3::UnitZ()) && (vertex.pos.z == 3.0f))
		{
			++openingRimVertexCount;
			const bool isOnOpeningRim = ((0.75f <= std::abs(vertex.pos.x))
				|| (1.5f <= std::abs(vertex.pos.y)));
			CHECK(isOnOpeningRim);
		}

		if ((vertex.normal == -Float3::UnitY())
			&& (vertex.pos.y == 1.5f)
			&& (vertex.pos.z == 3.0f))
		{
			innerTopReachesOpening = true;
		}
	}

	CHECK_EQ(openingRimVertexCount, size_t{ 16 });
	CHECK(innerTopReachesOpening);
}

TEST_CASE("Mesh3D::BoxShell multiple openings")
{
	SUBCASE("Adjacent openings")
	{
		const Mesh3D mesh = Mesh3D::BoxShell(
			Vec3{ 2.0, 4.0, 6.0 }, 0.25,
			(BoxFace::PositiveX | BoxFace::PositiveZ));
		CHECK_EQ(mesh.vertexCount(), size_t{ 56 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 28 });
		CheckMeshGeometry(mesh);
	}

	SUBCASE("Opposite openings form a tube")
	{
		const Mesh3D mesh = Mesh3D::BoxShell(
			Vec3{ 2.0, 4.0, 6.0 }, 0.25,
			(BoxFace::NegativeZ | BoxFace::PositiveZ));
		CHECK_EQ(mesh.vertexCount(), size_t{ 64 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 32 });
		CheckMeshGeometry(mesh);
	}

	SUBCASE("One remaining wall")
	{
		const BoxFace openFaces = static_cast<BoxFace>(
			static_cast<uint8>(BoxFace::All) & ~static_cast<uint8>(BoxFace::PositiveY));
		const Mesh3D mesh = Mesh3D::BoxShell(Vec3{ 2.0, 4.0, 6.0 }, 0.25, openFaces);
		CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
		CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
		CheckMeshGeometry(mesh);
	}
}

TEST_CASE("Mesh3D::BoxShell UV mapping")
{
	BoxUVMapping uvMapping;
	uvMapping.positiveZ = FloatRect{ 0.2f, 0.3f, 0.6f, 0.9f };
	const Mesh3D mesh = Mesh3D::BoxShell(
		Vec3{ 2.0, 4.0, 6.0 }, 0.25, uvMapping, BoxFace::PositiveZ);
	CheckMeshGeometry(mesh);

	for (const Vertex3D& vertex : mesh.vertices)
	{
		if ((vertex.normal == Float3::UnitZ()) && (vertex.pos.z == 3.0f))
		{
			CHECK((0.2f <= vertex.tex.x && vertex.tex.x <= 0.6f));
			CHECK((0.3f <= vertex.tex.y && vertex.tex.y <= 0.9f));
		}
	}

	uvMapping.negativeX.left = std::numeric_limits<float>::quiet_NaN();
	CHECK(Mesh3D::BoxShell(
		Vec3{ 2.0, 4.0, 6.0 }, 0.25, uvMapping, BoxFace::All).isEmpty());
	CHECK(Mesh3D::BoxShell(
		Vec3{ 2.0, 4.0, 6.0 }, 0.25, uvMapping, BoxFace::PositiveX).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addBoxShell transforms and storage")
{
	constexpr Vec3 OuterSize{ 2.0, 4.0, 6.0 };
	constexpr Vec3 Thickness{ 0.25, 0.5, 0.75 };
	const BoxFace openFaces = (BoxFace::NegativeX | BoxFace::PositiveZ);
	const Vec3 offset{ 3.0, 4.0, 5.0 };
	const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
	const Mat4x4 transform = Mat4x4::AffineTransform(
		Float3{ 2.0f, 3.0f, -4.0f }, rotation, Float3{ offset });

	Mesh3DBuilder builder;
	builder.reserve(168, 84);
	REQUIRE(builder.addBoxShell(OuterSize, Thickness, openFaces));
	const Vertex3D* const vertexData = builder.getMesh().vertices.data();
	const TriangleIndex32* const indexData = builder.getMesh().indices.data();
	REQUIRE(builder.addBoxShell(OuterSize, Thickness, offset, rotation, openFaces));
	REQUIRE(builder.addBoxShell(OuterSize, Thickness, BoxUVMapping{}, transform, openFaces));
	CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
	CHECK_EQ(builder.getMesh().indices.data(), indexData);

	Mesh3D expected = Mesh3D::BoxShell(OuterSize, Thickness, openFaces);
	REQUIRE(expected.append(
		Mesh3D::BoxShell(OuterSize, Thickness, openFaces),
		Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
	REQUIRE(expected.append(Mesh3D::BoxShell(OuterSize, Thickness, openFaces), transform));
	CheckMeshDataEqual(builder.getMesh(), expected);
}

TEST_CASE("Mesh3DBuilder::addBoxShell failure is atomic")
{
	Mesh3DBuilder builder;
	REQUIRE(builder.addBox());
	const Mesh3D expected = builder.getMesh();

	CHECK_FALSE(builder.addBoxShell(Vec3{ 0.0, 2.0, 2.0 }, 0.1));
	CHECK_FALSE(builder.addBoxShell(Vec3{ 2.0, 2.0, 2.0 }, 0.0));
	CHECK_FALSE(builder.addBoxShell(Vec3{ 2.0, 2.0, 2.0 }, 1.0));
	CHECK_FALSE(builder.addBoxShell(Vec3{ 2.0, 2.0, 2.0 }, Vec3{ 0.1, -0.1, 0.1 }));
	CHECK_FALSE(builder.addBoxShell(
		Vec3{ 2.0, 2.0, 2.0 }, std::numeric_limits<double>::infinity()));
	CHECK_FALSE(builder.addBoxShell(
		Vec3{ 2.0, 2.0, 2.0 }, 0.1, static_cast<BoxFace>(0x80)));

	BoxUVMapping invalidMapping;
	invalidMapping.positiveZ.left = std::numeric_limits<float>::quiet_NaN();
	CHECK_FALSE(builder.addBoxShell(
		Vec3{ 2.0, 2.0, 2.0 }, 0.1, invalidMapping, BoxFace::PositiveZ));
	CHECK(builder.addBoxShell(
		Vec3{ 2.0, 2.0, 2.0 }, 0.1, invalidMapping, BoxFace::All));
	CheckMeshDataEqual(builder.getMesh(), expected);
}
