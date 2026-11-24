//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/IRenderer2D.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include <type_traits>

namespace
{
	struct DrawState
	{
		ScopedRenderTarget2D target{ none };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D blend{ BlendState::Opaque };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
	};

	Image Read(const RenderTexture& texture)
	{
		Image image;
		REQUIRE(texture.readAsImage(image));
		return image;
	}
	Color ReadPixel(const RenderTexture& texture, const int32 x, const int32 y)
	{
		const Image image = Read(texture);
		return image[y][x];
	}
}

TEST_CASE("ScopedRenderTarget2D.nesting_move_and_viewport_restore")
{
	STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<ScopedRenderTarget2D>);
	STATIC_REQUIRE(std::is_move_constructible_v<ScopedRenderTarget2D>);
	const DrawState state;
	const RenderTexture a{ Size{ 31, 17 } }, b{ Size{ 11, 7 } };
	const ScopedViewport2D outerViewport{ Rect{ 3, 4, 23, 13 } };
	CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
	{
		ScopedRenderTarget2D first{ a };
		CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
		CHECK_FALSE(Graphics2D::GetViewport());
		CHECK(Graphics2D::GetDrawingSize() == a.size());
		CHECK(Graphics2D::GetRenderTargetSize() == a.size());
		const Camera2D camera;
		CHECK(camera.getCenter() == (a.size() * 0.5));
		CHECK(camera.getRegion().size == SizeF{ a.size() });
		CHECK(camera.getMat3x2() == Mat3x2::Identity());
		const ScopedViewport2D viewport{ Rect{ 2, 1, 9, 5 } };
		{
			const ScopedRenderTarget2D nested{ b };
			CHECK(Graphics2D::GetDrawingSize() == b.size());
			CHECK_FALSE(Graphics2D::GetViewport());
			{
				const ScopedRenderTarget2D scene{ none };
				CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
				CHECK(Graphics2D::GetDrawingSize() == Scene::Size());
				CHECK(Graphics2D::GetRenderTargetSize() == Scene::RenderResolution());
			}
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == b.id());
		}
		CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
		CHECK(Graphics2D::GetViewport() == Rect{ 2, 1, 9, 5 });
		{
			const ScopedRenderTarget2D same{ a };
			CHECK_FALSE(Graphics2D::GetViewport());
			const ScopedRenderTarget2D inert;
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
		}
		CHECK(Graphics2D::GetViewport() == Rect{ 2, 1, 9, 5 });
		// Destruction of the moved scope restores Scene; the moved-from scope is inert.
		{
			const ScopedRenderTarget2D moved{ std::move(first) };
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == a.id());
		}
		CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
		CHECK(Graphics2D::GetViewport() == Rect{ 3, 4, 23, 13 });
	}
	CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
}

TEST_CASE("ScopedRenderTarget2D.draw_all_color_formats")
{
	const DrawState state;
	for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
	{
		const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
		CAPTURE(format);
		const Size size{ 13, 7 };
		const RenderTexture texture{ size, format };
		const RenderTexture reference{ size, Palette::White, format };
		REQUIRE(texture);
		{
			const ScopedRenderTarget2D target{ texture };
			Rect{ size }.draw(Palette::White);
		}
		Array<Byte> actual(size.x * size.y * format.pixelSize()), expected(actual.size());
		REQUIRE(texture.read(std::span{ actual }));
		REQUIRE(reference.read(std::span{ expected }));
		CHECK(actual == expected);
	}
	for (const auto format : { TextureFormat::R8G8B8A8_Unorm, TextureFormat::R8G8B8A8_Unorm_SRGB })
	{
		const RenderTexture texture{ Size{ 5, 3 }, format };
		const ColorF color{ 0.25, 0.5, 0.75, 1.0 };
		{
			const ScopedRenderTarget2D target{ texture };
			Rect{ texture.size() }.draw(color);
		}
		const Color expected = (TextureFormat{ format }.isSRGB() ? color.linearToSRGB() : color).toColor();
		for (const Color pixel : Read(texture))
		{
			CHECK(std::abs(int(pixel.r) - expected.r) <= 1);
			CHECK(std::abs(int(pixel.g) - expected.g) <= 1);
			CHECK(std::abs(int(pixel.b) - expected.b) <= 1);
		}
	}
	const RenderTexture hdr{ Size{ 5, 3 }, TextureFormat::R32G32B32A32_Float };
	{
		const ScopedRenderTarget2D target{ hdr };
		Rect{ hdr.size() }.draw(ColorF{ 2.0, 4.0, 8.0, 1.0 });
	}
	Grid<Float4> output;
	REQUIRE(hdr.read(output));
	for (const Float4 pixel : output) { CHECK(pixel == Float4{ 2, 4, 8, 1 }); }
}

TEST_CASE("ScopedRenderTarget2D.nested_drawing_and_ping_pong")
{
	const DrawState state;
	const RenderTexture a{ Size{ 16, 8 }, Palette::Black };
	const RenderTexture b{ Size{ 8, 8 }, Palette::Blue };
	{
		const ScopedRenderTarget2D first{ a };
		Rect{ 0, 0, 8, 8 }.draw(Palette::Red);
		{
			const ScopedRenderTarget2D second{ b };
			a.draw(4, 0);
		}
		CHECK(ReadPixel(b, 2, 4) == Palette::Blue);
		CHECK(ReadPixel(b, 6, 4) == Palette::Red);
		b.draw(8, 0);
	}
	const Image image = Read(a);
	CHECK(image[4][2] == Palette::Red);
	CHECK(image[4][10] == Palette::Blue);
	CHECK(image[4][14] == Palette::Red);
	// Repeated target changes must not clear previous contents or retain stale SRVs.
	for (int32 i = 0; i < 3; ++i)
	{
		{ const ScopedRenderTarget2D target{ b }; a.draw(); }
		{ const ScopedRenderTarget2D target{ a }; b.draw(8, 0); }
	}
	CHECK(ReadPixel(a, 14, 4) == Palette::Red);
}

TEST_CASE("ScopedRenderTarget2D.clear_read_mips_and_state_continuation")
{
	const DrawState state;
	const RenderTexture texture{ Size{ 16, 8 }, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
	{
		const ScopedRenderTarget2D target{ texture };
		Rect{ texture.size() }.draw(Palette::Red);
		texture.clear(Palette::Blue);
		Rect{ 0, 0, 8, 8 }.draw(Palette::Green);
		const auto frame = Scene::FrameCount();
		const Image intermediate = Read(texture);
		CHECK(Scene::FrameCount() == frame);
		CHECK(intermediate[4][4] == Palette::Green);
		CHECK(intermediate[4][12] == Palette::Blue);
		CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == texture.id());
		texture.generateMips();
		Image mip;
		REQUIRE(texture.readAsImage(mip, 1));
		CHECK(mip[2][2] == Palette::Green);
		CHECK(mip[2][6] == Palette::Blue);
		Rect{ 8, 0, 8, 8 }.draw(Palette::Yellow);
		Graphics2D::Flush();
		Rect{ 0, 0, 4, 4 }.draw(Palette::White);
	}
	const Image final = Read(texture);
	CHECK(final[2][2] == Palette::White);
	CHECK(final[6][6] == Palette::Green);
	CHECK(final[4][12] == Palette::Yellow);
}

TEST_CASE("ScopedRenderTarget2D.viewport_scissor_transform_and_blend")
{
	const DrawState state;
	const RenderTexture texture{ Size{ 20, 12 }, Palette::Black };
	const RenderTexture other{ Size{ 3, 3 } };
	{
		const ScopedRenderTarget2D target{ texture };
		const ScopedViewport2D viewport{ Rect{ 4, 2, 12, 8 } };
		const ScopedScissorRect2D scissor{ Rect{ 6, 3, 4, 4 } };
		const Transformer2D local{ Mat3x2::Translate(2, 1) };
		const ScopedColorMul2D color{ Palette::Red };
		Rect{ 6, 6 }.draw();
		{ const ScopedRenderTarget2D nested{ other }; Rect{ other.size() }.draw(); }
		CHECK(Graphics2D::GetViewport() == Rect{ 4, 2, 12, 8 });
		CHECK(Graphics2D::GetScissorRect() == Rect{ 6, 3, 4, 4 });
		CHECK(Graphics2D::GetLocalTransform() == Mat3x2::Translate(2, 1));
		CHECK(Graphics2D::GetColorMul() == Float4{ 1, 0, 0, 1 });
		Rect{ 6, 6 }.draw();
	}
	const Image image = Read(texture);
	CHECK(image[3][6] == Palette::Red);
	CHECK(image[6][9] == Palette::Red);
	CHECK(image[3][5] == Palette::Black);
	CHECK(image[7][9] == Palette::Black);
	{
		const ScopedRenderTarget2D target{ texture };
		const ScopedRenderStates2D blend{ BlendState::Default2D };
		Rect{ 0, 0, 4, 4 }.draw(ColorF{ 0, 1, 0, 0.5 });
	}
	CHECK(std::abs(int(ReadPixel(texture, 1, 1).g) - 128) <= 1);
}

TEST_CASE("ScopedRenderTarget2D.invalid_feedback_exception_and_lifetime")
{
	const DrawState state;
	RenderTexture texture{ Size{ 8, 8 }, Palette::Black };
	const RenderTexture alias = texture;
	{
		const ScopedRenderTarget2D target{ texture };
		texture.release();
		Rect{ 4, 8 }.draw(Palette::Red);
		CHECK_THROWS_AS(ScopedRenderTarget2D{ RenderTexture{} }, Error);
		CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == alias.id());
		CHECK_THROWS_AS(alias.draw(), Error);
		CHECK_THROWS_AS(alias(0, 0, 4, 4).draw(), Error);
		CHECK_THROWS_AS(alias.mapped(8, 8).draw(), Error);
		CHECK_THROWS_AS((Circle{ 4, 4, 3 }(alias).draw()), Error);
		Rect{ 4, 0, 4, 8 }.draw(Palette::Blue);
	}
	CHECK(ReadPixel(alias, 2, 4) == Palette::Red);
	CHECK(ReadPixel(alias, 6, 4) == Palette::Blue);
	CHECK_THROWS_AS(([&]
	{
		const ScopedRenderTarget2D target{ alias };
		Rect{ 8, 8 }.draw(Palette::Green);
		throw Error{ "test unwind" };
	}()), Error);
	CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
	CHECK(Read(alias) == Image{ 8, 8, Palette::Green });
}

TEST_CASE("ScopedRenderTarget2D.feedback_cache_target_changes_and_failed_draws")
{
	const DrawState state;
	const RenderTexture a{ Size{ 8, 8 }, Palette::Black }, b{ Size{ 8, 8 }, Palette::Blue };
	{
		const ScopedRenderTarget2D first{ a };
		for (int32 i = 0; i < 32; ++i) { b.draw(); }
		Graphics2D::Flush();
		{
			const ScopedRenderTarget2D same{ a };
			b.draw();
		}
		// The previously validated input becomes the output on entry.
		{
			const ScopedRenderTarget2D second{ b };
			CHECK_THROWS_AS(b.draw(), Error);
			CHECK_THROWS_AS(b.draw(), Error); // Rejection must never validate an ID.
			a.draw();
			CHECK_THROWS_AS(b.draw(), Error);
			a.draw();
		}
		// The input validated inside the nested scope is now the restored output.
		CHECK_THROWS_AS(a.draw(), Error);
		CHECK_THROWS_AS(a.draw(), Error);
		b.draw();
		CHECK(Read(a) == Image{ 8, 8, Palette::Blue });
		b.draw(); // Readback flushes keep the logical output unchanged.
		{
			const ScopedRenderTarget2D scene{ none };
			a.draw(-16, -16);
		}
		CHECK_THROWS_AS(a.draw(), Error);
		CHECK_THROWS_AS(([&]
		{
			const ScopedRenderTarget2D second{ b };
			a.draw();
			throw Error{ "test cached input on unwind" };
		}()), Error);
		CHECK_THROWS_AS(a.draw(), Error);
		b.draw();
		Rect{ 4, 0, 4, 8 }.draw(Palette::Red);
	}
	const Image image = Read(a);
	CHECK(image[4][2] == Palette::Blue);
	CHECK(image[4][6] == Palette::Red);
	// Leaving the output scope permits sampling that resource again.
	CHECK_NOTHROW(a.draw(-16, -16));
}

TEST_CASE("ScopedRenderTarget2D.scene_msaa_resolution_and_frame_continuation")
{
	const DrawState state;
	const auto config = Scene::GetConfig();
	const auto samples = Scene::GetMSAASampleCount();
	const auto background = Scene::GetBackground();
	const ScopeExit restore{ [&] { Scene::SetBackground(background); Scene::SetMSAASampleCount(samples); Scene::Configure(config); } };
	Scene::SetBackground(Palette::Black);
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	{
		Scene::Configure(SceneConfig::Fixed({ 48, 16 }, resolution));
		for (const auto sampleCount : { MSAASampleCount::NoMSAA, Scene::DefaultMSAASampleCount })
		{
			Scene::SetMSAASampleCount(sampleCount);
			REQUIRE(System::Update());
			Rect{ 0, 0, 16, 16 }.draw(Palette::Red);
			const RenderTexture texture{ Size{ 16, 16 }, Palette::Black };
			{
				const ScopedRenderTarget2D target{ texture };
				CHECK(SIV3D_ENGINE(Renderer2D)->getSceneScale() == Float2{ 1, 1 });
				CHECK(SIV3D_ENGINE(Renderer2D)->getRasterScaling() == 1.0f);
				Rect{ 0, 0, 8, 16 }.draw(Palette::Blue);
				Rect{ 8, 0, 8, 16 }.draw(Palette::Green);
			}
			texture.draw(16, 0);
			Rect{ 32, 0, 16, 16 }.draw(Palette::Yellow);
			const Image pixels = Read(texture);
			CHECK(pixels[8][4] == Palette::Blue);
			CHECK(pixels[8][12] == Palette::Green);
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
			const auto& captured = ScreenCapture::GetFrame();
			const auto pixel = [&](int32 x) { return captured[captured.height() / 2][x * captured.width() / 48]; };
			CHECK(pixel(8) == Palette::Red);
			CHECK(pixel(20) == Palette::Blue);
			CHECK(pixel(28) == Palette::Green);
			CHECK(pixel(40) == Palette::Yellow);
			// Frame boundaries do not reset a live scope, and Scene still gets initialized.
			{
				const ScopedRenderTarget2D target{ texture };
				Rect{ texture.size() }.draw(Palette::White);
				ScreenCapture::RequestCurrentFrame();
				REQUIRE(System::Update());
				CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == texture.id());
				REQUIRE(ScreenCapture::HasNewFrame());
				const auto& blank = ScreenCapture::GetFrame();
				CHECK(blank[blank.height() / 2][blank.width() / 2] == Palette::Black);
				Rect{ 8, 16 }.draw(Palette::Blue);
			}
			CHECK(ReadPixel(texture, 4, 8) == Palette::Blue);
			CHECK(ReadPixel(texture, 12, 8) == Palette::White);
		}
	}
}

TEST_CASE("ScopedRenderTarget2D.custom_shaders_and_constant_buffers")
{
	const DrawState state;
	const VertexShader vs = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"VS_Offset" };
	const PixelShader ps = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"PS_TintShape" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"PS_TintShape" };
	REQUIRE(vs);
	REQUIRE(ps);
	struct alignas(16) Parameters { Float4 value; };
	const ScopedCustomShader2D shader{ vs, ps };
	const ScopedVSConstantBuffer2D offset{ 2, ConstantBuffer<Parameters>{ Parameters{ Float4{ 2, 0, 0, 0 } } } };
	const ScopedPSConstantBuffer2D tint{ 2, ConstantBuffer<Parameters>{ Parameters{ Float4{ 1, 0, 0, 1 } } } };
	const RenderTexture a{ Size{ 16, 8 }, Palette::Black }, b{ Size{ 8, 8 }, Palette::Black };
	{
		const ScopedRenderTarget2D first{ a };
		Rect{ 0, 0, 4, 8 }.draw();
		{
			const ScopedRenderTarget2D second{ b };
			Rect{ 0, 0, 4, 8 }.draw();
		}
		CHECK(ReadPixel(b, 3, 4) == Palette::Red);
		Rect{ 8, 0, 4, 8 }.draw();
	}
	const Image image = Read(a);
	CHECK(image[4][0] == Palette::Black);
	CHECK(image[4][3] == Palette::Red);
	CHECK(image[4][9] == Palette::Black);
	CHECK(image[4][11] == Palette::Red);
	CHECK(Graphics2D::GetCustomVertexShader()->id() == vs.id());
	CHECK(Graphics2D::GetCustomPixelShader()->id() == ps.id());
}

TEST_CASE("ScopedRenderTarget2D.vertex_ranges_across_targets")
{
	const DrawState state;
	const RenderTexture a{ Size{ 8, 8 }, Palette::Black }, b{ Size{ 8, 8 }, Palette::Black };
	{
		const ScopedRenderTarget2D first{ a };
		// Cross the 16-bit vertex range before changing targets in the same frame.
		for (int32 i = 0; i < 17000; ++i) { Rect{ -2, -2, 1, 1 }.draw(); }
		Rect{ 4, 8 }.draw(Palette::Red);
		{
			const ScopedRenderTarget2D second{ b };
			Rect{ 8, 8 }.draw(Palette::Blue);
		}
		b.draw(4, 0);
	}
	const Image image = Read(a);
	CHECK(image[4][2] == Palette::Red);
	CHECK(image[4][6] == Palette::Blue);
}

TEST_CASE("ScopedRenderTarget2D.scene_changes_do_not_rescale_texture_draws")
{
	const DrawState state;
	const auto config = Scene::GetConfig();
	const ScopeExit restore{ [&] { Scene::Configure(config); } };
	Scene::Configure(SceneConfig::Fixed({ 40, 20 }, SceneResolution::Display));
	const RenderTexture texture{ Size{ 16, 8 }, Palette::Black };
	{
		const ScopedRenderTarget2D target{ texture };
		Rect{ 0, 0, 8, 8 }.draw(Palette::Red);
		Scene::Configure(SceneConfig::Fixed({ 60, 30 }, SceneResolution::Display));
		CHECK(Graphics2D::GetDrawingSize() == texture.size());
		CHECK(SIV3D_ENGINE(Renderer2D)->getSceneScale() == Float2{ 1, 1 });
		Rect{ 8, 0, 8, 8 }.draw(Palette::Blue);
	}
	CHECK(Graphics2D::GetDrawingSize() == Size{ 60, 30 });
	const Float2 expectedScale{ float(Scene::RenderResolution().x) / 60, float(Scene::RenderResolution().y) / 30 };
	CHECK(SIV3D_ENGINE(Renderer2D)->getSceneScale() == expectedScale);
	const Image image = Read(texture);
	CHECK(image[4][4] == Palette::Red);
	CHECK(image[4][12] == Palette::Blue);
}
