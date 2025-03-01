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

# include <Siv3D/Emoji.hpp>
# include <Siv3D/Emoji/IEmoji.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Emoji::Emoji(const char32* emoji)
		: codePoints{ emoji } {}

	Emoji::Emoji(const StringView emoji)
		: codePoints{ emoji } {}

	Emoji::Emoji(String emoji)
		: codePoints{ std::move(emoji) } {}

	////////////////////////////////////////////////////////////////
	//
	//	HasGlyph
	//
	////////////////////////////////////////////////////////////////

	bool Emoji::HasGlyph(const StringView emoji)
	{
		return SIV3D_ENGINE(Emoji)->hasEmoji(emoji);
	}

	////////////////////////////////////////////////////////////////
	//
	//	GetGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex Emoji::GetGlyphIndex(const StringView emoji)
	{
		return SIV3D_ENGINE(Emoji)->getGlyphIndex(emoji);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateImage
	//
	////////////////////////////////////////////////////////////////

	Image Emoji::CreateImage(const StringView emoji, const int32 size)
	{
		return SIV3D_ENGINE(Emoji)->renderEmoji(SIV3D_ENGINE(Emoji)->getGlyphIndex(emoji), size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateImageByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	Image Emoji::CreateImageByGlyphIndex(const GlyphIndex glyphIndex, const int32 size)
	{
		return SIV3D_ENGINE(Emoji)->renderEmoji(glyphIndex, size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateFilledImage
	//
	////////////////////////////////////////////////////////////////

	Image Emoji::CreateFilledImage(const StringView emoji, const int32 size)
	{
		return SIV3D_ENGINE(Emoji)->renderFilledEmoji(SIV3D_ENGINE(Emoji)->getGlyphIndex(emoji), size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateFilledImageByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	Image Emoji::CreateFilledImageByGlyphIndex(const GlyphIndex glyphIndex, const int32 size)
	{
		return SIV3D_ENGINE(Emoji)->renderFilledEmoji(glyphIndex, size);
	}

	inline namespace Literals
	{
		inline namespace EmojiLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_emoji
			//
			////////////////////////////////////////////////////////////////

			Emoji operator ""_emoji(const char32* s, const size_t length)
			{
				return Emoji{ StringView{ s, length } };
			}
		}
	}
}
