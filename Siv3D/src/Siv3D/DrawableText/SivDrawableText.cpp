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

# include <Siv3D/DrawableText.hpp>

namespace s3d
{
	DrawableText::DrawableText(const Font& _font, const String& _text)
		: DrawableText{ _font, String{ _text } } {}

	DrawableText::DrawableText(const Font& _font, String&& _text)
		: font{ _font }
		, text{ std::move(_text) }
		, resolvedGlyphs{ font.getResolvedGlyphs(text) } {}
	
	Array<double> DrawableText::getXAdvances() const
	{
		return getXAdvances(font.baseSize());
	}

	Array<double> DrawableText::getXAdvances(const double textSize) const
	{
		return{};
		//return SIV3D_ENGINE(Font)->getXAdvances(font.id(), text, clusters, size);
	}
}
