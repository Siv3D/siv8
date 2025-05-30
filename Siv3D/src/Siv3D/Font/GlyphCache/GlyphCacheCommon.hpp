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
# include <Siv3D/Common.hpp>
# include <Siv3D/GlyphInfo.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/DynamicTexture.hpp>
# include <Siv3D/HashMap.hpp>
# include <Siv3D/Char.hpp>
# include "../FontData.hpp"

namespace s3d
{
	using InternalGlyphIndex = uint32;

	inline constexpr InternalGlyphIndex VerticalGlyphFlag = (1u << 31);

	[[nodiscard]]
	constexpr InternalGlyphIndex AsBaseGlyphIndex(const InternalGlyphIndex glyphIndex) noexcept
	{
		return (glyphIndex & ~VerticalGlyphFlag);
	}

	[[nodiscard]]
	constexpr InternalGlyphIndex AsVerticalGlyphIndex(const InternalGlyphIndex glyphIndex) noexcept
	{
		return (glyphIndex | VerticalGlyphFlag);
	}

	[[nodiscard]]
	constexpr InternalGlyphIndex AsInternalGlyphIndex(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) noexcept
	{
		if (readingDirection == ReadingDirection::TopToBottom)
		{
			return AsVerticalGlyphIndex(glyphIndex);
		}
		else
		{
			return glyphIndex;
		}
	}

	struct GlyphCache
	{
		GlyphInfo info;

		int16 textureRegionLeft = 0;

		int16 textureRegionTop = 0;

		int16 textureRegionWidth = 0;

		int16 textureRegionHeight = 0;
	};

	struct BufferImage
	{
		static constexpr int32 MaxHeight = 4096;

		static constexpr Color BackgroundColor{ 0, 0 };

		Image image;

		int32 bufferWidth = 2;

		int32 padding = 1;

		Point penPos{ 0, padding };

		int32 currentMaxHeight = 0;

		bool isDirty = false;
	};

	class GlyphCacheManager
	{
	public:

		const GlyphCache& get(GlyphIndex glyphIndex, ReadingDirection readingDirection) const
		{
			return m_glyphTable.find(AsInternalGlyphIndex(glyphIndex, readingDirection))->second;
		}

		bool isEmpty() const noexcept
		{
			return m_glyphTable.empty();
		}

		const Texture& getTexture() const noexcept
		{
			return m_texture;
		}

		bool cacheGlyph(FontData& font, GlyphIndex glyphIndex, ReadingDirection readingDirection);

		void updateTexture();

	private:

		static constexpr int32 MaxHeight = 4096;

		static constexpr Color BackgroundColor{ 0, 0 };

		HashMap<InternalGlyphIndex, GlyphCache> m_glyphTable;

		DynamicTexture m_texture;

		Image m_image;

		int32 m_bufferWidth = 2;

		int32 m_padding = 1;

		Point m_penPos{ 0, m_padding };

		int32 m_currentMaxHeight = 0;

		bool m_isDirty = false;
	};

	[[nodiscard]]
	double GetTabAdvance(double spaceWidth, double scale, double xBegin, double currentX, int32 tabSize);

	[[nodiscard]]
	bool ConsumeControlCharacter(char32 ch, Vec2& penPos, int32& lineCount, const Vec2& basePos, double scale, double lineHeightScale, const FontFaceInfo& info);
}
