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
	[[nodiscard]]
	constexpr ColorF GUIStyle::operator[](const Theme::Color color) const noexcept
	{
		return colors[FromEnum(color)];
	}

	[[nodiscard]]
	constexpr double GUIStyle::operator[](const Theme::Constant constant) const noexcept
	{
		return constants[FromEnum(constant)];
	}

	[[nodiscard]]
	constexpr ColorF GUIStyle::getButtonColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getBorderColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getTextColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getBackgroundColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getContainerColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getInnerShadowColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getFillColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getIndicatorColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getTooltipColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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

	[[nodiscard]]
	constexpr ColorF GUIStyle::getTooltipTextColor(const bool enabled, const bool hovered, const bool pressed) const noexcept
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
