//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "NamedParameter.hpp"

namespace s3d::Arg
{
	SIV3D_NAMED_PARAMETER(generator);			// Array, Image
	SIV3D_NAMED_PARAMETER(indexedGenerator);	// Array
	SIV3D_NAMED_PARAMETER(reserve);				// Array

	SIV3D_NAMED_PARAMETER(center);				// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(topLeft);				// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(topRight);			// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(bottomLeft);			// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(bottomRight);			// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(topCenter);			// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(bottomCenter);		// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(leftCenter);			// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(rightCenter);			// Rect, RectF, Circle
	SIV3D_NAMED_PARAMETER(top);					// Rect, RectF
	SIV3D_NAMED_PARAMETER(bottom);				// Rect, RectF
	SIV3D_NAMED_PARAMETER(left);				// Rect, RectF
	SIV3D_NAMED_PARAMETER(right);				// Rect, RectF
}
