//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
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
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, MSRenderTexture>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, Optional<MSRenderTexture>>);
	static_assert(std::is_constructible_v<ScopedRenderTarget2D, None_t>);
	static_assert(not std::is_constructible_v<ScopedRenderTarget2D, Texture>);
	static_assert(not std::is_constructible_v<ScopedRenderTarget2D, Optional<Texture>>);

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
			const ScopedRenderTarget2D scene{ none };
			const ScopedViewport2D viewport{ none };
			const ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
			const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
			const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
			const ScopedScissorRect2D scissor{ none };
			const ScopedRenderStates2D blend{ BlendState::Opaque };
			const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
			const ScopedColorAdd2D add{ ColorF{ 0.0, 0.0 }, ScopedColorAdd2D::Target::Set };
		};
	}

	TEST_CASE("MSRenderTexture.mrt_formats_and_native_attachments")
	{
		const DrawState state;
	# if SIV3D_PLATFORM(MACOS)
		const PixelShader ps = PixelShader::MSL(std::string{ R"(
#include <metal_stdlib>
using namespace metal;
struct Output { float4 a [[color(0)]]; float4 b [[color(1)]]; };
fragment Output PS() { return Output{ float4(1), float4(1) }; }
)" }, U"PS");
	# else
		const PixelShader ps = PixelShader::HLSL(std::string{ R"(
struct Output { float4 a : SV_Target0; float4 b : SV_Target1; };
Output PS() { Output o; o.a = float4(1, 1, 1, 1); o.b = o.a; return o; }
)" }, U"PS");
	# endif
		REQUIRE(ps);
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			const MSRenderTexture first{ Size{ 7, 5 }, Palette::Black, samples };
			REQUIRE(first);
			for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
			{
				const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
				CAPTURE(samples, format);
				const MSRenderTexture second{ first.size(), ColorF{ 0, 0 }, samples, format };
				if (not Supports(format, samples, false)) { CHECK_FALSE(second); continue; }
				REQUIRE(second);
				const RenderTexture white{ first.size(), Palette::White, format };
				{
					const ScopedRenderTarget2D target{ first, second };
					const ScopedCustomShader2D shader{ ps };
					Rect{ first.size() }.draw();
				}
				first.resolve(); second.resolve();
				CHECK(ReadBytes(second) == ReadBytes(white));
				for (const auto& texture : { first, second })
				{
				# if SIV3D_PLATFORM(WINDOWS)
					auto* manager = static_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture));
					D3D11_RENDER_TARGET_VIEW_DESC view{};
					manager->getRTV(texture.id())->GetDesc(&view);
					CHECK(view.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMS);
					ComPtr<ID3D11Resource> resource;
					manager->getRTV(texture.id())->GetResource(&resource);
					ComPtr<ID3D11Texture2D> attachment;
					REQUIRE(SUCCEEDED(resource.As(&attachment)));
					D3D11_TEXTURE2D_DESC desc{};
					attachment->GetDesc(&desc);
					CHECK(desc.SampleDesc.Count == FromEnum(samples));
					CHECK(desc.SampleDesc.Quality == 0);
					CHECK(attachment.Get() != manager->getTexture(texture.id()));
				# else
					auto* manager = static_cast<CTexture_Metal*>(SIV3D_ENGINE(Texture));
					auto* attachment = manager->getRenderTargetTexture(texture.id());
					CHECK(attachment->textureType() == MTL::TextureType2DMultisample);
					CHECK(attachment->sampleCount() == FromEnum(samples));
					CHECK(attachment != manager->getTexture(texture.id()));
				# endif
				}
			}
		}
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

	TEST_CASE("MSRenderTexture.target_formats_samples_and_explicit_resolve")
	{
		const DrawState state;
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			for (uint8 i = FromEnum(TexturePixelFormat::R8_Unorm); i <= FromEnum(TexturePixelFormat::R32G32B32A32_Float); ++i)
			{
				const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
				CAPTURE(samples, format);
				const MSRenderTexture texture{ Size{ 13, 7 }, samples, format };
				if (not Supports(format, samples, false)) { CHECK_FALSE(texture); continue; }
				REQUIRE(texture);
				{
					const ScopedRenderTarget2D target{ texture };
					CHECK(Graphics2D::GetDrawingSize() == texture.size());
					CHECK(Graphics2D::GetRenderTargetSize() == texture.size());
					Rect{ texture.size() }.draw(Palette::White);
				}
				CHECK(ReadBytes(texture).all([](Byte b) { return b == Byte{ 0 }; }));
				texture.resolve();
				const RenderTexture reference{ texture.size(), Palette::White, format };
				CHECK(ReadBytes(texture) == ReadBytes(reference));
			}
		}
	}

	TEST_CASE("MSRenderTexture.target_coverage_and_native_attachments")
	{
		const DrawState state;
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			CAPTURE(samples);
			const MSRenderTexture texture{ Size{ 16, 16 }, Palette::Black, samples };
			if (not Supports(TextureFormat::R8G8B8A8_Unorm, samples, false)) { CHECK_FALSE(texture); continue; }
			REQUIRE(texture);
			{
				const ScopedRenderTarget2D target{ texture };
				Triangle{ Vec2{ 1.2, 1.1 }, Vec2{ 14.6, 2.3 }, Vec2{ 2.7, 14.5 } }.draw(Palette::White);
			}
			texture.resolve();
			Image pixels;
			REQUIRE(texture.readAsImage(pixels));
			CHECK(std::any_of(pixels.begin(), pixels.end(), [](Color c)
			{
				return (0 < c.r) && (c.r < 255) && (c.r == c.g) && (c.r == c.b);
			}));
			CHECK(pixels[15][15] == Palette::Black);
			CHECK(pixels[4][4] == Palette::White);
		# if SIV3D_PLATFORM(WINDOWS)
			auto* manager = static_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture));
			D3D11_RENDER_TARGET_VIEW_DESC view{};
			manager->getRTV(texture.id())->GetDesc(&view);
			CHECK(view.ViewDimension == D3D11_RTV_DIMENSION_TEXTURE2DMS);
			ComPtr<ID3D11Resource> resource;
			manager->getRTV(texture.id())->GetResource(&resource);
			ComPtr<ID3D11Texture2D> attachment;
			REQUIRE(SUCCEEDED(resource.As(&attachment)));
			D3D11_TEXTURE2D_DESC desc{};
			attachment->GetDesc(&desc);
			CHECK(desc.SampleDesc.Count == FromEnum(samples));
			CHECK(attachment.Get() != manager->getTexture(texture.id()));
		# else
			auto* manager = static_cast<CTexture_Metal*>(SIV3D_ENGINE(Texture));
			auto* attachment = manager->getRenderTargetTexture(texture.id());
			CHECK(attachment->sampleCount() == FromEnum(samples));
			CHECK(attachment != manager->getTexture(texture.id()));
		# endif
		}
	}

	TEST_CASE("MSRenderTexture.target_pending_draws_readback_and_mips")
	{
		const DrawState state;
		for (const bool explicitFlush : { false, true })
		{
			CAPTURE(explicitFlush);
			const MSRenderTexture texture{ Size{ 8, 8 }, Palette::Blue, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
			Image pixels;
			{
				const ScopedRenderTarget2D target{ texture };
				Rect{ 8, 8 }.draw(Palette::Red);
				if (explicitFlush) { Graphics2D::Flush(); }
				texture.resolve();
				// Reading flushes the pending draw, but never resolves it retroactively.
				REQUIRE(texture.readAsImage(pixels));
				CHECK(pixels == Image{ 8, 8, explicitFlush ? Palette::Red : Palette::Blue });
				texture.resolve(); // The draw has now been flushed by readback.
				REQUIRE(texture.readAsImage(pixels));
				CHECK(pixels == Image{ 8, 8, Palette::Red });
				REQUIRE(texture.readAsImage(pixels, 1));
				CHECK(pixels == Image{ 4, 4, Palette::Blue });
				texture.generateMips();
				REQUIRE(texture.readAsImage(pixels, 1));
				CHECK(pixels == Image{ 4, 4, Palette::Red });
				// A clear affects MSAA only, and later passes load those samples.
				texture.clear(Palette::Green);
				REQUIRE(texture.readAsImage(pixels));
				CHECK(pixels == Image{ 8, 8, Palette::Red });
				Rect{ 4, 8 }.draw(Palette::Yellow);
				Graphics2D::Flush();
				texture.resolve();
				REQUIRE(texture.readAsImage(pixels));
				CHECK(pixels[4][2] == Palette::Yellow);
				CHECK(pixels[4][6] == Palette::Green);
				Rect{ 0, 0, 2, 2 }.draw(Palette::White);
			}
			// Scope exit flushes, but leaves the previously resolved image intact.
			REQUIRE(texture.readAsImage(pixels));
			CHECK(pixels[1][1] == Palette::Yellow);
			texture.resolve();
			REQUIRE(texture.readAsImage(pixels));
			CHECK(pixels[1][1] == Palette::White);
			CHECK(pixels[4][6] == Palette::Green);
		}
	}

	TEST_CASE("MSRenderTexture.target_nesting_move_empty_and_retention")
	{
		const DrawState state;
		MSRenderTexture a{ Size{ 16, 8 }, Palette::Black, MSAASampleCount::X4MSAA };
		const auto retained = a;
		const MSRenderTexture b{ Size{ 8, 8 }, Palette::Blue, MSAASampleCount::X2MSAA };
		const RenderTexture single{ Size{ 8, 8 }, Palette::Black };
		const ScopedViewport2D originalViewport{ Rect{ 2, 3, 10, 4 } };
		{
			ScopedRenderTarget2D first{ Optional<MSRenderTexture>{ a } };
			a.release();
			Rect{ 16, 8 }.draw(Palette::White);
			const ScopedViewport2D viewport{ Rect{ 1, 2, 12, 5 } };
			CHECK_THROWS_AS(ScopedRenderTarget2D{ MSRenderTexture{} }, Error);
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == retained.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 12, 5 });
			{
				const ScopedRenderTarget2D second{ b };
				CHECK_FALSE(Graphics2D::GetViewport());
				Rect{ 8, 8 }.draw(Palette::Red);
				{
					const ScopedRenderTarget2D third{ single };
					b.draw(); // B still exposes its initial resolved image.
				}
				{
					const ScopedRenderTarget2D scene{ Optional<MSRenderTexture>{} };
					CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
				}
				CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == b.id());
			}
			CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == retained.id());
			CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 12, 5 });
			{
				const ScopedRenderTarget2D same{ retained };
				CHECK_FALSE(Graphics2D::GetViewport());
			}
			CHECK(Graphics2D::GetViewport() == Rect{ 1, 2, 12, 5 });
			{ const ScopedRenderTarget2D moved{ std::move(first) }; }
			CHECK_FALSE(SIV3D_ENGINE(Renderer2D)->getRenderTarget());
			CHECK(Graphics2D::GetViewport() == Rect{ 2, 3, 10, 4 });
		}
		Image pixels;
		REQUIRE(single.readAsImage(pixels));
		CHECK(pixels == Image{ 8, 8, Palette::Blue });
		retained.resolve();
		REQUIRE(retained.readAsImage(pixels));
		CHECK(pixels == Image{ 16, 8, Palette::White });
		b.resolve();
		REQUIRE(b.readAsImage(pixels));
		CHECK(pixels == Image{ 8, 8, Palette::Red });
	}

	TEST_CASE("MSRenderTexture.target_can_sample_resolved_image_and_restores_feedback_checks")
	{
		const DrawState state;
		const MSRenderTexture ms{ Size{ 8, 4 }, Palette::Blue };
		const RenderTexture single{ Size{ 8, 4 }, Palette::Red };
		const Texture alias = ms;
		{
			const ScopedRenderTarget2D outer{ single };
			ms.draw(); // Validate MS as an input to a single-sample target.
			{
				const ScopedRenderTarget2D inner{ ms };
				ms.clear(Palette::Green);
				CHECK_NOTHROW(alias(0, 0, 4, 4).draw());
				Graphics2D::Flush();
				ms.resolve();
				CHECK_NOTHROW(alias(0, 0, 4, 4).draw(4, 0));
				CHECK_NOTHROW(single.draw(-8, 0)); // Must not poison validation after restore.
			}
			CHECK_THROWS_AS(single.draw(), Error);
			CHECK_NOTHROW(ms.draw());
		}
		ms.resolve();
		Image pixels;
		REQUIRE(ms.readAsImage(pixels));
		CHECK(pixels == Image{ 8, 4, Palette::Blue });
		CHECK_THROWS_AS(([&]
		{
			const ScopedRenderTarget2D outer{ ms };
			const ScopedRenderTarget2D inner{ single };
			single.draw();
		}()), Error);
	}


	TEST_CASE("MSRenderTexture.target_srgb_and_hdr")
	{
		const DrawState state;
		for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
		{
			for (const TextureFormat format : { TextureFormat::R8G8B8A8_Unorm_SRGB, TextureFormat::R16G16B16A16_Float })
			{
				CAPTURE(samples, format);
				if (not Supports(format, samples, false)) { continue; }
				const MSRenderTexture ms{ Size{ 8, 4 }, samples, format };
				const RenderTexture single{ ms.size(), format };
				const ColorF color = (format.isSRGB() ? ColorF{ 0.125, 0.25, 0.5, 1.0 } : ColorF{ 2.0, 0.5, 4.0, 1.0 });
				{
					const ScopedRenderTarget2D outer{ ms };
					Rect{ ms.size() }.draw(color);
					const ScopedRenderTarget2D inner{ single };
					Rect{ single.size() }.draw(color);
				}
				ms.resolve();
				if (format.isSRGB())
				{
					Image actual, expected;
					REQUIRE(ms.readAsImage(actual));
					REQUIRE(single.readAsImage(expected));
					for (size_t i = 0; i < actual.pixelCount(); ++i)
					{
						CHECK(Abs(int32(actual.data()[i].r) - expected.data()[i].r) <= 1);
						CHECK(Abs(int32(actual.data()[i].g) - expected.data()[i].g) <= 1);
						CHECK(Abs(int32(actual.data()[i].b) - expected.data()[i].b) <= 1);
						CHECK(actual.data()[i].a == 255);
					}
					CHECK(actual[0][0].r > 80); // Stored sRGB, rather than the linear value 32.
				}
				else
				{
					Grid<std::array<HalfFloat, 4>> actual;
					REQUIRE(ms.read(actual));
					for (const auto& pixel : actual)
					{
						CHECK(float(pixel[0]) == 2.0f);
						CHECK(float(pixel[1]) == 0.5f);
						CHECK(float(pixel[2]) == 4.0f);
						CHECK(float(pixel[3]) == 1.0f);
					}
				}
			}
		}
	}

	TEST_CASE("MSRenderTexture.target_scene_samples_and_frame_continuation")
	{
		const DrawState state;
		const auto config = Scene::GetConfig();
		const auto sceneSamples = Scene::GetMSAASampleCount();
		const auto background = Scene::GetBackground();
		const ScopeExit restore{ [&]
		{
			Scene::SetBackground(background);
			Scene::SetMSAASampleCount(sceneSamples);
			Scene::Configure(config);
		} };
		Scene::Configure(SceneConfig::Fixed({ 40, 20 }, SceneResolution::Display));
		Scene::SetBackground(Palette::Black);
		for (const auto sceneCount : { MSAASampleCount::NoMSAA, MSAASampleCount::X4MSAA })
		{
			Scene::SetMSAASampleCount(sceneCount);
			for (const auto samples : { MSAASampleCount::X2MSAA, MSAASampleCount::X4MSAA })
			{
				CAPTURE(sceneCount, samples);
				if (not Supports(TextureFormat::R8G8B8A8_Unorm, samples, false)) { continue; }
				REQUIRE(System::Update());
				const MSRenderTexture texture{ Size{ 16, 8 }, Palette::Black, samples };
				Image pixels;
				{
					const ScopedRenderTarget2D target{ texture };
					Rect{ 8, 8 }.draw(Palette::Red);
					ScreenCapture::RequestCurrentFrame();
					REQUIRE(System::Update());
					CHECK(SIV3D_ENGINE(Renderer2D)->getRenderTarget()->id() == texture.id());
					CHECK(SIV3D_ENGINE(Renderer2D)->getSceneScale() == Float2{ 1, 1 });
					CHECK(SIV3D_ENGINE(Renderer2D)->getRasterScaling() == 1.0f);
					CHECK(Graphics2D::GetDrawingSize() == texture.size());
					CHECK(Graphics2D::GetRenderTargetSize() == texture.size());
					REQUIRE(ScreenCapture::HasNewFrame());
					const auto& blank = ScreenCapture::GetFrame();
					CHECK(blank[blank.height() / 2][blank.width() / 2] == Palette::Black);
					REQUIRE(texture.readAsImage(pixels));
					CHECK(pixels == Image{ 16, 8, Palette::Black }); // Frame completion did not resolve.
					texture.resolve();
					Rect{ 8, 0, 8, 8 }.draw(Palette::Green);
				}
				REQUIRE(texture.readAsImage(pixels));
				CHECK(pixels[4][4] == Palette::Red);
				CHECK(pixels[4][12] == Palette::Black); // Scope exit did not resolve either.
				texture.resolve();
				texture.draw(16, 0);
				ScreenCapture::RequestCurrentFrame();
				REQUIRE(System::Update());
				REQUIRE(ScreenCapture::HasNewFrame());
				const auto& captured = ScreenCapture::GetFrame();
				CHECK(captured[4 * captured.height() / 20][20 * captured.width() / 40] == Palette::Red);
				CHECK(captured[4 * captured.height() / 20][28 * captured.width() / 40] == Palette::Green);
			}
		}
	}
}
