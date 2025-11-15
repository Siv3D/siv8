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
# include <bit>
# include "Common.hpp"

namespace s3d
{
	struct GUIColorStyle;
	struct GUIShapeStyle;

	class Theme
	{
	public:

		struct Predefined
		{
			uint8 colorID : 5 = 0;
			uint8 shapeID : 3 = 0;
		};

		static constexpr Predefined Light{ 0, 0 };
		static constexpr Predefined Dark{ 1, 0 };
		static constexpr Predefined Transparent{ 2, 0 };
		static constexpr Predefined Warm{ 3, 0 };
		static constexpr Predefined Ocean{ 4, 0 };
		static constexpr Predefined Forest{ 5, 0 };
		static constexpr Predefined Sunset{ 6, 0 };
		static constexpr Predefined Lavender{ 7, 0 };

		static constexpr Predefined LightCompact{ 0, 1 };
		static constexpr Predefined DarkCompact{ 1, 1 };
		static constexpr Predefined TransparentCompact{ 2, 1 };
		static constexpr Predefined WarmCompact{ 3, 1 };
		static constexpr Predefined OceanCompact{ 4, 1 };
		static constexpr Predefined ForestCompact{ 5, 1 };
		static constexpr Predefined SunsetCompact{ 6, 1 };
		static constexpr Predefined LavenderCompact{ 7, 1 };

		static constexpr Predefined LightLarge{ 0, 2 };
		static constexpr Predefined DarkLarge{ 1, 2 };
		static constexpr Predefined TransparentLarge{ 2, 2 };
		static constexpr Predefined WarmLarge{ 3, 2 };
		static constexpr Predefined OceanLarge{ 4, 2 };
		static constexpr Predefined ForestLarge{ 5, 2 };
		static constexpr Predefined SunsetLarge{ 6, 2 };
		static constexpr Predefined LavenderLarge{ 7, 2 };

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

			TooltipColor,
			TooltipColorDisabled,
			TooltipColorHover,
			TooltipColorPressed,

			TooltipTextColor,
			TooltipTextColorDisabled,
			TooltipTextColorHover,
			TooltipTextColorPressed,

			ColorSliderOverlayDisabled,
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
			TooltipArrowHeight,
			TooltipArrowOffset,

			ProgressBarHeight,
			ProgressBarCornerRadius,
			ProgressBarPaddingLeft,
			ProgressBarPaddingRight,
			ProgressBarVerticalPadding,

			ColorSliderBarHeight,
			ColorSliderBarCornerRadius,
			ColorSliderBarPaddingLeft,
			ColorSliderBarPaddingRight,
			ColorSliderBarVerticalPadding,
			ColorSliderKnobRadius,
			ColorSliderKnobFrameThickness,
		};

		[[nodiscard]]
		constexpr uint8 getColorID() const noexcept;

		[[nodiscard]]
		constexpr uint8 getShapeID() const noexcept;

		[[nodiscard]]
		const GUIColorStyle& getColorStyle() const noexcept;

		[[nodiscard]]
		const GUIShapeStyle& getShapeStyle() const noexcept;

	private:

		uint8 m_colorID : 5 = 0;

		uint8 m_shapeID : 3 = 0;
	};
}

# include "detail/Theme.ipp"
