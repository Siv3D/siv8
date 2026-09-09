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
	void CheckUVDirections(const Mesh3D& mesh)
	{
		// Compare against triangle UV derivatives, independently of the generators.
		for (const auto& triangle : mesh.indices)
		{
			const auto& a = mesh.vertices[triangle.i0];
			const auto& b = mesh.vertices[triangle.i1];
			const auto& c = mesh.vertices[triangle.i2];
			const Float2 uv1 = (b.tex - a.tex);
			const Float2 uv2 = (c.tex - a.tex);
			const float determinant = (uv1.x * uv2.y - uv1.y * uv2.x);
			REQUIRE(std::abs(determinant) > 1e-7f);
			const Float3 uDirection = ((b.pos - a.pos) * uv2.y - (c.pos - a.pos) * uv1.y) / determinant;
			const Float3 vDirection = ((c.pos - a.pos) * uv1.x - (b.pos - a.pos) * uv2.x) / determinant;
			for (const uint32 index : { triangle.i0, triangle.i1, triangle.i2 })
			{
				const auto& vertex = mesh.vertices[index];
				CHECK(vertex.tangent.xyz().dot(uDirection) > 0);
				CHECK(vertex.bitangent().dot(vDirection) > 0);
			}
		}
	}

	template <class Factory>
	void CheckUVScales(Factory&& make)
	{
		const Mesh3D original = make(Vec2::One());
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -2, 3 }, Vec2{ 2, -3 }, Vec2{ -2, -3 },
			Vec2{ 0, 1 }, Vec2{ 1, 0 }, Vec2{ 0, 0 }, Vec2{ -1, 0 }, Vec2{ 0, -1 } })
		{
			CAPTURE(scale.x);
			CAPTURE(scale.y);
			const Mesh3D mesh = make(scale);
			Mesh3DTest::CheckMeshGeometry(mesh);
			REQUIRE((mesh.vertexCount()) == (original.vertexCount()));
			REQUIRE((mesh.triangleCount()) == (original.triangleCount()));
			for (size_t i = 0; i < mesh.vertexCount(); ++i)
			{
				const auto& a = mesh.vertices[i];
				const auto& b = original.vertices[i];
				CHECK((a.pos) == (b.pos));
				CHECK((a.normal) == (b.normal));
				CHECK(a.tex.epsilonEquals(b.tex * Float2{ scale }, 1e-6f));
				CHECK(a.tangent.xyz().epsilonEquals(b.tangent.xyz() * ((scale.x < 0) ? -1.0f : 1.0f), 1e-6f));
				CHECK((a.tangent.w) == (b.tangent.w * (((scale.x < 0) != (scale.y < 0)) ? -1.0f : 1.0f)));
			}
			for (size_t i = 0; i < mesh.triangleCount(); ++i)
			{
				CHECK((mesh.indices[i].i0) == (original.indices[i].i0));
				CHECK((mesh.indices[i].i1) == (original.indices[i].i1));
				CHECK((mesh.indices[i].i2) == (original.indices[i].i2));
			}
			if (scale.x != 0 && scale.y != 0)
			{
				CheckUVDirections(mesh);
			}
		}
	}
}

TEST_CASE("Mesh3D generators preserve tangent space under UV reflection and collapse")
{
	SECTION("Plane")
	{
		CheckUVScales([](Vec2 scale) { return Mesh3D::Plane(SizeF{ 2, 3 }, scale); });
	}
	SECTION("Grid")
	{
		CheckUVScales([](Vec2 scale) { return Mesh3D::Grid(SizeF{ 2, 3 }, 2, 3, scale); });
	}
	SECTION("HeightField")
	{
		CheckUVScales([](Vec2 scale)
		{
			return Mesh3D::HeightField(Size{ 3, 3 }, SizeF{ 2, 2 },
				[](Point p) { return p.x * 0.25 + p.y * 0.5; }, { .uvScale = scale });
		});
	}
	SECTION("Tube constant and varying radii")
	{
		const Array<Vec3> path{ { 0, 0, 0 }, { 0, 1, 0 }, { 0, 2, 0 } };
		CheckUVScales([&](Vec2 scale) { return Mesh3D::Tube(path, 1, { .sides = 8, .uvScale = scale }); });
		const Array<double> radii{ 1, 0.75, 0.5 };
		CheckUVScales([&](Vec2 scale) { return Mesh3D::Tube(path, radii, { .sides = 8, .uvScale = scale }); });
	}
	SECTION("Sweep constant and varying sections")
	{
		const Polygon contour{ { { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } } };
		const Array<Vec3> path{ { 0, 0, 0 }, { 0, 1, 0 }, { 0, 2, 0 } };
		CheckUVScales([&](Vec2 scale) { return Mesh3D::Sweep(contour, path, { .uvScale = scale }); });
		const Array<SweepSectionTransform> transforms{ {}, { .scale = Vec2{ 0.8, 0.9 } }, { .scale = Vec2{ 0.6, 0.8 } } };
		CheckUVScales([&](Vec2 scale) { return Mesh3D::Sweep(contour, path, transforms, { .uvScale = scale }); });
	}
	SECTION("Revolve including sweep caps")
	{
		const Array<Vec2> profile{ { 0, -1 }, { 1, -1 }, { 1, 1 }, { 0, 1 } };
		CheckUVScales([&](Vec2 scale)
		{
			return Mesh3D::Revolve(profile, { .sweepAngle = Math::Pi, .segments = 8, .uvScale = scale, .closeSweepEnds = CloseEnds::Yes });
		});
	}
	SECTION("Loft")
	{
		const Array<Vec2> contour{ { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } };
		const std::array<LoftSection, 2> sections{ LoftSection{ contour, Vec3::Zero() }, LoftSection{ contour, Vec3{ 0, 2, 0 } } };
		CheckUVScales([&](Vec2 scale) { return Mesh3D::Loft(sections, { .uvScale = scale }); });
	}
}

TEST_CASE("Mesh3D UV edits explicitly rebuild tangents")
{
	Mesh3D mesh = Mesh3D::Plane();
	const auto original = mesh.vertices;
	mesh.transformUV(Mat3x2::Scale(Vec2{ -1, 1 }));
	for (size_t i = 0; i < mesh.vertexCount(); ++i)
	{
		CHECK((mesh.vertices[i].tangent) == (original[i].tangent));
	}
	mesh.computeTangents();
	Mesh3DTest::CheckMeshGeometry(mesh);
	CheckUVDirections(mesh);
}
