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

# include <Siv3D/MSRenderTexture.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Texture/TextureUtility.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	MSRenderTexture::MSRenderTexture(const Size& size, const TextureFormat& format, const TextureDesc desc, const MSAASampleCount sampleCount)
		: MSRenderTexture{ size, ColorF{ 0.0, 0.0 }, format, desc, sampleCount } {}

	MSRenderTexture::MSRenderTexture(const Size& size, const ColorF& color, const TextureFormat& format, const TextureDesc desc, const MSAASampleCount sampleCount)
		: Texture{ MSRender{}, size, color, format, desc, FromEnum(sampleCount) } {}

	MSRenderTexture::MSRenderTexture(const Size& size, const MSAASampleCount sampleCount, const TextureFormat& format, const TextureDesc desc)
		: MSRenderTexture{ size, format, desc, sampleCount } {}

	MSRenderTexture::MSRenderTexture(const Size& size, const ColorF& color, const MSAASampleCount sampleCount, const TextureFormat& format, const TextureDesc desc)
		: MSRenderTexture{ size, color, format, desc, sampleCount } {}

	MSAASampleCount MSRenderTexture::getMSAASampleCount() const
	{
		return isEmpty() ? MSAASampleCount::NoMSAA
			: static_cast<MSAASampleCount>(SIV3D_ENGINE(Texture)->getSampleCount(id()));
	}

	void MSRenderTexture::resolve() const
	{
		if (not isEmpty())
		{
			SIV3D_ENGINE(Texture)->resolveMSRT(id());
		}
	}

	const MSRenderTexture& MSRenderTexture::clear(const ColorF& color) const
	{
		if (not isEmpty())
		{
			SIV3D_ENGINE(Texture)->clearRT(id(), color);
		}
		return *this;
	}

	void MSRenderTexture::generateMips() const
	{
		if (not isEmpty() && (1 < mipLevels()))
		{
			SIV3D_ENGINE(Texture)->generateMips(id());
		}
	}

	bool MSRenderTexture::read(const std::span<Byte> destination, const uint32 mipLevel) const
	{
		return ReadRenderTexture(*this, destination, mipLevel);
	}

	bool MSRenderTexture::readAsImage(Image& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<uint8>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<uint16>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<HalfFloat>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<Color>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<std::pair<uint16, uint16>>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<std::pair<HalfFloat, HalfFloat>>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<float>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<std::array<HalfFloat, 4>>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<Float2>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<Float4>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<uint32>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

	bool MSRenderTexture::read(Grid<uint64>& image, const uint32 mipLevel) const
	{
		return ReadRenderTexturePixels(*this, image, mipLevel);
	}

}
