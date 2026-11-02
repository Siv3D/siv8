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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/TextureFormat.hpp>
# include <Siv3D/Texture.hpp>
# include <Siv3D/HalfFloat.hpp>
# include <array>
# include <ranges>
# include <type_traits>
# include <utility>

namespace s3d
{
	[[nodiscard]]
	bool IsValidRenderTexture(const Size& size, std::span<const Byte> data, const TextureFormat& format) noexcept;

	[[nodiscard]]
	bool ReadRenderTexture(const Texture& texture, std::span<Byte> destination, uint32 mipLevel);

	// Typed readback follows the public format mapping, not just sizeof(Pixel).
	template <class Pixel>
	[[nodiscard]]
	constexpr bool IsRenderTextureReadCompatible(const TexturePixelFormat format) noexcept
	{
		if constexpr (std::is_same_v<Pixel, uint8>)
		{
			return (format == TexturePixelFormat::R8_Unorm);
		}
		else if constexpr (std::is_same_v<Pixel, uint16>)
		{
			return (format == TexturePixelFormat::R8G8_Unorm);
		}
		else if constexpr (std::is_same_v<Pixel, HalfFloat>)
		{
			return (format == TexturePixelFormat::R16_Float);
		}
		else if constexpr (std::is_same_v<Pixel, Color>)
		{
			return (format == TexturePixelFormat::R8G8B8A8_Unorm)
				|| (format == TexturePixelFormat::R8G8B8A8_Unorm_SRGB);
		}
		else if constexpr (std::is_same_v<Pixel, std::pair<uint16, uint16>>)
		{
			return (format == TexturePixelFormat::R16G16_Unorm);
		}
		else if constexpr (std::is_same_v<Pixel, std::pair<HalfFloat, HalfFloat>>)
		{
			return (format == TexturePixelFormat::R16G16_Float);
		}
		else if constexpr (std::is_same_v<Pixel, float>)
		{
			return (format == TexturePixelFormat::R32_Float);
		}
		else if constexpr (std::is_same_v<Pixel, std::array<HalfFloat, 4>>)
		{
			return (format == TexturePixelFormat::R16G16B16A16_Float);
		}
		else if constexpr (std::is_same_v<Pixel, Float2>)
		{
			return (format == TexturePixelFormat::R32G32_Float);
		}
		else if constexpr (std::is_same_v<Pixel, Float4>)
		{
			return (format == TexturePixelFormat::R32G32B32A32_Float);
		}
		else if constexpr (std::is_same_v<Pixel, uint32>)
		{
			return (format == TexturePixelFormat::R16G16_Unorm)
				|| (format == TexturePixelFormat::R16G16_Float)
				|| (format == TexturePixelFormat::R10G10B10A2_Unorm)
				|| (format == TexturePixelFormat::R11G11B10_UFloat);
		}
		else if constexpr (std::is_same_v<Pixel, uint64>)
		{
			return (format == TexturePixelFormat::R16G16B16A16_Float);
		}
		else
		{
			static_assert(sizeof(Pixel) == 0, "Unsupported render texture readback type");
		}
	}

	template <class RenderTextureType, class Container>
	bool ReadRenderTexturePixels(const RenderTextureType& texture, Container& image, const uint32 mipLevel)
	{
		using Pixel = std::ranges::range_value_t<Container>;
		if (texture.isEmpty() || not IsRenderTextureReadCompatible<Pixel>(texture.format().value())
			|| (texture.mipLevels() <= mipLevel))
		{
			image.clear();
			return false;
		}
		const Size size{ Max(1, (texture.width() >> mipLevel)), Max(1, (texture.height() >> mipLevel)) };
		image.resize(size);
		if (not texture.read(std::as_writable_bytes(std::span{ image }), mipLevel))
		{
			image.clear();
			return false;
		}
		return true;
	}

	[[nodiscard]]
	Array<Byte> GenerateInitialColorBuffer(const Size& size, const ColorF& color, const TextureFormat& format);

	void FillWithColor(void* const pDst, size_t bufferSizeBytes, const ColorF& color, const TextureFormat& format);
	
	void FillWithImage(void* const pDst, const Size& size, const uint32 dstBytesPerRow, std::span<const Byte> src, const uint32 srcBytesPerRow);
}
