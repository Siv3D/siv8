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

namespace s3d
{
	[[nodiscard]]
	bool IsValidRenderTexture(const Size& size, std::span<const Byte> data, const TextureFormat& format) noexcept;

	[[nodiscard]]
	bool ReadRenderTexture(const Texture& texture, std::span<Byte> destination, uint32 mipLevel);

	template <class RenderTextureType, class Container>
	bool ReadRenderTexturePixels(const RenderTextureType& texture, Container& image, const bool compatible, const uint32 mipLevel)
	{
		if (texture.isEmpty() || not compatible || (texture.mipLevels() <= mipLevel))
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
