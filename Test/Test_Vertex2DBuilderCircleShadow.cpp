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
	const Float2 Center{ 17.25f, -13.5f };
	const Float4 ShadowColor{ 0.1f, 0.6f, 0.2f, 0.35f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
		Index offset = 0;
	};

	Mesh Capture(const float radius, const float blur, const float scale, const bool fill, const bool atVertexLimit)
	{
		Mesh mesh;
		Vertex2D guard;
		guard.set(-123, -456, -7, -8, Float4{ -1, -2, -3, -4 });
		uint32 requests = 0;
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++requests;
			mesh.vertices.assign((vertices + size_t{ 2 }), guard);
			mesh.indices.assign((indices + size_t{ 2 }), Index{ 65535 });
			mesh.offset = (atVertexLimit ? static_cast<Index>(65536 - vertices) : Index{ 37 });
			return Vertex2DBufferPointer{ (mesh.vertices.data() + 1), (mesh.indices.data() + 1), mesh.offset };
		};
		const uint32 count = Vertex2DBuilder::BuildCircleShadow(allocate, Circle{ Center, radius }, blur, ShadowColor, scale, fill);
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
		for (const auto index : mesh.indices)
		{
			REQUIRE(mesh.offset <= index);
			REQUIRE(index < (mesh.offset + mesh.vertices.size()));
		}
		return mesh;
	}
}

TEST_CASE("Vertex2DBuilder.circle_shadow_ring_vertices_and_fill")
{
	struct Fixture { float radius, blur, scale; size_t pairs; };
	const Fixture fixtures[]{ { 0, 0, 1, 8 }, { 1, 0, 1, 8 }, { 1, 0.5f, 1, 8 },
		{ 3, 2, 1, 12 }, { 3, 2.02f, 1, 16 }, { 4, 8, 1, 20 },
		{ 37, 12, 0, 8 }, { 37, 12, 1, 36 }, { 1, 0.5f, 8, 20 }, { 1, 0.5f, 512, 252 } };
	for (const auto& [radius, blur, scale, pairs] : fixtures)
	for (const bool atVertexLimit : { false, true })
	{
		CAPTURE(radius, blur, scale, atVertexLimit);
		const auto ring = Capture(radius, blur, scale, false, atVertexLimit);
		const auto filled = Capture(radius, blur, scale, true, atVertexLimit);
		REQUIRE(ring.vertices.size() == (pairs * 2));
		REQUIRE(filled.vertices.size() == (pairs * 2 + 1));
		REQUIRE(ring.indices.size() == (pairs * 6));
		REQUIRE(filled.indices.size() == (pairs * 9));
		CHECK(filled.vertices[0].pos == Center);
		CHECK(filled.vertices[0].tex == Float2{ 0.5f, 0.5f });
		CHECK(filled.vertices[0].color == ShadowColor);

		for (size_t i = 0; i < pairs; ++i)
		{
			const double angle = (Math::TwoPi * i / pairs);
			for (size_t side = 0; side < 2; ++side)
			{
				const double r = (radius + (side ? -0.5 : 0.5) * blur);
				const Vec2 expected = (Vec2{ Center } + Vec2{ std::sin(angle), -std::cos(angle) } * r);
				const size_t vertexIndex = (i * 2 + side);
				const auto& vertex = ring.vertices[vertexIndex];
				CHECK(std::abs(vertex.pos.x - expected.x) < 0.0001);
				CHECK(std::abs(vertex.pos.y - expected.y) < 0.0001);
				CHECK(vertex.tex == Float2{ (side ? 0.5f : 0.0f), 0.5f });
				CHECK(vertex.color == ShadowColor);
				CHECK(filled.vertices[vertexIndex + 1].pos == vertex.pos);
				CHECK(filled.vertices[vertexIndex + 1].tex == vertex.tex);
				CHECK(filled.vertices[vertexIndex + 1].color == vertex.color);
			}
			const size_t firstIndex = (pairs * 6 + i * 3);
			CHECK(filled.indices[firstIndex] == filled.offset);
			CHECK(filled.indices[firstIndex + 1] == (filled.offset + i * 2 + 2));
			CHECK(filled.indices[firstIndex + 2] == (filled.offset + ((i + 1) % pairs) * 2 + 2));
		}
		for (size_t i = 0; i < ring.indices.size(); ++i)
		{
			CHECK((filled.indices[i] - filled.offset) == (ring.indices[i] - ring.offset + 1));
		}
	}
}
