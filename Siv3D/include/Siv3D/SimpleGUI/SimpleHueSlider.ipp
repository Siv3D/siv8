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

namespace s3d
{
	SimpleHueSlider::SimpleHueSlider(double hue, const Vec2& pos, const Concept::ExactlyBool auto param, const Theme theme)
		: SimpleHueSlider{ hue, pos, DefaultWidth, param, theme } {}

	SimpleHueSlider::SimpleHueSlider(double hue, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto param, const Theme theme)
		: SimpleHueSlider{ hue, pos, anchor, DefaultWidth, param, theme } {}
}
