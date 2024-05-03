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

# include <Siv3D/ImageFormat/TGAEncoder.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <Siv3D/EngineLog.hpp>
# include "TGAHeader.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView TGAEncoder::name() const noexcept
	{
		return U"TGA"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat TGAEncoder::imageFormat() const noexcept
	{
		return ImageFormat::TGA;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	const Array<String>& TGAEncoder::possibleExtensions() const
	{
		static const Array<String> extensions{ U"tga" };

		return extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool TGAEncoder::save(const Image& image, const FilePathView path) const
	{
		BinaryWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		return encode(image, writer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool TGAEncoder::encode(const Image& image, IWriter& writer) const
	{
		if (not writer.isOpen())
		{
			return false;
		}

		const int32 width			= image.width();
		const int32 height			= image.height();
		const size_t stride_bytes	= image.stride();
		const TGAHeader header		= TGAHeader::Make(width, height);

		std::unique_ptr<uint8[]> line;
		try
		{
			line = std::make_unique<uint8[]>(stride_bytes);
		}
		catch (const std::bad_alloc&)
		{
			return false;
		}

		writer.write(header);

		const Color* pSrc = image[0];

		for (int32 y = 0; y < height; ++y)
		{
			std::memcpy(line.get(), pSrc, stride_bytes);

			size_t index = 0;

			for (int32 x = 0; x < width; ++x)
			{
				std::swap(line[index], line[index + 2]);
				index += 4;
			}

			writer.write(line.get(), stride_bytes);

			pSrc += width;
		}

		return true;
	}

	Blob TGAEncoder::encode(const Image& image) const
	{
		const int32 width			= image.width();
		const int32 height			= image.height();
		const size_t stride_bytes	= image.stride();
		const size_t size_bytes		= (stride_bytes * height);
		const TGAHeader header		= TGAHeader::Make(width, height);

		std::unique_ptr<uint8[]> line;
		try
		{
			line = std::make_unique<uint8[]>(stride_bytes);
		}
		catch (const std::bad_alloc&)
		{
			return{};
		}

		Blob blob(sizeof(TGAHeader) + size_bytes);

		std::memcpy(blob.data(), &header, sizeof(header));

		const Color* pSrc = image[0];
		Byte* pDstLine = (blob.data() + sizeof(header));

		for (int32 y = 0; y < height; ++y)
		{
			std::memcpy(line.get(), pSrc, stride_bytes);

			size_t index = 0;

			for (int32 x = 0; x < width; ++x)
			{
				std::swap(line[index], line[index + 2]);
				index += 4;
			}

			std::memcpy(pDstLine, line.get(), stride_bytes);
			pDstLine += stride_bytes;
			pSrc += width;
		}

		return blob;
	}
}
