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

# include "FontUtility.hpp"

//SIV3D_DISABLE_MSVC_WARNINGS_PUSH(5054)
//# include "agg/agg_curves.h"
//SIV3D_DISABLE_MSVC_WARNINGS_POP()

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

			return{};
		}

		[[nodiscard]]
		static bool IsCOLRv1(const ::FT_Face face)
		{
			// テーブルの先頭 4 バイト（version）を取得
			::FT_Byte buffer[4]{};
			::FT_ULong length = sizeof(buffer);
			::FT_Error error = ::FT_Load_Sfnt_Table(face, FT_MAKE_TAG('C', 'O', 'L', 'R'), 0, buffer, &length);

			if ((error != 0) || (length < 4))
			{
				return false;
			}

			const FT_UInt versionMajor = ((buffer[0] << 8) | buffer[1]);
			return (versionMajor == 1);
		}

		//struct GlyphBBox
		//{
		//	double xMin = Math::Inf;
		//	double yMin = Math::Inf;
		//	double xMax = -Math::Inf;
		//	double yMax = -Math::Inf;
		//};

		//struct UserData
		//{
		//	double xMin = Math::Inf;
		//	double yMin = Math::Inf;
		//	double xMax = -Math::Inf;
		//	double yMax = -Math::Inf;

		//	Vec2 last = { Math::NaN, Math::NaN };

		//	void update(double x, double y) noexcept
		//	{
		//		if (xMax < x)
		//		{
		//			xMax = x;
		//		}

		//		if (x < xMin)
		//		{
		//			xMin = x;
		//		}

		//		if (yMax < y)
		//		{
		//			yMax = y;
		//		}

		//		if (y < yMin)
		//		{
		//			yMin = y;
		//		}

		//		last.set(x, y);
		//	}
		//};

		//static int MoveTo(const FT_Vector* to, void* ptr)
		//{
		//	UserData* user = static_cast<UserData*>(ptr);
		//	user->update(to->x, to->y);
		//	return 0;
		//}

		//static int LineTo(const FT_Vector* to, void* ptr)
		//{
		//	UserData* user = static_cast<UserData*>(ptr);
		//	user->update(to->x, to->y);
		//	return 0;
		//}

		//static int ConicTo(const FT_Vector* control, const FT_Vector* to, void* ptr)
		//{
		//	UserData* user = static_cast<UserData*>(ptr);

		//	if (not user->last.hasNaN())
		//	{
		//		const Vec2 prev = user->last;

		//		agg_fontnik::curve3_div curve(prev.x, prev.y, control->x, control->y, to->x, to->y);
		//		curve.rewind(0);

		//		double x, y;
		//		unsigned cmd;
		//		while (agg_fontnik::path_cmd_stop != (cmd = curve.vertex(&x, &y)))
		//		{
		//			user->update(x, y);
		//		}
		//	}

		//	return 0;
		//}

		//static int CubicTo(const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* ptr)
		//{
		//	UserData* user = static_cast<UserData*>(ptr);

		//	if (not user->last.hasNaN())
		//	{
		//		const Vec2 prev = user->last;

		//		agg_fontnik::curve4_div curve(prev.x, prev.y, c1->x, c1->y, c2->x, c2->y, to->x, to->y);
		//		curve.rewind(0);

		//		double x, y;
		//		unsigned cmd;
		//		while (agg_fontnik::path_cmd_stop != (cmd = curve.vertex(&x, &y)))
		//		{
		//			user->update(x, y);
		//		}
		//	}

		//	return 0;
		//}

		//GlyphBBox GetOutlineGlyphBound(FT_Face face)
		//{
		//	UserData userData;
		//	{
		//		const FT_Outline_Funcs func_interface = {
		//			.move_to = &MoveTo,
		//			.line_to = &LineTo,
		//			.conic_to = &ConicTo,
		//			.cubic_to = &CubicTo,
		//			.shift = 0,
		//			.delta = 0
		//		};

		//		if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
		//		{
		//			if (::FT_Outline_Decompose(&face->glyph->outline, &func_interface, &userData))
		//			{
		//				return{};
		//			}

		//			if (userData.last.hasNaN())
		//			{
		//				return{};
		//			}
		//		}
		//		else
		//		{
		//			return{};
		//		}
		//	}

		//	userData.xMin /= 64.0;
		//	userData.yMin /= 64.0;
		//	userData.xMax /= 64.0;
		//	userData.yMax /= 64.0;

		//	const double xMin = std::round(userData.xMin);
		//	const double yMin = std::round(userData.yMin);
		//	const double xMax = std::round(userData.xMax);
		//	const double yMax = std::round(userData.yMax);

		//	if (((xMax - xMin) == 0)
		//		|| ((yMax - yMin) == 0))
		//	{
		//		return{};
		//	}

		//	return GlyphBBox{
		//		.xMin = xMin,
		//		.yMin = yMin,
		//		.xMax = xMax,
		//		.yMax = yMax,
		//	};
		//}
	}

	////////////////////////////////////////////////////////////////
	//
	//	GetFontFaceProperties
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	FontFaceProperties GetFontFaceProperties(const ::FT_Face face, const FT_Fixed* namedStyleCoords)
	{
		if (not face)
		{
			return{};
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

		properties.numGlyphs	= face->num_glyphs;
		properties.unitsPerEM	= face->units_per_EM;
		properties.isBold		= ((face->style_flags & FT_STYLE_FLAG_BOLD) != 0);

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
		properties.isCOLRv1			= (properties.hasColor && IsCOLRv1(face));
		properties.isItalic			= ((face->style_flags & FT_STYLE_FLAG_ITALIC) != 0);
		properties.isScalable		= FT_IS_SCALABLE(face);
		properties.isVariable		= FT_HAS_MULTIPLE_MASTERS(face);
		properties.isFixedPitch		= FT_IS_FIXED_WIDTH(face);
		properties.hasHorizontal	= FT_HAS_HORIZONTAL(face);
		properties.hasVertical		= FT_HAS_VERTICAL(face);
		properties.hasKerning		= FT_HAS_KERNING(face);
		properties.hasGlyphNames	= FT_HAS_GLYPH_NAMES(face);

		return properties;
	}

	////////////////////////////////////////////////////////////////
	//
	//	GetGlyphInfo
	//
	////////////////////////////////////////////////////////////////

	GlyphInfo GetGlyphInfo(const ::FT_Face face, const GlyphIndex glyphIndex, const FontFaceInfo& info, const int16 bufferThickness, const ReadingDirection readingDirection)
	{
		::FT_Int32 loadFlag = (info.hinting ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING);

		const bool ttb = (readingDirection == ReadingDirection::TopToBottom);

		if (ttb)
		{
			loadFlag |= FT_LOAD_VERTICAL_LAYOUT;
		}

		if (info.properties.hasColor
			&& (not ::FT_Load_Glyph(face, glyphIndex, loadFlag)))
			// カラー読み込み失敗時はフォールバック
		{
			return GlyphInfo
			{
				.glyphIndex			= glyphIndex,
				.bufferThickness	= bufferThickness,
				.left				= ((ttb ? face->glyph->metrics.vertBearingX : face->glyph->metrics.horiBearingX) / 64.0f),
				.top				= ((ttb ? face->glyph->metrics.vertBearingY : face->glyph->metrics.horiBearingY) / 64.0f),
				.width				= (face->glyph->metrics.width / 64.0f),
				.height				= (face->glyph->metrics.height / 64.0f),
				.ascender			= info.ascender,
				.descender			= info.descender,
				.advance			= ((ttb ? face->glyph->metrics.vertAdvance : face->glyph->metrics.horiAdvance) / 64.0f),
			};
		}

		if ((info.renderingMethod != FontMethod::Bitmap) || not(info.style & FontStyle::Bitmap))
		{
			loadFlag |= FT_LOAD_NO_BITMAP;
		}

		if (::FT_Load_Glyph(face, glyphIndex, loadFlag))
		{
			return{};
		}

		if (info.style & FontStyle::Bold)
		{
			::FT_GlyphSlot_Embolden(face->glyph);
		}

		if (info.style & FontStyle::Italic)
		{
			::FT_GlyphSlot_Oblique(face->glyph);
		}

		return GlyphInfo
		{
			.glyphIndex			= glyphIndex,
			.bufferThickness	= bufferThickness,
			.left				= ((ttb ? face->glyph->metrics.vertBearingX : face->glyph->metrics.horiBearingX) / 64.0f),
			.top				= ((ttb ? face->glyph->metrics.vertBearingY : face->glyph->metrics.horiBearingY) / 64.0f),
			.width				= (face->glyph->metrics.width / 64.0f),
			.height				= (face->glyph->metrics.height / 64.0f),
			.ascender			= info.ascender,
			.descender			= info.descender,
			.advance			= ((ttb ? face->glyph->metrics.vertAdvance : face->glyph->metrics.horiAdvance) / 64.0f),
		};

		//if (info.renderingMethod == FontMethod::Bitmap)
		//{
		//	if (::FT_Load_Glyph(face, glyphIndex, (loadFlag | FT_LOAD_NO_BITMAP)))
		//	{
		//		return{};
		//	}

		//	if (info.style & FontStyle::Bold)
		//	{
		//		::FT_GlyphSlot_Embolden(face->glyph);
		//	}

		//	if (info.style & FontStyle::Italic)
		//	{
		//		::FT_GlyphSlot_Oblique(face->glyph);
		//	}

		//	return GlyphInfo
		//	{
		//		.glyphIndex			= glyphIndex,
		//		.bufferThickness	= bufferThickness,
		//		.left				= static_cast<float>(face->glyph->bitmap_left),
		//		.top				= static_cast<float>(face->glyph->bitmap_top),
		//		.width				= static_cast<float>(face->glyph->bitmap.width),
		//		.height				= static_cast<float>(face->glyph->bitmap.rows),
		//		.ascender			= info.ascender,
		//		.descender			= info.descender,
		//		.advance			= (face->glyph->metrics.horiAdvance / 64.0f),
		//	};
		//}
		//else
		//{
		//	if (::FT_Load_Glyph(face, glyphIndex, (loadFlag | FT_LOAD_NO_BITMAP)))
		//	{
		//		return{};
		//	}

		//	if (info.style & FontStyle::Bold)
		//	{
		//		::FT_GlyphSlot_Embolden(face->glyph);
		//	}

		//	if (info.style & FontStyle::Italic)
		//	{
		//		::FT_GlyphSlot_Oblique(face->glyph);
		//	}
		//}

		//const GlyphBBox bbox = GetOutlineGlyphBound(face);

		//return GlyphInfo{
		//	.glyphIndex			= glyphIndex,
		//	.bufferThickness	= bufferThickness,
		//	.left				= static_cast<float>(bbox.xMin),
		//	.top				= static_cast<float>(bbox.yMax),
		//	.width				= static_cast<float>(bbox.xMax - bbox.xMin),
		//	.height				= static_cast<float>(bbox.yMax - bbox.yMin),
		//	.ascender			= info.ascender,
		//	.descender			= info.descender,
		//	.advance			= (face->glyph->metrics.horiAdvance / 64.0f),
		//};
	}
}
