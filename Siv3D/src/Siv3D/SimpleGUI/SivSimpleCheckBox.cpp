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
# include <Siv3D/SimpleGUI/SimpleCheckBox.hpp>

namespace s3d
{
	namespace
	{
		static void DrawCheck(double boxSize, const Vec2& center, const ColorF& indicatorColor)
		{
			const double scale = (boxSize / 24.0);
			Quad{ -9.5, 1.5, -6, -1, -2.5, 2.5, -2.5, 8.5 }.scaledFromOrigin(scale).movedBy(center).draw(indicatorColor);
			Quad{ -2.5, 8.5, -2.5, 2.5, 5.75, -8.25, 8.5, -5.5 }.scaledFromOrigin(scale).movedBy(center).draw(indicatorColor);
		}
	}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, Anchor::TopLeft, unspecified, true, theme } {}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Optional<double>& width, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, Anchor::TopLeft, width, true, theme } {}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Optional<double>& width, const bool enabled, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, Anchor::TopLeft, width, enabled,  theme } {}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Anchor anchor, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, anchor, unspecified, true, theme } {}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, anchor, width, true, theme } {}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		: ISimpleGUIElement{ pos, anchor, enabled, theme }
		, m_drawableText{ SimpleGUI::GetFont()(text) }
		, m_width{ width }
		, m_state{ .checked = checked } {}

	SimpleCheckBox& SimpleCheckBox::setChecked(const bool checked)
	{
		m_state.checked = checked;
		return *this;
	}

	SimpleCheckBox& SimpleCheckBox::setText(const StringView text)
	{
		m_drawableText = SimpleGUI::GetFont()(text);
		setDirty();
		return *this;
	}

	SimpleCheckBox& SimpleCheckBox::setWidth(const Optional<double>& width)
	{
		m_width = width;
		setDirty();
		return *this;
	}

	RectF SimpleCheckBox::region() const
	{
		const GUIStyle& style = getTheme().getStyle();

		if (isDirty())
		{
			const double fontSize = style[Theme::Constant::FontSize];
			m_cache.textWidth = m_drawableText.region(fontSize).w;
			clearDirty();
		}

		const double boxWidth = (style[Theme::Constant::CheckBoxPaddingLeft]
			+ style[Theme::Constant::CheckBoxSize] + style[Theme::Constant::CheckBoxPaddingRight]);
		const double textWidth = (m_cache.textWidth + style[Theme::Constant::CheckBoxLabelPaddingRight]);
		const double width = m_width.value_or(boxWidth + textWidth);

		return{ getAnchor(), getPos(), width, style[Theme::Constant::CheckBoxLabelHeight] };
	}

	bool SimpleCheckBox::update()
	{
		const RectF rect = region();
		const MouseState oldMouseState = m_mouseState;
		const bool previousChecked = m_state.checked;

		m_mouseState.hovered = ((not Cursor::IsCaptured()) && isVisible() && isEnabled() && rect.mouseOver());
		m_mouseState.pressed = (m_mouseState.hovered && Cursor::OnClientRect() && MouseL.pressed());
		m_state.checked ^= (m_mouseState.pressed && MouseL.down());

		if (m_mouseState.hovered)
		{
			Cursor::SetCapture(true);
		}

		return (previousChecked != m_state.checked);
	}

	void SimpleCheckBox::draw() const 
	{
		if (not isVisible())
		{
			return;
		}

		const GUIStyle& style = getTheme().getStyle();
		const RectF rect = region();
		const double boxSize = style[Theme::Constant::CheckBoxSize];

		// 背景描画
		{
			rect.draw(style.getBackgroundColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed));
		}

		// ボックス描画
		{
			const Vec2 boxPos{ rect.x + style[Theme::Constant::CheckBoxPaddingLeft], (rect.y + (rect.h - boxSize) / 2.0) };
			const RectF boxRect{ boxPos, boxSize };

			// ボックス描画
			{
				const RoundRect box = boxRect.rounded(style[Theme::Constant::CheckBoxRadius]);

				if (m_state.checked)
				{
					const ColorF fillColor = style.getFillColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
					box.draw(fillColor);
				}
				else
				{
					const ColorF innerShadowColor = style.getInnerShadowColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
					box.draw(innerShadowColor);

					const double innerShadowThickness = style[Theme::Constant::InnerShadowThickness];
					const double innerRadius = (((box.w - innerShadowThickness) / box.w) * box.r);
					const RoundRect innerRect = box.stretched(-innerShadowThickness).withR(innerRadius);
					const ColorF containerColor = style.getContainerColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
					innerRect.draw(containerColor);
				}
			}

			// チェックマーク描画
			if (m_state.checked)
			{
				const ColorF indicatorColor = style.getIndicatorColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
				DrawCheck(boxSize, boxRect.center(), indicatorColor);
			}
		}

		// テキスト描画
		{
			const Font& font = SimpleGUI::GetFont();
			const double fontSize = style[Theme::Constant::FontSize];
			const double scale = (fontSize / SimpleGUI::DefaultFontSize);
			const double fontYOffset = scale;
			const Vec2 textPos{ (rect.x + style[Theme::Constant::CheckBoxPaddingLeft]
					+ boxSize + style[Theme::Constant::CheckBoxPaddingRight]),
				(rect.centerY() - font.height() * scale / 2.0 - fontYOffset) };
			{
				const ColorF textColor = style.getTextColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
				m_drawableText.draw(fontSize, textPos, textColor);
			}
		}

		// カーソル変更
		if (m_mouseState.hovered)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	bool SimpleCheckBox::isChecked() const noexcept
	{
		return m_state.checked;
	}
}
