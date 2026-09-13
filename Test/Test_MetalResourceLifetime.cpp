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

# if SIV3D_PLATFORM(MACOS)

# include <objc/message.h>
# include <objc/runtime.h>

namespace
{
	// Avoid Foundation's legacy Point/Rect names in the shared C++ test PCH.
	auto MakeAutoreleasePool()
	{
		const auto send = reinterpret_cast<id(*)(id, SEL)>(objc_msgSend);
		id pool = send(reinterpret_cast<id>(objc_getClass("NSAutoreleasePool")), sel_registerName("alloc"));
		pool = send(pool, sel_registerName("init"));
		return ScopeExit{ [pool]
		{
			reinterpret_cast<void(*)(id, SEL)>(objc_msgSend)(pool, sel_registerName("drain"));
		} };
	}

	struct DrawScope
	{
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D blend{ BlendState::Opaque };
		ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
	};

	void UpdateWithPool()
	{
		const auto pool = MakeAutoreleasePool();
		REQUIRE(System::Update());
		// The next frame's command buffer must outlive this pool.
	}

	const Image& CaptureFrame()
	{
		ScreenCapture::RequestCurrentFrame();
		UpdateWithPool();
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("MetalResourceLifetime.texture_creation")
{
	const DrawScope drawScope;
	Array<Texture> textures;
	{
		const auto pool = MakeAutoreleasePool();
		const Image image{ 8, 8, Palette::Red };
		const Array<Image> mipmaps{ Image{ 4, 4, Palette::Red }, Image{ 2, 2, Palette::Red }, Image{ 1, 1, Palette::Red } };
		textures.emplace_back(image, TextureDesc::NoMipmap);
		textures.emplace_back(image, TextureDesc::Mipmap);
		textures.emplace_back(image, Array<Image>{}, TextureDesc::NoMipmap);
		textures.emplace_back(image, mipmaps, TextureDesc::Mipmap);

		// One BC1 block: red RGB565 endpoint and all indices selecting it.
		const std::array<uint8, 8> block{ 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		const BCnData bcn{ Size{ 4, 4 }, TextureFormat::BC1_RGBA_Unorm, { Blob{ block.data(), block.size() } } };
		textures.emplace_back(bcn);
		for (const auto& texture : textures)
		{
			REQUIRE(texture);
		}
	}
	// Creation and upload temporaries have been drained; textures stay usable.
	UpdateWithPool();
	for (size_t i = 0; i < textures.size(); ++i)
	{
		textures[i].resized(16, 16).draw((20 + i * 24), 20);
	}
	const Image& captured = CaptureFrame();
	REQUIRE(captured.width() >= 140);
	REQUIRE(captured.height() >= 36);
	for (size_t i = 0; i < textures.size(); ++i)
	{
		CHECK(captured[28][28 + i * 24] == Palette::Red);
	}
}

TEST_CASE("MetalResourceLifetime.dynamic_uploads")
{
	const DrawScope drawScope;
	for (const auto desc : { TextureDesc::NoMipmap, TextureDesc::Mipmap })
	{
		DynamicTexture texture;
		{
			const auto pool = MakeAutoreleasePool();
			texture = DynamicTexture{ Image{ 8, 8, Palette::Black }, desc };
			REQUIRE(texture);
		}
		UpdateWithPool();
		for (int32 frame = 0; frame < 4; ++frame)
		{
			INFO(frame);
			{
				const auto pool = MakeAutoreleasePool();
				REQUIRE(texture.fill(Palette::Black));
			}
			{
				const auto pool = MakeAutoreleasePool();
				// Each update inspects the preceding upload after its pool drained.
				REQUIRE(texture.fill(Image{ 8, 8, Palette::Red }));
			}
			{
				const auto pool = MakeAutoreleasePool();
				REQUIRE(texture.fillRegion(Palette::Blue, Rect{ 2, 2, 4, 4 }));
			}
			{
				const auto pool = MakeAutoreleasePool();
				REQUIRE(texture.fillRegion(Image{ 2, 2, Palette::Lime }, Rect{ 3, 3, 2, 2 }));
			}
			{
				const auto pool = MakeAutoreleasePool();
				texture.generateMips();
			}
			texture.resized(32, 32).draw(20, 20);
			const Image& captured = CaptureFrame();
			REQUIRE(captured.width() >= 52);
			REQUIRE(captured.height() >= 52);
			CHECK(captured[22][22] == Palette::Red);
			CHECK(captured[29][29] == Palette::Blue);
			CHECK(captured[33][33] == Palette::Lime);
		}
	}
}

TEST_CASE("MetalResourceLifetime.frame_command_buffer")
{
	const DrawScope drawScope;
	UpdateWithPool();
	for (const auto color : { Palette::Red, Palette::Blue, Palette::Lime, Palette::White })
	{
		// Encode and capture after the pool used by beginFrame has drained.
		Rect{ 20, 20, 16, 16 }.draw(color);
		const Image& captured = CaptureFrame();
		REQUIRE(captured.width() >= 36);
		REQUIRE(captured.height() >= 36);
		CHECK(captured[28][28] == color);
	}
}

# endif
