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
			return colors[FromEnum(Theme::Color::DisabledButtonColor)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::PressedButtonColor)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::HoverButtonColor)];
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
			return colors[FromEnum(Theme::Color::DisabledBorderColor)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::PressedBorderColor)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::HoverBorderColor)];
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
			return colors[FromEnum(Theme::Color::TextDisabledColor)];
		}
		else if (pressed)
		{
			return colors[FromEnum(Theme::Color::TextPressedColor)];
		}
		else if (hovered)
		{
			return colors[FromEnum(Theme::Color::TextHoverColor)];
		}
		else
		{
			return colors[FromEnum(Theme::Color::TextColor)];
		}
	}
}
