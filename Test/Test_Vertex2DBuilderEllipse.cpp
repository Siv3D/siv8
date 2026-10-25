//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <cmath>

namespace
{
	using Index = Vertex2D::IndexType;
	const Float2 Center{ 17.25f, -13.5f }, UnwrittenUV{ -7, -8 };
	const FloatRect UV{ 0.8f, 0.9f, 0.2f, 0.3f };
	const Float4 Color0{ 0.1f, 0.2f, 0.7f, 0.35f }, Color1{ 0.8f, 0.5f, 0.3f, 0.9f };
	enum class Shape { Circle, Ellipse, TexturedCircle };
	enum class Paint { Solid, Radial, TopBottom, LeftRight };
	constexpr Paint Paints[]{ Paint::Solid, Paint::Radial, Paint::TopBottom, Paint::LeftRight };

	uint32 Build(const BufferCreatorFunc& allocate, const Shape shape, const Paint paint, const Float2 axes, const float scale)
	{
		using namespace Vertex2DBuilder;
		const auto direction = ((paint == Paint::TopBottom) ? ColorFillDirection::TopBottom
			: (paint == Paint::LeftRight) ? ColorFillDirection::LeftRight : ColorFillDirection::InOut);
		const Float4& rimColor = ((paint == Paint::Solid) ? Color0 : Color1);
		switch (shape)
		{
		case Shape::Circle:
			return BuildCircle(allocate, Center, Max(axes.x, axes.y), direction, Color0, rimColor, scale);
		case Shape::Ellipse:
			return BuildEllipse(allocate, Center, axes.x, axes.y, direction, Color0, rimColor, scale);
		default:
			return BuildTexturedCircle(allocate, Circle{ Center, Max(axes.x, axes.y) }, UV, Color0, scale);
		}
	}

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
	};

	Mesh Capture(const Shape shape, const Paint paint, const Float2 axes, const float scale)
	{
		Mesh mesh;
		Vertex2D guard;
		guard.set(-123, -456, UnwrittenUV.x, UnwrittenUV.y, Float4{ -1, -2, -3, -4 });
		uint32 requests = 0;
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++requests;
			mesh.vertices.assign((vertices + size_t{ 2 }), guard);
			mesh.indices.assign((indices + size_t{ 2 }), Index{ 65535 });
			return Vertex2DBufferPointer{ (mesh.vertices.data() + 1), (mesh.indices.data() + 1), 37 };
		};
		const uint32 count = Build(allocate, shape, paint, axes, scale);
		REQUIRE(requests == 1);
		REQUIRE(mesh.indices.size() == (count + size_t{ 2 }));
		for (const auto* vertex : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(vertex->pos == guard.pos);
			CHECK(vertex->tex == guard.tex);
			CHECK(vertex->color == guard.color);
		}
		CHECK(mesh.indices.front() == 65535);
		CHECK(mesh.indices.back() == 65535);
		mesh.vertices.pop_back();
		mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back();
		mesh.indices.erase(mesh.indices.begin());
		return mesh;
	}
}

TEST_CASE("Vertex2DBuilder.ellipse_vertices_and_attributes")
{
	struct Fixture { Float2 axes; float scale; size_t rimVertices; };
	const Fixture fixtures[]{ { { 0, 0 }, 1, 8 }, { { 1, 0.25f }, 0, 8 }, { { 1, 0.25f }, 1, 8 },
		{ { 1, 0.25f }, 4, 12 }, { { 1, 0.25f }, 8, 20 }, { { 1, 0.25f }, 512, 252 },
		{ { 0.25f, 1 }, 512, 252 }, { { 37, 12 }, 1, 32 }, { { 12, 37 }, 8, 164 }, { { 0, 12 }, 1, 20 } };
	for (const auto& [axes, scale, rimVertices] : fixtures)
	{
		for (const auto shape : { Shape::Circle, Shape::Ellipse, Shape::TexturedCircle })
		{
			for (const auto paint : Paints)
			{
				if ((shape == Shape::TexturedCircle) && (paint != Paint::Solid)) continue;
				CAPTURE(axes, scale, static_cast<int32>(shape), static_cast<int32>(paint));
				const auto mesh = Capture(shape, paint, axes, scale);
				REQUIRE(mesh.vertices.size() == (rimVertices + 1));
				REQUIRE(mesh.indices.size() == (rimVertices * 3));
				const Float2 radii = ((shape == Shape::Ellipse) ? axes : Float2{ Max(axes.x, axes.y), Max(axes.x, axes.y) });
				for (size_t i = 0; i <= rimVertices; ++i)
				{
					CAPTURE(i);
					const double angle = ((double(i) - 1) / rimVertices * Math::TwoPi - Math::HalfPi);
					const Vec2 unit = (i ? Vec2{ std::cos(angle), std::sin(angle) } : Vec2{ 0, 0 });
					const auto& vertex = mesh.vertices[i];
					CHECK(std::abs(vertex.pos.x - (Center.x + unit.x * radii.x)) < 0.0001);
					CHECK(std::abs(vertex.pos.y - (Center.y + unit.y * radii.y)) < 0.0001);
					const float t = ((paint == Paint::TopBottom) ? float((unit.y + 1) * 0.5)
						: (paint == Paint::LeftRight) ? float((unit.x + 1) * 0.5) : ((paint == Paint::Radial) && i ? 1.0f : 0.0f));
					const Float4 expected = Color0.lerp(Color1, t);
					CHECK(std::abs(vertex.color.x - expected.x) < 0.000002f);
					CHECK(std::abs(vertex.color.y - expected.y) < 0.000002f);
					CHECK(std::abs(vertex.color.z - expected.z) < 0.000002f);
					CHECK(std::abs(vertex.color.w - expected.w) < 0.000002f);
					if (shape == Shape::TexturedCircle)
					{
						CHECK(std::abs(vertex.tex.x - (UV.left + (unit.x + 1) * 0.5 * (UV.right - UV.left))) < 0.000001);
						CHECK(std::abs(vertex.tex.y - (UV.top + (unit.y + 1) * 0.5 * (UV.bottom - UV.top))) < 0.000001);
					}
					else
					{
						CHECK(vertex.tex == UnwrittenUV);
					}
				}
				for (size_t i = 0; i < rimVertices; ++i)
				{
					CHECK(mesh.indices[i * 3] == 37);
					CHECK(mesh.indices[i * 3 + 1] == (38 + i));
					CHECK(mesh.indices[i * 3 + 2] == (38 + (i + 1) % rimVertices));
				}
			}
		}
	}
}

TEST_CASE("Vertex2DBuilder.circle_and_equal_axis_ellipse_match")
{
	for (const float radius : { 0.0f, 1.0f, 2.01f, 4.01f, 8.0f, 37.0f, 472.0f })
	{
		for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
		{
			for (const auto paint : Paints)
			{
				CAPTURE(radius, scale, static_cast<int32>(paint));
				const auto circle = Capture(Shape::Circle, paint, { radius, radius }, scale);
				const auto ellipse = Capture(Shape::Ellipse, paint, { radius, radius }, scale);
				REQUIRE(circle.vertices.size() == ellipse.vertices.size());
				CHECK(circle.indices == ellipse.indices);
				for (size_t i = 0; i < circle.vertices.size(); ++i)
				{
					CHECK(circle.vertices[i].pos == ellipse.vertices[i].pos);
					CHECK(circle.vertices[i].color == ellipse.vertices[i].color);
				}
			}
		}
	}
}
