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

# include <Siv3D/IImageDecoder.hpp>
# include <Siv3D/BinaryReader.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	getImageInfo
	//
	////////////////////////////////////////////////////////////////

	Optional<ImageInfo> IImageDecoder::getImageInfo(const FilePathView path) const
	{
		BinaryReader reader{ path };

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
		BinaryReader reader{ path };

		if (not reader)
		{
			return{};
		}

		return decode(reader, path, premultiplyAlpha);
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeGray16
	//
	////////////////////////////////////////////////////////////////

	Grid<uint16> IImageDecoder::decodeGray16(const FilePathView path) const
	{
		BinaryReader reader{ path };

		if (not reader)
		{
			return{};
		}

		return decodeGray16(reader, path);
	}

	Grid<uint16> IImageDecoder::decodeGray16(IReader&, FilePathView) const
	{
		return{};
	}
}
