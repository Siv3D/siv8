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

# include "SVGDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	parse
	//
	////////////////////////////////////////////////////////////////

	bool SVG::SVGDetail::parse(const std::string& source)
	{
		clear();

		m_document = lunasvg::Document::loadFromData(source);

		return static_cast<bool>(m_document);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool SVG::SVGDetail::isEmpty() const noexcept
	{
		return (not m_document);
	}

	////////////////////////////////////////////////////////////////
	//
	//	width
	//
	////////////////////////////////////////////////////////////////

	double SVG::SVGDetail::width() const noexcept
	{
		if (not m_document)
		{
			return 0.0;
		}

		return m_document->width();
	}

	////////////////////////////////////////////////////////////////
	//
	//	height
	//
	////////////////////////////////////////////////////////////////

	double SVG::SVGDetail::height() const noexcept
	{
		if (not m_document)
		{
			return 0.0;
		}

		return m_document->height();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void SVG::SVGDetail::clear()
	{
		m_document.reset();
	}

	////////////////////////////////////////////////////////////////
	//
	//	render
	//
	////////////////////////////////////////////////////////////////

	Image SVG::SVGDetail::render(const Optional<int32>& maxWidth, const Optional<int32>& maxHeight, const Color& background, const PremultiplyAlpha premultiplyAlpha) const
	{
		if (not m_document)
		{
			return{};
		}

		const int32 maxImageWidth = maxWidth.value_or(static_cast<int32>(std::ceil(m_document->width())));
		const int32 maxImageHeight = maxHeight.value_or(static_cast<int32>(std::ceil(m_document->height())));
		const uint32 color = background.abgr().asUint32();

		lunasvg::Bitmap bitmap = m_document->renderToBitmap(maxImageWidth, maxImageHeight, color);
		bitmap.convert(0, 1, 2, 3, (not premultiplyAlpha.getBool()));

		Image image{ bitmap.width(), bitmap.height() };
		assert(image.size_bytes() == (bitmap.stride() * bitmap.height()));
		
		std::memcpy(image.data(), bitmap.data(), image.size_bytes());

		return image;
	}
}
