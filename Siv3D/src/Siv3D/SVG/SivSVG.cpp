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

# include <Siv3D/SVG.hpp>
# include <Siv3D/BinaryReader.hpp>
# include "SVGDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	SVG::SVG()
		: pImpl{ std::make_shared<SVGDetail>() } {}

	SVG::SVG(const FilePathView path)
		: SVG{}
	{
		load(path);
	}

	SVG::SVG(std::unique_ptr<IReader>&& reader)
		: SVG{}
	{
		load(std::move(reader));
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	bool SVG::load(const FilePathView path)
	{
		BinaryReader binaryReader{ path };

		if (not binaryReader)
		{
			return false;
		}

		std::string source(static_cast<size_t>(binaryReader.size()), '\0');

		binaryReader.read(source.data(), source.size());

		return parse(source);
	}

	bool SVG::load(std::unique_ptr<IReader>&& reader)
	{
		std::string source(static_cast<size_t>(reader->size()), '\0');

		reader->read(source.data(), source.size());

		return parse(source);
	}

	////////////////////////////////////////////////////////////////
	//
	//	parse
	//
	////////////////////////////////////////////////////////////////

	bool SVG::parse(const std::string& source)
	{
		return pImpl->parse(source);
	}

	bool SVG::parse(const StringView source)
	{
		return parse(source.toUTF8());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool SVG::isEmpty() const noexcept
	{
		return pImpl->isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool()
	//
	////////////////////////////////////////////////////////////////

	SVG::operator bool() const noexcept
	{
		return (not pImpl->isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	width, height
	//
	////////////////////////////////////////////////////////////////

	int32 SVG::width() const
	{
		return static_cast<int32>(std::ceil(pImpl->width()));
	}

	int32 SVG::height() const
	{
		return static_cast<int32>(std::ceil(pImpl->height()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	Size SVG::size() const
	{
		return{ width(), height() };
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	widthF, heightF
	//
	////////////////////////////////////////////////////////////////

	double SVG::widthF() const
	{
		return pImpl->width();
	}

	double SVG::heightF() const
	{
		return pImpl->height();
	}

	////////////////////////////////////////////////////////////////
	//
	//	sizeF
	//
	////////////////////////////////////////////////////////////////

	SizeF SVG::sizeF() const
	{
		return{ pImpl->width(), pImpl->height() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void SVG::clear()
	{
		pImpl->clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	render
	//
	////////////////////////////////////////////////////////////////

	Image SVG::render(const Color& background, const PremultiplyAlpha premultiplyAlpha) const
	{
		return pImpl->render(unspecified, unspecified, background, premultiplyAlpha);
	}

	Image SVG::render(const Optional<int32>& maxWidth, const Optional<int32>& maxHeight, const Color& background, const PremultiplyAlpha premultiplyAlpha) const
	{
		return pImpl->render(maxWidth, maxHeight, background, premultiplyAlpha);
	}

	Image SVG::render(const Optional<Size>& maxSize, const Color& background, const PremultiplyAlpha premultiplyAlpha) const
	{
		if (maxSize)
		{
			return pImpl->render(maxSize->x, maxSize->y, background, premultiplyAlpha);
		}
		else
		{
			return pImpl->render(unspecified, unspecified, background, premultiplyAlpha);
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	SVG SVG::Load(const FilePathView path)
	{
		SVG svg;
		svg.load(path);
		return svg;
	}

	SVG SVG::Load(std::unique_ptr<IReader>&& reader)
	{
		SVG svg;
		svg.load(std::move(reader));
		return svg;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	SVG SVG::Parse(const std::string& source)
	{
		SVG svg;
		svg.parse(source);
		return svg;
	}

	SVG SVG::Parse(const StringView source)
	{
		SVG svg;
		svg.parse(source);
		return svg;
	}
}
