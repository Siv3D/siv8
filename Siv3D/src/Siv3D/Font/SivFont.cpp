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
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Font/FontFace.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

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

	Font::Font(const int32 baseSize, const FilePathView path)
		: Font{ FontMethod::MSDF, baseSize, path, 0, U"", FontStyle::Normal } {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const size_t faceIndex, const StringView styleName, const FontStyle style)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Font)->create(path, faceIndex, styleName, fontMethod, baseSize, style))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Font::~Font() {}

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
	//	num_glyphs
	//
	////////////////////////////////////////////////////////////////

	uint32 Font::num_glyphs() const
	{
		return SIV3D_ENGINE(Font)->getInfo(m_handle->id()).properties.numGlyphs;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlyph
	//
	////////////////////////////////////////////////////////////////

	bool Font::hasGlyph(const char32 codePoint) const
	{
		return (SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint) != GlyphIndexNotdef);
	}

	bool Font::hasGlyph(const StringView ch) const
	{
		return (SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch) != GlyphIndexNotdef);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlyphIndex
	//
	////////////////////////////////////////////////////////////////

	GlyphIndex Font::getGlyphIndex(const char32 codePoint) const
	{
		return SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), codePoint);
	}

	GlyphIndex Font::getGlyphIndex(const StringView ch) const
	{
		return SIV3D_ENGINE(Font)->getGlyphIndex(m_handle->id(), ch);
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
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void Font::swap(Font& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}

}
