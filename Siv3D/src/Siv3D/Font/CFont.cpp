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
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String ToString(const ::FT_SfntName& name)
		{
			if ((name.platform_id == TT_PLATFORM_APPLE_UNICODE)
				|| ((name.platform_id == TT_PLATFORM_MICROSOFT) && (name.encoding_id == 1)) // Unicode BMP
				|| ((name.platform_id == TT_PLATFORM_MICROSOFT) && (name.encoding_id == 10)) // UTS
				)
			{
				const size_t nameBytes = name.string_len;

				if ((nameBytes % 2) == 0)
				{
					const char16_t* pName = static_cast<const char16_t*>(static_cast<const void*>(name.string));
					const size_t nameLength = (nameBytes / sizeof(char16_t));
					return Unicode::FromUTF16BE(std::u16string_view{ pName, nameLength });
				}
			}
			else
			{
				const char* pName = static_cast<const char*>(static_cast<const void*>(name.string));
				const size_t nameLength = name.string_len;
				return Unicode::FromUTF8(std::string_view{ pName, nameLength });
			}

			return U"";
		}

		[[nodiscard]]
		static FontFaceProperties GetFontFaceProperties(const ::FT_Face face, const FT_Fixed* namedStyleCoords)
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

			if (const char* postscriptName = ::FT_Get_Postscript_Name(face))
			{
				properties.postscriptName = Unicode::FromUTF8(postscriptName);
			}

			if (const ::FT_UInt nameCount = ::FT_Get_Sfnt_Name_Count(face))
			{
				for (::FT_UInt i = 0; i < nameCount; ++i)
				{
					::FT_SfntName name;

					if (const ::FT_Error error = ::FT_Get_Sfnt_Name(face, i, &name))
					{
						continue;
					}

					if (name.name_id == TT_NAME_ID_VERSION_STRING)
					{
						properties.version = ToString(name);
						break;
					}
				}
			}

			for (int32 i = 0; i < face->num_fixed_sizes; ++i)
			{
				properties.availableBitmapSizes << face->available_sizes[i].height;
			}

			properties.numGlyphs = face->num_glyphs;
			properties.unitsPerEM = face->units_per_EM;
			properties.isBold = ((face->style_flags & FT_STYLE_FLAG_BOLD) != 0);

			{
				::FT_MM_Var* mmVar = nullptr;

				if (::FT_Get_MM_Var(face, &mmVar) == 0)
				{
					for (::FT_UInt axisIndex = 0; axisIndex < mmVar->num_axis; ++axisIndex)
					{
						const ::FT_Var_Axis& mmVarAxis = mmVar->axis[axisIndex];

						FontVariationAxis axis;

						// name
						if (mmVarAxis.name)
						{
							axis.name = Unicode::FromUTF8(mmVarAxis.name);
						}

						// tag
						{
							char32 tag[4]{};
							tag[0] = static_cast<char>(mmVarAxis.tag >> 24);
							tag[1] = static_cast<char>(mmVarAxis.tag >> 16);
							tag[2] = static_cast<char>(mmVarAxis.tag >> 8);
							tag[3] = static_cast<char>(mmVarAxis.tag >> 0);
							axis.tag = String{ tag[0], tag[1], tag[2], tag[3] };
						}

						axis.minValue		= (mmVarAxis.minimum / 65536.0f);
						axis.defaultValue	= (mmVarAxis.def / 65536.0f);
						axis.maxValue		= (mmVarAxis.maximum / 65536.0f);
						axis.value			= (namedStyleCoords ? (namedStyleCoords[axisIndex] / 65536.0f) : axis.defaultValue);

						if ((axis.tag == U"wght")
							&& (700 <= axis.value))
						{
							properties.isBold = true;
						}

						properties.variationAxes << axis;
					}
				}
			}

			properties.hasColor			= FT_HAS_COLOR(face);
			properties.isItalic			= ((face->style_flags & FT_STYLE_FLAG_ITALIC) != 0);
			properties.isScalable		= FT_IS_SCALABLE(face);
			properties.isVariable		= FT_HAS_MULTIPLE_MASTERS(face);
			properties.isFixedPitch		= FT_IS_FIXED_WIDTH(face);
			properties.hasHorizontal	= FT_HAS_HORIZONTAL(face);
			properties.hasVertical		= FT_HAS_VERTICAL(face);
			properties.hasKerning		= FT_HAS_KERNING(face);

			return properties;
		}

		[[nodiscard]]
		static Array<FontFaceProperties> GetFontFaces(const ::FT_Library library, const ::FT_Face face)
		{
			Array<FontFaceProperties> faces;

			::FT_MM_Var* mmVar = nullptr;

			if (::FT_Get_MM_Var(face, &mmVar) == 0)
			{
				for (::FT_UInt styleIndex = 0; styleIndex < mmVar->num_namedstyles; ++styleIndex)
				{
					::FT_Set_Named_Instance(face, (1 + styleIndex));
					
					faces << GetFontFaceProperties(face, mmVar->namedstyle[styleIndex].coords);
				}

				::FT_Done_MM_Var(library, mmVar);
			}
			else
			{
				faces << GetFontFaceProperties(face, nullptr);
			}

			return faces;
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

	Array<FontFaceProperties> CFont::getFontFaces(const FilePathView path) const
	{
		MemoryMappedFileView file{ path };

		const MappedMemoryView view = file.mapAll();

		FT_Face face0 = nullptr;

		if (const FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const FT_Byte*>(view.data), static_cast<FT_Long>(view.size), 0, &face0))
		{
			return{};
		}

		const FT_Long numFaces = face0->num_faces;

		Array<FontFaceProperties> faces;
		faces.append(GetFontFaces(m_freeType, face0));

		for (FT_Long index = 1; index < numFaces; ++index)
		{
			FT_Face face = nullptr;

			if (const FT_Error error = ::FT_New_Memory_Face(m_freeType, static_cast<const FT_Byte*>(view.data), static_cast<FT_Long>(view.size), index, &face))
			{
				faces.emplace_back();
				continue;
			}

			faces.append(GetFontFaces(m_freeType, face));

			::FT_Done_Face(face);
		}

		::FT_Done_Face(face0);

		return faces;
	}
}
