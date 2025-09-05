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

# include "CPrint.hpp"
# include <Siv3D/DrawableText.hpp>
# include <Siv3D/Format.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/ScopedCustomShader2D.hpp>
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CPrint::CPrint() {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CPrint::~CPrint()
	{
		LOG_SCOPED_DEBUG("CPrint::~CPrint()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CPrint::init()
	{
		LOG_SCOPED_DEBUG("CPrint::init()");

		m_font = std::make_unique<PrintFont>();
		m_font->textFont = Font{ FontMethod::MSDF, 18, Typeface::CJK_Regular_JP };

		if (Font::IsAvailable(Typeface::ColorEmoji))
		{
			m_font->textFont.addFallback(Font{ 18, Typeface::ColorEmoji });
		}

		m_font->ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FontPrint);
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	void CPrint::write(const StringView s)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	writeln
	//
	////////////////////////////////////////////////////////////////

	void CPrint::writeln(const StringView s)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CPrint::clear()
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	void CPrint::draw()
	{
		//{
		//	const ScopedCustomShader2D shader{ m_font->ps };
		//	
		//	m_font->textFont(U"Print テスト 12,345 漢字 🚀🐢 Print テスト 12,345 漢字 🚀🐢 Print テスト 12,345 漢字 🚀🐢").draw(TextStyle::CustomTextFontShader(), Vec2{ 10, 10 });
		//}
	}
}
