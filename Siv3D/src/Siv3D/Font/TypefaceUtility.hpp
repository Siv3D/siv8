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
# include <Siv3D/String.hpp>
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/Typeface.hpp>

namespace s3d
{
	struct TypefaceInfo
	{
		FilePath path;

		uint32 faceIndex = 0;

		String styleName;

		FontMethod preferredMethod = FontMethod::Bitmap;
	};

	[[nodiscard]]
	TypefaceInfo GetTypefaceInfo(Typeface typeface, FontMethod fontMethod);
}
