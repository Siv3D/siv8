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

# include <Siv3D/DynamicTexture.hpp>
# include <Siv3D/Anchor.hpp>
# include <Siv3D/Rect.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Texture/TextureUtility.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static_assert(sizeof(std::pair<uint16, uint16>) == 4);
		static_assert(sizeof(std::pair<HalfFloat, HalfFloat>) == 4);
		static_assert(sizeof(std::array<HalfFloat, 4>) == sizeof(uint64));

		template <class Container>
		bool FillTexture(DynamicTexture& texture, const Container& image, const bool wait)
		{
			const auto id = texture.id();
			if (id.isNull())
			{
				texture = DynamicTexture{ image };
				return (not texture.isEmpty());
			}

			using Pixel = std::ranges::range_value_t<Container>;
			auto* const textures = SIV3D_ENGINE(Texture);
			if ((image.size() != textures->getSize(id))
				|| not IsTexturePixelCompatible<Pixel>(textures->getFormat(id).value()))
			{
				return false;
			}
			const uint32 rowPitch = static_cast<uint32>(image.width() * sizeof(Pixel));
			return textures->fill(id, std::as_bytes(std::span{ image }), rowPitch, wait);
		}

		template <class Container>
		bool FillTextureRegion(DynamicTexture& texture, const Container& image, const Rect& rect, const bool wait)
		{
			const auto id = texture.id();
			if (id.isNull() || (image.size() != rect.size))
			{
				return false;
			}

			using Pixel = std::ranges::range_value_t<Container>;
			auto* const textures = SIV3D_ENGINE(Texture);
			if (not IsTexturePixelCompatible<Pixel>(textures->getFormat(id).value()))
			{
				return false;
			}
			const uint32 rowPitch = static_cast<uint32>(image.width() * sizeof(Pixel));
			return textures->fillRegion(id, std::as_bytes(std::span{ image }), rowPitch, rect, wait);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture::DynamicTexture(const Size& size, const TextureFormat& format, const TextureDesc desc)
		: Texture{ Texture::Dynamic{}, size, std::span<const Byte>{}, format, desc } {}

	DynamicTexture::DynamicTexture(const Size& size, const ColorF& color, const TextureFormat& format, const TextureDesc desc)
		: Texture{ Texture::Dynamic{}, size, color, format, desc } {}

	DynamicTexture::DynamicTexture(const Image& image, const TextureDesc desc)
		: Texture{ Texture::Dynamic{}, image.size(), std::as_bytes(std::span{ image }), (desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm), desc } {}

	DynamicTexture::DynamicTexture(const Grid<uint8>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR8_Unorm(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<uint16>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR8G8_Unorm(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<HalfFloat>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR16_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<Color>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR8G8B8A8_Unorm(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<std::pair<uint16, uint16>>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR16G16_Unorm(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR16G16_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<float>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR32_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<std::array<HalfFloat, 4>>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR16G16B16A16_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<Float2>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR32G32_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<Float4>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR32G32B32A32_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc)
		: Texture{ Texture::Dynamic{}, size, data, format, desc } {}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	bool DynamicTexture::fill(const ColorF& color)
	{
		if (isEmpty())
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), color, true);
	}

	bool DynamicTexture::fill(const std::span<const Byte> data, const uint32 sourceRowPitch)
	{
		if (isEmpty())
		{
			return false;
		}
		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), data, sourceRowPitch, true);
	}

	bool DynamicTexture::fill(const Image& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<uint8>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<uint16>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<HalfFloat>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<std::pair<uint16, uint16>>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<std::pair<HalfFloat, HalfFloat>>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<Color>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<float>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<std::array<HalfFloat, 4>>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<Float2>& image)
	{
		return FillTexture(*this, image, true);
	}

	bool DynamicTexture::fill(const Grid<Float4>& image)
	{
		return FillTexture(*this, image, true);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fillRegion
	//
	////////////////////////////////////////////////////////////////

	bool DynamicTexture::fillRegion(const ColorF& color, const Rect& rect)
	{
		if (isEmpty())
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fillRegion(m_handle->id(), color, rect, true);
	}

	bool DynamicTexture::fillRegion(const std::span<const Byte> data, const uint32 sourceRowPitch, const Rect& rect)
	{
		if (isEmpty())
		{
			return false;
		}
		return SIV3D_ENGINE(Texture)->fillRegion(m_handle->id(), data, sourceRowPitch, rect, true);
	}

	bool DynamicTexture::fillRegion(const Image& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<uint8>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<uint16>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<HalfFloat>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<std::pair<uint16, uint16>>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<Color>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<float>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<std::array<HalfFloat, 4>>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<Float2>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	bool DynamicTexture::fillRegion(const Grid<Float4>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, true);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fillIfNotBusy
	//
	////////////////////////////////////////////////////////////////

	bool DynamicTexture::fillIfNotBusy(const ColorF& color)
	{
		if (isEmpty())
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), color, false);
	}

	bool DynamicTexture::fillIfNotBusy(const std::span<const Byte> data, const uint32 sourceRowPitch)
	{
		if (isEmpty())
		{
			return false;
		}
		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), data, sourceRowPitch, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Image& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<uint8>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<uint16>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<HalfFloat>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<std::pair<uint16, uint16>>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<std::pair<HalfFloat, HalfFloat>>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<Color>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<float>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<std::array<HalfFloat, 4>>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<Float2>& image)
	{
		return FillTexture(*this, image, false);
	}

	bool DynamicTexture::fillIfNotBusy(const Grid<Float4>& image)
	{
		return FillTexture(*this, image, false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fillRegionIfNotBusy
	//
	////////////////////////////////////////////////////////////////

	bool DynamicTexture::fillRegionIfNotBusy(const ColorF& color, const Rect& rect)
	{
		if (isEmpty())
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fillRegion(m_handle->id(), color, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const std::span<const Byte> data, const uint32 sourceRowPitch, const Rect& rect)
	{
		if (isEmpty())
		{
			return false;
		}
		return SIV3D_ENGINE(Texture)->fillRegion(m_handle->id(), data, sourceRowPitch, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Image& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<uint8>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<uint16>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<HalfFloat>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<std::pair<uint16, uint16>>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<Color>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<float>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<std::array<HalfFloat, 4>>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<Float2>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	bool DynamicTexture::fillRegionIfNotBusy(const Grid<Float4>& image, const Rect& rect)
	{
		return FillTextureRegion(*this, image, rect, false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateMips
	//
	////////////////////////////////////////////////////////////////

	void DynamicTexture::generateMips() const
	{
		SIV3D_ENGINE(Texture)->generateMips(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void DynamicTexture::swap(DynamicTexture& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}

	DynamicTexture DynamicTexture::CreateWithRowPadding(const Size& size, const std::span<const Byte> data,
		const TextureFormat& format, const TextureDesc desc)
	{
		const uint32 rowBytes = static_cast<uint32>(format.packedRowBytes(size.x));
		const uint32 rowPitch = format.bytesPerRow(size.x);
		if (rowPitch == rowBytes)
		{
			return DynamicTexture{ size, data, format, desc };
		}

		Array<Byte> padded(static_cast<size_t>(rowPitch) * size.y);
		FillWithImage(padded.data(), size, rowPitch, data, rowBytes, rowBytes);
		return DynamicTexture{ size, padded, format, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR8_Unorm
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR8_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const size_t width = ((size.x + 3) & ~3);
		const size_t height = size.y;
		const Grid<uint8> padded(width, height, color.toR8_Unorm());

		return DynamicTexture{ size, std::as_bytes(std::span{ padded }), TextureFormat::R8_Unorm, desc };
	}

	DynamicTexture DynamicTexture::CreateR8_Unorm(const Grid<uint8>& image, const TextureDesc desc)
	{
		return CreateWithRowPadding(image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8_Unorm, desc);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR8G8_Unorm
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR8G8_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const size_t width = ((size.x + 1) & ~1);
		const size_t height = size.y;
		const Grid<uint16> padded(width, height, color.toR8G8_Unorm());

		return DynamicTexture{ size, std::as_bytes(std::span{ padded }), TextureFormat::R8G8_Unorm, desc };
	}

	DynamicTexture DynamicTexture::CreateR8G8_Unorm(const Grid<uint16>& image, const TextureDesc desc)
	{
		return CreateWithRowPadding(image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8_Unorm, desc);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR16_Float
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR16_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const size_t width = ((size.x + 1) & ~1);
		const size_t height = size.y;
		const Grid<HalfFloat> padded(width, height, color.toR16_Float());

		return DynamicTexture{ size, std::as_bytes(std::span{ padded }), TextureFormat::R16_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR16_Float(const Grid<HalfFloat>& image, const TextureDesc desc)
	{
		return CreateWithRowPadding(image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16_Float, desc);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR8G8B8A8_Unorm
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR8G8B8A8_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<Color> image(size, color.toR8G8B8A8_Unorm());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm, desc };
	}

	DynamicTexture DynamicTexture::CreateR8G8B8A8_Unorm(const Grid<Color>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR8G8B8A8_Unorm_SRGB
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR8G8B8A8_Unorm_SRGB(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<Color> image(size, color.toR8G8B8A8_Unorm());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm_SRGB, desc };
	}

	DynamicTexture DynamicTexture::CreateR8G8B8A8_Unorm_SRGB(const Grid<Color>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8B8A8_Unorm_SRGB, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR16G16_Unorm
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR16G16_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<uint32> image(size, color.toR16G16_Unorm());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R16G16_Unorm, desc };
	}

	DynamicTexture DynamicTexture::CreateR16G16_Unorm(const Grid<std::pair<uint16, uint16>>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Unorm, desc };
	}

	DynamicTexture DynamicTexture::CreateR16G16_Unorm(const Grid<uint32>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Unorm, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR16G16_Float
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR16G16_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<uint32> image(size, color.toR16G16_Float());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R16G16_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR16G16_Float(const Grid<std::pair<HalfFloat, HalfFloat>>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR16G16_Float(const Grid<uint32>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16_Float, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR32_Float
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR32_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<float> image(size, color.toR32_Float());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R32_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR32_Float(const Grid<float>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32_Float, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR10G10B10A2_Unorm
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR10G10B10A2_Unorm(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<uint32> image(size, color.toR10G10B10A2_Unorm());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R10G10B10A2_Unorm, desc };
	}

	DynamicTexture DynamicTexture::CreateR10G10B10A2_Unorm(const Grid<uint32>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R10G10B10A2_Unorm, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR11G11B10_UFloat
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR11G11B10_UFloat(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<uint32> image(size, color.toR11G11B10_UFloat());
		return DynamicTexture{ size,std::as_bytes(std::span{ image }), TextureFormat::R11G11B10_UFloat, desc };
	}

	DynamicTexture DynamicTexture::CreateR11G11B10_UFloat(const Grid<uint32>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R11G11B10_UFloat, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR16G16B16A16_Float
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR16G16B16A16_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<uint64> image(size, color.toR16G16B16A16_Float());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R16G16B16A16_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR16G16B16A16_Float(const Grid<std::array<HalfFloat, 4>>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16B16A16_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR16G16B16A16_Float(const Grid<uint64>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16G16B16A16_Float, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR32G32_Float
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR32G32_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<Float2> image(size, color.toR32G32_Float());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R32G32_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR32G32_Float(const Grid<Float2>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32G32_Float, desc };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR32G32B32A32_Float
	//
	////////////////////////////////////////////////////////////////

	DynamicTexture DynamicTexture::CreateR32G32B32A32_Float(const Size& size, const ColorF& color, const TextureDesc desc)
	{
		const Grid<Float4> image(size, color.toR32G32B32A32_Float());
		return DynamicTexture{ size, std::as_bytes(std::span{ image }), TextureFormat::R32G32B32A32_Float, desc };
	}

	DynamicTexture DynamicTexture::CreateR32G32B32A32_Float(const Grid<Float4>& image, const TextureDesc desc)
	{
		return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R32G32B32A32_Float, desc };
	}
}
