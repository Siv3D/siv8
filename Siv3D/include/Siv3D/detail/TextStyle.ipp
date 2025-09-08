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
	constexpr Float4 TextStyle::getShaderParams(const int32 fontBaseSize) const noexcept
	{
		static constexpr float BaseFontSize = 64.0f;
		const float scale = (fontBaseSize / BaseFontSize);
		
		if (type == Type::Glow)
		{
			return{ (1.0f / (glow * scale)), 0.0f, 0.0f, 0.0f };
		}
		else
		{
			return{ (0.5 + scale * innerThickness), (0.5 - scale * outerThickness), (shadowOffset * scale) };
		}
	}

	constexpr TextStyle TextStyle::Default() noexcept
	{
		return{};
	}

	constexpr TextStyle TextStyle::CustomTextFontShader() noexcept
	{
		TextStyle style;
		style.type = Type::CustomTextFontShader;
		return style;
	}

	constexpr TextStyle TextStyle::CustomColorFontShader() noexcept
	{
		TextStyle style;
		style.type = Type::CustomColorFontShader;
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
		style.innerThickness = static_cast<float>(inner);
		style.outerThickness = static_cast<float>(outer);
		style.outlineColor = ColorF::PremultiplyAlpha(color).toFloat4();
		return style;
	}

	constexpr TextStyle TextStyle::Shadow(const Vec2& offset, const ColorF& color) noexcept
	{
		TextStyle style;
		style.type = Type::Shadow;
		style.shadowOffset = offset;
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
		style.innerThickness = static_cast<float>(inner);
		style.outerThickness = static_cast<float>(outer);
		style.shadowOffset = offset;
		style.outlineColor = ColorF::PremultiplyAlpha(outlineColor).toFloat4();
		style.shadowColor = ColorF::PremultiplyAlpha(shadowColor).toFloat4();
		return style;
	}

	constexpr TextStyle TextStyle::OutlineShadow(const double p, const Vec2& offset, const ColorF& outlineColor, const ColorF& shadowColor) noexcept
	{
		return OutlineShadow((p * 0.5), (p * 0.5), outlineColor, offset, shadowColor);
	}

	constexpr TextStyle TextStyle::OutlineShadow(const double inner, const double outer, const Vec2& offset, const ColorF& outlineColor, const ColorF& shadowColor) noexcept
	{
		return OutlineShadow(inner, outer, outlineColor, offset, shadowColor);
	}

	constexpr TextStyle TextStyle::Glow(const double p) noexcept
	{
		if (p == 0.0)
		{
			return Default();
		}

		TextStyle style;
		style.type = Type::Glow;
		style.glow = static_cast<float>(p);
		return style;
	}
}
