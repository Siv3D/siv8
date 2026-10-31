//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Texture/D3D11/CTexture_D3D11.hpp>
# else
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Texture/Metal/CTexture_Metal.hpp>
# endif

namespace s3d
{
	static_assert(std::is_base_of_v<Texture, MSRenderTexture>);
	static_assert(not std::is_base_of_v<RenderTexture, MSRenderTexture>);
	static_assert(not std::is_constructible_v<ScopedRenderTarget2D, MSRenderTexture>);

	namespace
	{
		bool Supports(const TextureFormat& format, const MSAASampleCount samples, const bool mips)
		{
		# if SIV3D_PLATFORM(WINDOWS)
			auto* device = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice().getDevice();
			UINT levels = 0, flags = 0;
			const auto nativeFormat = static_cast<DXGI_FORMAT>(format.DXGIFormat());
			const UINT required = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE
				| D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET
				| D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE | (mips ? D3D11_FORMAT_SUPPORT_MIP_AUTOGEN : 0u);
			return (SUCCEEDED(device->CheckFormatSupport(nativeFormat, &flags)) && ((flags & required) == required)
				&& SUCCEEDED(device->CheckMultisampleQualityLevels(nativeFormat, FromEnum(samples), &levels)) && levels);
		# else
			auto* device = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer))->getDevice();
			const bool float32 = ((format == TextureFormat::R32_Float) || (format == TextureFormat::R32G32_Float)
				|| (format == TextureFormat::R32G32B32A32_Float));
			return device->supportsTextureSampleCount(FromEnum(samples))
				&& (not float32 || (device->supports32BitMSAA() && (not mips || device->supports32BitFloatFiltering())));
		# endif
		}

		template <class TextureType>
		Array<Byte> ReadBytes(const TextureType& texture, const uint32 level = 0)
		{
			Array<Byte> result(static_cast<size_t>(Max(1, texture.width() >> level))
				* Max(1, texture.height() >> level) * texture.format().pixelSize());
			REQUIRE(texture.read(std::span{ result }, level));
			return result;
		}

		template <class Pixel>
		void CheckTypedRead(const TextureFormat& format)
		{
			if (not Supports(format, MSAASampleCount::X4MSAA, false)) { return; }
			const MSRenderTexture texture{ Size{ 7, 3 }, ColorF{ 1.0, 1.0 }, format };
			REQUIRE(texture);
			Grid<Pixel> pixels;
			REQUIRE(texture.read(pixels));
			CHECK(pixels.size() == texture.size());
			const auto expected = ReadBytes(texture);
			const auto bytes = std::as_bytes(std::span{ pixels });
			CHECK(std::equal(bytes.begin(), bytes.end(), expected.begin(), expected.end()));
		}

		struct DrawState
		{
			const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
			const ScopedScissorRect2D scissor{ none };
			const ScopedRenderStates2D blend{ BlendState::Opaque };
			const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
			const ScopedColorAdd2D add{ ColorF{ 0.0, 0.0 }, ScopedColorAdd2D::Target::Set };
		};
	}

	TEST_CASE("MSRenderTexture.formats_samples_clear_resolve_and_mips")
	{
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
			{
				const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
				for (const bool mips : { false, true })
				{
					CAPTURE(samples, format, mips);
					const auto desc = mips ? TextureDesc::Mipmap : TextureDesc::NoMipmap;
					const MSRenderTexture texture{ Size{ 7, 5 }, samples, format, desc };
					if (not Supports(format, samples, mips))
					{
						CHECK_FALSE(texture);
						continue;
					}
					REQUIRE(texture);
					CHECK(texture.getMSAASampleCount() == samples);
					CHECK(texture.size() == Size{ 7, 5 });
					CHECK(texture.format() == format);
					CHECK(texture.isSRGB() == format.isSRGB());
					CHECK_FALSE(texture.hasDepth());
					CHECK(texture.mipLevels() == (mips ? 3 : 1));
					const RenderTexture white{ Size{ 7, 5 }, ColorF{ 1.0, 1.0 }, format, desc };
					for (uint32 level = 0; level < texture.mipLevels(); ++level)
					{
						CHECK(ReadBytes(texture, level).all([](Byte b) { return b == Byte{ 0 }; }));
					}
					CHECK(&texture.clear(ColorF{ 1.0, 1.0 }) == &texture);
					// Neither clear, readback nor mip generation implicitly resolves.
					texture.generateMips();
					for (uint32 level = 0; level < texture.mipLevels(); ++level)
					{
						CHECK(ReadBytes(texture, level).all([](Byte b) { return b == Byte{ 0 }; }));
					}
					texture.resolve();
					CHECK(ReadBytes(texture) == ReadBytes(white));
					if (mips) { CHECK(ReadBytes(texture, 2).all([](Byte b) { return b == Byte{ 0 }; })); }
					texture.generateMips();
					for (uint32 level = 0; level < texture.mipLevels(); ++level)
					{
						CHECK(ReadBytes(texture, level) == ReadBytes(white, level));
					}
					// Resolve preserves MSAA contents across a submission/readback boundary.
					texture.resolve();
					CHECK(ReadBytes(texture) == ReadBytes(white));
					texture.clear(ColorF{ 0.0, 0.0 });
					CHECK(ReadBytes(texture) == ReadBytes(white));
					texture.resolve();
					CHECK(ReadBytes(texture).all([](Byte b) { return b == Byte{ 0 }; }));
				}
			}
		}
	}

	TEST_CASE("MSRenderTexture.initial_color_typed_reads_and_srgb")
	{
		const MSRenderTexture defaultSamples{ Size{ 1, 1 } };
		REQUIRE(defaultSamples);
		CHECK(defaultSamples.getMSAASampleCount() == MSAASampleCount::X4MSAA);
		Image image;
		REQUIRE(defaultSamples.readAsImage(image));
		CHECK(image == Image{ 1, 1, Color{ 0, 0 } });
		for (const auto format : { TextureFormat::R8G8B8A8_Unorm, TextureFormat::R8G8B8A8_Unorm_SRGB })
		{
			const ColorF color{ 0.25, 0.5, 0.75, 0.5 };
			const MSRenderTexture ms{ Size{ 3, 7 }, color, format, TextureDesc::Mipmap };
			const RenderTexture reference{ Size{ 3, 7 }, color, format, TextureDesc::Mipmap };
			REQUIRE(ms);
			for (uint32 level = 0; level < ms.mipLevels(); ++level)
			{
				Image expected;
				REQUIRE(reference.readAsImage(expected, level));
				REQUIRE(ms.readAsImage(image, level));
				REQUIRE(image.size() == expected.size());
				for (size_t i = 0; i < image.pixelCount(); ++i)
				{
					CHECK(Abs(int32(image.data()[i].r) - int32(expected.data()[i].r)) <= 1);
					CHECK(Abs(int32(image.data()[i].g) - int32(expected.data()[i].g)) <= 1);
					CHECK(Abs(int32(image.data()[i].b) - int32(expected.data()[i].b)) <= 1);
					CHECK(image.data()[i].a == expected.data()[i].a);
				}
			}
		}
		const MSRenderTexture hdr{ Size{ 3, 2 }, ColorF{ 2.0, -4.0, 8.0, 0.5 }, TextureFormat::R16G16B16A16_Float };
		Grid<std::array<HalfFloat, 4>> pixels;
		REQUIRE(hdr.read(pixels));
		CHECK(pixels[0][0] == std::array<HalfFloat, 4>{ HalfFloat{ 2.0f }, HalfFloat{ -4.0f }, HalfFloat{ 8.0f }, HalfFloat{ 0.5f } });
		CheckTypedRead<uint8>(TextureFormat::R8_Unorm);
		CheckTypedRead<uint16>(TextureFormat::R8G8_Unorm);
		CheckTypedRead<HalfFloat>(TextureFormat::R16_Float);
		CheckTypedRead<Color>(TextureFormat::R8G8B8A8_Unorm);
		CheckTypedRead<std::pair<uint16, uint16>>(TextureFormat::R16G16_Unorm);
		CheckTypedRead<std::pair<HalfFloat, HalfFloat>>(TextureFormat::R16G16_Float);
		CheckTypedRead<float>(TextureFormat::R32_Float);
		CheckTypedRead<std::array<HalfFloat, 4>>(TextureFormat::R16G16B16A16_Float);
		CheckTypedRead<Float2>(TextureFormat::R32G32_Float);
		CheckTypedRead<Float4>(TextureFormat::R32G32B32A32_Float);
		CheckTypedRead<uint32>(TextureFormat::R10G10B10A2_Unorm);
		CheckTypedRead<uint32>(TextureFormat::R11G11B10_UFloat);
		CheckTypedRead<uint64>(TextureFormat::R16G16B16A16_Float);
	}

	TEST_CASE("MSRenderTexture.invalid_empty_and_read_failures")
	{
		for (const uint8 count : std::array<uint8, 5>{ 0, 1, 3, 8, 255 })
		{
			CHECK_FALSE(MSRenderTexture{ Size{ 3, 2 }, static_cast<MSAASampleCount>(count) });
		}
		for (const auto size : { Size{ 0, 2 }, Size{ 2, 0 }, Size{ -1, 2 }, Size{ 16385, 1 }, Size{ 1, 16385 } })
		{
			CHECK_FALSE(MSRenderTexture{ size });
		}
		for (const auto format : std::array<TextureFormat, 3>{ TextureFormat::Unknown, TextureFormat::BC1_RGBA_Unorm, TextureFormat{ static_cast<TexturePixelFormat>(255) } })
		{
			CHECK_FALSE(MSRenderTexture{ Size{ 3, 2 }, format });
		}
		const MSRenderTexture empty;
		CHECK(empty.getMSAASampleCount() == MSAASampleCount::NoMSAA);
		empty.clear(Palette::Red);
		empty.resolve();
		empty.generateMips();
		Image image{ 2, 2, Palette::White };
		CHECK_FALSE(empty.readAsImage(image));
		CHECK(image.isEmpty());
		const MSRenderTexture texture{ Size{ 3, 2 }, Palette::Blue };
		REQUIRE(texture);
		Grid<float> incompatible{ Size{ 2, 2 }, 3.0f };
		CHECK_FALSE(texture.read(incompatible));
		CHECK(incompatible.isEmpty());
		Array<Byte> invalid(23, Byte{ 0xCD });
		CHECK_FALSE(texture.read(std::span{ invalid }));
		CHECK(invalid.all([](Byte b) { return b == Byte{ 0xCD }; }));
		invalid.resize(24, Byte{ 0xCD });
		CHECK_FALSE(texture.read(std::span{ invalid }, 99));
		CHECK(invalid.all([](Byte b) { return b == Byte{ 0xCD }; }));
		image = Image{ 2, 2, Palette::White };
		CHECK_FALSE(texture.readAsImage(image, 1));
		CHECK(image.isEmpty());
	}

	TEST_CASE("MSRenderTexture.resolve_does_not_flush_pending_draws")
	{
		const DrawState state;
		for (const bool explicitFlush : { false, true })
		{
			const MSRenderTexture source{ Size{ 4, 4 }, Palette::Blue };
			const RenderTexture output{ Size{ 8, 4 }, Palette::Black };
			source.clear(Palette::Red);
			{
				const ScopedRenderTarget2D target{ output };
				source.draw();
				if (explicitFlush) { Graphics2D::Flush(); }
				source.resolve();
				source.draw(4, 0);
			}
			Image pixels;
			REQUIRE(output.readAsImage(pixels));
			CHECK(pixels[2][2] == (explicitFlush ? Palette::Blue : Palette::Red));
			CHECK(pixels[2][6] == Palette::Red);
		}
	}

	TEST_CASE("MSRenderTexture.shared_handles_and_sampled_resource")
	{
		const DrawState state;
		MSRenderTexture texture{ Size{ 4, 4 }, Palette::Blue };
		auto alias = texture;
		const Texture sampled = texture;
		CHECK(alias.id() == sampled.id());
		alias.clear(Palette::Red);
		Image pixels;
		REQUIRE(texture.readAsImage(pixels));
		CHECK(pixels == Image{ 4, 4, Palette::Blue });
		texture.release();
		alias.resolve();
		const RenderTexture output{ Size{ 8, 4 }, Palette::Black };
		{
			const ScopedRenderTarget2D target{ output };
			sampled.draw();
			alias(0, 0, 4, 4).draw(4, 0);
			alias.release();
		}
		REQUIRE(output.readAsImage(pixels));
		CHECK(pixels == Image{ 8, 4, Palette::Red });
		MSRenderTexture replacement{ Size{ 1, 1 }, Palette::Green };
		swap(alias, replacement); // Exchange an empty handle and a live pair.
		CHECK_FALSE(replacement);
		CHECK(alias.getMSAASampleCount() == MSAASampleCount::X4MSAA);
		REQUIRE(alias.readAsImage(pixels));
		CHECK(pixels == Image{ 1, 1, Palette::Green });
		// The Texture view always exposes the resolved, single-sample native image.
	# if SIV3D_PLATFORM(WINDOWS)
		auto* manager = static_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture));
		D3D11_TEXTURE2D_DESC native{};
		manager->getTexture(sampled.id())->GetDesc(&native);
		CHECK(native.SampleDesc.Count == 1);
		D3D11_SHADER_RESOURCE_VIEW_DESC srv{};
		(*manager->getSRVPtr(sampled.id()))->GetDesc(&srv);
		CHECK(srv.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D);
	# else
		auto* native = static_cast<CTexture_Metal*>(SIV3D_ENGINE(Texture))->getTexture(sampled.id());
		CHECK(native->sampleCount() == 1);
		CHECK(native->textureType() == MTL::TextureType2D);
	# endif
	}
}
