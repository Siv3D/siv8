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
# include "Common.hpp"

namespace s3d
{
	struct GUIStyle;

	class Theme
	{
	public:

		struct Predefined
		{
			uint8 themeID = 0;
		};

		static constexpr Predefined Light{ 0 };
		static constexpr Predefined Dark{ 1 };
		static constexpr Predefined Warm{ 2 };
		static constexpr Predefined Ocean{ 3 };
		static constexpr Predefined Forest{ 4 };
		static constexpr Predefined Sunset{ 5 };
		static constexpr Predefined Lavender{ 6 };
		static constexpr Predefined LightCompact{ 7 };
		static constexpr Predefined LightLarge{ 8 };

		[[nodiscard]]
		Theme() = default;

		[[nodiscard]]
		constexpr Theme(Predefined predefined) noexcept;

		enum class Color
		{
			ButtonColor,
			ButtonColorDisabled,
			ButtonColorHover,
			ButtonColorPressed,
			
			BorderColor,
			BorderColorDisabled,
			BorderColorHover,
			BorderColorPressed,

			TextColor,
			TextColorDisabled,
			TextColorHover,
			TextColorPressed,

			BackgroundColor,
			BackgroundColorDisabled,
			BackgroundColorHover,
			BackgroundColorPressed,

			ContainerColor,
			ContainerColorDisabled,
			ContainerColorHover,
			ContainerColorPressed,

			InnerShadowColor,
			InnerShadowColorDisabled,
			InnerShadowColorHover,
			InnerShadowColorPressed,

			FillColor,
			FillColorDisabled,
			FillColorHover,
			FillColorPressed,

			IndicatorColor,
			IndicatorColorDisabled,
			IndicatorColorHover,
			IndicatorColorPressed,

			TooltipBackgroundColor,
			TooltipTextColor,
		};

		enum class Constant
		{
			FontSize,

			ButtonTextHorizontalPadding,
			ButtonHeight,
			ButtonCornerRadius,
			BorderInnerThickness,

			InnerShadowThickness,

			CheckBoxPaddingLeft,
			CheckBoxSize,
			CheckBoxPaddingRight,
			CheckBoxRadius,
			CheckBoxLabelPaddingRight,
			CheckBoxLabelHeight,

			TooltipFontSize,
			TooltipTextHorizontalPadding,
			TooltipTextVerticalPadding,
			TooltipCornerRadius,
		};

		[[nodiscard]]
		constexpr uint8 getID() const noexcept;

		[[nodiscard]]
		const GUIStyle& getStyle() const noexcept;

	private:

		uint8 m_themeID = 0;
	};
}

# include "detail/Theme.ipp"
