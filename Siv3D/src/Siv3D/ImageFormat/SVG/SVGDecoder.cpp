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

# include <Siv3D/ImageFormat/SVGDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/SVG.hpp>
# include <ThirdParty/lunasvg/lunasvg.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView SVGDecoder::name() const noexcept
	{
		return U"SVG"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat SVGDecoder::imageFormat() const noexcept
	{
		return ImageFormat::SVG;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool SVGDecoder::isHeader(const uint8(&bytes)[16]) const noexcept
	{
		static constexpr uint8 signature1[] = { '<', 's', 'v', 'g', ' ', 'x', 'm', 'l', 'n', 's' };
		static constexpr uint8 signature2[] = { '<', 's', 'v', 'g', ':', 's', 'v', 'g', ' ', 'x', 'm', 'l', 'n', 's' };
		static constexpr uint8 signature3[] = { '<', '?', 'x', 'm', 'l', ' ', 'v', 'e', 'r', 's', 'i', 'o', 'n' };

		return (std::memcmp(bytes, signature1, sizeof(signature1)) == 0)
			|| (std::memcmp(bytes, signature2, sizeof(signature2)) == 0)
			|| (std::memcmp(bytes, signature3, sizeof(signature3)) == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	const Array<String>& SVGDecoder::possibleExtensions() const
	{
		static const Array<String> extensions{ U"svg" };

		return extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImageInfo
	//
	////////////////////////////////////////////////////////////////

	Optional<ImageInfo> SVGDecoder::getImageInfo(const FilePathView path) const
	{
		return IImageDecoder::getImageInfo(path);
	}

	Optional<ImageInfo> SVGDecoder::getImageInfo(IReader& reader, const FilePathView) const
	{
		std::string source(reader.size(), '\0');

		if (reader.read(source.data(), source.size())
			!= static_cast<int64>(source.size()))
		{
			LOG_FAIL("❌ SVGDecoder::getImageInfo(): Failed to read the source");
			return none;
		}

		std::unique_ptr<lunasvg::Document> document = lunasvg::Document::loadFromData(source);

		if (not document)
		{
			LOG_FAIL("❌ SVGDecoder::getImageInfo(): SVGDocument::loadFromData() failed");
			return none;
		}

		const int32 width = static_cast<int32>(std::ceil(document->width()));
		const int32 height = static_cast<int32>(std::ceil(document->height()));

		return ImageInfo{ Size{ width, height }, ImageFormat::SVG, ImagePixelFormat::R8G8B8A8, false };
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Image SVGDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		return IImageDecoder::decode(path, premultiplyAlpha);
	}

	Image SVGDecoder::decode(IReader& reader, const FilePathView, const PremultiplyAlpha premultiplyAlpha) const
	{
		LOG_SCOPED_DEBUG("SVGDecoder::decode()");

		std::string source(reader.size(), '\0');

		if (reader.read(source.data(), source.size())
			!= static_cast<int64>(source.size()))
		{
			LOG_FAIL("❌ SVGDecoder::decode(): Failed to read the source");
			return{};
		}

		const SVG svg = SVG::Parse(source);
		
		if (not svg)
		{
			LOG_FAIL("❌ SVGDecoder::decode(): Failed to parse the SVG document");
			return{};
		}

		Image image = svg.render(unspecified, Color{ 0, 0 }, premultiplyAlpha);

		if (not image)
		{
			LOG_FAIL("❌ SVGDecoder::decode(): Failed to render the SVG document");
			return{};
		}

		LOG_TRACE(fmt::format("Image ({}x{}) decoded", image.width(), image.height()));

		return image;
	}
}
