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

# include "CFont.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static FontFaceProperties GetFontFaceProperties(const FT_Face face)
		{
			if (not face)
			{
				return {};
			}

			FontFaceProperties properties;

			if (face->family_name)
			{
				properties.familyName = Unicode::FromUTF8(face->family_name);
			}

			if (face->style_name)
			{
				properties.styleName = Unicode::FromUTF8(face->style_name);
			}

			if (const char* postscriptName = FT_Get_Postscript_Name(face))
			{
				properties.postscriptName = Unicode::FromUTF8(postscriptName);
			}

			for (int32 i = 0; i < face->num_fixed_sizes; ++i)
			{
				properties.availableBitmapSizes << face->available_sizes[i].height;
			}

			properties.numGlyphs		= face->num_glyphs;
			properties.unitsPerEM		= face->units_per_EM;
			properties.ascender			= face->ascender;
			properties.descender		= face->descender;
			properties.height			= face->height;
			properties.bbox				= Rect{ face->bbox.xMin, face->bbox.yMin, (face->bbox.xMax - face->bbox.xMin), (face->bbox.yMax - face->bbox.yMin) };

			properties.hasColor			= FT_HAS_COLOR(face);
			properties.isScalable		= FT_IS_SCALABLE(face);
			properties.isVariable		= FT_HAS_MULTIPLE_MASTERS(face);
			properties.isFixedPitch		= FT_IS_FIXED_WIDTH(face);
			properties.hasHorizontal	= FT_HAS_HORIZONTAL(face);
			properties.hasVertical		= FT_HAS_VERTICAL(face);
			properties.hasKerning		= FT_HAS_KERNING(face);

			return properties;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CFont::CFont() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CFont::~CFont()
	{
		LOG_SCOPED_DEBUG("CFont::~CFont()");

		m_fonts.destroy();

		m_freeType = nullptr;
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CFont::init()
	{
		LOG_SCOPED_DEBUG("CFont::init()");

		if (const FT_Error error = ::FT_Init_FreeType(&m_freeType))
		{
			throw InternalEngineError{ U"FT_Init_FreeType() failed" };
		}

		// null Font を管理に登録
		{
			// null Font を作成
			auto nullFont = std::make_unique<FontData>(FontData::Null{});

			if (not nullFont->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null font" };
			}

			// 管理に登録
			m_fonts.setNullData(std::move(nullFont));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFontFileInfo
	//
	////////////////////////////////////////////////////////////////

	FontFileInfo CFont::getFontFileInfo(const FilePathView path) const
	{
		MemoryMappedFileView file{ path };

		const MappedMemoryView view = file.mapAll();

		FT_Face face0 = nullptr;

		if (const FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const FT_Byte*>(view.data), static_cast<FT_Long>(view.size), 0, &face0))
		{
			return{};
		}

		FontFileInfo info;
		info.fontFaces.reserve(face0->num_faces);
		info.fontFaces << GetFontFaceProperties(face0);

		for (FT_Long index = 1; index < face0->num_faces; ++index)
		{
			FT_Face face = nullptr;

			if (const FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const FT_Byte*>(view.data), static_cast<FT_Long>(view.size), index, &face))
			{
				info.fontFaces.emplace_back();
				continue;
			}

			info.fontFaces << GetFontFaceProperties(face);

			::FT_Done_Face(face);
		}

		::FT_Done_Face(face0);

		return info;
	}
}
