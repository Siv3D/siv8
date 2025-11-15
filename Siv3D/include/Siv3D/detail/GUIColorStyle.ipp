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
	constexpr ColorF GUIColorStyle::operator[](const Theme::Color color) const noexcept
	{
		return colors[FromEnum(color)];
	}

	constexpr ColorF GUIColorStyle::getButtonColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::ButtonColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::ButtonColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::ButtonColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::ButtonColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getBorderColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::BorderColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::BorderColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::BorderColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::BorderColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getTextColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::TextColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::TextColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::TextColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::TextColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getBackgroundColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::BackgroundColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::BackgroundColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::BackgroundColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::BackgroundColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getContainerColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::ContainerColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::ContainerColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::ContainerColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::ContainerColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getInnerShadowColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::InnerShadowColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::InnerShadowColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::InnerShadowColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::InnerShadowColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getFillColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::FillColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::FillColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::FillColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::FillColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getIndicatorColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::IndicatorColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::IndicatorColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::IndicatorColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::IndicatorColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getTooltipColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::TooltipColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::TooltipColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::TooltipColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::TooltipColor)];
		}
	}

	constexpr ColorF GUIColorStyle::getTooltipTextColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
	{
		if (not enabled)
		{
			return colors[FromEnum(Theme::Color::TooltipTextColorDisabled)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::TooltipTextColorPressed)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::TooltipTextColorHover)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::TooltipTextColor)];
		}
	}
}
