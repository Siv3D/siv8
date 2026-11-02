//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <iostream>

TEST_CASE("DynamicTexture.fillRegion")
{
	DynamicTexture texture{ Size{ 64, 64 }, Palette::Black };
	const Image image{ Size{ 7, 5 }, Palette::Red };
	const Rect rect{ 3, 4, image.size() };

	CHECK(texture.fillRegionIfNotBusy(image, rect));
	CHECK(texture.fillRegion(Palette::Blue, rect));
	CHECK(texture.fill(Palette::Black));

	CHECK_FALSE(texture.fillRegion(image, Rect{ -1, 4, image.size() }));
	CHECK_FALSE(texture.fillRegion(image, Rect{ 60, 60, image.size() }));
	CHECK_FALSE(texture.fillRegion(image, Rect{ 3, 4, 0, image.height() }));
	CHECK_FALSE(texture.fillRegion(Image{ 6, 5 }, rect));
}

TEST_CASE("DynamicTexture.R16G16B16A16_Float.Array")
{
	using Pixel = std::array<HalfFloat, 4>;

	const Pixel fullPixel{ HalfFloat{ 0.125f }, HalfFloat{ 0.25f }, HalfFloat{ 0.5f }, HalfFloat{ 0.75f } };
	const Grid<Pixel> image{ Size{ 7, 5 }, fullPixel };
	DynamicTexture texture;

	REQUIRE(texture.fill(image));
	REQUIRE(texture.format() == TextureFormat::R16G16B16A16_Float);
	CHECK(texture.fill(image));

	const Pixel regionPixel{ HalfFloat{ 0.75f }, HalfFloat{ 0.5f }, HalfFloat{ 0.25f }, HalfFloat{ 0.125f } };
	const Grid<Pixel> region{ Size{ 3, 2 }, regionPixel };
	CHECK(texture.fillRegion(region, Rect{ 2, 1, region.size() }));
}

TEST_CASE("DynamicTexture.fill.NonSquare")
{
	DynamicTexture wideTexture{ Size{ 64, 7 }, Palette::Black };
	DynamicTexture tallTexture{ Size{ 7, 64 }, Palette::Black };

	CHECK(wideTexture.fill(Palette::Red));
	CHECK(tallTexture.fill(Palette::Blue));
}

TEST_CASE("DynamicTexture.generateMips")
{
	DynamicTexture texture{ Size{ 64, 64 }, Palette::Black, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
	const Image image{ Size{ 7, 5 }, Palette::Red };
	const Rect rect{ 3, 4, image.size() };

	REQUIRE(texture.hasMipmaps());
	CHECK(1 < texture.mipLevels());
	CHECK(texture.fillRegion(image, rect));
	texture.generateMips();
}

namespace
{
	struct DynamicTextureDrawState
	{
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedScissorRect2D scissor{ none };
		const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
		const ScopedColorAdd2D add{ ColorF{ 0.0, 0.0 }, ScopedColorAdd2D::Target::Set };
	};
}

TEST_CASE("DynamicTexture.update_order_and_explicit_flush")
{
	const DynamicTextureDrawState state;
	const ScopedRenderStates2D blend{ BlendState::Opaque };
	for (const bool explicitFlush : { false, true })
	{
		for (int32 method = 0; method < 4; ++method)
		{
			CAPTURE(explicitFlush, method);
			DynamicTexture texture{ Size{ 4, 4 }, Palette::Blue };
			auto alias = texture;
			const RenderTexture output{ Size{ 12, 4 }, Palette::Black };
			const auto update = [&](const Color color)
			{
				switch (method)
				{
				case 0: return alias.fill(color);
				case 1: return alias.fill(Image{ 4, 4, color });
				case 2: return alias.fillRegion(color, Rect{ 4, 4 });
				default: return alias.fillRegion(Image{ 4, 4, color }, Rect{ 4, 4 });
				}
			};
			const auto frame = Scene::FrameCount();
			{
				const ScopedRenderTarget2D target{ output };
				texture.draw();
				if (explicitFlush) { Graphics2D::Flush(); }
				REQUIRE(update(Palette::Red));
				texture.draw(4, 0);
				if (explicitFlush) { Graphics2D::Flush(); }
				REQUIRE(update(Palette::Green));
				texture.draw(8, 0);
			}
			Image pixels;
			REQUIRE(output.readAsImage(pixels));
			CHECK(pixels[2][2] == (explicitFlush ? Palette::Blue : Palette::Green));
			CHECK(pixels[2][6] == (explicitFlush ? Palette::Red : Palette::Green));
			CHECK(pixels[2][10] == Palette::Green);
			CHECK(Scene::FrameCount() == frame);
		}
	}
}

TEST_CASE("DynamicTexture.mipmap_order_and_explicit_flush")
{
	const DynamicTextureDrawState state;
	const ScopedRenderStates2D blend{ BlendState::Opaque };
	for (const bool explicitFlush : { false, true })
	{
		CAPTURE(explicitFlush);
		DynamicTexture texture{ Size{ 16, 16 }, Palette::Blue, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
		const RenderTexture output{ Size{ 4, 2 }, Palette::Black };
		REQUIRE(texture.fill(Palette::Red));
		{
			const ScopedRenderTarget2D target{ output };
			texture.resized(2, 2).draw(); // Still samples the blue mip levels.
			if (explicitFlush) { Graphics2D::Flush(); }
			texture.generateMips();
			texture.resized(2, 2).draw(2, 0);
		}
		Image pixels;
		REQUIRE(output.readAsImage(pixels));
		CHECK(pixels[1][0] == (explicitFlush ? Palette::Blue : Palette::Red));
		CHECK(pixels[1][2] == Palette::Red);
	}
}

TEST_CASE("DynamicTexture.upload_readback_and_frame_continuation")
{
	const DynamicTextureDrawState state;
	const ScopedRenderStates2D blend{ BlendState::Opaque };
	DynamicTexture texture{ Size{ 4, 4 }, Palette::Blue };
	const RenderTexture output{ Size{ 8, 4 }, Palette::Black };
	for (int32 frame = 0; frame < 6; ++frame)
	{
		REQUIRE(System::Update());
		Image pixels;
		{
			const ScopedRenderTarget2D target{ output };
			texture.draw();
			Graphics2D::Flush();
			REQUIRE(texture.fill(Palette::Red));
			texture.draw(4, 0);
			REQUIRE(output.readAsImage(pixels));
			CHECK(pixels[2][2] == Palette::Blue);
			CHECK(pixels[2][6] == Palette::Red);
			REQUIRE(texture.fill(Palette::Blue));
			texture.draw(4, 0);
		}
		REQUIRE(output.readAsImage(pixels));
		CHECK(pixels == Image{ 8, 4, Palette::Blue });
	}
}

TEST_CASE("DynamicTexture.frame_upload_benchmark", "[.benchmark]")
{
	const DynamicTextureDrawState state;
	const ScopedViewport2D viewport{ none };
	const ScopedRenderStates2D blend{ BlendState::Default2D };
	const auto config = Scene::GetConfig();
	const auto samples = Scene::GetMSAASampleCount();
	const bool vsync = Graphics::IsVSyncEnabled();
	const ScopeExit restore{ [&]
	{
		Graphics::SetVSyncEnabled(vsync);
		Scene::SetMSAASampleCount(samples);
		Scene::Configure(config);
	} };
	Graphics::SetVSyncEnabled(false);
	Scene::SetMSAASampleCount(MSAASampleCount::NoMSAA);
	Scene::Configure(SceneConfig::Fixed({ 1920, 1080 }, SceneResolution::SceneSize));
	constexpr int32 WarmupFrames = 30;
	constexpr int32 SampleFrames = 120;
	const RenderTexture drain{ Size{ 1, 1 } };
	Image drained;
	for (const Size size : { Size{ 640, 360 }, Size{ 1920, 1080 } })
	{
		const Image red{ size, Color{ 255, 0, 0, 128 } };
		const Image blue{ size, Color{ 0, 0, 255, 128 } };
		DynamicTexture texture{ red };
		for (const int32 layers : { 1, 32 })
		{
			for (const bool afterFlush : { false, true })
			{
				for (int32 trial = 0; trial < 3; ++trial)
				{
					Array<int64> fillTimes, frameTimes;
					fillTimes.reserve(SampleFrames);
					frameTimes.reserve(SampleFrames);
					REQUIRE(System::Update());
					for (int32 frame = 0; frame < WarmupFrames + SampleFrames; ++frame)
					{
						const int64 frameStart = Time::GetNanosec();
						const auto draw = [&]
						{
							for (int32 layer = 0; layer < layers; ++layer)
							{
								texture.resized(1920, 1080).draw();
							}
						};
						if (afterFlush) { draw(); Graphics2D::Flush(); }
						const int64 fillStart = Time::GetNanosec();
						const bool filled = texture.fill((frame % 2) ? red : blue);
						const int64 fillTime = (Time::GetNanosec() - fillStart);
						REQUIRE(filled);
						if (not afterFlush) { draw(); }
						REQUIRE(System::Update());
						const int64 frameTime = (Time::GetNanosec() - frameStart);
						if (WarmupFrames <= frame)
						{
							fillTimes.push_back(fillTime);
							frameTimes.push_back(frameTime);
						}
					}
					REQUIRE(drain.readAsImage(drained));
					fillTimes.sort();
					frameTimes.sort();
					std::cout << "texture-upload size=" << size.x << 'x' << size.y
						<< " layers=" << layers << " after_flush=" << afterFlush << " trial=" << trial
						<< " fill_us=" << (fillTimes[SampleFrames / 2] / 1000.0)
						<< " fill_p95_us=" << (fillTimes[SampleFrames * 95 / 100] / 1000.0)
						<< " frame_ms=" << (frameTimes[SampleFrames / 2] / 1000000.0)
						<< " frame_p95_ms=" << (frameTimes[SampleFrames * 95 / 100] / 1000000.0) << '\n';
				}
			}
		}
	}
}
