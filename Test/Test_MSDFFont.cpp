//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
//
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
//
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "ImageDrawingTestHelper.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DCommon.hpp"

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

namespace
{
	constexpr std::array MSDFShaders{ EnginePS::FontMSDF, EnginePS::FontMSDF_Outline,
		EnginePS::FontMSDF_Shadow, EnginePS::FontMSDF_OutlineShadow, EnginePS::FontPrint };
	constexpr std::array MSDFStyles{ TextStyle::Default(),
		TextStyle::Outline(0.15, ColorF{ 0.1, 0.7, 0.3, 0.6 }),
		TextStyle::Shadow(Vec2{ 2, 3 }, ColorF{ 0.6, 0.1, 0.2, 0.5 }),
		TextStyle::OutlineShadow(0.15, ColorF{ 0.1, 0.7, 0.3, 0.6 }, Vec2{ 2, 3 }, ColorF{ 0.6, 0.1, 0.2, 0.5 }),
		TextStyle::Default() };
	const std::array MSDFTransforms{ Mat3x2::Scale(0.55), Mat3x2::Identity(),
		Mat3x2::Scale(1.6), Mat3x2::Scale(1.3, 0.7).rotated(29_deg) };

	template <class Draw>
	Image CaptureMSDFDraw(Draw&& draw)
	{
		const ColorF background = Scene::GetBackground();
		const ScopeExit restore{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(ColorF{ 0.08, 0.12, 0.18 });
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedViewport2D viewport{ none };
		const ScopedScissorRect2D scissor{ none };
		const ScopedColorMul2D mul{ ColorF{ 0.8, 0.6, 0.9, 0.75 }, ScopedColorMul2D::Target::Set };
		const ScopedColorAdd2D add{ ColorF{ 0.1, -0.05, 0.15 }, ScopedColorAdd2D::Target::Set };
		const ScopedRenderStates2D states{ BlendState::Default2D, RasterizerState::Default2D, SamplerState::ClampLinear };
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		Image result = ScreenCapture::GetFrame();
		REQUIRE(result.width() >= 520);
		REQUIRE(result.height() >= 360);
		return result;
	}

	void DrawMSDFGlyphs(const TextureRegion& glyph)
	{
		for (size_t style = 0; style < MSDFShaders.size(); ++style)
		{
			const ScopedCustomShader2D shader{ SIV3D_ENGINE(EngineShader)->getPS(MSDFShaders[style]) };
			Graphics2D::SetMSDFParameters(MSDFStyles[style], 24);
			for (size_t row = 0; row < MSDFTransforms.size(); ++row)
			{
				const Transformer2D transform{ MSDFTransforms[row].translated(50 + style * 100, 50 + row * 80) };
				glyph.drawAt(0, 0, ColorF{ 0.9, 0.7, 0.4, 0.8 });
			}
		}
	}
}

TEST_CASE("MSDFFont.atlas_dimensions")
{
	const Font font{ FontMethod::MSDF, 24, 8 };
	REQUIRE(font);
	for (const char32 ch : { U'A', U'あ' })
	{
		INFO("code point = " << static_cast<uint32>(ch));
		const auto glyph = font.generateMSDFGlyph(ch);
		REQUIRE(not glyph.image.isEmpty());
		REQUIRE(glyph.image.width() < 80);
		REQUIRE(glyph.image.height() < 80);
		Image reference;
		for (const Size size : { Size{ 128, 128 }, Size{ 512, 128 }, Size{ 128, 256 }, Size{ 768, 256 } })
		{
			INFO("atlas = " << size);
			// Keep glyph texels and their pixel position fixed; only atlas dimensions change.
			const Point origin{ 16, 16 };
			Image atlas{ size, Color{ 0, 0, 0, 0 } };
			glyph.image.overwrite(atlas, origin);
			const Texture texture{ atlas, TextureDesc::SDF };
			REQUIRE(texture);
			const auto region = texture(Rect{ origin, glyph.image.size() });
			const auto actual = CaptureMSDFDraw([&] { DrawMSDFGlyphs(region); });
			if (reference.isEmpty())
			{
				reference = actual;
			}
			for (int32 style = 0; style < 5; ++style)
			{
				INFO("style = " << style);
				int32 differences = 0;
				int32 visiblePixels = 0;
				for (int32 y = 10; y < 330; ++y)
				{
					for (int32 x = (style * 100); x < (style + 1) * 100; ++x)
					{
						const Color a = actual[y][x], b = reference[y][x];
						// Atlas UV normalization and filtering can differ by one RGBA8 unit.
						differences += (Abs(int32(a.r) - int32(b.r)) > 1
							|| Abs(int32(a.g) - int32(b.g)) > 1 || Abs(int32(a.b) - int32(b.b)) > 1
							|| Abs(int32(a.a) - int32(b.a)) > 1);
						visiblePixels += (a != actual[0][0]);
					}
				}
				CHECK(differences == 0);
				CHECK(visiblePixels > 100);
			}
		}
	}
}

TEST_CASE("MSDFFont.scoped_color_mul_styles")
{
	// Style PMA colors and the straight scoped multiplier retain separate shader constants.
	static_assert(sizeof(PSConstants2D) == 80);
	static_assert(offsetof(PSConstants2D, patternBackgroundColorMul) == 0);
	static_assert(offsetof(PSConstants2D, sdfOuterColorPMA) == 48);
	static_assert(offsetof(PSConstants2D, sdfShadowColorPMA) == 64);
	using Mul = ScopedColorMul2D;
	using Add = ScopedColorAdd2D;
	const Font font{ FontMethod::MSDF, 48, 8 };
	REQUIRE(font);
	const SceneConfig oldConfig = Scene::GetConfig();
	const ScopeExit restoreScene{ [&] { Scene::Configure(oldConfig); } };
	Scene::Configure(SceneConfig::Fixed({ 720, 440 }, SceneResolution::SceneSize));
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	const ColorF textColor{ 0.75, 0.5, 0.25, 0.625 };
	const ColorF outerColor{ 0.75, 0.5, 1.0, 0.5 };
	const auto style = [](const int32 variant, const ColorF& tint)
	{
		const ColorF outline = (ColorF{ 0.75, 0.25, 0.5, 0.75 } * tint);
		const ColorF shadow = (ColorF{ 0.25, 0.75, 1.0, 0.5 } * tint);
		switch (variant)
		{
		case 0: return TextStyle::Outline(0.2, outline);
		case 1: return TextStyle::Shadow(Vec2{ 4, 3 }, shadow);
		default: return TextStyle::OutlineShadow(0.2, outline, Vec2{ 4, 3 }, shadow);
		}
	};
	const auto draw = [&](const int32 column, const int32 row, const int32 variant, const ColorF& tint)
	{
		// The reference bakes tint into straight colors before TextStyle creates PMA.
		font(U"A").draw(style(variant, tint), Vec2{ 20 + column * 85, 20 + row * 100 }, textColor * tint);
	};
	const auto capture = [&](auto render, const Float3& add)
	{
		return CaptureMSDFDraw([&]
		{
			const Mul resetMul{ 1.0, Mul::Target::Set };
			const Add resetAdd{ add, Add::Target::Set };
			render();
		});
	};
	const std::array effective{
		ColorF{ 1.0 }, outerColor, outerColor * ColorF{ 1.0, 0.5 }, outerColor * ColorF{ 1.0, 0.0 },
		outerColor * ColorF{ 1.0, 0.5 }, ColorF{ 1.0 }, outerColor, ColorF{ 1.0 } };
	for (const Float3 add : { Float3{ 0, 0, 0 }, Float3{ 0.125f, -0.0625f, 0.25f } })
	{
		INFO("ColorAdd = " << add);
		const Image reference = capture([&]
		{
			for (int32 row = 0; row < 3; ++row)
			{
				for (int32 column = 0; column < 8; ++column)
				{
					draw(column, row, row, effective[column]);
				}
			}
			for (int32 column = 0; column < 8; ++column)
			{
				draw(column, 3, (column % 3), outerColor);
			}
		}, add);
		REQUIRE(reference.size() == Size{ 720, 440 });
		for (int32 row = 0; row < 3; ++row)
		{
			int32 visible = 0;
			int32 transparentDifferences = 0;
			for (int32 y = (20 + row * 100); y < (110 + row * 100); ++y)
			{
				for (int32 x = 10; x < 90; ++x)
				{
					visible += (reference[y][x] != reference[0][0]);
					transparentDifferences += (reference[y][x + 3 * 85] != reference[0][0]);
				}
			}
			CHECK(visible > 100);
			CHECK(transparentDifferences == 0);
		}
		for (const bool flush : { false, true })
		{
			INFO("Flush = " << flush);
			const Image actual = capture([&]
			{
				for (int32 row = 0; row < 3; ++row)
				{
					// Reuse one style while changing only ColorMul, including restoring from zero.
					draw(0, row, row, ColorF{ 1.0 });
					{
						const Mul outer{ outerColor };
						draw(1, row, row, ColorF{ 1.0 });
						{
							const Mul inner{ 1.0, 0.5 };
							draw(2, row, row, ColorF{ 1.0 });
							{
								const Mul invisible{ 1.0, 0.0 };
								draw(3, row, row, ColorF{ 1.0 });
								if (flush) Graphics2D::Flush();
							}
							draw(4, row, row, ColorF{ 1.0 });
							{
								const Mul replace{ 1.0, Mul::Target::Set };
								draw(5, row, row, ColorF{ 1.0 });
							}
						}
						if (flush) Graphics2D::Flush();
						draw(6, row, row, ColorF{ 1.0 });
					}
					draw(7, row, row, ColorF{ 1.0 });
				}
				// Change only the style while the same non-neutral ColorMul remains active.
				const Mul outer{ outerColor };
				for (int32 column = 0; column < 8; ++column)
				{
					draw(column, 3, (column % 3), ColorF{ 1.0 });
				}
			}, add);
			CHECK_THAT(actual, s3d::ImageDrawingTest::ImageEquals(reference, 1));
			REQUIRE(actual.size() == reference.size());
			for (int32 row = 0; row < 3; ++row)
			{
				int32 transparentDifferences = 0;
				for (int32 y = (20 + row * 100); y < (110 + row * 100); ++y)
				{
					for (int32 x = (10 + 3 * 85); x < (90 + 3 * 85); ++x)
					{
						transparentDifferences += (actual[y][x] != actual[0][0]);
					}
				}
				CHECK(transparentDifferences == 0);
			}
		}
	}
}

# endif
