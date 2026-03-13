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
# include <Siv3D/IImageDecoder.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DImageDecoder
	{
	public:

		[[nodiscard]]
		static ISiv3DImageDecoder* Create();

		virtual ~ISiv3DImageDecoder() = default;

		virtual void init() = 0;

		virtual bool add(std::unique_ptr<IImageDecoder> decoder) = 0;

		virtual void remove(StringView decoderName) = 0;

		virtual const Array<std::unique_ptr<IImageDecoder>>& enumDecoder() const noexcept = 0;

		virtual Optional<ImageInfo> getImageInfo(const IReader& reader, FilePathView pathHint, StringView decoderName) = 0;

		virtual Image decode(std::unique_ptr<IReader> reader, FilePathView pathHint, PremultiplyAlpha premultiplyAlpha, StringView decoderName) = 0;

		virtual Grid<uint16> decodeGray16(std::unique_ptr<IReader> reader, FilePathView pathHint, StringView decoderName) = 0;
	};
}
