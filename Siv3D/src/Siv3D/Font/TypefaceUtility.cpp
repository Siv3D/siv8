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
				if (const FilePath pathCJK = (fontFilePath + U"noto-cjk/NotoSansCJK-Medium.ttc"); FileSystem::Exists(pathCJK))
				{
					const uint32 index = (FromEnum(typeface) - FromEnum(Typeface::CJK_Regular_JP));
					return{ pathCJK, index, U"", fontMethod };
				}

				if (const FilePath pathJP = (fontFilePath + U"noto-cjk/NotoSansJP-Medium.otf"); FileSystem::Exists(pathJP))
				{
					return{ pathJP, 0, U"", fontMethod };
				}

				if (const FilePath pathMin = (fontFilePath + U"min/siv3d-min.otf"); FileSystem::Exists(pathMin))
				{
					return{ pathMin, 0, U"", fontMethod };
				}

				break;
			}
		case Typeface::ColorEmoji:
			return{ (fontFilePath + U"noto-emoji/Noto-COLRv1.ttf"), 0, U"", FontMethod::Bitmap };
		case Typeface::Mplus_Thin:
			return{ (fontFilePath + U"mplus/mplus-1p-thin.ttf"), 0,  U"", fontMethod };
		case Typeface::Mplus_Light:
			return{ (fontFilePath + U"mplus/mplus-1p-light.ttf"), 0, U"", fontMethod };
		case Typeface::Mplus_Regular:
			return{ (fontFilePath + U"mplus/mplus-1p-regular.ttf"), 0, U"", fontMethod };
		case Typeface::Mplus_Medium:
			return{ (fontFilePath + U"mplus/mplus-1p-medium.ttf"), 0, U"", fontMethod };
		case Typeface::Mplus_Bold:
			return{ (fontFilePath + U"mplus/mplus-1p-bold.ttf"), 0, U"", fontMethod };
		case Typeface::Mplus_Heavy:
			return{ (fontFilePath + U"mplus/mplus-1p-heavy.ttf"), 0, U"", fontMethod };
		case Typeface::Mplus_Black:
			return{ (fontFilePath + U"mplus/mplus-1p-black.ttf"), 0, U"", fontMethod };
		case Typeface::Icon_MaterialDesign:
			return{ (fontFilePath + U"materialdesignicons/materialdesignicons-webfont.ttf"), 0, U"", fontMethod };
		}

		return{ U"", 0, U"", FontMethod::Bitmap };
	}
}
