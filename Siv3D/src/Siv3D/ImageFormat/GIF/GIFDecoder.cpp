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

# include <Siv3D/ImageFormat/GIFDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include "../../../ThirdParty/wuffs/WuffsCodecs.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsGIFSignature(const uint8* const bytes) noexcept
		{
			return ((std::memcmp(bytes, "GIF87a", 6) == 0)
				|| (std::memcmp(bytes, "GIF89a", 6) == 0));
		}
	}

	StringView GIFDecoder::name() const noexcept
	{
		return U"GIF"_sv;
	}

	ImageFormat GIFDecoder::imageFormat() const noexcept
	{
		return ImageFormat::GIF;
	}

	bool GIFDecoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		return IsGIFSignature(bytes);
	}

	std::span<const StringView> GIFDecoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"gif" };
		return Extensions;
	}

	Optional<ImageInfo> GIFDecoder::getImageInfo(const FilePathView path) const
	{
		return IImageDecoder::getImageInfo(path);
	}

	Optional<ImageInfo> GIFDecoder::getImageInfo(const IReader& reader, const FilePathView) const
	{
		uint8 header[10];

		if ((reader.lookahead(header, sizeof(header)) != sizeof(header))
			|| (not IsGIFSignature(header)))
		{
			return{};
		}

		const uint32 width = (static_cast<uint32>(header[6])
			| (static_cast<uint32>(header[7]) << 8));
		const uint32 height = (static_cast<uint32>(header[8])
			| (static_cast<uint32>(header[9]) << 8));

		if ((width == 0) || (height == 0)
			|| (Image::MaxWidth < width) || (Image::MaxHeight < height))
		{
			return{};
		}

		return ImageInfo{
			Size{ static_cast<int32>(width), static_cast<int32>(height) },
			ImageFormat::GIF,
			ImagePixelFormat::R8G8B8A8,
			false
		};
	}

	Image GIFDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		return IImageDecoder::decode(path, premultiplyAlpha);
	}

	Image GIFDecoder::decode(std::unique_ptr<IReader> reader, const FilePathView, const PremultiplyAlpha premultiplyAlpha) const
	{
		return detail::DecodeFirstGIFFrame(std::move(reader), premultiplyAlpha);
	}
}
