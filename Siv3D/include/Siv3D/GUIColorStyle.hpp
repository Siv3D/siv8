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
	struct GUIColorStyle
	{
		std::array<Color, 41> colors
		{
			Color{ 255 },			// ButtonColor
			Color{ 235 },			// ButtonColorDisabled
			Color{ 230, 242, 255 },	// ButtonColorHover
			Color{ 204, 230, 255 },	// ButtonColorPressed

			Color{ 171 },			// BorderColor
			Color{ 204 },			// BorderColorDisabled
			Color{ 128 },			// BorderColorHover
			Color{ 77 },			// BorderColorPressed

			Color{ 26 },			// TextColor
			Color{ 153 },			// TextColorDisabled
			Color{ 26 },			// TextColorHover
			Color{ 26 },			// TextColorPressed

			Color{ 255 },			// BackgroundColor
			Color{ 255 },			// BackgroundColorDisabled
			Color{ 255 },			// BackgroundColorHover
			Color{ 255 },			// BackgroundColorPressed

			Color{ 242 },			// ContainerColor
			Color{ 204 },			// ContainerColorDisabled
			Color{ 230, 242, 255 },	// ContainerColorHover
			Color{ 230, 242, 255 },	// ContainerColorPressed

			Color{ 171 },			// InnerShadowColor
			Color{ 191 },			// InnerShadowColorDisabled
			Color{ 171 },			// InnerShadowColorHover
			Color{ 171 },			// InnerShadowColorPressed

			Color{ 89, 179, 255 },	// FillColor
			Color{ 191, 217, 255 },	// FillColorDisabled
			Color{ 115, 204, 255 },	// FillColorHover
			Color{ 115, 204, 255 },	// FillColorPressed

			Color{ 255 },			// IndicatorColor
			Color{ 255 },			// IndicatorColorDisabled
			Color{ 255 },			// IndicatorColorHover
			Color{ 255 },			// IndicatorColorPressed

			Color{ 15 },			// TooltipColor
			Color{ 80 },			// TooltipColorDisabled
			Color{ 32 },			// TooltipColorHover
			Color{ 40 },			// TooltipColorPressed

			Color{ 255 },			// TooltipTextColor
			Color{ 171 },			// TooltipTextColorDisabled
			Color{ 255 },			// TooltipTextColorHover
			Color{ 255 },			// TooltipTextColorPressed

			Color{ 224, 179 },		// ColorSliderOverlayDisabled
		};

		[[nodiscard]]
		constexpr ColorF operator[](Theme::Color color) const noexcept;

		[[nodiscard]]
		constexpr ColorF getButtonColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getBorderColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getTextColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getBackgroundColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getContainerColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getInnerShadowColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getFillColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]]
		constexpr ColorF getIndicatorColor(bool enabled, bool hovered, bool pressed) const noexcept;

		[[nodiscard]] 
		constexpr ColorF getTooltipColor(bool enabled, bool hovered, bool pressed) const noexcept; 

		[[nodiscard]] 
		constexpr ColorF getTooltipTextColor(bool enabled, bool hovered, bool pressed) const noexcept;
	};
}

# include "detail/GUIColorStyle.ipp"
