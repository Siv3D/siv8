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

namespace s3d
{
	struct FontVariationAxis
	{
		String name;

		String tag;

		double minValue = 0.0;

		double defaultValue = 0.0;

		double maxValue = 0.0;

		double value = 0.0;
	};
}
