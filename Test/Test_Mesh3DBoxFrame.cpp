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
		static_cast<Mesh3D (*)(Vec3, double)>(&Mesh3D::BoxFrame);
		static_cast<Mesh3D (*)(Vec3, Vec3)>(&Mesh3D::BoxFrame);
		static_cast<Mesh3D (*)(Vec3, double, const BoxUVMapping&)>(&Mesh3D::BoxFrame);
		static_cast<Mesh3D (*)(Vec3, Vec3, const BoxUVMapping&)>(&Mesh3D::BoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, Vec3)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, Vec3, const Quaternion&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const Mat4x4&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, Vec3)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, Vec3, const Quaternion&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const Mat4x4&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, Vec3)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, Vec3, const Quaternion&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, double, const BoxUVMapping&, const Mat4x4&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, Vec3)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, Vec3, const Quaternion&)>(&Mesh3DBuilder::addBoxFrame);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(Vec3, Vec3, const BoxUVMapping&, const Mat4x4&)>(&Mesh3DBuilder::addBoxFrame);
	});

	[[nodiscard]]
	bool IsAtCoordinate(const float value, const float expected)
	{
		return (std::abs(std::abs(value) - expected) < 1e-6f);
	}
}

TEST_CASE("Mesh3D::BoxFrame geometry")
{
	constexpr Vec3 Size{ 2.0, 4.0, 6.0 };
	constexpr Vec3 BeamSize{ 0.25, 0.5, 0.75 };
	const Mesh3D mesh = Mesh3D::BoxFrame(Size, BeamSize);

	CHECK_EQ(mesh.vertexCount(), size_t{ 192 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 96 });
	CHECK(mesh.vertexCount() < (12 * Mesh3D::Box().vertexCount()));
	CHECK(mesh.triangleCount() < (12 * Mesh3D::Box().triangleCount()));
	CheckMeshGeometry(mesh);

	constexpr Float3 OuterHalf{ 1.0f, 2.0f, 3.0f };
	constexpr Float3 InnerHalf{ 0.75f, 1.5f, 2.25f };
	for (const Vertex3D& vertex : mesh.vertices)
	{
		const bool validX = (IsAtCoordinate(vertex.pos.x, OuterHalf.x)
			|| IsAtCoordinate(vertex.pos.x, InnerHalf.x));
		const bool validY = (IsAtCoordinate(vertex.pos.y, OuterHalf.y)
			|| IsAtCoordinate(vertex.pos.y, InnerHalf.y));
		const bool validZ = (IsAtCoordinate(vertex.pos.z, OuterHalf.z)
			|| IsAtCoordinate(vertex.pos.z, InnerHalf.z));
		CHECK(validX);
		CHECK(validY);
		CHECK(validZ);
		CHECK((0.0f <= vertex.tex.x && vertex.tex.x <= 1.0f));
		CHECK((0.0f <= vertex.tex.y && vertex.tex.y <= 1.0f));
	}

	for (const TriangleIndex32& triangle : mesh.indices)
	{
		const Float3 center = ((mesh.vertices[triangle.i0].pos
			+ mesh.vertices[triangle.i1].pos
			+ mesh.vertices[triangle.i2].pos) / 3.0f);
		const bool xBorder = (InnerHalf.x < std::abs(center.x));
		const bool yBorder = (InnerHalf.y < std::abs(center.y));
		const bool zBorder = (InnerHalf.z < std::abs(center.z));
		CHECK((static_cast<int32>(xBorder) + static_cast<int32>(yBorder)
			+ static_cast<int32>(zBorder)) >= 1);
	}
}

TEST_CASE("Mesh3D::BoxFrame uniform thickness")
{
	constexpr Vec3 Size{ 2.0, 4.0, 6.0 };
	CheckMeshDataEqual(
		Mesh3D::BoxFrame(Size, 0.25),
		Mesh3D::BoxFrame(Size, Vec3{ 0.25, 0.25, 0.25 }));
}

TEST_CASE("Mesh3D::BoxFrame UV mapping")
{
	const BoxUVMapping uvMapping{
		.negativeZ = FloatRect{ 0.0f, 0.0f, 0.1f, 0.5f },
		.positiveZ = FloatRect{ 0.1f, 0.0f, 0.2f, 0.5f },
		.positiveX = FloatRect{ 0.2f, 0.0f, 0.3f, 0.5f },
		.negativeX = FloatRect{ 0.3f, 0.0f, 0.4f, 0.5f },
		.positiveY = FloatRect{ 0.4f, 0.0f, 0.5f, 0.5f },
		.negativeY = FloatRect{ 0.5f, 0.0f, 0.6f, 0.5f },
	};
	const Mesh3D mesh = Mesh3D::BoxFrame(
		Vec3{ 2.0, 4.0, 6.0 }, Vec3{ 0.25, 0.5, 0.75 }, uvMapping);
	CheckMeshGeometry(mesh);

	for (const Vertex3D& vertex : mesh.vertices)
	{
		FloatRect expected;
		if (vertex.normal == -Float3::UnitZ()) expected = uvMapping.negativeZ;
		else if (vertex.normal == Float3::UnitZ()) expected = uvMapping.positiveZ;
		else if (vertex.normal == Float3::UnitX()) expected = uvMapping.positiveX;
		else if (vertex.normal == -Float3::UnitX()) expected = uvMapping.negativeX;
		else if (vertex.normal == Float3::UnitY()) expected = uvMapping.positiveY;
		else expected = uvMapping.negativeY;

		CHECK((expected.left <= vertex.tex.x && vertex.tex.x <= expected.right));
		CHECK((expected.top <= vertex.tex.y && vertex.tex.y <= expected.bottom));
	}
}

TEST_CASE("Mesh3DBuilder::addBoxFrame transforms and storage")
{
	constexpr Vec3 Size{ 2.0, 4.0, 6.0 };
	constexpr Vec3 BeamSize{ 0.25, 0.5, 0.75 };
	const Vec3 offset{ 3.0, 4.0, 5.0 };
	const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
	const Mat4x4 transform = Mat4x4::AffineTransform(
		Float3{ 2.0f, 3.0f, -4.0f }, rotation, Float3{ offset });

	Mesh3DBuilder builder;
	builder.reserve(576, 288);
	REQUIRE(builder.addBoxFrame(Size, BeamSize));
	const Vertex3D* const vertexData = builder.getMesh().vertices.data();
	const TriangleIndex32* const indexData = builder.getMesh().indices.data();
	REQUIRE(builder.addBoxFrame(Size, BeamSize, offset, rotation));
	REQUIRE(builder.addBoxFrame(Size, BeamSize, BoxUVMapping{}, transform));
	CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
	CHECK_EQ(builder.getMesh().indices.data(), indexData);

	Mesh3D expected = Mesh3D::BoxFrame(Size, BeamSize);
	REQUIRE(expected.append(
		Mesh3D::BoxFrame(Size, BeamSize),
		Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
	REQUIRE(expected.append(Mesh3D::BoxFrame(Size, BeamSize), transform));
	CheckMeshDataEqual(builder.getMesh(), expected);
}

TEST_CASE("Mesh3DBuilder::addBoxFrame failure is atomic")
{
	Mesh3DBuilder builder;
	REQUIRE(builder.addBox());
	const Mesh3D expected = builder.getMesh();

	CHECK_FALSE(builder.addBoxFrame(Vec3{ 0.0, 2.0, 2.0 }, 0.1));
	CHECK_FALSE(builder.addBoxFrame(Vec3{ 2.0, 2.0, 2.0 }, 0.0));
	CHECK_FALSE(builder.addBoxFrame(Vec3{ 2.0, 2.0, 2.0 }, 1.0));
	CHECK_FALSE(builder.addBoxFrame(Vec3{ 2.0, 2.0, 2.0 }, Vec3{ 0.1, -0.1, 0.1 }));
	CHECK_FALSE(builder.addBoxFrame(
		Vec3{ 2.0, 2.0, 2.0 }, std::numeric_limits<double>::infinity()));

	BoxUVMapping invalidMapping;
	invalidMapping.positiveZ.left = std::numeric_limits<float>::quiet_NaN();
	CHECK_FALSE(builder.addBoxFrame(Vec3{ 2.0, 2.0, 2.0 }, 0.1, invalidMapping));
	CheckMeshDataEqual(builder.getMesh(), expected);
}
