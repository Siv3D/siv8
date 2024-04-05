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
# include "String.hpp"
# include "Array.hpp"
# include "ImageFormat.hpp"
# include "Image.hpp"
# include "Blob.hpp"

namespace s3d
{
	struct IImageEncoder
	{
		virtual ~IImageEncoder() = default;

		[[nodiscard]]
		virtual StringView name() const noexcept = 0;

		[[nodiscard]]
		virtual ImageFormat imageFormat() const noexcept = 0;

		[[nodiscard]]
		virtual const Array<String>& possibleExtensions() const noexcept = 0;

		virtual bool save(const Image& image, FilePathView path) const = 0;

		virtual bool encode(const Image& image, IWriter& writer) const = 0;

		[[nodiscard]]
		virtual Blob encode(const Image& image) const = 0;
	};
}
