//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "IImageDecoder.hpp"

namespace s3d
{
	class CImageDecoder final : public ISiv3DImageDecoder
	{
	public:
		
		CImageDecoder() = default;

		~CImageDecoder() override;

		void init() override;

		bool add(std::unique_ptr<IImageDecoder>&& decoder) override;

		void remove(StringView decoderName) override;

		const Array<std::unique_ptr<IImageDecoder>>& enumDecoder() const noexcept override;

		Optional<ImageInfo> getImageInfo(IReader& reader, FilePathView pathHint, ImageFormat imageFormat) override;

		Image decode(IReader& reader, FilePathView pathHint, PremultiplyAlpha premultiplyAlpha, ImageFormat imageFormat) override;

		Grid<uint16> decodeGray16(IReader& reader, FilePathView pathHint, ImageFormat imageFormat) override;

	private:

		Array<std::unique_ptr<IImageDecoder>> m_decoders;
	};
}
