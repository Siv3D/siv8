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
# include "IFont.hpp"
# include <Siv3D/Font.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "FontData.hpp"

namespace s3d
{
	class CFont final : public ISiv3DFont
	{
	public:

		CFont();

		~CFont();

		void init() override;

		[[nodiscard]]
		Array<FontFaceProperties> getFontFaces(FilePathView path) const override;
		
		[[nodiscard]]
		Font::IDType create(FilePathView path, size_t faceIndex, StringView styleName, FontMethod fontMethod, int32 baseSize, FontStyle style) override;

		void release(Font::IDType handleID) override;

		[[nodiscard]]
		const FontFaceInfo& getInfo(Font::IDType handleID) override;

		void setTabSize(Font::IDType handleID, int32 tabSize) override;

		GlyphIndex getGlyphIndex(Font::IDType handleID, char32 codePoint) override;

		GlyphIndex getGlyphIndex(Font::IDType handleID, StringView ch) override;

		String getGlyphNameByGlyphIndex(Font::IDType handleID, GlyphIndex glyphIndex) override;

	private:

		FT_Library m_freeType = nullptr;

		AssetHandleManager<Font::IDType, FontData> m_fonts{ "Font" };
	};
}
