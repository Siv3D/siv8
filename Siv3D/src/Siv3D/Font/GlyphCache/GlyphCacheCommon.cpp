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

# include "GlyphCacheCommon.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr Size CalculateInitialBitmapSize(const int32 fontBaseSize) noexcept
		{
			const int32 width =
				(fontBaseSize <= 16) ? 512 :
				(fontBaseSize <= 32) ? 768 :
				(fontBaseSize <= 48) ? 1024 :
				(fontBaseSize <= 64) ? 1536 :
				(fontBaseSize <= 256) ? 2048 : 4096;
			
			const int32 height = (fontBaseSize <= 256) ? 256 : 512;

			return{ width, height };
		}
	}

	double GetTabAdvance(const double spaceWidth, const double scale, const double xBegin, const double currentX, const int32 tabSize)
	{
		// タブの基本幅（ピクセル）
		const double baseTabWidth = (spaceWidth * tabSize * scale);

		// 現在何番目のタブ位置か
		const int32 currentIndentLevel = ((currentX - xBegin) / baseTabWidth);

		// 次のタブ位置は何番目か
		const int32 nextIndentLevel = (currentIndentLevel + 1);

		// 次のタブ位置の X 座標
		const double nextX = (xBegin + (nextIndentLevel * baseTabWidth));

		// 次のタブ位置までの距離を返す
		return (nextX - currentX);
	}

	bool CacheGlyph(const FontData& font, const Image& glyphImage, const GlyphInfo& glyphInfo, BufferImage& buffer, HashMap<GlyphIndex, GlyphCache>& glyphTable)
	{
		// バッファ画像が作成されていない場合、作成する
		if (not buffer.image)
		{
			const Size initialBitmapSize = CalculateInitialBitmapSize(font.getInfo().baseSize);
			buffer.image.resize(initialBitmapSize, BufferImage::BackgroundColor);
		}

		buffer.penPos.x += buffer.padding;

		const int32 glyphWidth	= glyphImage.width();
		const int32 glyphHeight	= glyphImage.height();

		// 右に余白が足りない場合、次の行へ進む
		if (buffer.image.width() < (buffer.penPos.x + (glyphWidth + buffer.padding)))
		{
			buffer.penPos.x = buffer.padding;
			buffer.penPos.y += (buffer.currentMaxHeight + (buffer.padding * 2));
			buffer.currentMaxHeight = 0;
		}

		// 下に余白が足りない場合、バッファ画像の高さを拡張する
		if (buffer.image.height() < (buffer.penPos.y + (glyphHeight + buffer.padding)))
		{
			const int32 newBitmapHeight = (((buffer.penPos.y + (glyphHeight + buffer.padding)) + 255) / 256 * 256);

			if (BufferImage::MaxHeight < newBitmapHeight)
			{
				return false;
			}

			buffer.image.resizeHeight(newBitmapHeight, BufferImage::BackgroundColor);
		}

		// キャッシュ画像にグリフ画像を書き込む
		glyphImage.overwrite(buffer.image, buffer.penPos);

		GlyphCache cache;
		cache.info = glyphInfo;
		cache.textureRegionLeft		= static_cast<int16>(buffer.penPos.x);
		cache.textureRegionTop		= static_cast<int16>(buffer.penPos.y);
		cache.textureRegionWidth	= static_cast<int16>(glyphWidth);
		cache.textureRegionHeight	= static_cast<int16>(glyphHeight);
		glyphTable.emplace(glyphInfo.glyphIndex, cache);

		buffer.currentMaxHeight = Max(buffer.currentMaxHeight, glyphHeight);
		buffer.penPos.x += (glyphWidth + buffer.padding);

		return true;
	}
}
