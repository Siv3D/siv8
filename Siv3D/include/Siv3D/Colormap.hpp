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
# include "ColorHSV.hpp"
# include "ColorScale.hpp"

namespace s3d
{
	[[nodiscard]]
	ColorF Colormap(double t, ColorScale scale = ColorScale::Turbo);

	[[nodiscard]]
	ColorF Colormap(double t, double min, double max, ColorScale scale = ColorScale::Turbo);
}
