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
# include "String.hpp"
# include "2DShapes.hpp"

namespace s3d
{
	struct FontFaceProperties
	{
		String familyName;
		
		String styleName;

		String postscriptName;

		Array<int32> availableBitmapSizes;
		
		uint32 numGlyphs = 0;

		uint16 unitsPerEM = 0;

		int16 ascender = 0;

		int16 descender = 0;

		int16 height = 0;

		Rect bbox{ 0, 0, 0, 0 };

		bool hasColor = false;

		bool isScalable = false;

		bool isVariable = false;

		bool isFixedPitch = false;

		bool hasHorizontal = false;

		bool hasVertical = false;

		bool hasKerning = false;
	};
}
