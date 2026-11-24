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
	const Float4 Color0{ 0.1f, 0.2f, 0.7f, 0.35f }, Color1{ 0.8f, 0.5f, 0.3f, 0.9f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
	};

	template <class Build>
	Mesh Capture(Build&& build)
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
		const uint32 count = build(allocate);
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
		for (const auto& vertex : mesh.vertices) CHECK(vertex.tex == UnwrittenUV);
		return mesh;
	}

	Vec2 Unit(const double angle)
	{
		return{ std::sin(angle), -std::cos(angle) };
	}

	void CheckPosition(const Float2 actual, const Vec2 expected)
	{
		CHECK(std::abs(actual.x - expected.x) < 0.0001);
		CHECK(std::abs(actual.y - expected.y) < 0.0001);
	}

	void CheckColor(const Float4 actual, const Float4 expected)
	{
		CHECK(std::abs(actual.x - expected.x) < 0.000002f);
		CHECK(std::abs(actual.y - expected.y) < 0.000002f);
		CHECK(std::abs(actual.z - expected.z) < 0.000002f);
		CHECK(std::abs(actual.w - expected.w) < 0.000002f);
	}

	void CheckFan(const Mesh& mesh, const size_t firstVertex, const size_t firstIndex, const size_t points,
		const Vec2 center, const Vec2 rimCenter, const Vec2 radii, const double start, const double sweep)
	{
		REQUIRE((firstVertex + points + 1) <= mesh.vertices.size());
		REQUIRE((firstIndex + (points - 1) * 3) <= mesh.indices.size());
		CheckPosition(mesh.vertices[firstVertex].pos, center);
		for (size_t i = 0; i < points; ++i)
		{
			const Vec2 unit = Unit(start + sweep * i / (points - 1));
			CheckPosition(mesh.vertices[firstVertex + 1 + i].pos, rimCenter + unit * radii);
		}
		for (size_t i = 0; i + 1 < points; ++i)
		{
			CHECK(mesh.indices[firstIndex + i * 3] == (37 + firstVertex));
			CHECK(mesh.indices[firstIndex + i * 3 + 1] == (38 + firstVertex + i));
			CHECK(mesh.indices[firstIndex + i * 3 + 2] == (39 + firstVertex + i));
		}
	}
}

TEST_CASE("Vertex2DBuilder.arc_vertices_pies_and_segments")
{
	for (const float radius : { 0.0f, 1.0f, 37.0f })
	for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
	for (const float start : { -0.37f, 7.4f })
	for (const float sweep : { -Math::TwoPiF, -4.3f, -0.003f, 0.003f, Math::PiF, Math::TwoPiF })
	for (int32 shape = 0; shape < 3; ++shape)
	{
		CAPTURE(radius, scale, start, sweep, shape);
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
		{
			using namespace Vertex2DBuilder;
			if (shape == 0) return BuildCirclePie(allocate, Center, radius, start, sweep, Color0, Color1, scale);
			if (shape == 1) return BuildEllipsePie(allocate, Center, radius, radius / 4, start, sweep, Color0, Color1, scale);
			return BuildCircleSegment(allocate, Center, radius, start, sweep, Color0, scale);
		});
		const size_t points = (mesh.vertices.size() - 1);
		REQUIRE(points >= ((shape == 2) ? 5 : 2));
		CHECK(mesh.indices.size() == ((points - 1) * 3));
		const Vec2 fanCenter = ((shape == 2) ? (Vec2{ Center } + (Unit(start) + Unit(double(start) + sweep)) * (radius * 0.5)) : Vec2{ Center });
		CheckFan(mesh, 0, 0, points, fanCenter, Center, { radius, ((shape == 1) ? radius / 4 : radius) },
			double(start) + Min(sweep, 0.0f), std::abs(double(sweep)));
		CHECK(mesh.vertices[0].color == Color0);
		for (size_t i = 1; i <= points; ++i) CHECK(mesh.vertices[i].color == ((shape == 2) ? Color0 : Color1));
	}
}

TEST_CASE("Vertex2DBuilder.arc_vertices_solid_caps")
{
	const Float2 end = (Center + Float2{ 4, 3 });
	for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
	{
		CAPTURE(scale);
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildLine(allocate, LineCap::Round, LineCap::Round, Center, end, 2, { Color0, Color1 }, scale);
		});
		const size_t capVertices = ((mesh.vertices.size() - 4) / 2);
		const size_t points = (capVertices - 1);
		REQUIRE(points >= 5);
		CHECK(mesh.indices.size() == (6 + (points - 1) * 6));
		const double start = std::atan2(-0.6, -0.8);
		for (size_t cap = 0; cap < 2; ++cap)
		{
			const Vec2 center = (cap ? end : Center);
			CheckFan(mesh, 4 + cap * capVertices, 6 + cap * (points - 1) * 3, points,
				center, center, { 1, 1 }, start + cap * Math::Pi, Math::Pi);
			for (size_t i = 0; i < capVertices; ++i) CHECK(mesh.vertices[4 + cap * capVertices + i].color == (cap ? Color1 : Color0));
		}
	}
}

TEST_CASE("Vertex2DBuilder.arc_vertices_gradient_caps")
{
	for (const float scale : { 0.0f, 1.0f, 8.0f, 512.0f })
	for (const float start : { -0.37f, 7.4f })
	for (const auto direction : { ColorFillDirection::InOut, ColorFillDirection::LeftRight })
	{
		CAPTURE(scale, start, static_cast<int32>(direction));
		// A zero-sweep round arc contains only its two semicircular caps.
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleArc(allocate, LineCap::Round, Center, 1, start, 0, 2, direction, Color0, Color1, scale);
		});
		const size_t capVertices = (mesh.vertices.size() / 2), points = (capVertices - 1);
		REQUIRE(points >= 5);
		CHECK(mesh.indices.size() == ((points - 1) * 6));
		const Vec2 center = (Vec2{ Center } + Unit(start) * 2);
		const Float4 middle = Color0.lerp(Color1, 0.5f);
		for (size_t cap = 0; cap < 2; ++cap)
		{
			CheckFan(mesh, cap * capVertices, cap * (points - 1) * 3, points,
				center, center, { 1, 1 }, start + (cap ? 0 : Math::Pi), Math::Pi);
			CheckColor(mesh.vertices[cap * capVertices].color, middle);
			for (size_t i = 0; i < points; ++i)
			{
				const double angle = (Math::Pi * i / (points - 1));
				const Float4 expected = ((direction == ColorFillDirection::LeftRight)
					? middle.lerp((cap ? Color1 : Color0), float(std::sin(angle)))
					: (cap ? Color1 : Color0).lerp((cap ? Color0 : Color1), float((1 - std::cos(angle)) * 0.5)));
				CheckColor(mesh.vertices[cap * capVertices + 1 + i].color, expected);
			}
		}
	}
}

TEST_CASE("Vertex2DBuilder.arc_vertices_strip")
{
	constexpr float Thickness = 8.0f;
	for (const float innerRadius : { 0.0f, 37.0f })
	for (const float scale : { 0.0f, 1.0f, 512.0f })
	for (const float start : { -0.37f, 7.4f })
	for (const float sweep : { -Math::TwoPiF, -4.3f, -0.003f, 0.0f, 0.003f, Math::PiF, Math::TwoPiF })
	for (const auto cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const auto direction : { ColorFillDirection::InOut, ColorFillDirection::LeftRight })
	for (const Float4 color1 : { Color0, Color1 })
	{
		CAPTURE(innerRadius, scale, start, sweep, static_cast<int32>(cap), static_cast<int32>(direction), color1);
		// Zero-sweep Flat and Round arcs have no body; their behavior is covered separately.
		if ((sweep == 0.0f) && (cap != LineCap::Square)) continue;
		const bool square = ((cap == LineCap::Square) && (std::abs(sweep) < Math::TwoPiF));
		const bool round = (cap == LineCap::Round);
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircleArc(allocate, cap, Center, innerRadius, start, sweep, Thickness,
				direction, Color0, color1, scale);
		});
		const size_t points = ((sweep == 0.0f) ? 1
			: Vertex2DBuilder::CalculateCircleArcQuality((innerRadius + Thickness) * scale, std::abs(sweep)));
		REQUIRE(mesh.vertices.size() >= (points * 2));
		const size_t firstVertex = (round ? ((mesh.vertices.size() - points * 2) / 2) : (square ? 2 : 0));
		if (round) REQUIRE(firstVertex >= 6);
		const size_t firstIndex = (round ? ((firstVertex - 2) * 3) : (square ? 6 : 0));
		REQUIRE(mesh.vertices.size() == (points * 2 + firstVertex * 2));
		REQUIRE(mesh.indices.size() == ((points - 1) * 6 + firstIndex * 2));
		const double angle = std::abs(double(sweep));
		const double arcStart = (double(start) + Min(sweep, 0.0f));
		const double length = ((innerRadius + Thickness * 0.5) * angle);
		const Float4 startColor = ((sweep < 0.0f) ? color1 : Color0);
		const Float4 endColor = ((sweep < 0.0f) ? Color0 : color1);
		for (size_t i = 0; i < points; ++i)
		{
			const double u = ((points == 1) ? 0.5 : (double(i) / (points - 1)));
			const Vec2 radial = Unit(arcStart + angle * u);
			const auto& inner = mesh.vertices[firstVertex + i * 2];
			const auto& outer = mesh.vertices[firstVertex + i * 2 + 1];
			CheckPosition(inner.pos, Vec2{ Center } + radial * innerRadius);
			CheckPosition(outer.pos, Vec2{ Center } + radial * (innerRadius + Thickness));
			// Cap extensions participate in the length used for an along-arc gradient.
			const double t = ((square || round) ? ((Thickness * 0.5 + length * u) / (length + Thickness)) : u);
			const Float4 along = startColor.lerp(endColor, static_cast<float>(t));
			CheckColor(inner.color, ((direction == ColorFillDirection::LeftRight) ? along : Color0));
			CheckColor(outer.color, ((direction == ColorFillDirection::LeftRight) ? along : color1));
		}
		// Square cap pairs are part of the same strip; Round caps have separate fans.
		const size_t stripVertex = (square ? 0 : firstVertex);
		const size_t stripIndex = (square ? 0 : firstIndex);
		const size_t pairs = (points + (square ? 2 : 0));
		for (size_t i = 0; i + 1 < pairs; ++i)
		{
			constexpr size_t Offsets[]{ 0, 1, 2, 2, 1, 3 };
			for (size_t j = 0; j < 6; ++j)
			{
				CHECK(mesh.indices[stripIndex + i * 6 + j] == (37 + stripVertex + i * 2 + Offsets[j]));
			}
		}
		if (square)
		{
			for (size_t end = 0; end < 2; ++end)
			{
				const Vec2 radial = Unit(arcStart + end * angle);
				const Vec2 extension = (Vec2{ -radial.y, radial.x } * (Thickness * (end ? 0.5 : -0.5)));
				const size_t vertex = (end ? (mesh.vertices.size() - 2) : 0);
				CheckPosition(mesh.vertices[vertex].pos, Vec2{ Center } + radial * innerRadius + extension);
				CheckPosition(mesh.vertices[vertex + 1].pos, Vec2{ Center } + radial * (innerRadius + Thickness) + extension);
				const Float4 along = (end ? endColor : startColor);
				CheckColor(mesh.vertices[vertex].color, ((direction == ColorFillDirection::LeftRight) ? along : Color0));
				CheckColor(mesh.vertices[vertex + 1].color, ((direction == ColorFillDirection::LeftRight) ? along : color1));
			}
		}
	}
}
