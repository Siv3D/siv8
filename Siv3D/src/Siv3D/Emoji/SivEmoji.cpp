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

# include <Siv3D/Emoji.hpp>
# include <Siv3D/Emoji/IEmoji.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	Emoji::Emoji(const char32* emoji)
		: codePoints{ emoji } {}

	Emoji::Emoji(const StringView emoji)
		: codePoints{ emoji } {}

	Emoji::Emoji(String emoji)
		: codePoints{ std::move(emoji) } {}

	bool Emoji::HasGlyph(const StringView emoji)
	{
		return SIV3D_ENGINE(Emoji)->hasEmoji(emoji);
	}

	//GlyphIndex Emoji::GetGlyphIndex(const StringView emoji)
	//{
	//	return SIV3D_ENGINE(Font)->getEmojiGlyphIndex(emoji);
	//}

	Image Emoji::CreateImage(const StringView emoji, const int32 size)
	{
		return SIV3D_ENGINE(Emoji)->renderEmoji(emoji, size);
	}

	//Image Emoji::CreateImageByGlyphIndex(const GlyphIndex glyphIndex)
	//{
	//	if (glyphIndex == 0)
	//	{
	//		return{};
	//	}

	//	return SIV3D_ENGINE(Font)->renderEmojiBitmap(glyphIndex);
	//}

	//Image Emoji::CreateSilhouetteImage(const StringView emoji)
	//{
	//	const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getEmojiGlyphIndex(emoji);

	//	return CreateSilhouetteImageByGlyphIndex(glyphIndex);
	//}

	//Image Emoji::CreateSilhouetteImageByGlyphIndex(const GlyphIndex glyphIndex)
	//{
	//	Image image = CreateImageByGlyphIndex(glyphIndex);

	//	for (auto& pixel : image)
	//	{
	//		pixel.setRGB(255, 255, 255);
	//	}

	//	return image;
	//}

	inline namespace Literals
	{
		inline namespace EmojiLiterals
		{
			Emoji operator ""_emoji(const char32* s, const size_t length)
			{
				return Emoji{ StringView{ s, length } };
			}
		}
	}
}
