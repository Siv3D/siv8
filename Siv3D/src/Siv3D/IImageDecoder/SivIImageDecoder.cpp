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

# include <Siv3D/IImageDecoder.hpp>
# include <Siv3D/BinaryFileReader.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	getImageInfo
	//
	////////////////////////////////////////////////////////////////

	Optional<ImageInfo> IImageDecoder::getImageInfo(const FilePathView path) const
	{
		BinaryFileReader reader{ path };

		if (not reader)
		{
			return{};
		}

		return getImageInfo(reader, path);
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Image IImageDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

		if (not reader->isOpen())
		{
			return{};
		}

		return decode(std::move(reader), path, premultiplyAlpha);
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeGray16
	//
	////////////////////////////////////////////////////////////////

	Grid<uint16> IImageDecoder::decodeGray16(const FilePathView path) const
	{
		std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

		if (not reader)
		{
			return{};
		}

		return decodeGray16(std::move(reader), path);
	}

	Grid<uint16> IImageDecoder::decodeGray16(std::unique_ptr<IReader>, FilePathView) const
	{
		return{};
	}
}
