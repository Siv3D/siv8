//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	const ColorF Tint0{ 0.9, 0.3, 0.6, 0.2 }, Tint1{ 0.2, 0.8, 0.4, 0.9 };

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
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

	void CheckNear(const Image& actual, const Image& expected)
	{
		REQUIRE(actual.size() == expected.size());
		int32 maxDifference = 0;
		for (size_t i = 0; i < actual.pixelCount(); ++i)
		{
			const Color a = actual.data()[i], b = expected.data()[i];
			maxDifference = Max(maxDifference, Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g));
			maxDifference = Max(maxDifference, Abs(int32(a.b) - b.b), Abs(int32(a.a) - b.a));
		}
		CHECK(maxDifference <= 1);
	}

	template <class First, class Second>
	RoundRect DrawVariant(const TexturedRoundRect& textured, const int32 variant, First first, Second second)
	{
		switch (variant)
		{
		case 0:
			CHECK(&textured.draw(first, second) == &textured.rect);
			return textured.rect;
		case 1: return textured.draw(120, 60, first, second);
		case 2: return textured.draw(Vec2{ 120, 60 }, first, second);
		case 3: return textured.drawAt(240, 140, first, second);
		default: return textured.drawAt(Vec2{ 240, 140 }, first, second);
		}
	}
}

TEST_CASE("Renderer2D.textured_round_rect_gradient_overloads")
{
	const SceneScope scene;
	const Texture white{ Image{ 2, 2, Palette::White } };
	REQUIRE(white);
	const RoundRect original{ 17, 13, 96, 64, 16 };
	const auto textured = original(white);
	for (const bool vertical : { false, true })
	for (int32 variant = 0; variant < 5; ++variant)
	{
		CAPTURE(vertical, variant);
		const RoundRect expectedRect = ((variant == 0) ? original : (variant < 3)
			? original.movedBy(120, 60) : original.movedBy(240 - 48, 140 - 32));
		const auto actual = Capture([&]
		{
			const RoundRect returned = (vertical
				? DrawVariant(textured, variant, Arg::top = Tint0, Arg::bottom = Tint1)
				: DrawVariant(textured, variant, Arg::left = Tint0, Arg::right = Tint1));
			CHECK(returned == expectedRect);
			CHECK(textured.rect == original);
		});
		const auto expected = Capture([&]
		{
			if (vertical) expectedRect.draw(Arg::top = Tint0, Arg::bottom = Tint1);
			else expectedRect.draw(Arg::left = Tint0, Arg::right = Tint1);
		});
		CHECK(actual.triangles == expected.triangles);
		CheckNear(actual.image, expected.image);
		CHECK(actual.image[static_cast<int32>(expectedRect.center().y)][static_cast<int32>(expectedRect.center().x)] != Palette::Black);
	}
}

TEST_CASE("Renderer2D.textured_round_rect_gradient_uv_and_alpha")
{
	const SceneScope scene;
	Image pixels{ 8, 8 };
	for (int32 y = 0; y < 8; ++y)
	for (int32 x = 0; x < 8; ++x)
	{
		pixels[y][x] = Color{ uint8(40 + x * 24), uint8(40 + y * 24), uint8(200 - x * 16), uint8(64 + y * 24) };
	}
	const Texture texture{ pixels };
	REQUIRE(texture);
	const RoundRect rect{ 100, 60, 160, 100, 20 };
	for (const FloatRect uv : { FloatRect{ 0, 0, 1, 1 }, FloatRect{ 0.25f, 0.125f, 0.75f, 0.875f },
		FloatRect{ 0.75f, 0.875f, 0.25f, 0.125f } })
	for (const bool vertical : { false, true })
	// VS premultiplication makes simultaneous RGB/alpha changes depend on triangulation.
	for (const auto& [color0, color1] : { std::pair{ Tint0.withAlpha(0.65), Tint1.withAlpha(0.65) },
		std::pair{ Tint0.withAlpha(0.0), Tint0.withAlpha(1.0) } })
	{
		CAPTURE(uv, vertical, color0, color1);
		const TexturedRoundRect rounded{ texture, uv, rect };
		const TexturedQuad quad{ texture, uv, rect.rect.asQuad(), Float2{ rect.center() } };
		const auto actual = Capture([&]
		{
			if (vertical) rounded.draw(Arg::top = color0, Arg::bottom = color1);
			else rounded.draw(Arg::left = color0, Arg::right = color1);
		});
		const auto expected = Capture([&]
		{
			if (vertical) quad.draw(Arg::top = color0, Arg::bottom = color1);
			else quad.draw(Arg::left = color0, Arg::right = color1);
		});
		for (int32 y = 64; y < 156; y += 7)
		for (int32 x = 104; x < 256; x += 7)
		{
			// Compare the bands away from rounded corners.
			if (((x < 124) || (236 < x)) && ((y < 84) || (136 < y))) continue;
			const Color a = actual.image[y][x], b = expected.image[y][x];
			CHECK(Abs(int32(a.r) - b.r) <= 1);
			CHECK(Abs(int32(a.g) - b.g) <= 1);
			CHECK(Abs(int32(a.b) - b.b) <= 1);
			CHECK(Abs(int32(a.a) - b.a) <= 1);
		}
		CHECK(actual.image[60][100] == Palette::Black);
		CHECK(actual.image[110][180] != Palette::Black);
	}
}

TEST_CASE("Renderer2D.textured_round_rect_gradient_transforms")
{
	const SceneScope scene;
	const Texture white{ Image{ 2, 2, Palette::White } };
	REQUIRE(white);
	for (const double radius : { 0.0, 8.0, 32.0 })
	for (const bool vertical : { false, true })
	for (const auto& [local, camera] : {
		std::pair{ Mat3x2::Scale(0.5f), Mat3x2::Identity() },
		std::pair{ Mat3x2::Scale(2.0f, 0.75f), Mat3x2::Rotate(30_deg) },
		std::pair{ Mat3x2::Scale(2.0f), Mat3x2::Scale(0.5f) } })
	{
		CAPTURE(radius, vertical, local, camera);
		const Transformer2D l{ local, Transformer2D::Target::SetLocal };
		const Transformer2D c{ camera.translated(240, 140), Transformer2D::Target::SetCamera };
		const RoundRect rect{ -48, -32, 96, 64, radius };
		const auto actual = Capture([&]
		{
			if (vertical) rect(white).draw(Arg::top = Tint0, Arg::bottom = Tint1);
			else rect(white).draw(Arg::left = Tint0, Arg::right = Tint1);
		});
		const auto expected = Capture([&]
		{
			if (vertical) rect.draw(Arg::top = Tint0, Arg::bottom = Tint1);
			else rect.draw(Arg::left = Tint0, Arg::right = Tint1);
		});
		if (radius > 0) CHECK(actual.triangles == expected.triangles);
		CheckNear(actual.image, expected.image);
		CHECK(actual.image[140][240] != Palette::Black);
	}
}

TEST_CASE("Renderer2D.textured_round_rect_gradient_scene_resolution")
{
	const SceneScope scene;
	const Texture white{ Image{ 2, 2, Palette::White } };
	REQUIRE(white);
	const Size logical{ 480, 280 };
	for (const bool vertical : { false, true })
	{
		const auto draw = [&]
		{
			const Transformer2D local{ Mat3x2::Scale(1.5f).translated(240, 140), Transformer2D::Target::SetLocal };
			const RoundRect rect{ -48, -32, 96, 64, 12 };
			if (vertical) rect(white).draw(Arg::top = Tint0, Arg::bottom = Tint1);
			else rect(white).draw(Arg::left = Tint0, Arg::right = Tint1);
		};
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::SceneSize));
		const auto base = Capture(draw);
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > logical.x);
		const auto high = Capture(draw);
		CHECK(high.triangles > base.triangles);
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

TEST_CASE("Renderer2D.textured_round_rect_equal_gradient_matches_diffuse")
{
	const SceneScope scene;
	const Texture texture{ Image{ 2, 2, Color{ 160, 200, 120, 128 } } };
	REQUIRE(texture);
	const auto textured = RoundRect{ 100, 60, 160, 100, -20 }(texture);
	for (const ColorF tint : { Tint0, ColorF{ 1.0, 0.0 } })
	{
		const auto expected = Capture([&] { textured.draw(tint); });
		for (const bool vertical : { false, true })
		{
			const auto actual = Capture([&]
			{
				if (vertical) textured.draw(Arg::top = tint, Arg::bottom = tint);
				else textured.draw(Arg::left = tint, Arg::right = tint);
			});
			CHECK(actual.triangles == expected.triangles);
			CHECK(actual.image == expected.image);
		}
	}
}
