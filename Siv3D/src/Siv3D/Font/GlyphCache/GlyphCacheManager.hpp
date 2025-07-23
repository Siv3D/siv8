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
# include <Siv3D/HashMap.hpp>
# include <Siv3D/GlyphInfo.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/DynamicTexture.hpp>
# include "GlyphCacheCommon.hpp"

namespace s3d
{
	class FontData;

	using InternalGlyphIndex = uint32;

	struct GlyphCache
	{
		GlyphInfo info;

		int16 textureRegionLeft = 0;

		int16 textureRegionTop = 0;

		int16 textureRegionWidth = 0;

		int16 textureRegionHeight = 0;
	};

	struct PenPosInfo
	{
		Float2 penPos;

		Float2 drawPos;
	};

	class GlyphCacheManager
	{
	public:

		GlyphCacheManager() = default;

		explicit GlyphCacheManager(int32 bufferThickness);

		void setBufferThickness(int32 bufferThickness) noexcept;

		int32 getBufferThickness() const noexcept;

		const GlyphCache& get(GlyphIndex glyphIndex, ReadingDirection readingDirection) const;

		bool isEmpty() const noexcept;

		const Texture& getTexture() const noexcept;

		bool cacheGlyph(FontData& font, GlyphIndex glyphIndex, ReadingDirection readingDirection);

		void updateTexture();

	private:

		static constexpr int32 MaxHeight = 4096;

		static constexpr Color BackgroundColor{ 0, 0 };

		HashMap<InternalGlyphIndex, GlyphCache> m_glyphTable;

		DynamicTexture m_texture;

		Image m_image;

		int32 m_bufferThickness = 2;

		int32 m_padding = 1;

		Point m_penPos{ 0, m_padding };

		int32 m_currentMaxHeight = 0;

		bool m_isDirty = false;
	};
}
