//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cstring>

namespace
{
	template <class Pixel>
	void CheckRoundTrip(const Grid<Pixel>& input, const RenderTexture& texture)
	{
		REQUIRE(texture);
		CHECK_FALSE(texture.hasDepth());
		CHECK(texture.size() == input.size());
		Grid<Pixel> output;
		REQUIRE(texture.read(output));
		REQUIRE(output.size() == input.size());
		const auto source = std::as_bytes(std::span{ input });
		const auto result = std::as_bytes(std::span{ output });
		CHECK(std::equal(source.begin(), source.end(), result.begin(), result.end()));
	}
}

TEST_CASE("RenderTexture.creation_round_trips")
{
	for (const Size size : { Size{ 1, 1 }, Size{ 7, 3 }, Size{ 3, 11 } })
	{
		CAPTURE(size);
		const Grid<uint8> r8{ size, 173 };
		CheckRoundTrip(r8, RenderTexture{ r8 });
		const Grid<uint16> rg8{ size, 0x9B23 };
		CheckRoundTrip(rg8, RenderTexture::CreateR8G8_Unorm(rg8));
		const Grid<HalfFloat> r16{ size, HalfFloat{ -3.5f } };
		CheckRoundTrip(r16, RenderTexture{ r16 });
		Grid<Color> rgba8{ size, Color{ 17, 83, 139, 217 } };
		rgba8[size.y - 1][size.x - 1] = Color{ 219, 121, 27, 11 };
		CheckRoundTrip(rgba8, RenderTexture{ rgba8 });
		CheckRoundTrip(rgba8, RenderTexture::CreateR8G8B8A8_Unorm_SRGB(rgba8));
		const Grid<std::pair<uint16, uint16>> rg16{ size, { 17321, 61234 } };
		CheckRoundTrip(rg16, RenderTexture{ rg16 });
		const Grid<std::pair<HalfFloat, HalfFloat>> rg16f{ size, { HalfFloat{ 1.5f }, HalfFloat{ -2.25f } } };
		CheckRoundTrip(rg16f, RenderTexture{ rg16f });
		const Grid<float> r32{ size, -123.75f };
		CheckRoundTrip(r32, RenderTexture{ r32 });
		const Grid<uint32> rgb10{ size, ColorF{ 0.25, 0.5, 0.75, 1.0 }.toR10G10B10A2_Unorm() };
		CheckRoundTrip(rgb10, RenderTexture::CreateR10G10B10A2_Unorm(rgb10));
		const Grid<uint32> rgb11{ size, ColorF{ 2.0, 4.0, 8.0 }.toR11G11B10_UFloat() };
		CheckRoundTrip(rgb11, RenderTexture::CreateR11G11B10_UFloat(rgb11));
		const Grid<std::array<HalfFloat, 4>> rgba16{ size, { HalfFloat{ 2.0f }, HalfFloat{ -4.0f }, HalfFloat{ 8.0f }, HalfFloat{ 0.5f } } };
		CheckRoundTrip(rgba16, RenderTexture{ rgba16 });
		const Grid<Float2> rg32{ size, Float2{ -16.0f, 32.0f } };
		CheckRoundTrip(rg32, RenderTexture{ rg32 });
		const Grid<Float4> rgba32{ size, Float4{ -1.0f, 2.0f, 4.0f, 0.25f } };
		CheckRoundTrip(rgba32, RenderTexture{ rgba32 });
	}
}

TEST_CASE("RenderTexture.clear_all_formats_and_mips")
{
	for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
	{
		const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
		CAPTURE(format);
		RenderTexture texture{ Size{ 7, 5 }, format, TextureDesc::Mipmap };
		REQUIRE(texture);
		CHECK(texture.format() == format);
		CHECK(texture.isSRGB() == format.isSRGB());
		CHECK(texture.mipLevels() == 3);
		for (uint32 level = 0; level < texture.mipLevels(); ++level)
		{
			Array<Byte> bytes(static_cast<size_t>(Max(1, (7 >> level))) * Max(1, (5 >> level)) * format.pixelSize(), Byte{ 0xCD });
			REQUIRE(texture.read(std::span{ bytes }, level));
			CHECK(std::all_of(bytes.begin(), bytes.end(), [](Byte b) { return (b == Byte{ 0 }); }));
		}
		texture.clear(ColorF{ 1.0, 1.0 });
		texture.generateMips();
		Array<Byte> base(7 * 5 * format.pixelSize());
		REQUIRE(texture.read(std::span{ base }));
		CHECK(std::any_of(base.begin(), base.end(), [](Byte b) { return (b != Byte{ 0 }); }));
		Array<Byte> last(format.pixelSize());
		REQUIRE(texture.read(std::span{ last }, 2));
		CHECK(std::memcmp(base.data(), last.data(), last.size()) == 0);
	}
}

TEST_CASE("RenderTexture.clear_values_and_srgb")
{
	const ColorF color{ 0.25, 0.5, 0.75, 0.5 };
	for (const auto format : { TextureFormat::R8G8B8A8_Unorm, TextureFormat::R8G8B8A8_Unorm_SRGB })
	{
		const RenderTexture texture{ Size{ 7, 3 }, color, format };
		Image image;
		REQUIRE(texture.readAsImage(image));
		const Color expected = ((format == TextureFormat::R8G8B8A8_Unorm_SRGB) ? color.linearToSRGB().toColor() : color.toColor());
		for (const Color pixel : image)
		{
			CHECK(std::abs(static_cast<int>(pixel.r) - expected.r) <= 1);
			CHECK(std::abs(static_cast<int>(pixel.g) - expected.g) <= 1);
			CHECK(std::abs(static_cast<int>(pixel.b) - expected.b) <= 1);
			CHECK(std::abs(static_cast<int>(pixel.a) - expected.a) <= 1);
		}
	}
	const RenderTexture hdr{ Size{ 3, 2 }, ColorF{ -2.0, 4.0, 8.0, 0.25 }, TextureFormat::R32G32B32A32_Float };
	Grid<Float4> values;
	REQUIRE(hdr.read(values));
	for (const auto& pixel : values) { CHECK(pixel == Float4{ -2, 4, 8, 0.25f }); }
}

TEST_CASE("RenderTexture.mipmap_content_and_regeneration")
{
	Image checker{ 8, 8, Palette::Black };
	for (int32 y = 0; y < 8; ++y)
	{
		for (int32 x = 0; x < 8; ++x) { checker[y][x] = (((x + y) & 1) ? Palette::White : Palette::Black); }
	}
	RenderTexture texture{ checker, TextureDesc::Mipmap };
	Image mip;
	REQUIRE(texture.readAsImage(mip, 1));
	REQUIRE(mip.size() == Size{ 4, 4 });
	for (const Color pixel : mip) { CHECK(std::abs(static_cast<int>(pixel.r) - 128) <= 1); }
	texture.clear(Palette::Red);
	REQUIRE(texture.readAsImage(mip, 1));
	CHECK(std::abs(static_cast<int>(mip[0][0].r) - 128) <= 1);
	texture.generateMips();
	REQUIRE(texture.readAsImage(mip, 3));
	CHECK(mip == Image{ 1, 1, Palette::Red });
}

TEST_CASE("RenderTexture.empty_invalid_and_shared_handles")
{
	RenderTexture empty;
	CHECK_FALSE(empty);
	empty.clear(Palette::Red).generateMips();
	Image image{ 2, 2, Palette::Red };
	CHECK_FALSE(empty.readAsImage(image));
	CHECK(image.isEmpty());
	for (const Size size : { Size{ 0, 3 }, Size{ -1, 3 }, Size{ 3, 0 }, Size{ 16385, 1 } })
	{
		CHECK_FALSE(RenderTexture{ size });
	}
	for (const TextureFormat format : { TextureFormat{ TextureFormat::Unknown }, TextureFormat{ TextureFormat::BC1_RGBA_Unorm }, TextureFormat{ static_cast<TexturePixelFormat>(255) } })
	{
		CHECK_FALSE(RenderTexture{ Size{ 3, 2 }, format });
	}
	const Array<Byte> wrong(3);
	CHECK_FALSE(RenderTexture{ Size{ 3, 2 }, std::span<const Byte>{ wrong }, TextureFormat::R8G8B8A8_Unorm });
	RenderTexture texture{ Size{ 3, 2 }, Palette::Red };
	RenderTexture copy = texture;
	copy.clear(Palette::Blue);
	REQUIRE(texture.readAsImage(image));
	CHECK(image == Image{ 3, 2, Palette::Blue });
	texture.release();
	CHECK(copy);
	Grid<float> incompatible{ Size{ 2, 2 }, 1.0f };
	CHECK_FALSE(copy.read(incompatible));
	CHECK(incompatible.isEmpty());
	CHECK_FALSE(copy.readAsImage(image, 1));
	CHECK(image.isEmpty());
	Array<Byte> bytes(3, Byte{ 0xCD });
	CHECK_FALSE(copy.read(std::span{ bytes }));
	CHECK(bytes == Array<Byte>(3, Byte{ 0xCD }));
	swap(copy, empty);
	CHECK_FALSE(copy);
	CHECK(empty);
}

TEST_CASE("RenderTexture.drawing_clear_read_order_and_frame_continuation")
{
	const auto config = Scene::GetConfig();
	const auto samples = Scene::GetMSAASampleCount();
	const auto background = Scene::GetBackground();
	const ScopeExit restore{ [&] { Scene::SetBackground(background); Scene::SetMSAASampleCount(samples); Scene::Configure(config); } };
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedViewport2D viewport{ none };
	const ScopedScissorRect2D scissor{ none };
	Scene::Configure(SceneConfig::Fixed({ 48, 16 }));
	Scene::SetBackground(Palette::Black);
	for (const auto sampleCount : { MSAASampleCount::NoMSAA, Scene::DefaultMSAASampleCount })
	{
		Scene::SetMSAASampleCount(sampleCount);
		for (const bool explicitFlush : { false, true })
		{
			REQUIRE(System::Update());
			RenderTexture texture{ Size{ 16, 16 }, Palette::Red };
			texture.draw(0, 0);
			if (explicitFlush) { Graphics2D::Flush(); }
			texture.clear(Palette::Blue);
			texture.draw(16, 0);
			Image readback;
			const auto frame = Scene::FrameCount();
			REQUIRE(texture.readAsImage(readback));
			CHECK(Scene::FrameCount() == frame);
			CHECK(readback == Image{ 16, 16, Palette::Blue });
			texture.clear(Palette::Green);
			REQUIRE(texture.readAsImage(readback));
			texture.draw(32, 0);
			texture.release();
			ScreenCapture::RequestCurrentFrame();
			REQUIRE(System::Update());
			REQUIRE(ScreenCapture::HasNewFrame());
			const Image captured = ScreenCapture::GetFrame();
			CHECK(captured[8][8] == Palette::Red);
			CHECK(captured[8][24] == Palette::Blue);
			CHECK(captured[8][40] == Palette::Green);
		}
	}
}

TEST_CASE("RenderTexture.creation_preserves_pending_scene_draws")
{
	const auto config = Scene::GetConfig();
	const auto samples = Scene::GetMSAASampleCount();
	const auto background = Scene::GetBackground();
	const ScopeExit restore{ [&] { Scene::SetBackground(background); Scene::SetMSAASampleCount(samples); Scene::Configure(config); } };
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	Scene::SetMSAASampleCount(MSAASampleCount::NoMSAA);
	Scene::SetBackground(Palette::Black);
	for (const bool fromImage : { false, true })
	{
		Scene::Configure(SceneConfig::Fixed({ 32, 16 }));
		REQUIRE(System::Update());
		Rect{ 0, 0, 16, 16 }.draw(Palette::Red);
		const RenderTexture texture = fromImage ? RenderTexture{ Image{ 16, 16, Palette::Blue }, TextureDesc::Mipmap }
			: RenderTexture{ Size{ 16, 16 }, Palette::Blue, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
		REQUIRE(texture);
		// Changing the scene replaces executed pixels, but must retain pending draws.
		Scene::Configure(SceneConfig::Fixed({ 48, 16 }));
		texture.draw(16, 0);
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		const auto& captured = ScreenCapture::GetFrame();
		CHECK(captured[8][8] == Palette::Red);
		CHECK(captured[8][24] == Palette::Blue);
	}
}
