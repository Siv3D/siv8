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
	const FloatRect Bounds{ 17, -13, 217, 107 };
	const FloatRect InnerBounds{ 24, -6, 210, 100 };
	const FloatRect UV{ 0.8f, 0.9f, 0.2f, 0.3f };
	const Float4 Color0{ 0.1f, 0.2f, 0.7f, 0.35f }, Color1{ 0.8f, 0.5f, 0.3f, 0.9f };
	const Float2 UnwrittenUV{ -7, -8 };

	enum class Style { Solid, TopBottom, LeftRight, Textured, Frame, FrameTopBottom, FrameLeftRight };
	constexpr Style Styles[]{ Style::Solid, Style::TopBottom, Style::LeftRight, Style::Textured,
		Style::Frame, Style::FrameTopBottom, Style::FrameLeftRight };

	uint32 Build(const BufferCreatorFunc& allocate, const Style style, const float radius, const float scale)
	{
		using namespace Vertex2DBuilder;
		const float innerR = Max((radius - 7), 0.0f);
		switch (style)
		{
		case Style::Solid:
			return BuildRoundRect(allocate, Bounds, radius, Color0, scale);
		case Style::TopBottom:
		case Style::LeftRight:
			return BuildRoundRect(allocate, Bounds, radius,
				((style == Style::TopBottom) ? ColorFillDirection::TopBottom : ColorFillDirection::LeftRight), Color0, Color1, scale);
		case Style::Textured:
			return BuildTexturedRoundRect(allocate, Bounds, 200, 120, radius, UV, Color0, scale);
		case Style::Frame:
			return BuildRoundRectFrame(allocate, InnerBounds, innerR, Bounds, radius, Color0, scale);
		default:
			return BuildRoundRectFrame(allocate, InnerBounds, innerR, Bounds, radius,
				((style == Style::FrameTopBottom) ? ColorFillDirection::TopBottom : ColorFillDirection::LeftRight), Color0, Color1, scale);
		}
	}

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
	};

	Mesh Capture(const Style style, const float radius, const float scale)
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
		const uint32 count = Build(allocate, style, radius, scale);
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

	void CheckPosition(const Float2 actual, const Vec2 expected)
	{
		CHECK(std::abs(actual.x - expected.x) < 0.0001);
		CHECK(std::abs(actual.y - expected.y) < 0.0001);
	}

	void CheckContour(const Mesh& mesh, const FloatRect& rect, const float radius, const size_t pointsPerCorner, const size_t stride, const size_t first)
	{
		const Vec2 centers[]{ { rect.right - radius, rect.top + radius }, { rect.right - radius, rect.bottom - radius },
			{ rect.left + radius, rect.bottom - radius }, { rect.left + radius, rect.top + radius } };
		for (size_t corner = 0; corner < 4; ++corner)
		{
			for (size_t i = 0; i < pointsPerCorner; ++i)
			{
				const double angle = ((corner - 1.0 + double(i) / (pointsPerCorner - 1)) * Math::HalfPi);
				const Vec2 expected = (centers[corner] + Vec2{ std::cos(angle), std::sin(angle) } * radius);
				CheckPosition(mesh.vertices[first + (corner * pointsPerCorner + i) * stride].pos, expected);
			}
		}
	}
}

TEST_CASE("Vertex2DBuilder.round_rect_vertices_and_attributes")
{
	for (const float radius : { 0.0f, 4.0f, 12.0f, 60.0f })
	{
		for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
		{
			for (const auto style : Styles)
			{
				CAPTURE(radius, scale, static_cast<int32>(style));
				const auto mesh = Capture(style, radius, scale);
				const bool frame = (Style::Frame <= style);
				const size_t points = (frame ? (mesh.vertices.size() / 8) : ((mesh.vertices.size() - 4) / 4));
				REQUIRE(points >= 3);
				CHECK(mesh.vertices.size() == (frame ? (points * 8) : (points * 4 + 4)));
				CheckContour(mesh, Bounds, radius, points, (frame ? 2 : 1), 0);
				if (frame)
				{
					CheckContour(mesh, InnerBounds, Max((radius - 7), 0.0f), points, 2, 1);
				}
				else
				{
					const Vec2 centers[]{ { Bounds.right - radius, Bounds.top + radius }, { Bounds.right - radius, Bounds.bottom - radius },
						{ Bounds.left + radius, Bounds.bottom - radius }, { Bounds.left + radius, Bounds.top + radius } };
					for (size_t corner = 0; corner < 4; ++corner)
					{
						CheckPosition(mesh.vertices[points * 4 + corner].pos, centers[corner]);
					}
				}

				for (const auto& vertex : mesh.vertices)
				{
					const bool vertical = ((style == Style::TopBottom) || (style == Style::FrameTopBottom));
					const bool gradient = (vertical || (style == Style::LeftRight) || (style == Style::FrameLeftRight));
					if (gradient)
					{
						const float t = (vertical ? ((vertex.pos.y - Bounds.top) / 120) : ((vertex.pos.x - Bounds.left) / 200));
						const Float4 expected = Color0.lerp(Color1, t);
						CHECK(std::abs(vertex.color.x - expected.x) < 0.000002f);
						CHECK(std::abs(vertex.color.y - expected.y) < 0.000002f);
						CHECK(std::abs(vertex.color.z - expected.z) < 0.000002f);
						CHECK(std::abs(vertex.color.w - expected.w) < 0.000002f);
					}
					else
					{
						CHECK(vertex.color == Color0);
					}
					if (style == Style::Textured)
					{
						const Float2 expected{ (UV.left + (vertex.pos.x - Bounds.left) * ((UV.right - UV.left) / 200)),
							(UV.top + (vertex.pos.y - Bounds.top) * ((UV.bottom - UV.top) / 120)) };
						CHECK(vertex.tex == expected);
					}
					else
					{
						CHECK(vertex.tex == UnwrittenUV);
					}
				}
			}
		}
	}
}

TEST_CASE("Vertex2DBuilder.round_rect_variants_share_geometry")
{
	for (const float radius : { 0.0f, 4.0f, 12.0f, 60.0f })
	{
		for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
		{
			for (const auto style : Styles)
			{
				CAPTURE(radius, scale, static_cast<int32>(style));
				const auto reference = Capture(((style < Style::Frame) ? Style::Solid : Style::Frame), radius, scale);
				const auto mesh = Capture(style, radius, scale);
				REQUIRE(mesh.vertices.size() == reference.vertices.size());
				CHECK(mesh.indices == reference.indices);
				for (size_t i = 0; i < mesh.vertices.size(); ++i)
				{
					CHECK(mesh.vertices[i].pos == reference.vertices[i].pos);
				}
			}
		}
	}
}
