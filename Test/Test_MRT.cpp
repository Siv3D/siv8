//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <type_traits>

namespace s3d
{
	namespace
	{
		struct DrawState
		{
			const ScopedRenderTarget2D scene{ none };
			const ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
			const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
			const ScopedViewport2D viewport{ none };
			const ScopedScissorRect2D scissor{ none };
			const ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
			const ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
			const ScopedRenderStates2D states{ BlendState::Opaque, RasterizerState::Default2D };
		};

		const std::array<Color, 8> Colors{ Palette::Red, Palette::Lime, Palette::Blue,
			Palette::Yellow, Palette::Magenta, Palette::Cyan, Palette::White, Palette::Black };

		PixelShader Shader(const std::span<const Float4> colors)
		{
			std::string source;
		# if SIV3D_PLATFORM(MACOS)
			source = "#include <metal_stdlib>\nusing namespace metal;\n";
		# endif
			source += "struct Output {\n";
			for (size_t i = 0; i < colors.size(); ++i)
			{
			# if SIV3D_PLATFORM(MACOS)
				source += fmt::format("float4 c{} [[color({})]];\n", i, i);
			# else
				source += fmt::format("float4 c{} : SV_Target{};\n", i, i);
			# endif
			}
			source += "};\n";
		# if SIV3D_PLATFORM(MACOS)
			source += "fragment ";
		# endif
			source += "Output Main() { Output o;\n";
			for (size_t i = 0; i < colors.size(); ++i)
			{
				const auto c = colors[i];
				source += fmt::format("o.c{} = float4({}, {}, {}, {});\n", i, c.x, c.y, c.z, c.w);
			}
			source += "return o; }\n";
		# if SIV3D_PLATFORM(MACOS)
			return PixelShader::MSL(source, U"Main");
		# else
			return PixelShader::HLSL(source, U"Main");
		# endif
		}

		PixelShader Shader(const size_t count)
		{
			std::array<Float4, 8> colors;
			for (size_t i = 0; i < count; ++i) { colors[i] = ColorF{ Colors[i] }.toFloat4(); }
			return Shader(std::span{ colors.data(), count });
		}

		Image Read(const RenderTexture& target, const uint32 mipLevel = 0)
		{
			Image result;
			REQUIRE(target.readAsImage(result, mipLevel));
			return result;
		}

		Color Pixel(const RenderTexture& target, const int32 x = 0, const int32 y = 0, const uint32 mipLevel = 0)
		{
			const Image image = Read(target, mipLevel);
			return image[y][x];
		}

		void CheckTargets(const std::span<const ColorRenderTarget> expected)
		{
			const auto& actual = SIV3D_ENGINE(Renderer2D)->getRenderTargets();
			REQUIRE(actual.count == expected.size());
			for (uint32 i = 0; i < actual.count; ++i) { CHECK(*actual.colors[i] == expected[i]); }
		}
	}

	TEST_CASE("MRT.array_list_and_single_compatibility")
	{
		STATIC_REQUIRE(std::is_constructible_v<ScopedRenderTarget2D, std::array<ColorRenderTarget, 2>>);
		STATIC_REQUIRE(std::is_constructible_v<ScopedRenderTarget2D, Array<ColorRenderTarget>>);
		STATIC_REQUIRE_FALSE(std::is_constructible_v<ScopedRenderTarget2D, std::array<Texture, 2>>);
		const DrawState state;
		const RenderTexture a{ Size{ 8, 8 }, Palette::Black }, b{ Size{ 8, 8 }, Palette::Blue };
		const std::array<ColorRenderTarget, 2> fixed{ a, b };
		const Array<ColorRenderTarget> dynamic{ a, b };
		const ColorRenderTarget raw[]{ a, b };
		{ const ScopedRenderTarget2D scope{ fixed }; CheckTargets(fixed); }
		{ const ScopedRenderTarget2D scope{ dynamic }; CheckTargets(fixed); }
		{ const ScopedRenderTarget2D scope{ raw }; CheckTargets(fixed); }
		{ const ScopedRenderTarget2D scope{ { a, b } }; CheckTargets(fixed); }
		{ const ScopedRenderTarget2D scope{ a, b }; CheckTargets(fixed); }
		{ const ScopedRenderTarget2D scope{ std::span{ fixed } }; CheckTargets(fixed); }
		{ const ScopedRenderTarget2D scope{ std::array<ColorRenderTarget, 1>{ a } }; Rect{ a.size() }.draw(Palette::Red); }
		CHECK(Read(a) == Image{ a.size(), Palette::Red });
		CHECK(Read(b) == Image{ b.size(), Palette::Blue });
		const MSRenderTexture ms{ Size{ 8, 8 }, Palette::Black };
		REQUIRE(ms);
		{
			const ScopedRenderTarget2D scope{ std::array<ColorRenderTarget, 1>{ ms } };
			Rect{ ms.size() }.draw(Palette::Red);
			ms.resolve(); // Must not flush pending draws.
			Image resolved;
			REQUIRE(ms.readAsImage(resolved));
			CHECK(resolved == Image{ ms.size(), Palette::Black });
			Graphics2D::Flush();
			ms.resolve();
			REQUIRE(ms.readAsImage(resolved));
			CHECK(resolved == Image{ ms.size(), Palette::Red });
		}
	}

	TEST_CASE("MRT.one_to_eight_outputs_and_standard_shader_preservation")
	{
		const DrawState state;
		for (size_t count = 1; count <= 8; ++count)
		{
			CAPTURE(count);
			Array<RenderTexture> textures;
			Array<ColorRenderTarget> targets;
			for (size_t i = 0; i < count; ++i)
			{
				textures.emplace_back(Size{ 8, 4 }, Palette::Magenta);
				REQUIRE(textures.back());
				targets.emplace_back(textures.back());
			}
			const PixelShader ps = Shader(count);
			REQUIRE(ps);
			{
				const ScopedRenderTarget2D scope{ targets };
				{ const ScopedCustomShader2D shader{ ps }; Rect{ 8, 4 }.draw(); }
				Rect{ 4, 4 }.draw(Palette::Cyan);
				// Repeat the custom/standard transition without changing target or blend.
				{ const ScopedCustomShader2D shader{ ps }; Rect{ 0, 0, 2, 4 }.draw(); }
				for (const auto& texture : textures) { CHECK_THROWS_AS(texture.draw(), Error); }
			}
			for (size_t i = 0; i < count; ++i)
			{
				const Image image = Read(textures[i]);
				CHECK(image[2][0] == Colors[i]);
				CHECK(image[2][3] == (i ? Colors[i] : Palette::Cyan));
				CHECK(image[2][6] == Colors[i]);
			}
		}
	}

	TEST_CASE("MRT.mixed_formats_srgb_hdr_and_layout_cache")
	{
		const DrawState state;
		const PixelShader white = Shader(std::array{ Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 } });
		REQUIRE(white);
		const RenderTexture first{ Size{ 7, 5 }, Palette::Black };
		for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
		{
			const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
			CAPTURE(format);
			const RenderTexture second{ first.size(), format }, reference{ first.size(), Palette::White, format };
			REQUIRE(second);
			for (const bool reverse : { false, true })
			{
				{ const ScopedRenderTarget2D scope{ reverse ? Array<ColorRenderTarget>{ second, first } : Array<ColorRenderTarget>{ first, second } };
				  const ScopedCustomShader2D shader{ white }; Rect{ first.size() }.draw(); }
				Array<Byte> actual(first.size().area() * format.pixelSize()), expected(actual.size());
				REQUIRE(second.read(std::span{ actual }));
				REQUIRE(reference.read(std::span{ expected }));
				CHECK(actual == expected);
			}
		}
		const RenderTexture srgb{ first.size(), TextureFormat::R8G8B8A8_Unorm_SRGB };
		const RenderTexture hdr{ first.size(), TextureFormat::R32G32B32A32_Float };
		const Float4 linear{ 0.25f, 0.5f, 0.75f, 1 }, high{ 2, 4, 8, 1 };
		const PixelShader ps = Shader(std::array{ linear, high });
		REQUIRE(ps);
		{ const ScopedRenderTarget2D scope{ srgb, hdr }; const ScopedCustomShader2D shader{ ps }; Rect{ first.size() }.draw(); }
		const Color expected = ColorF{ linear }.linearToSRGB().toColor();
		for (const Color c : Read(srgb))
		{
			CHECK(Abs(int(c.r) - expected.r) <= 1);
			CHECK(Abs(int(c.g) - expected.g) <= 1);
			CHECK(Abs(int(c.b) - expected.b) <= 1);
		}
		Grid<Float4> pixels;
		REQUIRE(hdr.read(pixels));
		for (const auto p : pixels) { CHECK(p == high); }
	}

	TEST_CASE("MRT.nesting_move_viewport_order_and_lifetime")
	{
		const DrawState state;
		RenderTexture a{ Size{ 8, 8 }, Palette::Black }, b{ a.size(), Palette::Black };
		const RenderTexture small{ Size{ 4, 4 }, Palette::Black };
		const MSRenderTexture ms{ Size{ 4, 4 }, Palette::Black };
		REQUIRE(ms);
		const RenderTexture keepA = a, keepB = b;
		const std::array<ColorRenderTarget, 2> expected{ a, b };
		const PixelShader ps = Shader(2);
		REQUIRE(ps);
		const ScopedViewport2D sceneViewport{ Rect{ 2, 3, 9, 7 } };
		{
			ScopedRenderTarget2D original{ Array<ColorRenderTarget>{ a, b } };
			a.release(); b.release();
			const ScopedRenderTarget2D moved{ std::move(original) };
			CheckTargets(expected);
			CHECK(Graphics2D::GetRenderTargetSize() == Size{ 8, 8 });
			CHECK(Graphics2D::GetDrawingSize() == Size{ 8, 8 });
			{ const ScopedCustomShader2D shader{ ps }; Rect{ 8, 8 }.draw(); }
			{
				const ScopedViewport2D viewport{ Rect{ 1, 2, 4, 5 } };
				{ const ScopedRenderTarget2D single{ small }; Rect{ small.size() }.draw(Palette::Blue); }
				CheckTargets(expected);
				CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 4, 5 });
				{ const ScopedRenderTarget2D single{ ms }; Rect{ ms.size() }.draw(Palette::Yellow); }
				CheckTargets(expected);
				{ const ScopedRenderTarget2D scene{ none }; CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTargets()); }
				CheckTargets(expected);
				{
					const ScopedRenderTarget2D reversed{ keepB, keepA };
					CHECK_FALSE(Graphics2D::GetViewport());
					const ScopedCustomShader2D shader{ ps };
					Rect{ 4, 8 }.draw();
				}
				CheckTargets(expected);
				CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 4, 5 });
			}
			Rect{ 0, 0, 2, 8 }.draw(Palette::Blue);
		}
		CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTargets());
		CHECK(Graphics2D::GetViewport() == Rect{ 2, 3, 9, 7 });
		const Image imageA = Read(keepA), imageB = Read(keepB);
		CHECK(imageA[4][0] == Palette::Blue);
		CHECK(imageA[4][3] == Palette::Lime);
		CHECK(imageA[4][6] == Palette::Red);
		CHECK(imageB[4][0] == Palette::Red);
		CHECK(imageB[4][6] == Palette::Lime);
		CHECK(Read(small) == Image{ small.size(), Palette::Blue });
		Image resolved;
		REQUIRE(ms.readAsImage(resolved));
		CHECK(resolved == Image{ ms.size(), Palette::Black }); // No automatic resolve on switching.
		ms.resolve();
		REQUIRE(ms.readAsImage(resolved));
		CHECK(resolved == Image{ ms.size(), Palette::Yellow });
	}

	TEST_CASE("MRT.scope_retains_temporary_resources_across_nested_scope")
	{
		const DrawState state;
		const PixelShader ps = Shader(2);
		REQUIRE(ps);
		const ScopedRenderTarget2D outer{ RenderTexture{ Size{ 8, 8 } }, RenderTexture{ Size{ 8, 8 } } };
		{
			const ScopedRenderTarget2D nested{ RenderTexture{ Size{ 4, 4 } } };
			Rect{ 4, 4 }.draw();
		}
		{ const ScopedCustomShader2D shader{ ps }; Rect{ 8, 8 }.draw(); }
		const Texture a = SIV3D_ENGINE(Renderer2D)->getRenderTargets().colors[0]->getTexture();
		const Texture b = SIV3D_ENGINE(Renderer2D)->getRenderTargets().colors[1]->getTexture();
		const RenderTexture copy{ Size{ 16, 8 } };
		{
			const ScopedRenderTarget2D target{ copy };
			a.draw(); b.draw(8, 0);
		}
		const Image image = Read(copy);
		CHECK(image[4][4] == Palette::Red);
		CHECK(image[4][12] == Palette::Lime);
	}

	TEST_CASE("MRT.invalid_sets_preserve_state_and_feedback_checks_every_output")
	{
		const DrawState state;
		const RenderTexture a{ Size{ 8, 8 }, Palette::Black }, b{ a.size(), Palette::Black }, c{ a.size(), Palette::Black };
		const RenderTexture wrongSize{ Size{ 4, 8 } };
		const MSRenderTexture ms{ a.size() }, otherMS{ a.size() };
		REQUIRE(ms);
		REQUIRE(otherMS);
		const std::array<ColorRenderTarget, 2> expected{ a, b };
		const ScopedRenderTarget2D target{ expected };
		const ScopedViewport2D viewport{ Rect{ 0, 0, 4, 4 } };
		const std::array<Array<ColorRenderTarget>, 6> invalid{
			Array<ColorRenderTarget>{}, { a, ColorRenderTarget{} }, { a, a }, { a, wrongSize }, { a, ms }, { ms, otherMS }
		};
		for (const auto& set : invalid)
		{
			CHECK_THROWS_AS(ScopedRenderTarget2D{ set }, Error);
			CheckTargets(expected);
			CHECK(Graphics2D::GetViewport() == Rect{ 0, 0, 4, 4 });
		}
		Array<ColorRenderTarget> tooMany;
		for (size_t i = 0; i < 9; ++i) { tooMany.emplace_back(RenderTexture{ a.size() }); }
		CHECK_THROWS_AS(ScopedRenderTarget2D{ tooMany }, Error);
		for (int i = 0; i < 3; ++i)
		{
			CHECK_THROWS_AS(a.draw(), Error);
			CHECK_THROWS_AS(ColorRenderTarget{ b }.getTexture().draw(), Error);
			CHECK_NOTHROW(c.draw());
			CHECK_NOTHROW(c.draw());
			{ const ScopedRenderTarget2D inner{ c }; CHECK_NOTHROW(b.draw()); }
			CHECK_THROWS_AS(b.draw(), Error); // Invalidate a successful input cached in the inner scope.
		}
	}

	TEST_CASE("MRT.viewport_and_common_channel_mask")
	{
		const DrawState state;
		const RenderTexture a{ Size{ 8, 8 }, Palette::Blue }, b{ a.size(), Palette::Blue };
		const PixelShader ps = Shader(2);
		REQUIRE(ps);
		BlendState mask = BlendState::Opaque;
		mask.writeB = false;
		{
			const ScopedRenderTarget2D targets{ a, b };
			const ScopedViewport2D viewport{ Rect{ 2, 1, 4, 3 } };
			const ScopedRenderStates2D states{ mask };
			const ScopedCustomShader2D shader{ ps };
			Rect{ 4, 3 }.draw();
		}
		const Image imageA = Read(a), imageB = Read(b);
		for (int32 y = 0; y < 8; ++y)
		{
			for (int32 x = 0; x < 8; ++x)
			{
				const bool inside = ((2 <= x) && (x < 6) && (1 <= y) && (y < 4));
				CHECK(imageA[y][x] == (inside ? Palette::Magenta : Palette::Blue));
				CHECK(imageB[y][x] == (inside ? Palette::Cyan : Palette::Blue));
			}
		}
	}

	TEST_CASE("MRT.blending_clear_mips_read_and_frame_continuation")
	{
		const DrawState state;
		const auto oldSamples = Scene::GetMSAASampleCount();
		const ScopeExit restore{ [&] { Scene::SetMSAASampleCount(oldSamples); } };
		const PixelShader ps = Shader(std::array{ Float4{ 0.5f, 0, 0, 0.5f }, Float4{ 0, 0.5f, 0, 0.5f } });
		REQUIRE(ps);
		for (const auto samples : { MSAASampleCount::NoMSAA, MSAASampleCount::X4MSAA })
		{
			Scene::SetMSAASampleCount(samples);
			REQUIRE(System::Update());
			const RenderTexture a{ Size{ 8, 8 }, Palette::Black, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
			const RenderTexture b{ a.size(), Palette::Blue };
			const std::array<ColorRenderTarget, 2> targets{ a, b };
			const ScopedRenderTarget2D scope{ targets };
			const ScopedRenderStates2D blend{ BlendState::Default2D };
			{ const ScopedCustomShader2D shader{ ps }; Rect{ a.size() }.draw(); }
			const Color left = Pixel(a), right = Pixel(b);
			CHECK(Abs(int(left.r) - 128) <= 1);
			CHECK(left.g == 0);
			CHECK(Abs(int(right.g) - 128) <= 1);
			CHECK(Abs(int(right.b) - 128) <= 1);
			CHECK(Read(a, 1) == Image{ Size{ 4, 4 }, Palette::Black });
			a.clear(Palette::White); // Keeps B intact and restores the whole set.
			CHECK(Pixel(b) == right);
			CheckTargets(targets);
			Rect{ 4, 8 }.draw(Palette::Red);
			a.generateMips();
			CHECK(Pixel(a, 0, 0, 1) == Palette::Red);
			CHECK(Pixel(a, 3, 0, 1) == Palette::White);
			CHECK(Pixel(b) == right);
			REQUIRE(System::Update());
			CheckTargets(targets);
			CHECK(SIV3D_ENGINE(Renderer2D)->getSceneScale() == Float2{ 1, 1 });
			{ const ScopedCustomShader2D shader{ ps }; Rect{ 4, 0, 4, 8 }.draw(); }
			const Image finalA = Read(a), finalB = Read(b);
			CHECK(finalA[0][0] == Palette::Red);
			CHECK(Abs(int(finalA[0][6].g) - 128) <= 1);
			CHECK(Abs(int(finalB[0][6].g) - 191) <= 1);
		}
	}
}
