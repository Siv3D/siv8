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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Texture.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DTexture
	{
	public:

		[[nodiscard]]
		static ISiv3DTexture* Create();

		virtual ~ISiv3DTexture() = default;

		virtual void init() = 0;

		[[nodiscard]]
		virtual Texture::IDType create(IReader&& reader, FilePathView pathHint, TextureDesc desc) = 0;

		[[nodiscard]]
		virtual Texture::IDType create(const Image& image, const Array<Image>& mipmaps, TextureDesc desc) = 0;

		[[nodiscard]]
		virtual Texture::IDType create(const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc) = 0;

		[[nodiscard]]
		virtual Texture::IDType create(const BCnData& bcnData) = 0;

		[[nodiscard]]
		virtual Texture::IDType createDynamic(const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc) = 0;

		virtual void release(Texture::IDType handleID) = 0;

		[[nodiscard]]
		virtual Size getSize(Texture::IDType handleID) = 0;

		[[nodiscard]]
		virtual uint32 getMipLevels(Texture::IDType handleID) = 0;

		[[nodiscard]]
		virtual TextureDesc getDesc(Texture::IDType handleID) = 0;

		[[nodiscard]]
		virtual TextureFormat getFormat(Texture::IDType handleID) = 0;

		[[nodiscard]]
		virtual bool hasDepth(Texture::IDType handleID) = 0;

		virtual bool fill(Texture::IDType handleID, const ColorF& color, bool wait) = 0;

		virtual bool fill(Texture::IDType handleID, std::span<const Byte> src, uint32 srcBytesPerRow, bool wait) = 0;

		virtual bool fillRegion(Texture::IDType handleID, const ColorF& color, const Rect& rect) = 0;

		virtual bool fillRegion(Texture::IDType handleID, std::span<const Byte> src, uint32 srcBytesPerRow, const Rect& rect, bool wait) = 0;

		virtual void generateMips(Texture::IDType handleID) = 0;
	};
}
