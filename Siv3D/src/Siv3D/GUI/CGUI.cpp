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

# include "CGUI.hpp"
# include <Siv3D/SimpleGUI.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CGUI::~CGUI()
	{
		LOG_SCOPED_DEBUG("CGUI::~CGUI()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CGUI::init()
	{
		LOG_SCOPED_DEBUG("CGUI::init()");

		constexpr int32 baseFontSize = static_cast<int32>(SimpleGUI::DefaultFontSize);

		m_defaultFont = std::make_unique<Font>(FontMethod::MSDF, baseFontSize, Typeface::CJK_Regular_JP);

		if (Font::IsAvailable(Typeface::Icon_MaterialDesign))
		{
			m_defaultFont->addFallback(Font{ FontMethod::MSDF, baseFontSize, Typeface::Icon_MaterialDesign});
		}

		if (Font::IsAvailable(Typeface::ColorEmoji))
		{
			m_defaultFont->addFallback(Font{ 24, Typeface::ColorEmoji });
		}

		// Transparent
		m_styles[Theme::Light.themeID].colors =
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
		};

		// Dark
		m_styles[Theme::Dark.themeID].colors =
		{
			Color{ 51 },			// ButtonColor
			Color{ 38 },			// ButtonColorDisabled
			Color{ 64, 77, 89 },	// ButtonColorHover
			Color{ 77, 102, 128 },	// ButtonColorPressed

			Color{ 102 },			// BorderColor
			Color{ 64 },			// BorderColorDisabled
			Color{ 128 },			// BorderColorHover
			Color{ 153 },			// BorderColorPressed

			Color{ 230 },			// TextColor
			Color{ 102 },			// TextColorDisabled
			Color{ 230 },			// TextColorHover
			Color{ 230 },			// TextColorPressed

			Color{ 26 },			// BackgroundColor
			Color{ 26 },			// BackgroundColorDisabled
			Color{ 26 },			// BackgroundColorHover
			Color{ 26 },			// BackgroundColorPressed

			Color{ 38 },			// ContainerColor
			Color{ 51 },			// ContainerColorDisabled
			Color{ 64, 77, 89 },	// ContainerColorHover
			Color{ 64, 77, 89 },	// ContainerColorPressed

			Color{ 77 },			// InnerShadowColor
			Color{ 64 },			// InnerShadowColorDisabled
			Color{ 77 },			// InnerShadowColorHover
			Color{ 77 },			// InnerShadowColorPressed

			Color{ 64, 153, 230 },	// FillColor
			Color{ 51, 89, 128 },	// FillColorDisabled
			Color{ 89, 179, 255 },	// FillColorHover
			Color{ 89, 179, 255 },	// FillColorPressed

			Color{ 26 },			// IndicatorColor
			Color{ 26 },			// IndicatorColorDisabled
			Color{ 26 },			// IndicatorColorHover
			Color{ 26 },			// IndicatorColorPressed

			Color{ 230 },			// TooltipColor
			Color{ 153 },			// TooltipColorDisabled
			Color{ 204 },			// TooltipColorHover
			Color{ 191 },			// TooltipColorPressed

			Color{ 26 },			// TooltipTextColor
			Color{ 77 },			// TooltipTextColorDisabled
			Color{ 26 },			// TooltipTextColorHover
			Color{ 26 },			// TooltipTextColorPressed
		};

		// Transparent
		m_styles[Theme::Transparent.themeID].colors =
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

			Color{ 0, 0 },			// BackgroundColor (Transparent)
			Color{ 0, 0 },			// BackgroundColorDisabled (Transparent)
			Color{ 0, 0 },			// BackgroundColorHover (Transparent)
			Color{ 0, 0 },			// BackgroundColorPressed (Transparent)

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
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDefaultFont
	//
	////////////////////////////////////////////////////////////////

	const Font& CGUI::getDefaultFont() const noexcept
	{
		return *m_defaultFont;
	}


	const GUIStyle& CGUI::getStyle(const Theme& theme) const noexcept
	{
		return m_styles[theme.getID()];
	}
}
