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
# include <Siv3D/HashMap.hpp>
# include <Siv3D/Char.hpp>
# include "../FontData.hpp"

namespace s3d
{
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

		Image image;

		Color backgroundColor{ 255, 0 };

		int32 bufferWidth = 2;

		int32 padding = 1;

		Point penPos{ 0, padding };

		int32 currentMaxHeight = 0;
	};

	[[nodiscard]]
	double GetTabAdvance(double spaceWidth, double scale, double baseX, double currentX, int32 tabSize);

	[[nodiscard]]
	bool CacheGlyph(const FontData& font, const Image& image, const GlyphInfo& glyphInfo, BufferImage& buffer, HashMap<GlyphIndex, GlyphCache>& glyphTable);
}
