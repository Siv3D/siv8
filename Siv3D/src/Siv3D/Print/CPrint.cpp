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
# include <Siv3D/ScopedRenderStates2D.hpp>
# include <Siv3D/ScopedCustomShader2D.hpp>
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		constexpr size_t CalculateMaxLines(const int32 sceneHeight, const int32 fontHeight) noexcept
		{
			const int32 regionHeight = Max((sceneHeight - (CPrint::TextPadding * 2)), 1);
			return ((regionHeight + fontHeight - 1) / fontHeight);
		}

		[[nodiscard]]
		constexpr int32 CalculateMaxWidth(const int32 sceneWidth) noexcept
		{
			return Max((sceneWidth - (CPrint::TextPadding * 2)), 40);
		}

		[[nodiscard]]
		static size_t CountLines(const DrawableText& drawableText, const int32 maxWidth)
		{
			const Array<double> xAdvances = drawableText.getXAdvances();
			size_t result = 1;
			double xPos = 0.0;

			for (double xAdvance : xAdvances)
			{
				if (maxWidth < (xPos + xAdvance))
				{
					++result;
					xPos = 0.0;
				}

				xPos += xAdvance;
			}

			return result;
		}
	}

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
		if (Array<String> lines = s.split(U'\n'))
		{
			std::lock_guard lock{ m_mutex };

			if (m_newLine)
			{
				m_lines.push_back(std::move(lines[0]));
				m_newLine = false;
			}
			else
			{
				m_lines.back().append(lines[0]);
			}

			for (size_t i = 1; i < lines.size(); ++i)
			{
				m_lines.push_back(std::move(lines[i]));
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeln
	//
	////////////////////////////////////////////////////////////////

	void CPrint::writeln(const StringView s)
	{
		if (Array<String> lines = s.split(U'\n'))
		{
			std::lock_guard lock{ m_mutex };

			if (m_newLine)
			{
				m_lines.push_back(std::move(lines[0]));
				m_newLine = false;
			}
			else
			{
				m_lines.back().append(lines[0]);
			}

			for (size_t i = 1; i < lines.size(); ++i)
			{
				m_lines.push_back(std::move(lines[i]));
			}

			m_newLine = true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CPrint::clear()
	{
		std::lock_guard lock{ m_mutex };

		m_lines.clear();
		m_newLine = true;
		m_overflowLevel = 0;
		m_overflowAnimation = 0.0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	void CPrint::draw()
	{
		std::lock_guard lock{ m_mutex };

		// Print する内容が無い場合は何もしない
		if (m_lines.isEmpty())
		{
			return;
		}

		const Font& font = m_font->textFont;
		const double fontHeight = font.height();
		const size_t maxLineCount = CalculateMaxLines(Scene::Height(), static_cast<int32>(fontHeight));

		// 超過メッセージの削除
		if (maxLineCount < m_lines.size())
		{
			m_lines.pop_front_N(m_lines.size() - maxLineCount);
			m_overflowLevel = 1;
			m_overflowAnimation = 1.0;
		}

		// 各メッセージの行数計算
		const int32 maxWidth = CalculateMaxWidth(Scene::Width());
		Array<DrawableText> drawableTexts;
		Array<size_t> lineCounts;
		{
			size_t totalLineCount = 0;

			for (auto it = m_lines.rbegin(); it != m_lines.rend(); ++it)
			{
				DrawableText drawableText = m_font->textFont(*it);
				const size_t lineCount = CountLines(drawableText, maxWidth);

				if (maxLineCount == (totalLineCount + lineCount + 1))
				{
					m_overflowLevel = 1;
				}
				else if (maxLineCount == (totalLineCount + lineCount))
				{
					m_overflowLevel = 2;
				}
				if (maxLineCount < (totalLineCount + lineCount))
				{
					// 超過するメッセージの削除
					m_lines.pop_front_N(m_lines.size() - drawableTexts.size());
					m_overflowLevel = 1;
					m_overflowAnimation = 1.0;
					break;
				}
				
				drawableTexts.push_front(std::move(drawableText));
				lineCounts.push_front(lineCount);

				totalLineCount += lineCount;
			}
		}

		// 描画
		{
			m_overflowAnimation = Math::MoveTowards(m_overflowAnimation, m_overflowLevel, (Scene::DeltaTime() * 15));
			const double overflowOffsetY = (m_overflowAnimation ? (m_overflowAnimation * fontHeight) : 0);
			const Vec2 basePos{ TextStartPos.x, (TextStartPos.y - overflowOffsetY) };
		
			// レンダーステートはデフォルトに戻す
			const ScopedRenderStates2D rb{ BlendState::Default2D, RasterizerState::Default2D, SamplerState::Default2D };
			const ScopedCustomShader2D shader{ m_font->ps };

			size_t lineOffset = 0;

			for (size_t i = 0; i < m_lines.size(); ++i)
			{
				const size_t lineCount = lineCounts[i];
				const DrawableText& drawableText = drawableTexts[i];

				if (drawableText.text)
				{
					const Vec2 pos{ basePos.x, (basePos.y + (lineOffset * fontHeight)) };
					const RectF area{ pos, maxWidth, 65536 };
					drawableText.draw(TextStyle::CustomTextFontShader(), area);
				}

				lineOffset += lineCount;
			}
		}
	}
}
