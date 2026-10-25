//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"

namespace
{
	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
	};

	template <class Build>
	Mesh BuildMesh(Build&& build)
	{
		Mesh mesh;
		uint32 requests = 0;
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++requests;
			mesh.vertices.resize(vertices);
			mesh.indices.resize(indices);
			return Vertex2DBufferPointer{ mesh.vertices.data(), mesh.indices.data(), 37 };
		};
		const auto count = build(allocate);
		REQUIRE(count == mesh.indices.size());
		CHECK(requests == 1);
		for (const auto index : mesh.indices)
		{
			REQUIRE(37 <= index);
			REQUIRE(index < (37 + mesh.vertices.size()));
		}
		return mesh;
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D states{ BlendState::Default2D, SamplerState::ClampNearest };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	struct Captured { Image image; int64 triangles; };

	template <class Draw>
	Captured Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return{ ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}

	Texture MakeTexture()
	{
		Image image{ 16, 16, Palette::White };
		for (int32 y = 0; y < 16; ++y)
		for (int32 x = 0; x < 16; ++x)
		{
			image[y][x] = ((y < 8) ? ((x < 8) ? Palette::Red : Palette::Green)
				: ((x < 8) ? Palette::Blue : Palette::Yellow));
		}
		return Texture{ image };
	}
}

TEST_CASE("Renderer2D.textured_circle_mesh_matches_circle")
{
	const Float4 color{ 0.25f, 0.5f, 0.75f, 0.5f };
	for (const auto [radius, intervals] : { std::pair{ 0.0f, 8 }, { 1.0f, 8 }, { 2.0f, 8 },
		{ 2.01f, 12 }, { 4.0f, 12 }, { 4.01f, 16 }, { 7.99f, 16 }, { 8.0f, 20 },
		{ 15.99f, 20 }, { 16.0f, 24 }, { 80.0f, 56 }, { 471.9f, 248 }, { 472.0f, 252 }, { 10000.0f, 252 } })
	for (const float scale : { 0.25f, 1.0f, 4.0f })
	for (const float sign : { -1.0f, 1.0f })
	{
		CAPTURE(radius, scale, sign);
		const Circle circle{ 13, -7, (radius * sign) };
		const auto actual = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildTexturedCircle(allocate, circle, { 0, 0, 1, 1 }, color, scale);
		});
		const auto expected = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildCircle(allocate, circle.center, radius, ColorFillDirection::InOut, color, color, scale);
		});
		CHECK(actual.indices == expected.indices);
		REQUIRE(actual.vertices.size() == expected.vertices.size());
		CHECK(8 <= (actual.indices.size() / 3));
		CHECK((actual.indices.size() / 3) <= 252);
		if (scale == 1)
		{
			CHECK((actual.indices.size() / 3) == intervals);
		}
		for (size_t i = 0; i < actual.vertices.size(); ++i)
		{
			CHECK(actual.vertices[i].pos == expected.vertices[i].pos);
			CHECK(actual.vertices[i].color == color);
		}
		if (radius > 0)
		{
			for (size_t i = 0; i < actual.indices.size(); i += 3)
			{
				const Vec2 a = actual.vertices[actual.indices[i] - 37].pos;
				const Vec2 b = actual.vertices[actual.indices[i + 1] - 37].pos;
				const Vec2 c = actual.vertices[actual.indices[i + 2] - 37].pos;
				CHECK((b - a).cross(c - a) > 0);
			}
		}
	}
}

TEST_CASE("Renderer2D.textured_circle_uv_mapping")
{
	for (const FloatRect uv : { FloatRect{ 0, 0, 1, 1 }, FloatRect{ 0.125f, 0.25f, 0.75f, 0.875f },
		FloatRect{ 0.875f, 0.75f, 0.25f, 0.125f } })
	for (const float scale : { 0.25f, 1.0f, 16.0f })
	{
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildTexturedCircle(allocate, Circle{ 16, -8, 32 }, uv, Float4{ 1, 1, 1, 1 }, scale);
		});
		for (const auto& vertex : mesh.vertices)
		{
			const double u = (uv.left + ((vertex.pos.x - 16.0) / 64.0 + 0.5) * (uv.right - uv.left));
			const double v = (uv.top + ((vertex.pos.y + 8.0) / 64.0 + 0.5) * (uv.bottom - uv.top));
			CHECK(Abs(vertex.tex.x - u) < 0.000001);
			CHECK(Abs(vertex.tex.y - v) < 0.000001);
		}
	}
	const auto fail = [](uint16, uint32) { return Vertex2DBufferPointer{ nullptr, nullptr, 0 }; };
	CHECK(Vertex2DBuilder::BuildTexturedCircle(fail, Circle{ 0, 0, 32 }, { 0, 0, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1) == 0);
}

TEST_CASE("Renderer2D.textured_circle_transforms_match_circle")
{
	const SceneScope scene;
	const Texture white{ Image{ 2, 2, Palette::White } };
	REQUIRE(white);
	for (const double radius : { 1.0, 3.0, 6.0, 24.0, 120.0 })
	for (const auto& [local, camera] : {
		std::pair{ Mat3x2::Scale(0.5f), Mat3x2::Identity() },
		std::pair{ Mat3x2::Scale(2.0f), Mat3x2::Scale(2.0f) },
		std::pair{ Mat3x2::Rotate(45_deg), Mat3x2::Scale(4.0f, 1.0f) },
		std::pair{ Mat3x2::Scale(4.0f, 1.0f), Mat3x2::Scale(0.25f, 1.0f) } })
	{
		CAPTURE(radius, local, camera);
		const Transformer2D l{ local, Transformer2D::Target::SetLocal };
		const Transformer2D c{ camera.translated(240, 140), Transformer2D::Target::SetCamera };
		const Circle circle{ 0, 0, radius };
		const ColorF tint{ 0.5, 0.75, 1.0, 0.5 };
		const auto actual = Capture([&] { circle(white).draw(tint); });
		const auto expected = Capture([&] { circle.draw(tint); });
		CHECK(actual.triangles == expected.triangles);
		CHECK(actual.image == expected.image);
		CHECK(actual.image[140][240] != Palette::Black);
	}
}

TEST_CASE("Renderer2D.textured_circle_uniform_scaling")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	for (const double scale : { 0.5, 4.0 })
	{
		CAPTURE(scale);
		const auto actual = Capture([&]
		{
			const Transformer2D local{ Mat3x2::Scale(scale).translated(240, 140) };
			Circle{ 0, 0, 24 }(texture).draw();
		});
		const auto expected = Capture([&]
		{
			// Keep translation in the same coordinate space to isolate scale-dependent detail.
			const Transformer2D placement{ Mat3x2::Translate(240, 140) };
			Circle{ 0, 0, (24 * scale) }(texture).draw();
		});
		CHECK(actual.triangles == expected.triangles);
		CHECK(actual.image == expected.image);
		const int32 offset = static_cast<int32>(6 * scale);
		CHECK(actual.image[140 - offset][240 - offset] == Palette::Red);
		CHECK(actual.image[140 - offset][240 + offset] == Palette::Green);
		CHECK(actual.image[140 + offset][240 - offset] == Palette::Blue);
		CHECK(actual.image[140 + offset][240 + offset] == Palette::Yellow);
	}
}

TEST_CASE("Renderer2D.textured_circle_scene_resolution")
{
	const SceneScope scene;
	const Texture texture = MakeTexture();
	REQUIRE(texture);
	const Size logical{ 480, 280 };
	for (const double radius : { 1.0, 32.0, 512.0 })
	{
		CAPTURE(radius);
		const auto draw = [&]
		{
			const Transformer2D local{ Mat3x2::Scale(1.5f).translated(240, 140), Transformer2D::Target::SetLocal };
			Circle{ 0, 0, radius }(texture).draw();
		};
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::SceneSize));
		const auto base = Capture(draw);
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > logical.x);
		const auto high = Capture(draw);
		if (radius < 512)
		{
			CHECK(high.triangles > base.triangles);
		}
		else
		{
			CHECK(base.triangles == 252);
			CHECK(high.triangles == 252);
		}
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const auto expected = Capture([&]
		{
			const Transformer2D camera{ Mat3x2::Scale(Float2{ float(pixels.x) / logical.x, float(pixels.y) / logical.y }),
				Transformer2D::Target::SetCamera };
			draw();
		});
		CHECK(high.triangles == expected.triangles);
		CHECK(high.image == expected.image);
	}
}
