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
# include "Common.hpp"
# include "IImageDecoder.hpp"

namespace s3d
{
	namespace ImageDecoder
	{
		[[nodiscard]]
		Optional<ImageInfo> GetImageInfo(FilePathView path, ImageFormat imageFormat = ImageFormat::Unspecified);

		[[nodiscard]]
		Optional<ImageInfo> GetImageInfo(IReader& reader, ImageFormat imageFormat = ImageFormat::Unspecified);

		[[nodiscard]]
		Image Decode(FilePathView path, ImageFormat imageFormat = ImageFormat::Unspecified);

		[[nodiscard]]
		Image Decode(IReader& reader, ImageFormat imageFormat = ImageFormat::Unspecified);

		[[nodiscard]]
		Grid<uint16> DecodeGray16(FilePathView path, ImageFormat imageFormat = ImageFormat::Unspecified);

		[[nodiscard]]
		Grid<uint16> DecodeGray16(IReader& reader, ImageFormat imageFormat = ImageFormat::Unspecified);

		template <class ImageDecoder> requires std::derived_from<ImageDecoder, IImageDecoder>
		bool Add();

		bool Add(std::unique_ptr<IImageDecoder>&& decoder);

		void Remove(StringView decoderName);

		[[nodiscard]]
		bool IsRegistered(StringView decoderName) noexcept;

		const Array<std::unique_ptr<IImageDecoder>>& Enum() noexcept;
	}
}

# include "detail/ImageDecoder.ipp"
