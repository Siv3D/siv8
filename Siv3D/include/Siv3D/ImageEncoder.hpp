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
# include "IImageEncoder.hpp"

namespace s3d
{
	namespace ImageEncoder
	{
		bool Save(const Image& image, ImageFormat format, FilePathView path);

		bool Save(const Image& image, StringView encoderName, FilePathView path);

		bool Encode(const Image& image, ImageFormat format, IWriter& writer);

		bool Encode(const Image& image, StringView encoderName, IWriter& writer);

		Blob Encode(const Image& image, ImageFormat format);

		Blob Encode(const Image& image, StringView encoderName);

		template <class ImageEncoder> requires std::derived_from<ImageEncoder, IImageEncoder>
		bool Add();

		bool Add(std::unique_ptr<IImageEncoder>&& encoder);

		void Remove(StringView encoderName);

		[[nodiscard]]
		bool IsRegistered(StringView encoderName);

		const Array<std::unique_ptr<IImageEncoder>>& Enum();
	}
}

# include "detail/ImageEncoder.ipp"
