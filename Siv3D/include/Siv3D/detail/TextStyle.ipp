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
	constexpr TextStyle TextStyle::Default() noexcept
	{
		return{};
	}

	constexpr TextStyle TextStyle::CustomShader() noexcept
	{
		TextStyle style;
		style.type = Type::CustomShader;
		return style;
	}

	constexpr TextStyle TextStyle::Outline(const double p, const ColorF& color) noexcept
	{
		return Outline((p * 0.5), (p * 0.5), color);
	}

	constexpr TextStyle TextStyle::Outline(const double inner, const double outer, const ColorF& color) noexcept
	{
		TextStyle style;
		style.type = Type::Outline;
		style.param.x = static_cast<float>(0.5 + inner);
		style.param.y = static_cast<float>(0.5 - outer);
		style.outlineColor = ColorF::PremultiplyAlpha(color).toFloat4();
		return style;
	}

	constexpr TextStyle TextStyle::Shadow(const Vec2& offset, const ColorF& color) noexcept
	{
		TextStyle style;
		style.type = Type::Shadow;
		style.param.z = static_cast<float>(offset.x);
		style.param.w = static_cast<float>(offset.y);
		style.shadowColor = ColorF::PremultiplyAlpha(color).toFloat4();
		return style;
	}

	constexpr TextStyle TextStyle::OutlineShadow(const double p, const ColorF& outlineColor, const Vec2& offset, const ColorF& shadowColor) noexcept
	{
		return OutlineShadow((p * 0.5), (p * 0.5), outlineColor, offset, shadowColor);
	}

	constexpr TextStyle TextStyle::OutlineShadow(const double inner, const double outer, const ColorF& outlineColor, const Vec2& offset, const ColorF& shadowColor) noexcept
	{
		TextStyle style;
		style.type = Type::OutlineShadow;
		style.param.x = static_cast<float>(0.5 + inner);
		style.param.y = static_cast<float>(0.5 - outer);
		style.param.z = static_cast<float>(offset.x);
		style.param.w = static_cast<float>(offset.y);
		style.outlineColor = ColorF::PremultiplyAlpha(outlineColor).toFloat4();
		style.shadowColor = ColorF::PremultiplyAlpha(shadowColor).toFloat4();
		return style;
	}

	constexpr TextStyle TextStyle::Glow(const double p) noexcept
	{
		if (p == 0.0)
		{
			return Default();
		}

		TextStyle style;
		style.type = Type::Glow;
		style.param.x = static_cast<float>(1.0 / p);
		return style;
	}
}
