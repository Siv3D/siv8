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
# include "../Siv3D/src/Siv3D/Texture/TextureUtility.hpp"
# include <iostream>
# include <limits>

# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# endif

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

TEST_CASE("DynamicTexture.typed_and_color_upload_benchmark", "[.benchmark]")
{
# if SIV3D_PLATFORM(MACOS)
	const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
	REQUIRE(qos);
	const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
# endif
	constexpr Size TextureSize{ 1920, 1080 };
	constexpr std::array Inputs{ "typed", "raw", "color" };
	constexpr size_t ModeCount = (Inputs.size() * 2); // Waiting and non-waiting overloads.
	constexpr size_t Repetitions = 24;
	const std::array colors{ Color{ 32, 128, 224, 255 }, Color{ 210, 76, 19, 173 } };
	const std::array paints{ ColorF{ colors[0] }, ColorF{ colors[1] } };
	const auto measure = [&](const char* formatName, const auto& makePixels)
	{
		const auto background = makePixels(TextureSize, Color{ 9, 55, 103, 191 });
		DynamicTexture texture{ background };
		REQUIRE(texture);
		const auto id = texture.id();
		const RenderTexture output{ TextureSize, texture.format() };
		const RenderTexture probe{ Size{ 1, 1 }, texture.format() };
		Array<Byte> actual(texture.format().packedDataSize(TextureSize));
		Array<Byte> probePixel(texture.format().pixelSize());
		for (const Rect rect : { Rect{ TextureSize }, Rect{ 317, 193, 63, 31 } })
		{
			const bool whole = (rect.size == TextureSize);
			std::array sources{ makePixels(rect.size, colors[0]), makePixels(rect.size, colors[1]) };
			const std::array pixels{ sources[0][0][0], sources[1][0][0] };
			constexpr size_t LastPhase = ((Repetitions - 1) % 2);
			auto expectedColor = background;
			auto expectedTyped = background;
			for (int32 y = 0; y < rect.h; ++y)
			{
				// A marker at a different column in each row exposes source-pitch errors.
				for (auto& source : sources) { source[y][(y + 1) % rect.w] = background[0][0]; }
				std::copy_n(sources[LastPhase][y], rect.w, &expectedTyped[rect.y + y][rect.x]);
				std::fill_n(&expectedColor[rect.y + y][rect.x], rect.w, pixels[LastPhase]);
			}
			const std::array packed{ std::as_bytes(std::span{ sources[0] }), std::as_bytes(std::span{ sources[1] }) };
			const uint32 pitch = sources[0].bytesPerRow();
			const auto checkProbe = [&](const auto& expected)
			{
				RenderTextureOps::Copy(texture, Rect{ rect.pos, 1, 1 }, probe, Rect{ 1, 1 });
				REQUIRE(probe.read(probePixel));
				REQUIRE(std::memcmp(probePixel.data(), &expected, probePixel.size()) == 0);
			};
			REQUIRE(texture.fill(background));
			checkProbe(background[0][0]);
			std::array<Array<int64>, ModeCount> timings;
			for (auto& times : timings) { times.reserve(ModeCount * Repetitions); }
			for (size_t batch = 0; batch < ModeCount; ++batch)
			{
				REQUIRE(System::Update());
				for (size_t order = 0; order < ModeCount; ++order)
				{
					const size_t mode = ((batch + order) % ModeCount);
					const size_t input = (mode / 2);
					const bool nonWaiting = (mode % 2 != 0);
					CAPTURE(formatName, rect, batch, Inputs[input], nonWaiting);
					const auto fill = [&](const auto&... args)
					{
						if (whole) { return nonWaiting ? texture.fillIfNotBusy(args...) : texture.fill(args...); }
						return nonWaiting ? texture.fillRegionIfNotBusy(args..., rect) : texture.fillRegion(args..., rect);
					};
					const auto update = [&](const size_t phase)
					{
						switch (input)
						{
						case 0: return fill(sources[phase]);
						case 1: return fill(packed[phase], pitch);
						default: return fill(paints[phase]);
						}
					};

					// Warm up with the other pixel so every timed update must change the probe.
					REQUIRE(update(1));
					checkProbe(pixels[1]);
					for (size_t i = 0; i < Repetitions; ++i)
					{
						const size_t phase = (i % 2);
						const int64 start = Time::GetNanosec();
						const bool success = update(phase);
						const int64 elapsed = (Time::GetNanosec() - start);
						REQUIRE(success);
						timings[mode].push_back(elapsed);
						// The dependent read completes this upload before the next call, including IfNotBusy().
						checkProbe(pixels[phase]);
					}
					RenderTextureOps::Copy(texture, output);
					REQUIRE(output.read(actual));
					const auto& expected = (input == 2) ? expectedColor : expectedTyped;
					REQUIRE(std::memcmp(actual.data(), expected.data(), actual.size()) == 0);
					REQUIRE(texture.id() == id);
				}
			}
			for (size_t mode = 0; mode < ModeCount; ++mode)
			{
				auto& times = timings[mode];
				times.sort();
				std::cout << "typed-color-upload " << formatName << ' ' << rect.w << 'x' << rect.h
					<< " at=" << rect.x << ',' << rect.y << ' ' << Inputs[mode / 2]
					<< ((mode % 2) ? "-if-not-busy" : "")
					<< " median_us=" << (times[times.size() / 2] / 1000.0)
					<< " p95_us=" << (times[times.size() * 95 / 100] / 1000.0) << '\n';
			}
		}
	};
	measure("RG8", [](Size size, Color color) { return Grid<uint16>{ size, ColorF{ color }.toR8G8_Unorm() }; });
	measure("RGBA8", [](Size size, Color color) { return Image{ size, color }; });
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

TEST_CASE("DynamicTexture.R8G8_Unorm.typed_updates")
{
	for (const Size size : { Size{ 1, 1 }, Size{ 3, 5 }, Size{ 4, 2 }, Size{ 7, 5 } })
	{
		CAPTURE(size);
		Grid<uint16> source(size);
		for (int32 y = 0; y < size.y; ++y)
		{
			for (int32 x = 0; x < size.x; ++x)
			{
				source[y][x] = static_cast<uint16>((17 + x * 13) | ((31 + y * 19) << 8));
			}
		}
		const auto checkPixels = [&](const DynamicTexture& texture, const Grid<uint16>& expected)
		{
			const auto bytes = ReadDynamicBytes(texture);
			const auto data = std::as_bytes(std::span{ expected });
			REQUIRE(bytes.size() == data.size());
			CHECK(std::memcmp(bytes.data(), data.data(), bytes.size()) == 0);
		};
		DynamicTexture texture{ source, TextureDesc::Mipmap };
		REQUIRE(texture);
		CHECK(texture.format() == TextureFormat::R8G8_Unorm);
		CHECK(texture.size() == size);
		CHECK(texture.mipLevels() == ImageProcessing::CalculateMipmapLevel(size.x, size.y));
		checkPixels(texture, source);
		const auto id = texture.id();
		for (const bool nonWaiting : { false, true })
		{
			const Grid<uint16> full{ size, uint16{ 0xA34B } };
			REQUIRE((nonWaiting ? texture.fillIfNotBusy(full) : texture.fill(full)));
			checkPixels(texture, full);
			const Rect rect{ size.x - 1, size.y - 1, 1, 1 };
			const Grid<uint16> region{ Size{ 1, 1 }, uint16{ 0x12E7 } };
			REQUIRE((nonWaiting ? texture.fillRegionIfNotBusy(region, rect) : texture.fillRegion(region, rect)));
			auto expected = full;
			expected[rect.y][rect.x] = region[0][0];
			checkPixels(texture, expected);
			CHECK(texture.id() == id);
			CHECK_FALSE((nonWaiting ? texture.fillIfNotBusy(Grid<uint16>{ Size{ size.x + 1, size.y } })
				: texture.fill(Grid<uint16>{ Size{ size.x + 1, size.y } })));
			CHECK_FALSE((nonWaiting ? texture.fillRegionIfNotBusy(region, Rect{ size.x, size.y, 1, 1 })
				: texture.fillRegion(region, Rect{ size.x, size.y, 1, 1 })));
			CHECK_FALSE((nonWaiting ? texture.fillRegionIfNotBusy(source, Rect{ size.x + 1, size.y })
				: texture.fillRegion(source, Rect{ size.x + 1, size.y })));
			checkPixels(texture, expected);
		}
		DynamicTexture wrongFormat{ size, Palette::Blue };
		const auto original = ReadDynamicBytes(wrongFormat);
		CHECK_FALSE(wrongFormat.fill(source));
		CHECK_FALSE(wrongFormat.fillIfNotBusy(source));
		CHECK_FALSE(wrongFormat.fillRegion(source, Rect{ size }));
		CHECK_FALSE(wrongFormat.fillRegionIfNotBusy(source, Rect{ size }));
		CHECK(ReadDynamicBytes(wrongFormat) == original);
	}
}

TEST_CASE("DynamicTexture.typed_creation_reports_failure")
{
	const auto checkCreation = [](const auto& source)
	{
		using Container = std::decay_t<decltype(source)>;
		for (const bool nonWaiting : { false, true })
		{
			DynamicTexture texture;
			const auto update = [&](const Container& data)
			{
				return nonWaiting ? texture.fillIfNotBusy(data) : texture.fill(data);
			};
			CHECK_FALSE(update(Container{}));
			CHECK(texture.isEmpty());
			REQUIRE(update(source));
			REQUIRE(texture);
			CHECK(texture.size() == source.size());
			const auto id = texture.id();
			const auto before = ReadDynamicBytes(texture);
			CHECK_FALSE(update(Container{}));
			CHECK(texture.id() == id);
			CHECK(ReadDynamicBytes(texture) == before);

			// Exercise the existing-texture path for every typed overload.
			REQUIRE(texture.fill(ColorF{ 0.0, 0.0 }));
			auto expected = ReadDynamicBytes(texture);
			const Rect corner{ source.width() - 1, source.height() - 1, 1, 1 };
			const Container pixel{ corner.size, source[0][0] };
			REQUIRE((nonWaiting ? texture.fillRegionIfNotBusy(pixel, corner) : texture.fillRegion(pixel, corner)));
			const size_t pixelBytes = texture.format().pixelSize();
			std::memcpy(expected.data() + expected.size() - pixelBytes, before.data(), pixelBytes);
			CHECK(ReadDynamicBytes(texture) == expected);
			REQUIRE(update(source));
			CHECK(ReadDynamicBytes(texture) == before);
			CHECK(texture.id() == id);
		}
	};
	const Size size{ 3, 2 };
	checkCreation(Image{ size, Palette::Red });
	checkCreation(Grid<uint8>{ size, uint8{ 64 } });
	checkCreation(Grid<uint16>{ size, uint16{ 0xA34B } });
	checkCreation(Grid<HalfFloat>{ size, HalfFloat{ 0.25f } });
	checkCreation(Grid<Color>{ size, Palette::Red });
	checkCreation(Grid<std::pair<uint16, uint16>>{ size, { 1234, 5678 } });
	checkCreation(Grid<std::pair<HalfFloat, HalfFloat>>{ size, { HalfFloat{ 0.25f }, HalfFloat{ 0.5f } } });
	checkCreation(Grid<float>{ size, 0.25f });
	checkCreation(Grid<std::array<HalfFloat, 4>>{ size, { HalfFloat{ 0.25f }, HalfFloat{ 0.5f }, HalfFloat{ 0.75f }, HalfFloat{ 1.0f } } });
	checkCreation(Grid<Float2>{ size, Float2{ 0.25f, 0.5f } });
	checkCreation(Grid<Float4>{ size, Float4{ 0.25f, 0.5f, 0.75f, 1.0f } });
}

TEST_CASE("DynamicTexture.color_upload_rows_and_padding")
{
	const auto check = [](const TextureFormat format, const auto redPixel)
	{
		CAPTURE(format);
		constexpr int32 pixelBytes = sizeof(redPixel);
		for (const int32 width : { 1, 64 / pixelBytes - 1, 64 / pixelBytes, 64 / pixelBytes + 1 })
		{
			const Size size{ width, 3 };
			for (const uint32 padding : { 0u, 3u })
			{
				const uint32 pitch = (width * pixelBytes + padding);
				Array<Byte> bytes(1 + pitch * size.y + 1, Byte{ 0xCD });
				auto expected = bytes;
				for (int32 y = 0; y < size.y; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						std::memcpy(expected.data() + 1 + y * pitch + x * pixelBytes, &redPixel, pixelBytes);
					}
				}
				FillWithColor(bytes.data() + 1, size, pitch, Palette::Red, format);
				CHECK(bytes == expected);
			}
		}
	};
	check(TextureFormat::R8_Unorm, uint8{ 255 });
	check(TextureFormat::R8G8_Unorm, uint16{ 0x00FF });
	check(TextureFormat::R16_Float, uint16{ 0x3C00 });
	check(TextureFormat::R8G8B8A8_Unorm, Color{ 255, 0, 0, 255 });
	check(TextureFormat::R8G8B8A8_Unorm_SRGB, Color{ 255, 0, 0, 255 });
	check(TextureFormat::R16G16_Unorm, uint32{ 0x0000FFFF });
	check(TextureFormat::R16G16_Float, uint32{ 0x00003C00 });
	check(TextureFormat::R32_Float, 1.0f);
	check(TextureFormat::R10G10B10A2_Unorm, uint32{ 0xC00003FF });
	check(TextureFormat::R11G11B10_UFloat, uint32{ 0x000003C0 });
	check(TextureFormat::R16G16B16A16_Float, uint64{ 0x3C00000000003C00 });
	check(TextureFormat::R32G32_Float, Float2{ 1, 0 });
	check(TextureFormat::R32G32B32A32_Float, Float4{ 1, 0, 0, 1 });
}

TEST_CASE("DynamicTexture.srgb_image_and_grid_updates")
{
	const Size size{ 3, 2 };
	DynamicTexture texture{ size, TextureFormat::R8G8B8A8_Unorm_SRGB };
	const Image green{ size, Palette::Green };
	const Grid<Color> red{ size, Palette::Red };
	const Rect rect{ 2, 1, 1, 1 };
	const auto bytesOf = [](const auto& image)
	{
		const auto bytes = std::as_bytes(std::span{ image });
		return Array<Byte>(bytes.begin(), bytes.end());
	};
	REQUIRE(texture.fill(green));
	CHECK(ReadDynamicBytes(texture) == bytesOf(green));
	REQUIRE(texture.fillIfNotBusy(red));
	CHECK(ReadDynamicBytes(texture) == bytesOf(red));
	auto expected = red;
	expected[1][2] = Palette::Green;
	REQUIRE(texture.fillRegionIfNotBusy(Image{ rect.size, Palette::Green }, rect));
	CHECK(ReadDynamicBytes(texture) == bytesOf(expected));
	REQUIRE(texture.fillRegion(Grid<Color>{ rect.size, Palette::Red }, rect));
	CHECK(ReadDynamicBytes(texture) == bytesOf(red));
}

TEST_CASE("DynamicTexture.non_waiting_color_updates")
{
	const Size size{ 7, 5 };
	const Rect region{ 2, 1, 3, 3 };
	for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
	{
		const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
		CAPTURE(format);
		const DynamicTexture red{ size, Palette::Red, format };
		DynamicTexture texture{ size, Palette::Blue, format };
		const auto id = texture.id();
		const auto redBytes = ReadDynamicBytes(red);
		REQUIRE(texture.fillIfNotBusy(Palette::Red));
		CHECK(ReadDynamicBytes(texture) == redBytes);
		REQUIRE(texture.fillRegionIfNotBusy(Palette::Blue, region));
		auto expected = redBytes;
		const auto bluePixel = ReadDynamicBytes(DynamicTexture{ Size{ 1, 1 }, Palette::Blue, format });
		for (int32 y = region.y; y < region.y + region.h; ++y)
		{
			for (int32 x = region.x; x < region.x + region.w; ++x)
			{
				std::memcpy(expected.data() + (y * size.x + x) * bluePixel.size(), bluePixel.data(), bluePixel.size());
			}
		}
		CHECK(ReadDynamicBytes(texture) == expected);
		CHECK(texture.id() == id);
		for (const Rect invalid : { Rect{ -1, 0, 1, 1 }, Rect{ 0, 0, 0, 1 }, Rect{ 0, 0, 1, -1 }, Rect{ 6, 4, 2, 1 } })
		{
			CHECK_FALSE(texture.fillRegionIfNotBusy(Palette::Green, invalid));
		}
		CHECK(ReadDynamicBytes(texture) == expected);
	}
	DynamicTexture empty;
	CHECK_FALSE(empty.fillIfNotBusy(Palette::Red));
	CHECK_FALSE(empty.fillRegionIfNotBusy(Palette::Red, Rect{ 1, 1 }));
	CHECK(empty.isEmpty());
}
