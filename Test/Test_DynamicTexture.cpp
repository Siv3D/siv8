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
# include <limits>

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

namespace
{
	Array<Byte> ReadDynamicBytes(const DynamicTexture& texture)
	{
		const RenderTexture result{ texture.size(), texture.format() };
		RenderTextureOps::Copy(texture, result);
		Array<Byte> bytes(texture.format().packedDataSize(texture.size()));
		REQUIRE(result.read(bytes));
		return bytes;
	}
}

TEST_CASE("DynamicTexture.raw_rows_formats_and_regions")
{
	const DynamicTextureDrawState state;
	const Size size{ 7, 5 };
	for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
	{
		const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
		CAPTURE(format);
		const size_t pixelBytes = format.pixelSize();
		Array<Byte> first(pixelBytes), second(pixelBytes);
		REQUIRE(RenderTexture{ Size{ 1, 1 }, ColorF{ 0.25, 0.5, 0.75, 1.0 }, format }.read(first));
		REQUIRE(RenderTexture{ Size{ 1, 1 }, ColorF{ 0.75, 0.25, 0.5, 0.5 }, format }.read(second));
		Array<Byte> expected(format.packedDataSize(size));
		for (int32 y = 0; y < size.y; ++y)
		{
			for (int32 x = 0; x < size.x; ++x)
			{
				const auto& pixel = ((x + 2 * y) % 3) ? first : second;
				std::memcpy(expected.data() + (y * size.x + x) * pixelBytes, pixel.data(), pixelBytes);
			}
		}
		DynamicTexture texture{ size, format };
		REQUIRE(texture);
		const auto id = texture.id();
		const uint32 rowBytes = static_cast<uint32>(format.packedRowBytes(size.x));
		for (const uint32 padding : { 0u, 1u, 13u })
		{
			const uint32 pitch = (rowBytes + padding);
			// The span starts at an unaligned address and ends at the final pixel, without tail padding.
			Array<Byte> storage(3 + (size.y - 1) * pitch + rowBytes, Byte{ 0xCD });
			const std::span<Byte> data{ storage.data() + 3, storage.size() - 3 };
			for (int32 y = 0; y < size.y; ++y)
			{
				std::memcpy(data.data() + y * pitch, expected.data() + y * rowBytes, rowBytes);
			}
			REQUIRE(texture.fill(data, pitch));
			CHECK(ReadDynamicBytes(texture) == expected);
			REQUIRE(texture.fillIfNotBusy(data, pitch));
			CHECK(ReadDynamicBytes(texture) == expected);
			CHECK(texture.id() == id);
		}
		const Rect rect{ 2, 1, 3, 3 };
		const uint32 regionBytes = static_cast<uint32>(format.packedRowBytes(rect.w));
		const uint32 pitch = (regionBytes + 11);
		Array<Byte> region((rect.h - 1) * pitch + regionBytes, Byte{ 0xEF });
		for (int32 y = 0; y < rect.h; ++y)
		{
			for (int32 x = 0; x < rect.w; ++x)
			{
				const auto& pixel = ((x + y) % 2) ? second : first;
				std::memcpy(region.data() + y * pitch + x * pixelBytes, pixel.data(), pixelBytes);
				std::memcpy(expected.data() + ((rect.y + y) * size.x + rect.x + x) * pixelBytes, pixel.data(), pixelBytes);
			}
		}
		REQUIRE(texture.fillRegion(region, pitch, rect));
		CHECK(ReadDynamicBytes(texture) == expected);
		REQUIRE(texture.fillRegionIfNotBusy(region, pitch, rect));
		CHECK(ReadDynamicBytes(texture) == expected);
		// A single row only needs its pixels, even when the stated row pitch is large.
		REQUIRE(texture.fillRegion(first, 1000, Rect{ 0, 0, 1, 1 }));
		std::memcpy(expected.data(), first.data(), pixelBytes);
		CHECK(ReadDynamicBytes(texture) == expected);
	}
}

TEST_CASE("DynamicTexture.raw_invalid_inputs_preserve_contents")
{
	const DynamicTextureDrawState state;
	DynamicTexture texture{ Size{ 3, 2 }, Palette::Blue };
	const Array<Byte> expected = ReadDynamicBytes(texture);
	Array<Byte> storage(32, Byte{ 0xFF });
	const std::span<const Byte> data{ storage };
	for (const bool wait : { true, false })
	{
		const auto fill = [&](const std::span<const Byte> src, const uint32 pitch)
		{
			return wait ? texture.fill(src, pitch) : texture.fillIfNotBusy(src, pitch);
		};
		CHECK_FALSE(fill({}, 12));
		CHECK_FALSE(fill(data.first(23), 12));
		CHECK_FALSE(fill(data, 11));
		CHECK_FALSE(fill(data, 0));
		CHECK_FALSE(fill(data, std::numeric_limits<uint32>::max()));
		for (const Rect rect : { Rect{ -1, 0, 1, 1 }, Rect{ 0, -1, 1, 1 }, Rect{ 2, 0, 2, 1 }, Rect{ 0, 1, 1, 2 }, Rect{ 0, 0, 0, 1 }, Rect{ 0, 0, 1, -1 }, Rect{ std::numeric_limits<int32>::max(), 0, 2, 1 } })
		{
			CHECK_FALSE((wait ? texture.fillRegion(data, 12, rect) : texture.fillRegionIfNotBusy(data, 12, rect)));
		}
		CHECK_FALSE((wait ? texture.fillRegion(data.first(7), 4, Rect{ 0, 0, 1, 2 }) : texture.fillRegionIfNotBusy(data.first(7), 4, Rect{ 0, 0, 1, 2 })));
		CHECK_FALSE((wait ? texture.fillRegion(data, 3, Rect{ 0, 0, 1, 1 }) : texture.fillRegionIfNotBusy(data, 3, Rect{ 0, 0, 1, 1 })));
		CHECK(ReadDynamicBytes(texture) == expected);
	}
	DynamicTexture empty;
	CHECK_FALSE(empty.fill(data, 12));
	CHECK_FALSE(empty.fillIfNotBusy(data, 12));
	CHECK_FALSE(empty.fillRegion(data, 12, Rect{ 3, 2 }));
	CHECK_FALSE(empty.fillRegionIfNotBusy(data, 12, Rect{ 3, 2 }));
	CHECK(empty.isEmpty());
	REQUIRE(empty.fill(Image{ 3, 2, Palette::Blue }));
	CHECK(ReadDynamicBytes(empty) == expected);
	DynamicTexture typed;
	REQUIRE(typed.fillIfNotBusy(Grid<float>{ 3, 2, 0.5f }));
	CHECK(typed.format() == TextureFormat::R32_Float);
	CHECK(typed.size() == Size{ 3, 2 });
}

TEST_CASE("DynamicTexture.raw_source_lifetime_order_and_mips")
{
	// The guide example includes padding between rows, but none after the final row.
	DynamicTexture r8{ Size{ 3, 2 }, TextureFormat::R8_Unorm };
	const std::array<uint8, 8> source{ 0, 64, 128, 0xCC, 0xCC, 255, 128, 64 };
	REQUIRE(r8.fill(std::as_bytes(std::span{ source }), 5));
	const Array<Byte> expected{ Byte{ 0 }, Byte{ 64 }, Byte{ 128 }, Byte{ 255 }, Byte{ 128 }, Byte{ 64 } };
	CHECK(ReadDynamicBytes(r8) == expected);

	const DynamicTextureDrawState state;
	const ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
	const ScopedRenderStates2D states{ BlendState::Opaque, SamplerState::ClampNearest };
	DynamicTexture texture{ Image{ 4, 4, Palette::Blue }, TextureDesc::Mipmap };
	const RenderTexture output{ Size{ 6, 2 } };
	{
		const ScopedRenderTarget2D target{ output };
		const ScopedViewport2D viewport{ Rect{ output.size() } };
		texture.resized(2, 2).draw(0, 0);
		Graphics2D::Flush();
		{
			Image temporary{ 4, 4, Palette::Red };
			REQUIRE(texture.fill(std::as_bytes(std::span{ temporary }), temporary.bytesPerRow()));
			temporary.fill(Palette::Green);
		}
		// The old mip remains blue until explicit generation.
		texture.resized(2, 2).draw(2, 0);
		Graphics2D::Flush();
		texture.generateMips();
		texture.resized(2, 2).draw(4, 0);
	}
	Image actual;
	REQUIRE(output.readAsImage(actual));
	for (int32 y = 0; y < 2; ++y)
	{
		for (int32 x = 0; x < 6; ++x)
		{
			CHECK(actual[y][x] == (x < 4 ? Palette::Blue : Palette::Red));
		}
	}
}
