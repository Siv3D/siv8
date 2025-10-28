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

# include <Siv3D/SimpleGUI.hpp>
# include <Siv3D/GUIStyle.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/SimpleGUI/SimpleButton.hpp>

namespace s3d
{
	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, const Theme theme)
		: SimpleButton{ text, pos, Anchor::TopLeft, unspecified, true, theme } {}

	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, const Optional<double>& width, const Theme theme)
		: SimpleButton{ text, pos, Anchor::TopLeft, width, true, theme } {}

	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, const Optional<double>& width, const bool enabled, const Theme theme)
		: SimpleButton{ text, pos, Anchor::TopLeft, width, enabled, theme } {}

	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, const Anchor anchor, const Theme theme)
		: SimpleButton{ text, pos, anchor, unspecified, true, theme } {}

	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		: ISimpleGUIElement{ pos, anchor, enabled, theme }
		, m_drawableText{ SimpleGUI::GetFont()(text) }
		, m_width{ width } {}

	SimpleButton& SimpleButton::setText(const StringView text)
	{
		m_drawableText = SimpleGUI::GetFont()(text);
		setDirty();
		return *this;
	}

	SimpleButton& SimpleButton::setWidth(const Optional<double>& width)
	{
		m_width = width;
		setDirty();
		return *this;
	}

	RectF SimpleButton::region() const
	{
		const GUIStyle& style = getTheme().getStyle();

		if (isDirty())
		{
			const double fontSize = style[Theme::Constant::FontSize];
			m_cache.textWidth = m_drawableText.region(fontSize).w;
			clearDirty();
		}

		const double width = m_width.value_or(m_cache.textWidth + (style[Theme::Constant::HorizontalPadding] * 2));
		return{ getAnchor(), getPos(), width, style[Theme::Constant::ButtonHeight] };
	}

	void SimpleButton::draw() const
	{
		if (not isVisible())
		{
			return;
		}

		const GUIStyle& style = getTheme().getStyle();
		const RectF rect = region();

		// ボタン描画
		{
			const RoundRect roundRect = rect.rounded(style[Theme::Constant::ButtonRadius]);

			// ボタン本体描画
			{
				const ColorF buttonColor = style.getButtonColor(isEnabled(), m_state.hovered, m_state.pressed);
				roundRect.draw(buttonColor);
			}

			// ボタン枠線描画
			{
				const ColorF borderColor = style.getBorderColor(isEnabled(), m_state.hovered, m_state.pressed);
				roundRect.drawFrame(style[Theme::Constant::BorderInnerThickness], 0, borderColor);
			}
		}

		// テキスト描画
		{
			const Font& font = SimpleGUI::GetFont();
			const double fontSize = style[Theme::Constant::FontSize];
			const double scale = (fontSize / SimpleGUI::DefaultFontSize);
			const double fontYOffset = scale;
			const Vec2 textPos{ (rect.x + (rect.w - m_cache.textWidth) / 2.0), (rect.centerY() - font.height() * scale / 2.0 - fontYOffset) };
			{
				const ColorF textColor = style.getTextColor(isEnabled(), m_state.hovered, m_state.pressed);
				m_drawableText.draw(fontSize, textPos, textColor);
			}
		}

		// カーソル変更
		if (m_state.hovered)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	bool SimpleButton::update()
	{
		const RectF rect = region();

		m_state.hovered = ((not Cursor::IsCaptured()) && isVisible() && isEnabled() && rect.mouseOver());
		m_state.pressed = (m_state.hovered && Cursor::OnClientRect() && MouseL.pressed());

		if (m_state.hovered)
		{
			Cursor::SetCapture(true);
		}

		return (m_state.pressed && MouseL.down());
	}

	bool SimpleButton::isHovered() const noexcept
	{
		return m_state.hovered;
	}

	bool SimpleButton::isPressed() const noexcept
	{
		return m_state.pressed;
	}
}
