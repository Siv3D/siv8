//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/SimpleGUI.hpp>
# include <Siv3D/GUIColorStyle.hpp>
# include <Siv3D/GUIShapeStyle.hpp>
# include <Siv3D/SimpleGUI/SimpleHueSlider.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/TexturedQuad.hpp>
# include <Siv3D/TexturedRoundRect.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	SimpleHueSlider::SimpleHueSlider(const double hue, const Vec2& pos, const Theme theme)
		: SimpleHueSlider{ hue, pos, Anchor::TopLeft, DefaultWidth, theme } {}

	SimpleHueSlider::SimpleHueSlider(const double hue, const Vec2& pos, const double width, const Theme theme)
		: SimpleHueSlider{ hue, pos, Anchor::TopLeft, width,  theme } {}

	SimpleHueSlider::SimpleHueSlider(const double hue, const Vec2& pos, const double width, const bool enabled, const Theme theme)
		: SimpleHueSlider{ hue, pos, Anchor::TopLeft, width, enabled, theme } {}

	SimpleHueSlider::SimpleHueSlider(const double hue, const Vec2& pos, const Anchor anchor, const Theme theme)
		: SimpleHueSlider{ hue, pos, anchor, DefaultWidth, theme } {}

	SimpleHueSlider::SimpleHueSlider(const double hue, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		: SimpleHueSlider{ hue, pos, anchor, width, true, theme } {}

	SimpleHueSlider::SimpleHueSlider(const double hue, const Vec2& pos, const Anchor anchor, const double width, const bool enabled, const Theme theme)
		: ISimpleGUIElement{ pos, anchor, enabled, theme }
		, m_width{ width }
		, m_state{ .hue = Clamp(hue, 0.0, 360.0) } {}

	////////////////////////////////////////////////////////////////
	//
	//	setWidth, getWidth
	//
	////////////////////////////////////////////////////////////////

	SimpleHueSlider& SimpleHueSlider::setWidth(const double width)
	{
		if (const auto oldWidth = std::exchange(m_width, width);
			oldWidth != m_width)
		{
			setDirty();
		}

		return *this;
	}

	double SimpleHueSlider::getWidth() const
	{
		return m_width;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setHue, getHue
	//
	////////////////////////////////////////////////////////////////

	SimpleHueSlider& SimpleHueSlider::setHue(const double hue)
	{
		m_state.hue = Clamp(hue, 0.0, 360.0);
		return *this;
	}

	double SimpleHueSlider::getHue() const
	{
		return m_state.hue;
	}

	////////////////////////////////////////////////////////////////
	//
	//	region
	//
	////////////////////////////////////////////////////////////////

	RectF SimpleHueSlider::region() const
	{
		const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
		const double height = (shapeStyle[Theme::Constant::ColorSliderBarHeight] + shapeStyle[Theme::Constant::ColorSliderBarVerticalPadding] * 2.0);
		return{ getAnchor(), getPos(), m_width, height };
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateBase
	//
	////////////////////////////////////////////////////////////////

	bool SimpleHueSlider::updateBase()
	{
		const RectF rect = region();
		const MouseState oldMouseState = m_mouseState;
		const double previousHue = m_state.hue;

		m_mouseState.hovered = ((not Cursor::IsCaptured()) && isVisible() && isEnabled() && rect.mouseOver());
		m_mouseState.pressed = (m_mouseState.hovered && Cursor::OnClientRect() && MouseL.pressed());
		
		if (m_mouseState.pressed)
		{
			const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
			const double barWidth = Max(1.0, (m_width - shapeStyle[Theme::Constant::ColorSliderBarPaddingLeft] - shapeStyle[Theme::Constant::ColorSliderBarPaddingRight]));
			const double barX = (rect.x + shapeStyle[Theme::Constant::ColorSliderBarPaddingLeft]);
			const double mouseX = Clamp(Cursor::PosF().x, barX, (barX + barWidth));
			m_state.hue = (((mouseX - barX) / barWidth) * 360.0);
		}

		updateMouseEvent(oldMouseState, m_mouseState);

		return (previousHue != m_state.hue);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawBase
	//
	////////////////////////////////////////////////////////////////

	void SimpleHueSlider::drawBase() const
	{
		if (not isVisible())
		{
			return;
		}

		const GUIColorStyle& colorStyle = getTheme().getColorStyle();
		const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
		const RectF backgroundRect = region();
		const ColorF backgroundColor = colorStyle.getBackgroundColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);

		// 背景描画
		if (0.0 < backgroundColor.a)
		{
			backgroundRect.draw(backgroundColor);
		}

		// プログレスバー描画
		{
			const double barWidth = Max(1.0, (m_width - shapeStyle[Theme::Constant::ColorSliderBarPaddingLeft] - shapeStyle[Theme::Constant::ColorSliderBarPaddingRight]));
			const RectF barRect{
					(backgroundRect.x + shapeStyle[Theme::Constant::ColorSliderBarPaddingLeft]),
					backgroundRect.y + shapeStyle[Theme::Constant::ColorSliderBarVerticalPadding],
					barWidth,
					shapeStyle[Theme::Constant::ColorSliderBarHeight]
			};

			const double cornerRadius = Min(shapeStyle[Theme::Constant::ColorSliderBarCornerRadius], (barRect.h / 2.0));
			const RoundRect box{ barRect, cornerRadius };

			const ColorF innerShadowColor = colorStyle.getInnerShadowColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
			box.draw(innerShadowColor);

			const double innerShadowThickness = shapeStyle[Theme::Constant::InnerShadowThickness];
			const double innerRadius = (((box.w - innerShadowThickness) / box.w) * box.r);
			const RoundRect innerRect = box.stretched(-innerShadowThickness).withR(innerRadius);

			const Texture& hueTexture = SimpleGUI::GetHueTexture();
			innerRect(hueTexture).draw();

			const ColorF disableOverlayColor = colorStyle[Theme::Color::ColorSliderOverlayDisabled];

			if (not isEnabled())
			{
				innerRect.draw(disableOverlayColor);
			}

			// ノブ描画
			{
				const Circle knob{
					(barRect.x + (barRect.w * (m_state.hue / 360.0))),
					(barRect.y + (barRect.h / 2.0) - 0.75),
					shapeStyle[Theme::Constant::ColorSliderKnobRadius]
				};

				// ノブの影
				{
					const bool darkBackground = (backgroundColor.grayscale() < 0.5);
					const ColorF shadowColor = HSV{ m_state.hue, (darkBackground ? 0.4 : 1.0), (darkBackground ? 0.5 : 0.1), 0.5 };
					knob.drawShadow(Vec2{ 0, 1.0 }, 4.5, 0.5, shadowColor, false);
				}

				// ノブの内側
				{
					const ColorF knobColor = HSV{ m_state.hue, 1.0, 0.96 };
					knob.draw(knobColor);

					if (not isEnabled())
					{
						knob.draw(disableOverlayColor);
					}

					{
						const double smallRadius = Max(0.0, knob.r - shapeStyle[Theme::Constant::BorderInnerThickness]
							- shapeStyle[Theme::Constant::InnerShadowThickness] - shapeStyle[Theme::Constant::ColorSliderKnobFrameThickness]);
						const ColorF buttonColor = colorStyle.getButtonColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
						knob.withR(smallRadius).draw(buttonColor);
					}
				}

				// ノブの枠線
				{
					const ColorF borderColor = colorStyle.getBorderColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
					knob.drawFrame(shapeStyle[Theme::Constant::BorderInnerThickness], 0, borderColor);
				}
			}

			// カーソル変更
			if (m_mouseState.hovered)
			{
				Cursor::RequestStyle(CursorStyle::Hand);
			}
		}
	}
}
