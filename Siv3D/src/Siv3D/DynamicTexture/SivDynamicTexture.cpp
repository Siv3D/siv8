//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/DynamicTexture.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
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

	DynamicTexture::DynamicTexture(const Grid<HalfFloat>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR16_Float(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<Color>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR8G8B8A8_Unorm(image, desc) } {}

	DynamicTexture::DynamicTexture(const Grid<float>& image, const TextureDesc desc)
		: DynamicTexture{ CreateR32_Float(image, desc) } {}

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

	bool DynamicTexture::fill(const Image& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}
		
		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID); 
			(format != TextureFormat::R8G8B8A8_Unorm) && (format != TextureFormat::R8G8B8A8_Unorm_SRGB))
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(handleID, std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	}

	bool DynamicTexture::fill(const Grid<uint8>& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}

		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
			format != TextureFormat::R8_Unorm)
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(handleID, std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	}

	bool DynamicTexture::fill(const Grid<HalfFloat>& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}

		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
			format != TextureFormat::R16_Float)
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(handleID, std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	}

	//bool DynamicTexture::fill(const Grid<uint16>& image)
	//{
	//	if (isEmpty())
	//	{
	//		*this = CreateR8G8_Unorm(image);
	//		return true;
	//	}

	//	if (image.size() != size())
	//	{
	//		return false;
	//	}

	//	const auto handleID = m_handle->id();

	//	if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
	//		format != TextureFormat::R8G8_Unorm)
	//	{
	//		return false;
	//	}

	//	return SIV3D_ENGINE(Texture)->fill(handleID, std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	//}

	bool DynamicTexture::fill(const Grid<Color>& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}

		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
			(format != TextureFormat::R8G8B8A8_Unorm) && (format != TextureFormat::R8G8B8A8_Unorm_SRGB))
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	}

	bool DynamicTexture::fill(const Grid<float>& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}

		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
			format != TextureFormat::R32_Float)
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	}

	//bool DynamicTexture::fill(const Grid<uint32>& image)
	//{
	//	if (isEmpty())
	//	{
	//		*this = DynamicTexture{ image };
	//		return true;
	//	}
	//	else if (image.size() != size())
	//	{
	//		return false;
	//	}

	//	return SIV3D_ENGINE(Texture)->fill(m_handle->id(), std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	//}

	//bool DynamicTexture::fill(const Grid<uint64>& image)
	//{
	//	if (isEmpty())
	//	{
	//		*this = DynamicTexture{ image };
	//		return true;
	//	}
	//	else if (image.size() != size())
	//	{
	//		return false;
	//	}

	//	return SIV3D_ENGINE(Texture)->fill(m_handle->id(), std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	//}

	bool DynamicTexture::fill(const Grid<Float2>& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}

		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
			format != TextureFormat::R32G32_Float)
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
	}

	bool DynamicTexture::fill(const Grid<Float4>& image)
	{
		if (isEmpty())
		{
			*this = DynamicTexture{ image };
			return true;
		}

		if (image.size() != size())
		{
			return false;
		}

		const auto handleID = m_handle->id();

		if (const TextureFormat format = SIV3D_ENGINE(Texture)->getFormat(handleID);
			format != TextureFormat::R32G32B32A32_Float)
		{
			return false;
		}

		return SIV3D_ENGINE(Texture)->fill(m_handle->id(), std::as_bytes(std::span{ image }), image.bytesPerRow(), true);
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
		if ((image.width()) % 4 == 0)
		{
			return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8_Unorm, desc };
		}
		else
		{
			const size_t width = ((image.width() + 3) & ~3);
			const size_t height = image.height();
			Grid<uint8> padded(width, height);

			for (size_t y = 0; y < height; ++y)
			{
				std::memcpy(padded[y], image[y], image.width());
			}

			return DynamicTexture{ image.size(), std::as_bytes(std::span{ padded }), TextureFormat::R8_Unorm, desc };
		}
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
		if ((image.width()) % 2 == 0)
		{
			return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R8G8_Unorm, desc };
		}
		else
		{
			const size_t width = ((image.width() + 1) & ~1);
			const size_t height = image.height();
			Grid<uint16> padded(width, height);

			for (size_t y = 0; y < height; ++y)
			{
				std::memcpy(padded[y], image[y], image.width() * sizeof(uint16));
			}

			return DynamicTexture{ image.size(), std::as_bytes(std::span{ padded }), TextureFormat::R8G8_Unorm, desc };
		}
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
		if ((image.width()) % 2 == 0)
		{
			return DynamicTexture{ image.size(), std::as_bytes(std::span{ image }), TextureFormat::R16_Float, desc };
		}
		else
		{
			const size_t width = ((image.width() + 1) & ~1);
			const size_t height = image.height();
			Grid<HalfFloat> padded(width, height);

			for (size_t y = 0; y < height; ++y)
			{
				std::memcpy(padded[y], image[y], image.width() * sizeof(HalfFloat));
			}

			return DynamicTexture{ image.size(), std::as_bytes(std::span{ padded }), TextureFormat::R16_Float, desc };
		}
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
