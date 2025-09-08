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

# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/FileSystem.hpp>
# include "TypefaceUtility.hpp"

namespace s3d
{
	TypefaceInfo GetTypefaceInfo(const Typeface typeface, const FontMethod fontMethod)
	{
		const FilePath fontFilePath = (CacheDirectory::Engine() + U"font/");

		switch (typeface)
		{
		case Typeface::CJK_Regular_JP:
		case Typeface::CJK_Regular_KR:
		case Typeface::CJK_Regular_SC:
		case Typeface::CJK_Regular_TC:
		case Typeface::CJK_Regular_HK:
			{
				if (const FilePath pathCJK = (fontFilePath + U"NotoSansCJK-Medium.ttc"); FileSystem::Exists(pathCJK))
				{
					const uint32 index = (FromEnum(typeface) - FromEnum(Typeface::CJK_Regular_JP));
					return{ pathCJK, index, U"", fontMethod };
				}

				if (const FilePath pathJP = (fontFilePath + U"NotoSansJP-Medium.otf"); FileSystem::Exists(pathJP))
				{
					return{ pathJP, 0, U"", fontMethod };
				}

				if (const FilePath pathMin = (fontFilePath + U"siv3d-min.otf"); FileSystem::Exists(pathMin))
				{
					return{ pathMin, 0, U"", fontMethod };
				}

				break;
			}
		case Typeface::ColorEmoji:
			return{ (fontFilePath + U"Noto-COLRv1.ttf"), 0, U"", FontMethod::Bitmap };
		case Typeface::MPlus2_Thin:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0,  U"Thin", fontMethod };
		case Typeface::MPlus2_ExtraLight:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"ExtraLight", fontMethod };
		case Typeface::MPlus2_Light:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"Light", fontMethod };
		case Typeface::MPlus2_Regular:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"Regular", fontMethod };
		case Typeface::MPlus2_Medium:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"Medium", fontMethod };
		case Typeface::MPlus2_SemiBold:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"SemiBold", fontMethod };
		case Typeface::MPlus2_Bold:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"Bold", fontMethod };
		case Typeface::MPlus2_ExtraBold:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"ExtraBold", fontMethod };
		case Typeface::MPlus2_Black:
			return{ (fontFilePath + U"MPLUS2.ttf"), 0, U"Black", fontMethod };
		case Typeface::Icon_MaterialDesign:
			return{ (fontFilePath + U"materialdesignicons-webfont.ttf"), 0, U"", fontMethod };
		}

		return{ U"", 0, U"", FontMethod::Bitmap };
	}
}
