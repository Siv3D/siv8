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

# include <Siv3D/Font.hpp>
# include <Siv3D/DrawableText.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Font/FontFace.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
# include "TypefaceUtility.hpp"

namespace s3d
{
	namespace
	{
		static void CheckEngine(const StringView type = U"Font")
		{
			if (Siv3DEngine::isNull())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, type);
				std::exit(EXIT_FAILURE);
			}
		}
	}

	template <>
	AssetIDWrapper<AssetHandle<Font>>::AssetIDWrapper()
	{
		CheckEngine();
	}

	template <>
	AssetIDWrapper<AssetHandle<Font>>::~AssetIDWrapper()
	{
		if (not Siv3DEngine::isAvailable())
		{
			return;
		}

		SIV3D_ENGINE(Font)->release(m_id);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Font::Font() {}

	Font::Font(const int32 baseSize, const Typeface typeface, const FontStyle style)
		: Font{ FontMethod::Bitmap, baseSize, typeface, style } {}

	Font::Font(const int32 baseSize, const FilePathView path, const FontStyle style)
		: Font{ FontMethod::Bitmap, baseSize, path, 0, U"", style } {}

	Font::Font(const int32 baseSize, const FilePathView path, const size_t faceIndex, const FontStyle style)
		: Font{ FontMethod::Bitmap, baseSize, path, faceIndex, U"", style } {}

	Font::Font(const int32 baseSize, const FilePathView path, const StringView styleName, const FontStyle style)
		: Font{ FontMethod::Bitmap, baseSize, path, 0, styleName, style } {}

	Font::Font(const int32 baseSize, const FilePathView path, const size_t faceIndex, const StringView styleName, const FontStyle style)
		: Font{ FontMethod::Bitmap, baseSize, path, faceIndex, styleName, style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const Typeface typeface, const FontStyle style)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Font)->create(typeface, fontMethod, baseSize, style))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const FontStyle style)
		: Font{ fontMethod, baseSize, path, 0, U"", style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const size_t faceIndex, const FontStyle style)
		: Font{ fontMethod, baseSize, path, faceIndex, U"", style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const StringView styleName, const FontStyle style)
		: Font{ fontMethod, baseSize, path, 0, styleName, style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const size_t faceIndex, const StringView styleName, const FontStyle style)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Font)->create(path, faceIndex, styleName, fontMethod, baseSize, style))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const int32 bufferThickness, const Typeface typeface, const FontStyle style)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Font)->create(typeface, fontMethod, baseSize, style))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
		setBufferThickness(bufferThickness);
	}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const int32 bufferThickness, const FilePathView path, const FontStyle style)
		: Font{ fontMethod, baseSize, bufferThickness, path, 0, U"", style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const int32 bufferThickness, const FilePathView path, const size_t faceIndex, const FontStyle style)
		: Font{ fontMethod, baseSize, bufferThickness, path, faceIndex, U"", style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const int32 bufferThickness, const FilePathView path, const StringView styleName, const FontStyle style)
		: Font{ fontMethod, baseSize, bufferThickness, path, 0, styleName, style } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const int32 bufferThickness, const FilePathView path, const size_t faceIndex, const StringView styleName, const FontStyle style)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Font)->create(path, faceIndex, styleName, fontMethod, baseSize, style))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
		setBufferThickness(bufferThickness);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Font::~Font() {}

	////////////////////////////////////////////////////////////////
	//
	//	addFallback
	//
	////////////////////////////////////////////////////////////////

	bool Font::addFallback(const Font& font) const
	{
		if (not font)
		{
			LOG_FAIL("Font::addFallback(): Empty font");
			return false;
		}

		if (font.id() == id())
		{
			LOG_FAIL(U"Font::addFallback(): Cannot add self as fallback");
			return false;
		}

		SIV3D_ENGINE(Font)->addFallbackFont(m_handle->id(), font);

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	baseSize
	//
	////////////////////////////////////////////////////////////////

	int32 Font::baseSize() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).baseSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	method
	//
	////////////////////////////////////////////////////////////////

	FontMethod Font::method() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).renderingMethod;
	}

	////////////////////////////////////////////////////////////////
	//
	//	style
	//
	////////////////////////////////////////////////////////////////

	FontStyle Font::style() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).style;
	}

	////////////////////////////////////////////////////////////////
	//
	//	properties
	//
	////////////////////////////////////////////////////////////////

	const FontFaceProperties& Font::properties() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).properties;
	}

	////////////////////////////////////////////////////////////////
	//
	//	familyName
	//
	////////////////////////////////////////////////////////////////

	const String& Font::familyName() const
	{
		return properties().familyName;
	}

	////////////////////////////////////////////////////////////////
	//
	//	styleName
	//
	////////////////////////////////////////////////////////////////

	const String& Font::styleName() const
	{
		return properties().styleName;
	}

	////////////////////////////////////////////////////////////////
	//
	//	postscriptName
	//
	////////////////////////////////////////////////////////////////

	const String& Font::postscriptName() const
	{
		return properties().postscriptName;
	}

	////////////////////////////////////////////////////////////////
	//
	//	version
	//
	////////////////////////////////////////////////////////////////

	const String& Font::version() const
	{
		return properties().version;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasColor
	//
	////////////////////////////////////////////////////////////////

	bool Font::hasColor() const
	{
		return properties().hasColor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlyphNames
	//
	////////////////////////////////////////////////////////////////

	bool Font::hasGlyphNames() const
	{
		return properties().hasGlyphNames;
	}

	////////////////////////////////////////////////////////////////
	//
	//	ascender
	//
	////////////////////////////////////////////////////////////////

	double Font::ascender() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).ascender;
	}

	////////////////////////////////////////////////////////////////
	//
	//	descender
	//
	////////////////////////////////////////////////////////////////

	double Font::descender() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).descender;
	}

	////////////////////////////////////////////////////////////////
	//
	//	height
	//
	////////////////////////////////////////////////////////////////

	double Font::height() const
	{
		const auto& info = SIV3D_ENGINE(Font)->getInfo(m_handle->id());
		const double h = (info.ascender + info.descender);
		return h;
	}

	////////////////////////////////////////////////////////////////
	//
	//	spaceXAdvance
	//
	////////////////////////////////////////////////////////////////

	double Font::spaceXAdvance() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).spaceXAdvance;
	}

	////////////////////////////////////////////////////////////////
	//
	//	spaceYAdvance
	//
	////////////////////////////////////////////////////////////////

	double Font::spaceYAdvance() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).spaceYAdvance;
	}

	////////////////////////////////////////////////////////////////
	//
	//	tabSize
	//
	////////////////////////////////////////////////////////////////

	int32 Font::tabSize() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).tabSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTabSize
	//
	////////////////////////////////////////////////////////////////

	const Font& Font::setTabSize(const int32 tabSize) const
	{
		SIV3D_ENGINE(Font)->setTabSize(m_handle->id(), tabSize);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBufferThickness
	//
	////////////////////////////////////////////////////////////////

	int32 Font::getBufferThickness() const
	{
		return SIV3D_ENGINE(Font)->getBufferThickness(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setBufferThickness
	//
	////////////////////////////////////////////////////////////////

	const Font& Font::setBufferThickness(const int32 thickness) const
	{
		SIV3D_ENGINE(Font)->setBufferThickness(m_handle->id(), thickness);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_glyphs
	//
	////////////////////////////////////////////////////////////////

	uint32 Font::num_glyphs() const
	{
		return properties().numGlyphs;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlyph
	//
	////////////////////////////////////////////////////////////////

	bool Font::hasGlyph(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		return (SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection) != GlyphIndexNotdef);
	}

	bool Font::hasGlyph(const StringView ch, const ReadingDirection readingDirection) const
	{
		return (SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection) != GlyphIndexNotdef);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex Font::getGlyphIndex(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		return SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
	}

	GlyphIndex Font::getGlyphIndex(const StringView ch, const ReadingDirection readingDirection) const
	{
		return SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphName
	//
	////////////////////////////////////////////////////////////////

	String Font::getGlyphName(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
		return getGlyphNameByGlyphIndex(glyphIndex);
	}

	String Font::getGlyphName(const StringView ch, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
		return getGlyphNameByGlyphIndex(glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphNameByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	String Font::getGlyphNameByGlyphIndex(const GlyphIndex glyphIndex) const
	{
		return SIV3D_ENGINE(Font)->getGlyphNameByGlyphIndex(m_handle->id(), glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvance
	//
	////////////////////////////////////////////////////////////////

	double Font::getXAdvance(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
		return getXAdvanceByGlyphIndex(glyphIndex);
	}

	double Font::getXAdvance(const StringView ch, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
		return getXAdvanceByGlyphIndex(glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double Font::getXAdvanceByGlyphIndex(const GlyphIndex glyphIndex) const
	{
		return SIV3D_ENGINE(Font)->getXAdvanceByGlyphIndex(m_handle->id(), glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvance
	//
	////////////////////////////////////////////////////////////////

	double Font::getYAdvance(const char32 codePoint) const
	{
		return SIV3D_ENGINE(Font)->getYAdvance(m_handle->id(), StringView{ &codePoint, 1 });
	}

	double Font::getYAdvance(const StringView ch) const
	{
		return SIV3D_ENGINE(Font)->getYAdvance(m_handle->id(), ch);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getYAdvanceByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	double Font::getYAdvanceByGlyphIndex(const GlyphIndex glyphIndex) const
	{
		return SIV3D_ENGINE(Font)->getYAdvanceByGlyphIndex(m_handle->id(), glyphIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getResolvedGlyphs
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	Array<ResolvedGlyph> Font::getResolvedGlyphs(const StringView s, const ReadingDirection readingDirection, const EnableFallback enableFallback, const EnableLigatures enableLigatures) const
	{
		return SIV3D_ENGINE(Font)->getResolvedGlyphs(m_handle->id(), s, readingDirection, enableFallback, enableLigatures);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphInfo
	//
	////////////////////////////////////////////////////////////////

	GlyphInfo Font::getGlyphInfo(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
		return getGlyphInfoByGlyphIndex(glyphIndex, readingDirection);
	}

	GlyphInfo Font::getGlyphInfo(const StringView ch, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
		return getGlyphInfoByGlyphIndex(glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphInfoByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphInfo Font::getGlyphInfoByGlyphIndex(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) const
	{
		return SIV3D_ENGINE(Font)->getGlyphInfoByGlyphIndex(m_handle->id(), glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderOutline
	//
	////////////////////////////////////////////////////////////////

	OutlineGlyph Font::renderOutline(const char32 codePoint, const CloseRing closeRing, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
		return renderOutlineByGlyphIndex(glyphIndex, closeRing, readingDirection);
	}

	OutlineGlyph Font::renderOutline(const StringView ch, const CloseRing closeRing, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
		return renderOutlineByGlyphIndex(glyphIndex, closeRing, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderOutline
	//
	////////////////////////////////////////////////////////////////

	OutlineGlyph Font::renderOutlineByGlyphIndex(const GlyphIndex glyphIndex, const CloseRing closeRing, const ReadingDirection readingDirection) const
	{
		return SIV3D_ENGINE(Font)->renderOutlineByGlyphIndex(m_handle->id(), glyphIndex, closeRing, readingDirection);
	}



	////////////////////////////////////////////////////////////////
	//
	//	renderBitmap
	//
	////////////////////////////////////////////////////////////////

	BitmapGlyph Font::renderBitmap(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
		return renderBitmapByGlyphIndex(glyphIndex, readingDirection);
	}

	BitmapGlyph Font::renderBitmap(const StringView ch, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
		return renderBitmapByGlyphIndex(glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderBitmapByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	BitmapGlyph Font::renderBitmapByGlyphIndex(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) const
	{
		return SIV3D_ENGINE(Font)->renderBitmapByGlyphIndex(m_handle->id(), glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderMSDF
	//
	////////////////////////////////////////////////////////////////

	MSDFGlyph Font::renderMSDF(const char32 codePoint, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint, readingDirection);
		return renderMSDFByGlyphIndex(glyphIndex, readingDirection);
	}

	MSDFGlyph Font::renderMSDF(const StringView ch, const ReadingDirection readingDirection) const
	{
		const GlyphIndex glyphIndex = SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch, readingDirection);
		return renderMSDFByGlyphIndex(glyphIndex, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderMSDFByGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	MSDFGlyph Font::renderMSDFByGlyphIndex(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) const
	{
		return SIV3D_ENGINE(Font)->renderMSDFByGlyphIndex(m_handle->id(), glyphIndex, readingDirection);
	}


	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	const Texture& Font::getTexture() const
	{
		return SIV3D_ENGINE(Font)->getTexture(m_handle->id());
	}



	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void Font::swap(Font& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ()
	//
	////////////////////////////////////////////////////////////////

	DrawableText Font::operator ()(const String& text) const
	{
		return{ *this, text, ReadingDirection::LeftToRight };
	}

	DrawableText Font::operator ()(String&& text) const
	{
		return{ *this, std::move(text), ReadingDirection::LeftToRight };
	}

	DrawableText Font::operator ()(const ReadingDirection readingDirection, const String& text) const
	{
		return{ *this, text, readingDirection };
	}

	DrawableText Font::operator ()(const ReadingDirection readingDirection, String&& text) const
	{
		return{ *this, std::move(text), readingDirection };
	}




	////////////////////////////////////////////////////////////////
	//
	//	GetFaces
	//
	////////////////////////////////////////////////////////////////

	Array<FontFaceProperties> Font::GetFaces(const FilePathView path)
	{
		return SIV3D_ENGINE(Font)->getFontFaces(path);
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsAvailable
	//
	////////////////////////////////////////////////////////////////

	bool Font::IsAvailable(const Typeface typeface)
	{
		const FilePath fontFilePath = GetTypefaceInfo(typeface, FontMethod::Bitmap).path;

		return FileSystem::Exists(fontFilePath);
	}
}

