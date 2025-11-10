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
	struct GUIShapeStyle
	{
		std::array<double, 23> constants
		{
			20.0,	// FontSize

			20.0,	// ButtonTextHorizontalPadding
			36.0,	// ButtonHeight
			4.6,	// ButtonCornerRadius
			1.0,	// BorderInnerThickness

			1.2,	// InnerShadowThickness

			8.0,	// CheckBoxBoxPaddingLeft
			24.0,	// CheckBoxBoxSize
			8.0,	// CheckBoxBoxPaddingRight
			3.2,	// CheckBoxBoxRadius
			8.0,	// CheckBoxLabelPaddingRight
			36.0,	// CheckBoxLabelHeight

			13.0,	// TooltipFontSize
			12.0,	// TooltipTextHorizontalPadding
			6.0,	// TooltipTextVerticalPadding
			5.6,	// TooltipCornerRadius
			10.0,	// TooltipArrowHeight
			24.0,	// TooltipArrowOffset

			10.0,	// ProgressBarHeight
			5.0,	// ProgressBarCornerRadius
			8.0,	// ProgressBarPaddingLeft
			8.0,	// ProgressBarPaddingRight
			13.0,	// ProgressBarVerticalPadding
		};

		[[nodiscard]]
		constexpr double operator[](Theme::Constant constant) const noexcept;
	};
}

# include "detail/GUIShapeStyle.ipp"
