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
# include <array>
# include "ColorHSV.hpp"
# include "Theme.hpp"

namespace s3d
{
	struct GUIStyle
	{
		std::array<Color, 12> colors
		{
			Color{ 255 },			// ButtonColor
			Color{ 235 },			// DisabledButtonColor
			Color{ 230, 242, 255 },	// HoverButtonColor
			Color{ 204, 230, 255 },	// PressedButtonColor

			Color{ 171 },			// BorderColor
			Color{ 204 },			// DisabledBorderColor
			Color{ 128 },			// HoverBorderColor
			Color{ 77 },			// PressedBorderColor

			Color{ 26 },			// TextColor
			Color{ 153 },			// TextDisabledColor
			Color{ 15 },			// TextHoverColor
			Color{ 5 },				// TextPressedColor
		};

		std::array<double, 5> constants
		{
			20.0,	// FontSize
			20.0,	// HorizontalPadding
			36.0,	// ButtonHeight
			4.6,	// ButtonRadius
			1.0,	// BorderInnerThickness
		};

		[[nodiscard]]
		constexpr ColorF operator[](Theme::Color color) const noexcept;

		[[nodiscard]]
		constexpr double operator[](Theme::Constant constant) const noexcept;

		[[nodiscard]]
		constexpr ColorF getButtonColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getBorderColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getTextColor(bool enabled, bool hovered, bool pressed) const noexcept;
	};
}

# include "detail/GUIStyle.ipp"

