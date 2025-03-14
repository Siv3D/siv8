﻿//-----------------------------------------------
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
# include "NamedParameter.hpp"

namespace s3d::Arg
{
	SIV3D_NAMED_PARAMETER(generator);			// Array, Image, Grid
	SIV3D_NAMED_PARAMETER(generator0_1);		// Image
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
	SIV3D_NAMED_PARAMETER(start);				// Circle
	SIV3D_NAMED_PARAMETER(end);					// Circle

	SIV3D_NAMED_PARAMETER(r);					// Circular
	SIV3D_NAMED_PARAMETER(theta);				// Circular

	SIV3D_NAMED_PARAMETER(angle);				// Line
	SIV3D_NAMED_PARAMETER(direction);			// Line
}
