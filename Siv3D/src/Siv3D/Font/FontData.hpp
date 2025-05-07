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
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include "IFont.hpp"
# include "FontFace.hpp"

namespace s3d
{
	class FontData
	{
	public:

		struct Null {};

		FontData() = default;

		explicit FontData(Null);

		FontData(::FT_Library library, FilePathView path, size_t faceIndex, StringView styleName, FontMethod fontMethod, int32 baseSize, FontStyle style);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		String toString() const;

		[[nodiscard]]
		const FontFaceInfo& getInfo() const noexcept;

		void setTabSize(int32 tabSize) noexcept;

		[[nodiscard]]
		GlyphIndex getGlyphIndex(char32 codePoint);

		[[nodiscard]]
		GlyphIndex getGlyphIndex(StringView ch);
		
		[[nodiscard]]
		String getGlyphNameByGlyphIndex(GlyphIndex glyphIndex);

	private:

		MemoryMappedFileView m_mappedFileView;

		std::unique_ptr<FontFace> m_face;

		FontMethod m_renderingMethod = FontMethod::Bitmap;

		bool m_initialized = false;
	};
}
