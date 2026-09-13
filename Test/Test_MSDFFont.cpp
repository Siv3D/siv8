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
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp"

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
		const ScopedColorMul2D mul{ ColorF{ 0.8, 0.6, 0.9, 0.75 } };
		const ScopedColorAdd2D add{ ColorF{ 0.1, -0.05, 0.15 } };
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

# endif
