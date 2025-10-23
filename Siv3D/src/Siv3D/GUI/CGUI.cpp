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

		m_defaultFont = std::make_unique<Font>(FontMethod::MSDF, 20, Typeface::CJK_Regular_JP);

		if (Font::IsAvailable(Typeface::Icon_MaterialDesign))
		{
			m_defaultFont->addFallback(Font{ FontMethod::MSDF, 20, Typeface::Icon_MaterialDesign });
		}

		if (Font::IsAvailable(Typeface::ColorEmoji))
		{
			m_defaultFont->addFallback(Font{ 24, Typeface::ColorEmoji });
		}
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
}
