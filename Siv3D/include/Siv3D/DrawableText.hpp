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

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Font.hpp"
# include "Format.hpp"

namespace s3d
{
	struct DrawableText
	{
		Font font;

		String text;

		Array<ResolvedGlyph> resolvedGlyphs;

		[[nodiscard]]
		DrawableText() = default;

		[[nodiscard]]
		DrawableText(const Font& _font, const String& _text);

		[[nodiscard]]
		DrawableText(const Font& _font, String&& _text);

		[[nodiscard]]
		Array<double> getXAdvances() const;

		[[nodiscard]]
		Array<double> getXAdvances(double fontSize) const;
	};
}

# include "detail/DrawableText.ipp"
