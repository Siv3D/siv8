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

		template <class Target>
		Image Read(const Target& target, const uint32 mipLevel = 0)
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
		const MSRenderTexture ms{ a.size(), MSAASampleCount::X2MSAA }, otherMS{ a.size(), MSAASampleCount::X4MSAA };
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

	TEST_CASE("MRT.MSAA.outputs_and_independent_explicit_resolve")
	{
		const DrawState state;
		for (const size_t count : { size_t{ 2 }, size_t{ 8 } })
		{
			const PixelShader ps = Shader(count);
			REQUIRE(ps);
			for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
			{
				CAPTURE(count, samples);
				Array<MSRenderTexture> textures;
				Array<ColorRenderTarget> targets;
				for (size_t i = 0; i < count; ++i)
				{
					textures.emplace_back(Size{ 8, 4 }, Palette::Magenta, samples);
					REQUIRE(textures.back());
					targets.emplace_back(textures.back());
				}
				{
					const ScopedRenderTarget2D scope{ targets };
					{ const ScopedCustomShader2D shader{ ps }; Rect{ 8, 4 }.draw(); }
					{ const ScopedRenderTarget2D same{ std::span{ targets } }; }
					for (const auto& target : targets) { target.resolve(); }
					// Neither entering the identical set nor resolving submits queued draws.
					// Readback submits them, but must not update any resolve result.
					for (const auto& texture : textures) { CHECK(Read(texture) == Image{ texture.size(), Palette::Magenta }); }
					Graphics2D::Flush();
					targets[0].resolve();
					CHECK(Read(textures[0]) == Image{ Size{ 8, 4 }, Colors[0] });
					for (size_t i = 1; i < count; ++i) { CHECK(Read(textures[i]) == Image{ Size{ 8, 4 }, Palette::Magenta }); }
					for (const auto& target : targets) { target.resolve(); }
					for (size_t i = 0; i < count; ++i) { CHECK(Read(textures[i]) == Image{ Size{ 8, 4 }, Colors[i] }); }
					// A standard shader writes only output zero, including in an MSAA pass.
					Rect{ 4, 4 }.draw(Palette::Cyan);
					Graphics2D::Flush();
					for (const auto& target : targets) { target.resolve(); }
					const Image first = Read(textures[0]);
					CHECK(first[2][1] == Palette::Cyan);
					CHECK(first[2][6] == Colors[0]);
					for (size_t i = 1; i < count; ++i) { CHECK(Read(textures[i]) == Image{ Size{ 8, 4 }, Colors[i] }); }
					Rect{ 4, 4 }.draw(Palette::Blue);
				}
				const Image beforeResolve = Read(textures[0]);
				CHECK(beforeResolve[2][1] == Palette::Cyan); // Scope exit does not resolve.
				textures[0].resolve();
				const Image afterResolve = Read(textures[0]);
				CHECK(afterResolve[2][1] == Palette::Blue);
				CHECK(afterResolve[2][6] == Colors[0]);
			}
		}
	}

	TEST_CASE("MRT.MSAA.coverage_and_sample_count_changes")
	{
		const DrawState state;
		const PixelShader ps = Shader(2);
		REQUIRE(ps);
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA, MSAASampleCount::X2MSAA })
		{
			const MSRenderTexture a{ Size{ 16, 16 }, Palette::Black, samples }, b{ a.size(), Palette::Black, samples };
			REQUIRE(a); REQUIRE(b);
			{
				const ScopedRenderTarget2D scope{ { a, b } };
				const ScopedCustomShader2D shader{ ps };
				Triangle{ Vec2{ 1.2, 1.1 }, Vec2{ 14.6, 2.3 }, Vec2{ 2.7, 14.5 } }.draw();
			}
			a.resolve(); b.resolve();
			const Image imageA = Read(a), imageB = Read(b);
			CHECK(imageA[4][4] == Palette::Red);
			CHECK(imageB[4][4] == Palette::Lime);
			CHECK(imageA[15][15] == Palette::Black);
			CHECK(imageB[15][15] == Palette::Black);
			size_t intermediate = 0;
			for (size_t i = 0; i < imageA.pixelCount(); ++i)
			{
				const auto ca = imageA.data()[i], cb = imageB.data()[i];
				CHECK(ca.r == cb.g);
				if ((0 < ca.r) && (ca.r < 255)) { ++intermediate; }
			}
			CHECK(intermediate > 0);
		}
	}

	TEST_CASE("MRT.MSAA.nesting_reorder_move_and_temporary_lifetime")
	{
		const DrawState state;
		const PixelShader ps = Shader(2);
		REQUIRE(ps);
		const MSRenderTexture a{ Size{ 8, 8 }, Palette::Black, MSAASampleCount::X2MSAA }, b{ a.size(), Palette::Black, MSAASampleCount::X2MSAA };
		const MSRenderTexture c{ Size{ 4, 4 }, Palette::Black, MSAASampleCount::X4MSAA }, d{ c.size(), Palette::Black, MSAASampleCount::X4MSAA };
		const RenderTexture single{ c.size(), Palette::Black }, other{ c.size(), Palette::Black };
		REQUIRE(a); REQUIRE(b); REQUIRE(c); REQUIRE(d);
		const std::array<ColorRenderTarget, 2> targets{ a, b };
		const ScopedViewport2D sceneViewport{ Rect{ 1, 2, 3, 4 } };
		{
			ScopedRenderTarget2D original{ targets };
			const ScopedRenderTarget2D moved{ std::move(original) };
			{ const ScopedCustomShader2D shader{ ps }; Rect{ a.size() }.draw(); }
			{
				const ScopedViewport2D viewport{ Rect{ 2, 1, 4, 5 } };
				{
					const ScopedRenderTarget2D inner{ c, d };
					{ const ScopedCustomShader2D shader{ ps }; Rect{ c.size() }.draw(); }
					{
						const ScopedRenderTarget2D nested{ single, other };
						const ScopedCustomShader2D shader{ ps }; Rect{ single.size() }.draw();
					}
					{ const ScopedRenderTarget2D one{ single }; Rect{ 1, 4 }.draw(Palette::Blue); }
					{ const ScopedRenderTarget2D one{ c }; Rect{ 1, 4 }.draw(Palette::Blue); }
					{ const ScopedRenderTarget2D scene{ none }; CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTargets()); }
				}
				CheckTargets(targets);
				CHECK(Graphics2D::GetViewport() == Rect{ 2, 1, 4, 5 });
				CHECK(Graphics2D::GetRenderTargetSize() == a.size());
				{
					const ScopedRenderTarget2D reordered{ b, a };
					CHECK_FALSE(Graphics2D::GetViewport());
					const ScopedCustomShader2D shader{ ps }; Rect{ 4, 8 }.draw();
				}
				CheckTargets(targets);
				CHECK(Graphics2D::GetViewport() == Rect{ 2, 1, 4, 5 });
			}
		}
		CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 3, 4 });
		for (const auto& texture : { a, b, c, d }) { CHECK(Read(texture) == Image{ texture.size(), Palette::Black }); texture.resolve(); }
		const Image imageA = Read(a), imageB = Read(b), imageC = Read(c);
		CHECK(imageA[4][2] == Palette::Lime);
		CHECK(imageA[4][6] == Palette::Red);
		CHECK(imageB[4][2] == Palette::Red);
		CHECK(imageB[4][6] == Palette::Lime);
		CHECK(imageC[2][0] == Palette::Blue);
		CHECK(imageC[2][2] == Palette::Red);
		CHECK(Read(d) == Image{ d.size(), Palette::Lime });
		{
			const ScopedRenderTarget2D temporary{ MSRenderTexture{ Size{ 8, 8 } }, MSRenderTexture{ Size{ 8, 8 } } };
			{ const ScopedRenderTarget2D nested{ single }; }
			{ const ScopedCustomShader2D shader{ ps }; Rect{ 8, 8 }.draw(); }
			Graphics2D::Flush();
			const auto saved = SIV3D_ENGINE(Renderer2D)->getRenderTargets();
			for (uint32 i = 0; i < saved.count; ++i) { saved.colors[i]->resolve(); }
			const RenderTexture copy{ Size{ 16, 8 } };
			{ const ScopedRenderTarget2D output{ copy }; saved.colors[0]->getTexture().draw(); saved.colors[1]->getTexture().draw(8, 0); }
			const Image image = Read(copy);
			CHECK(image[4][4] == Palette::Red);
			CHECK(image[4][12] == Palette::Lime);
		}
	}

	TEST_CASE("MRT.MSAA.read_clear_mips_and_frame_continuation")
	{
		const DrawState state;
		const auto oldSamples = Scene::GetMSAASampleCount();
		const ScopeExit restore{ [&] { Scene::SetMSAASampleCount(oldSamples); } };
		const PixelShader ps = Shader(2);
		REQUIRE(ps);
		for (const auto sceneSamples : { MSAASampleCount::NoMSAA, MSAASampleCount::X4MSAA })
		{
			Scene::SetMSAASampleCount(sceneSamples);
			REQUIRE(System::Update());
			for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
			{
				const MSRenderTexture a{ Size{ 8, 8 }, Palette::Black, samples, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
				const MSRenderTexture b{ a.size(), Palette::Black, samples };
				REQUIRE(a); REQUIRE(b);
				const std::array<ColorRenderTarget, 2> targets{ a, b };
				const ScopedRenderTarget2D scope{ targets };
				{ const ScopedCustomShader2D shader{ ps }; Rect{ a.size() }.draw(); }
				Graphics2D::Flush();
				a.resolve(); b.resolve();
				CHECK(Read(a, 1) == Image{ Size{ 4, 4 }, Palette::Black });
				a.clear(Palette::Blue);
				a.generateMips(); // Uses the preceding red resolve result, not the blue MSAA image.
				CHECK(Read(a) == Image{ a.size(), Palette::Red });
				CHECK(Read(a, 1) == Image{ Size{ 4, 4 }, Palette::Red });
				CHECK(Read(b) == Image{ b.size(), Palette::Lime });
				a.resolve();
				CHECK(Read(a) == Image{ a.size(), Palette::Blue });
				for (int frame = 0; frame < 2; ++frame)
				{
					{ const ScopedCustomShader2D shader{ ps }; Rect{ 4, 0, 4, 8 }.draw(); }
					REQUIRE(System::Update());
					CheckTargets(targets);
					CHECK(SIV3D_ENGINE(Renderer2D)->getSceneScale() == Float2{ 1, 1 });
					const Image stale = Read(a);
					CHECK(stale[4][6] == (frame ? Palette::Red : Palette::Blue));
					a.resolve(); b.resolve();
					const Image image = Read(a);
					CHECK(image[4][2] == (frame ? Palette::White : Palette::Blue));
					CHECK(image[4][6] == Palette::Red);
					CHECK(Read(b) == Image{ b.size(), Palette::Lime });
					Rect{ 4, 8 }.draw(Palette::White);
				}
				Graphics2D::Flush();
				a.resolve(); a.generateMips();
				const Image mip = Read(a, 1);
				CHECK(mip[0][0] == Palette::White);
				CHECK(mip[0][3] == Palette::Red);
			}
		}
	}

	TEST_CASE("MRT.MSAA.resolved_inputs_and_feedback_restoration")
	{
		const DrawState state;
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			const RenderTexture single{ Size{ 8, 4 }, Palette::Black }, other{ single.size(), Palette::Black };
			const MSRenderTexture a{ single.size(), Palette::Red, samples }, b{ single.size(), Palette::Blue, samples };
			REQUIRE(a); REQUIRE(b);
			const ScopedRenderTarget2D outer{ single, other };
			{
				const ScopedRenderTarget2D scope{ a, b };
				b.clear(Palette::Lime);
				CHECK_NOTHROW(ColorRenderTarget{ b }.getTexture().draw());
				Graphics2D::Flush();
				a.resolve(); b.resolve();
				CHECK(Read(a) == Image{ a.size(), Palette::Blue });
				CHECK(Read(b) == Image{ b.size(), Palette::Lime });
				b.clear(Palette::Yellow);
				b.draw(); // Queued sampling must observe the following resolve without an implicit Flush.
				b.resolve();
				Graphics2D::Flush();
				a.resolve();
				CHECK(Read(a) == Image{ a.size(), Palette::Yellow });
				CHECK_NOTHROW(a.draw());
				CHECK_NOTHROW(other.draw(-8, 0)); // Successful input must not remain cached after restore.
			}
			CHECK_THROWS_AS(single.draw(), Error);
			CHECK_THROWS_AS(other.draw(), Error);
			CHECK_NOTHROW(a.draw());
		}
	}

	TEST_CASE("MRT.MSAA.srgb_hdr_and_common_blend")
	{
		const DrawState state;
		const Float4 linear{ 0.25f, 0.5f, 0.75f, 1 }, high{ 2, 4, 8, 1 };
		const PixelShader opaque = Shader(std::array{ linear, high });
		const PixelShader translucent = Shader(std::array{ Float4{ 0.25f, 0.25f, 0.25f, 0.5f }, Float4{ 0.25f, 0.25f, 0.25f, 0.5f } });
		REQUIRE(opaque); REQUIRE(translucent);
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			const MSRenderTexture srgb{ Size{ 7, 5 }, samples, TextureFormat::R8G8B8A8_Unorm_SRGB };
			const MSRenderTexture hdr{ srgb.size(), samples, TextureFormat::R16G16B16A16_Float };
			REQUIRE(srgb); REQUIRE(hdr);
			const ScopedRenderTarget2D targets{ srgb, hdr };
			{
				const ScopedCustomShader2D shader{ opaque };
				Rect{ srgb.size() }.draw();
			}
			Graphics2D::Flush();
			srgb.resolve(); hdr.resolve();
			const Color expected = ColorF{ linear }.linearToSRGB().toColor();
			for (const auto pixel : Read(srgb))
			{
				CHECK(Abs(int(pixel.r) - expected.r) <= 1);
				CHECK(Abs(int(pixel.g) - expected.g) <= 1);
				CHECK(Abs(int(pixel.b) - expected.b) <= 1);
			}
			Grid<std::array<HalfFloat, 4>> pixels;
			REQUIRE(hdr.read(pixels));
			for (const auto pixel : pixels)
			{
				CHECK(float(pixel[0]) == high.x);
				CHECK(float(pixel[1]) == high.y);
				CHECK(float(pixel[2]) == high.z);
			}
			{
				const ScopedRenderStates2D blend{ BlendState::Default2D };
				const ScopedCustomShader2D shader{ translucent };
				Rect{ srgb.size() }.draw();
			}
			Graphics2D::Flush();
			srgb.resolve(); hdr.resolve();
			const Color blended = ColorF{ 0.375, 0.5, 0.625, 1 }.linearToSRGB().toColor();
			for (const auto pixel : Read(srgb))
			{
				CHECK(Abs(int(pixel.r) - blended.r) <= 2);
				CHECK(Abs(int(pixel.g) - blended.g) <= 2);
				CHECK(Abs(int(pixel.b) - blended.b) <= 2);
			}
			REQUIRE(hdr.read(pixels));
			for (const auto pixel : pixels)
			{
				CHECK(float(pixel[0]) == 1.25f);
				CHECK(float(pixel[1]) == 2.25f);
				CHECK(float(pixel[2]) == 4.25f);
				CHECK(float(pixel[3]) == 1.0f);
			}
		}
	}
}
