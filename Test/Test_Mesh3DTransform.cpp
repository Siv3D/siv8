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
	void CheckOrientation(const Mesh3D& actual, const Mesh3D& source, const bool mirrored)
	{
		REQUIRE_EQ(actual.triangleCount(), source.triangleCount());
		for (size_t i = 0; i < source.triangleCount(); ++i)
		{
			CHECK_EQ(actual.indices[i].i0, source.indices[i].i0);
			CHECK_EQ(actual.indices[i].i1, mirrored ? source.indices[i].i2 : source.indices[i].i1);
			CHECK_EQ(actual.indices[i].i2, mirrored ? source.indices[i].i1 : source.indices[i].i2);
		}
		Mesh3DTest::CheckMeshGeometry(actual);
	}

	void CheckAttributesNear(const Mesh3D& actual, const Mesh3D& expected)
	{
		REQUIRE_EQ(actual.vertexCount(), expected.vertexCount());
		for (size_t i = 0; i < actual.vertexCount(); ++i)
		{
			const auto& a = actual.vertices[i];
			const auto& b = expected.vertices[i];
			CHECK((a.pos - b.pos).length() < 1e-5f);
			CHECK((a.normal - b.normal).length() < 1e-5f);
			CHECK((a.tangent.xyz() - b.tangent.xyz()).length() < 1e-5f);
			CHECK_EQ(a.tangent.w, b.tangent.w);
			CHECK_EQ(a.tex, b.tex);
		}
	}

	template <class Add>
	void CheckPlacementRoutes(const Mesh3D& source, Add&& add)
	{
		const Mat4x4 transform = Mat4x4{
			-2, 0.5f, 0, 0,
			0, 3, 0, 0,
			0, 0, 4, 0,
			5, 6, 7, 1
		};
		const Mesh3D placed = source.transformed(transform);
		CheckOrientation(placed, source, true);

		Mesh3DBuilder builder;
		const auto prefix = builder.addBox();
		REQUIRE(prefix);
		const Mesh3D original = builder.getMesh();
		builder.reserve(original.vertexCount() + source.vertexCount(), original.triangleCount() + source.triangleCount());
		const auto* vertices = builder.getMesh().vertices.data();
		const auto* indices = builder.getMesh().indices.data();
		const auto added = add(builder, transform);
		REQUIRE(added);
		CHECK_EQ(added->vertexOffset, original.vertexCount());
		CHECK_EQ(added->triangleOffset, original.triangleCount());
		CHECK_EQ(added->vertexCount, source.vertexCount());
		CHECK_EQ(added->triangleCount, source.triangleCount());
		CHECK_EQ(vertices, builder.getMesh().vertices.data());
		CHECK_EQ(indices, builder.getMesh().indices.data());

		Mesh3D expected = original;
		REQUIRE(expected.append(placed));
		Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
		Mesh3DTest::CheckMeshGeometry(builder.getMesh());
		Mesh3D appended = original;
		REQUIRE(appended.append(source, transform));
		Mesh3DTest::CheckMeshDataEqual(appended, expected);

		Mesh3DAssembly assembly;
		const auto mesh = assembly.addMesh(source);
		REQUIRE(mesh);
		REQUIRE(assembly.addPart({ .mesh = *mesh, .placement = transform }));
		const auto baked = assembly.bake();
		REQUIRE(baked);
		Mesh3DTest::CheckMeshDataEqual(baked->mesh, placed);
	}
}

TEST_CASE("Mesh3D transform preserves facing for every scale sign combination")
{
	const Mesh3D source = Mesh3D::Wedge(Vec3{ 2, 3, 4 });
	for (uint32 mask = 0; mask < 8; ++mask)
	{
		CAPTURE(mask);
		const Float3 scale{ (mask & 1) ? -2.0f : 2.0f, (mask & 2) ? -3.0f : 3.0f, (mask & 4) ? -4.0f : 4.0f };
		const bool mirrored = ((scale.x * scale.y * scale.z) < 0);
		const Mesh3D transformed = source.transformed(Mat4x4::Scale(scale));
		const Mesh3D scaled = source.scaled(scale);
		CheckOrientation(transformed, source, mirrored);
		CheckOrientation(scaled, source, mirrored);
		CheckAttributesNear(scaled, transformed);
		CheckAttributesNear(Mesh3D{ source }.scaled(scale), scaled);
		Mesh3DTest::CheckMeshDataEqual(Mesh3D{ source }.transformed(Mat4x4::Scale(scale)), transformed);
	}

	const Mesh3D uniform = source.scaled(-2.0f);
	CheckOrientation(uniform, source, true);
	CheckAttributesNear(uniform, source.transformed(Mat4x4::Scale(-2.0f)));
	CheckAttributesNear(Mesh3D{ source }.scaled(-2.0f), uniform);
}

TEST_CASE("Mesh3D reflection round trip preserves storage and inward facing meshes")
{
	Mesh3D source = Mesh3D::Box();
	const Mat4x4 reflection = Mat4x4::Scale(Float3{ -1, 1, 1 });
	Mesh3D mesh = source;
	const auto* vertices = mesh.vertices.data();
	const auto* indices = mesh.indices.data();
	mesh.transform(reflection).transform(reflection);
	CHECK_EQ(vertices, mesh.vertices.data());
	CHECK_EQ(indices, mesh.indices.data());
	Mesh3DTest::CheckMeshDataEqual(mesh, source);

	source.invert();
	mesh = source.transformed(reflection);
	CheckOrientation(mesh, source, true);
	for (const auto& vertex : mesh.vertices)
	{
		CHECK(vertex.pos.dot(vertex.normal) < 0);
	}
	mesh.transform(reflection);
	Mesh3DTest::CheckMeshDataEqual(mesh, source);
}

TEST_CASE("Mesh3D placement agrees across factories Builder append and Assembly")
{
	SUBCASE("Box family")
	{
		CheckPlacementRoutes(Mesh3D::ChamferedBox(Vec3{ 2, 3, 4 }, 0.2), [](auto& builder, const auto& transform)
		{
			return builder.addChamferedBox(Vec3{ 2, 3, 4 }, 0.2, transform);
		});
	}
	SUBCASE("Polyhedron")
	{
		CheckPlacementRoutes(Mesh3D::IcoSphere(1, 1), [](auto& builder, const auto& transform)
		{
			return builder.addIcoSphere(1, 1, transform);
		});
	}
	SUBCASE("Round primitive")
	{
		CheckPlacementRoutes(Mesh3D::Cylinder(1, 2, 8), [](auto& builder, const auto& transform)
		{
			return builder.addCylinder(1, 2, 8, transform);
		});
	}
	SUBCASE("Grid")
	{
		CheckPlacementRoutes(Mesh3D::Grid(SizeF{ 2, 3 }, 2, 3), [](auto& builder, const auto& transform)
		{
			return builder.addGrid(SizeF{ 2, 3 }, 2, 3, transform);
		});
	}
	SUBCASE("Loft with separately constrained section frames")
	{
		const Array<Vec2> contour{ { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };
		const std::array<LoftSection, 2> sections{ LoftSection{ contour, Vec3{ 0, 0, 0 } }, LoftSection{ contour, Vec3{ 0, 2, 0 } } };
		CheckPlacementRoutes(Mesh3D::Loft(sections), [&](auto& builder, const auto& transform)
		{
			return builder.addLoft(sections, transform);
		});
	}
}

TEST_CASE("Mesh3D mirrored addition handles aliasing no-op failure and singular transforms")
{
	const Mat4x4 reflection = Mat4x4::Scale(Float3{ -1, 1, 1 });
	const Mesh3D source = Mesh3D::Box();
	Mesh3D expected = source;
	REQUIRE(expected.append(source.transformed(reflection)));
	Mesh3D actual = source;
	REQUIRE(actual.append(actual, reflection));
	Mesh3DTest::CheckMeshDataEqual(actual, expected);

	Mesh3DBuilder builder;
	REQUIRE(builder.addMesh(source));
	REQUIRE(builder.addMesh(builder.getMesh(), reflection));
	Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);
	const auto noOp = builder.addBox(Vec3::One(), reflection, BoxFace::None_);
	REQUIRE(noOp);
	CHECK(noOp->isEmpty());
	CHECK_FALSE(builder.addBox(Vec3{ -1, 1, 1 }, reflection));
	CHECK_FALSE(builder.addLoft(std::span<const LoftSection>{}, reflection));
	Mesh3DTest::CheckMeshDataEqual(builder.getMesh(), expected);

	Mesh3D empty;
	empty.transform(reflection).scale(-1.0f);
	CHECK(empty.vertices.isEmpty());
	CHECK(empty.indices.isEmpty());

	Mesh3D singular = source;
	singular.transform(Mat4x4::Scale(Float3{ -1, 0, 1 }));
	for (size_t i = 0; i < source.triangleCount(); ++i)
	{
		CHECK_EQ(singular.indices[i].i1, source.indices[i].i1);
		CHECK_EQ(singular.indices[i].i2, source.indices[i].i2);
	}
	for (size_t i = 0; i < source.vertexCount(); ++i)
	{
		CHECK_EQ(singular.vertices[i].normal, source.vertices[i].normal);
		CHECK_EQ(singular.vertices[i].tangent, source.vertices[i].tangent);
	}
}
