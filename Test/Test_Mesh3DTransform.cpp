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
		REQUIRE((actual.triangleCount()) == (source.triangleCount()));
		for (size_t i = 0; i < source.triangleCount(); ++i)
		{
			CHECK((actual.indices[i].i0) == (source.indices[i].i0));
			CHECK((actual.indices[i].i1) == (mirrored ? source.indices[i].i2 : source.indices[i].i1));
			CHECK((actual.indices[i].i2) == (mirrored ? source.indices[i].i1 : source.indices[i].i2));
		}
		Mesh3DTest::CheckMeshGeometry(actual);
	}

	void CheckAttributesNear(const Mesh3D& actual, const Mesh3D& expected)
	{
		REQUIRE((actual.vertexCount()) == (expected.vertexCount()));
		for (size_t i = 0; i < actual.vertexCount(); ++i)
		{
			const auto& a = actual.vertices[i];
			const auto& b = expected.vertices[i];
			CHECK((a.pos - b.pos).length() < 1e-5f);
			CHECK((a.normal - b.normal).length() < 1e-5f);
			CHECK((a.tangent.xyz() - b.tangent.xyz()).length() < 1e-5f);
			CHECK((a.tangent.w) == (b.tangent.w));
			CHECK((a.tex) == (b.tex));
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
		CHECK((added->vertexOffset) == (original.vertexCount()));
		CHECK((added->triangleOffset) == (original.triangleCount()));
		CHECK((added->vertexCount) == (source.vertexCount()));
		CHECK((added->triangleCount) == (source.triangleCount()));
		CHECK((vertices) == (builder.getMesh().vertices.data()));
		CHECK((indices) == (builder.getMesh().indices.data()));

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
	CHECK((vertices) == (mesh.vertices.data()));
	CHECK((indices) == (mesh.indices.data()));
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
	SECTION("Box family")
	{
		CheckPlacementRoutes(Mesh3D::ChamferedBox(Vec3{ 2, 3, 4 }, 0.2), [](auto& builder, const auto& transform)
		{
			return builder.addChamferedBox(Vec3{ 2, 3, 4 }, 0.2, transform);
		});
	}
	SECTION("Polyhedron")
	{
		CheckPlacementRoutes(Mesh3D::IcoSphere(1, 1), [](auto& builder, const auto& transform)
		{
			return builder.addIcoSphere(1, 1, transform);
		});
	}
	SECTION("Round primitive")
	{
		CheckPlacementRoutes(Mesh3D::Cylinder(1, 2, 8), [](auto& builder, const auto& transform)
		{
			return builder.addCylinder(1, 2, 8, transform);
		});
	}
	SECTION("Grid")
	{
		CheckPlacementRoutes(Mesh3D::Grid(SizeF{ 2, 3 }, 2, 3), [](auto& builder, const auto& transform)
		{
			return builder.addGrid(SizeF{ 2, 3 }, 2, 3, transform);
		});
	}
	SECTION("Loft with separately constrained section frames")
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
		CHECK((singular.indices[i].i1) == (source.indices[i].i1));
		CHECK((singular.indices[i].i2) == (source.indices[i].i2));
	}
	for (size_t i = 0; i < source.vertexCount(); ++i)
	{
		CHECK((singular.vertices[i].normal) == (source.vertices[i].normal));
		CHECK((singular.vertices[i].tangent) == (source.vertices[i].tangent));
	}
}

TEST_CASE("Mesh3D transform parameters use double and Vec3")
{
	static_assert(requires
	{
		static_cast<Mesh3D& (Mesh3D::*)(Vec3) noexcept>(&Mesh3D::translate);
		static_cast<Mesh3D (Mesh3D::*)(Vec3) const&>(&Mesh3D::translated);
		static_cast<Mesh3D (Mesh3D::*)(Vec3) && noexcept>(&Mesh3D::translated);
		static_cast<Mesh3D& (Mesh3D::*)(double) noexcept>(&Mesh3D::scale);
		static_cast<Mesh3D& (Mesh3D::*)(Vec3) noexcept>(&Mesh3D::scale);
		static_cast<Mesh3D (Mesh3D::*)(double) const&>(&Mesh3D::scaled);
		static_cast<Mesh3D (Mesh3D::*)(Vec3) const&>(&Mesh3D::scaled);
		static_cast<Mesh3D (Mesh3D::*)(double) && noexcept>(&Mesh3D::scaled);
		static_cast<Mesh3D (Mesh3D::*)(Vec3) && noexcept>(&Mesh3D::scaled);
	});
	static_assert(std::is_convertible_v<Float3, Vec3>);

	const Mesh3D source = Mesh3D::Wedge(Vec3{ 2, 3, 4 });
	const Vec3 offset{ 0.1, -2.3, 4.7 };
	Mesh3D expected = source;
	for (auto& vertex : expected.vertices)
	{
		vertex.pos += Float3{ offset };
	}
	Mesh3DTest::CheckMeshDataEqual(source.translated(offset), expected);
	Mesh3D moved = source;
	const auto* vertices = moved.vertices.data();
	const auto* indices = moved.indices.data();
	CHECK((&moved.translate(offset)) == (&moved));
	CHECK((moved.vertices.data()) == (vertices));
	CHECK((moved.indices.data()) == (indices));
	Mesh3DTest::CheckMeshDataEqual(moved, expected);
	Mesh3D extracted = std::move(moved).translated(-offset);
	CHECK((extracted.vertices.data()) == (vertices));
	CHECK((extracted.indices.data()) == (indices));
	CheckAttributesNear(extracted, source);
	Mesh3DTest::CheckMeshDataEqual(source, Mesh3D::Wedge(Vec3{ 2, 3, 4 }));

	// Existing float arguments still work through widening conversions.
	Mesh3DTest::CheckMeshDataEqual(source.translated(Float3{ 1, 2, 3 }), source.translated(Vec3{ 1, 2, 3 }));
	Mesh3DTest::CheckMeshDataEqual(source.scaled(2.0f), source.scaled(2.0));
	Mesh3DTest::CheckMeshDataEqual(source.scaled(Float3{ 2, 3, 4 }), source.scaled(Vec3{ 2, 3, 4 }));
}

TEST_CASE("Mesh3D converts double transform parameters before vertex arithmetic")
{
	// A position-only fixture distinguishes float arithmetic from double arithmetic
	// followed by rounding the result back to float.
	Mesh3D source(1, 0);
	source.vertices[0].pos = Float3{ 1, -1, 1 };
	const Vec3 offset{ -1.0 + std::ldexp(1.0, -26), 1.0 - std::ldexp(1.0, -26), 0 };
	CHECK((source.translated(offset).vertices[0].pos) == (Float3(0, 0, 1)));

	const float position = std::nextafter(1.0f, 2.0f);
	source.vertices[0].pos = Float3{ position, position, position };
	const double factor = 1.0 + std::ldexp(1.0, -24);
	REQUIRE((static_cast<float>(factor)) == (1.0f));
	REQUIRE((static_cast<float>(static_cast<double>(position) * factor)) != (position));
	CHECK((source.scaled(factor).vertices[0].pos) == (source.vertices[0].pos));
	CHECK((source.scaled(Vec3{ factor, factor, factor }).vertices[0].pos) == (source.vertices[0].pos));
}

TEST_CASE("Mesh3D double scaling preserves reflection zero and storage contracts")
{
	const Mesh3D source = Mesh3D::Wedge(Vec3{ 2, 3, 4 });
	const double tiny = static_cast<double>(std::numeric_limits<float>::denorm_min()) * 0.25;
	REQUIRE((static_cast<float>(tiny)) == (0.0f));
	for (const double factor : { 1.0, 1.7, -2.3, 0.0, -0.0, tiny, -tiny })
	{
		Mesh3D expected = source;
		const float rounded = static_cast<float>(factor);
		for (auto& vertex : expected.vertices)
		{
			vertex.pos *= rounded;
			if (rounded < 0)
			{
				vertex.normal = -vertex.normal;
				vertex.tangent = -vertex.tangent;
			}
		}
		if (rounded < 0)
		{
			expected.reverseWinding();
		}
		Mesh3DTest::CheckMeshDataEqual(source.scaled(factor), expected);
		Mesh3D moved = source;
		const auto* vertices = moved.vertices.data();
		const auto* indices = moved.indices.data();
		CHECK((&moved.scale(factor)) == (&moved));
		Mesh3D result = std::move(moved).scaled(1.0);
		CHECK((result.vertices.data()) == (vertices));
		CHECK((result.indices.data()) == (indices));
		Mesh3DTest::CheckMeshDataEqual(result, expected);
	}
	for (const Vec3 factor : { Vec3{ 0.7, 1.3, 2.1 }, Vec3{ -0.7, 1.3, 2.1 },
		Vec3{ -tiny, 1.3, 2.1 }, Vec3{ 0, 1.3, 2.1 } })
	{
		const Mesh3D expected = source.transformed(Mat4x4::Scale(Float3{ factor }));
		const Mesh3D scaled = source.scaled(factor);
		CheckAttributesNear(scaled, expected);
		for (size_t i = 0; i < scaled.indices.size(); ++i)
		{
			CHECK((scaled.indices[i].i0) == (expected.indices[i].i0));
			CHECK((scaled.indices[i].i1) == (expected.indices[i].i1));
			CHECK((scaled.indices[i].i2) == (expected.indices[i].i2));
		}
		Mesh3D moved = source;
		const auto* vertices = moved.vertices.data();
		const auto* indices = moved.indices.data();
		CHECK((&moved.scale(factor)) == (&moved));
		Mesh3D result = std::move(moved).scaled(Vec3::One());
		CHECK((result.vertices.data()) == (vertices));
		CHECK((result.indices.data()) == (indices));
		CheckAttributesNear(result, scaled);
	}
	Mesh3D empty;
	CHECK(empty.translate(Vec3{ 1, 2, 3 }).scale(-2.5).scale(Vec3{ 0, 2, 3 }).isEmpty());
}
