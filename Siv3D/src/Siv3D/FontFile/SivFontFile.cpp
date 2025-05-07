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

# include <Siv3D/FontFile.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace FontFile
	{
		Array<FontFaceProperties> GetFontFaces(const FilePathView path)
		{
			return SIV3D_ENGINE(Font)->getFontFaces(path);
		}
	}
}
