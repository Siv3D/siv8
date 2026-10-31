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

# include <Siv3D/RenderTexture.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static_assert(sizeof(std::pair<uint16, uint16>) == 4);
		static_assert(sizeof(std::pair<HalfFloat, HalfFloat>) == 4);
		static_assert(sizeof(std::array<HalfFloat, 4>) == 8);

		template <class Container>
		bool ReadPixels(const RenderTexture& texture, Container& image, const bool compatible, const uint32 mipLevel)
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
	}

	RenderTexture::RenderTexture(const Size& size, const TextureFormat& format, const TextureDesc desc)
		: RenderTexture{ size, ColorF{ 0.0, 0.0 }, format, desc } {}

	RenderTexture::RenderTexture(const Size& size, const ColorF& color, const TextureFormat& format, const TextureDesc desc)
		: Texture{ Render{}, size, color, format, desc } {}

	RenderTexture::RenderTexture(const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: Texture{ Render{}, size, data, format, desc } {}

	RenderTexture::RenderTexture(const Image& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }),
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm), desc } {}

	RenderTexture::RenderTexture(const Grid<uint8>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8_Unorm, desc } {}

	RenderTexture::RenderTexture(const Grid<uint16>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8_Unorm, desc } {}

	RenderTexture::RenderTexture(const Grid<HalfFloat>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16_Float, desc } {}

	RenderTexture::RenderTexture(const Grid<Color>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm, desc } {}

	RenderTexture::RenderTexture(const Grid<std::pair<uint16, uint16>>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Unorm, desc } {}

	RenderTexture::RenderTexture(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Float, desc } {}

	RenderTexture::RenderTexture(const Grid<float>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32_Float, desc } {}

	RenderTexture::RenderTexture(const Grid<std::array<HalfFloat, 4>>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16B16A16_Float, desc } {}

	RenderTexture::RenderTexture(const Grid<Float2>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32G32_Float, desc } {}

	RenderTexture::RenderTexture(const Grid<Float4>& image, const TextureDesc desc)
		: RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32G32B32A32_Float, desc } {}

	const RenderTexture& RenderTexture::clear(const ColorF& color) const
	{
		if (not isEmpty())
		{
			SIV3D_ENGINE(Texture)->clearRT(id(), color);
		}
		return *this;
	}

	void RenderTexture::generateMips() const
	{
		if (not isEmpty() && (1 < mipLevels()))
		{
			SIV3D_ENGINE(Texture)->generateMips(id());
		}
	}

	bool RenderTexture::read(const std::span<Byte> destination, const uint32 mipLevel) const
	{
		if (isEmpty() || (mipLevels() <= mipLevel))
		{
			return false;
		}
		const Size levelSize{ Max(1, (width() >> mipLevel)), Max(1, (height() >> mipLevel)) };
		const size_t bytesPerRow = (static_cast<size_t>(levelSize.x) * format().pixelSize());
		if (destination.size_bytes() != (bytesPerRow * levelSize.y))
		{
			return false;
		}
		return SIV3D_ENGINE(Texture)->readRT(id(), destination, mipLevel);
	}

	bool RenderTexture::readAsImage(Image& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R8G8B8A8_Unorm)
			|| (format() == TextureFormat::R8G8B8A8_Unorm_SRGB)), mipLevel);
	}

	bool RenderTexture::read(Grid<uint8>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R8_Unorm)), mipLevel);
	}

	bool RenderTexture::read(Grid<uint16>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R8G8_Unorm)), mipLevel);
	}

	bool RenderTexture::read(Grid<HalfFloat>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R16_Float)), mipLevel);
	}

	bool RenderTexture::read(Grid<Color>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R8G8B8A8_Unorm) || (format() == TextureFormat::R8G8B8A8_Unorm_SRGB)), mipLevel);
	}

	bool RenderTexture::read(Grid<std::pair<uint16, uint16>>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R16G16_Unorm)), mipLevel);
	}

	bool RenderTexture::read(Grid<std::pair<HalfFloat, HalfFloat>>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R16G16_Float)), mipLevel);
	}

	bool RenderTexture::read(Grid<float>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R32_Float)), mipLevel);
	}

	bool RenderTexture::read(Grid<std::array<HalfFloat, 4>>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R16G16B16A16_Float)), mipLevel);
	}

	bool RenderTexture::read(Grid<Float2>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R32G32_Float)), mipLevel);
	}

	bool RenderTexture::read(Grid<Float4>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R32G32B32A32_Float)), mipLevel);
	}

	bool RenderTexture::read(Grid<uint32>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R16G16_Unorm) || (format() == TextureFormat::R16G16_Float) || (format() == TextureFormat::R10G10B10A2_Unorm) || (format() == TextureFormat::R11G11B10_UFloat)), mipLevel);
	}

	bool RenderTexture::read(Grid<uint64>& image, const uint32 mipLevel) const
	{
		return ReadPixels(*this, image, ((format() == TextureFormat::R16G16B16A16_Float)), mipLevel);
	}

	RenderTexture RenderTexture::CreateR8_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R8_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR8_Unorm(const Grid<uint8>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR8G8_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R8G8_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR8G8_Unorm(const Grid<uint16>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR16_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR16_Float(const Grid<HalfFloat>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR8G8B8A8_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R8G8B8A8_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR8G8B8A8_Unorm(const Grid<Color>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR8G8B8A8_Unorm_SRGB(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R8G8B8A8_Unorm_SRGB, desc };
	}

	RenderTexture RenderTexture::CreateR8G8B8A8_Unorm_SRGB(const Grid<Color>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm_SRGB, desc };
	}

	RenderTexture RenderTexture::CreateR16G16_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R16G16_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR16G16_Unorm(const Grid<std::pair<uint16, uint16>>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR16G16_Unorm(const Grid<uint32>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR16G16_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R16G16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR16G16_Float(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR16G16_Float(const Grid<uint32>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR32_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R32_Float, desc };
	}

	RenderTexture RenderTexture::CreateR32_Float(const Grid<float>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32_Float, desc };
	}

	RenderTexture RenderTexture::CreateR10G10B10A2_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R10G10B10A2_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR10G10B10A2_Unorm(const Grid<uint32>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R10G10B10A2_Unorm, desc };
	}

	RenderTexture RenderTexture::CreateR11G11B10_UFloat(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R11G11B10_UFloat, desc };
	}

	RenderTexture RenderTexture::CreateR11G11B10_UFloat(const Grid<uint32>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R11G11B10_UFloat, desc };
	}

	RenderTexture RenderTexture::CreateR16G16B16A16_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R16G16B16A16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR16G16B16A16_Float(const Grid<std::array<HalfFloat, 4>>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16B16A16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR16G16B16A16_Float(const Grid<uint64>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16B16A16_Float, desc };
	}

	RenderTexture RenderTexture::CreateR32G32_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R32G32_Float, desc };
	}

	RenderTexture RenderTexture::CreateR32G32_Float(const Grid<Float2>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32G32_Float, desc };
	}

	RenderTexture RenderTexture::CreateR32G32B32A32_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		return RenderTexture{ size, color, TextureFormat::R32G32B32A32_Float, desc };
	}

	RenderTexture RenderTexture::CreateR32G32B32A32_Float(const Grid<Float4>& image, const TextureDesc desc)
	{
		return RenderTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32G32B32A32_Float, desc };
	}
}
