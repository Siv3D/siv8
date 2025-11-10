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
# include <Siv3D/GUIColorStyle.hpp>
# include <Siv3D/GUIShapeStyle.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/SimpleGUI/SimpleTooltip.hpp>

namespace s3d
{
	namespace
	{
		static Array<DrawableText> CreateDrawableTexts(const StringView text, const Font& font)
		{
			Array<DrawableText> drawableTexts;

			for (const auto& line : text.splitLines())
			{
				drawableTexts.emplace_back(font(line));
			}

			return drawableTexts;
		}
	}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Theme theme)
		: SimpleTooltip(text, pos, Anchor::TopLeft, unspecified, true, theme) {}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Optional<double>& width, const Theme theme)
		: SimpleTooltip(text, pos, Anchor::TopLeft, width, true, theme) {}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Optional<double>& width, const bool enabled, const Theme theme)
		: SimpleTooltip(text, pos, Anchor::TopLeft, width, enabled, theme) {}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Anchor anchor, Theme theme)
		: SimpleTooltip(text, pos, anchor, unspecified, true, theme) {}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, Theme theme)
		: SimpleTooltip(text, pos, anchor, width, true, theme) {}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Anchor anchor, const Optional<double>& width, const bool enabled, const Theme theme)
		: ISimpleGUIElement{ pos, anchor, enabled, theme }
		, m_drawableTexts{ CreateDrawableTexts(text, SimpleGUI::GetFont()) }
		, m_width{ width } {}

	SimpleTooltip& SimpleTooltip::setText(const StringView text)
	{
		if (m_text == text)
		{
			return *this;
		}

		m_drawableTexts = CreateDrawableTexts(text, SimpleGUI::GetFont());
		m_text = text;
		setDirty();
		return *this;
	}

	SimpleTooltip& SimpleTooltip::setWidth(const Optional<double>& width)
	{
		if (const auto oldWidth = std::exchange(m_width, width);
			oldWidth != m_width)
		{
			setDirty();
		}

		return *this;
	}

	RectF SimpleTooltip::region() const
	{
		return{ getAnchor(), getPos(), 0 };
	}

	RectF SimpleTooltip::regionOverlay() const
	{
		const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
		const double fontSize = shapeStyle[Theme::Constant::TooltipFontSize];

		if (isDirty())
		{
			double maxWidth = 0.0;
			for (const auto& drawableText : m_drawableTexts)
			{
				maxWidth = Max(maxWidth, drawableText.region(fontSize).w);
			}

			m_cache.textWidth = maxWidth;
			clearDirty();
		}

		const Anchor anchor = getAnchor();
		const double baseWidth = m_width.value_or(m_cache.textWidth + (shapeStyle[Theme::Constant::TooltipTextHorizontalPadding] * 2));
		const double baseHeight = Math::Ceil(SimpleGUI::GetFont().height(fontSize) * Max<size_t>(1, m_drawableTexts.size())) + (shapeStyle[Theme::Constant::TooltipTextVerticalPadding] * 2);
		RectF baseRect{ anchor, getPos(), baseWidth, baseHeight };

		const double offset = Min(shapeStyle[Theme::Constant::TooltipArrowOffset], (baseWidth * 0.4));
		const double arrowHeight = shapeStyle[Theme::Constant::TooltipArrowHeight];

		// X 方向のオフセット調整
		if ((anchor == Anchor::TopLeft) || (anchor == Anchor::BottomLeft))
		{
			baseRect.moveBy(-offset, 0.0);
		}
		else if ((anchor == Anchor::TopRight) || (anchor == Anchor::BottomRight))
		{
			baseRect.moveBy(offset, 0.0);
		}
		else if (anchor == Anchor::MiddleLeft)
		{
			baseRect.moveBy(arrowHeight, 0.0);
		}
		else if (anchor == Anchor::MiddleRight)
		{
			baseRect.moveBy(-arrowHeight, 0.0);
		}

		// Y 方向のオフセット調整
		if (anchor.isTop())
		{
			baseRect.moveBy(0.0, arrowHeight);
		}
		else if (anchor.isBottom())
		{
			baseRect.moveBy(0.0, -arrowHeight);
		}

		return baseRect;
	}

	bool SimpleTooltip::update()
	{
		const RectF rect = regionOverlay();
		const MouseState oldMouseState = m_mouseState;

		m_mouseState.hovered = ((not Cursor::IsCaptured()) && isVisible() && isEnabled() && rect.mouseOver());
		m_mouseState.pressed = (m_mouseState.hovered && Cursor::OnClientRect() && MouseL.pressed());

		updateMouseEvent(oldMouseState, m_mouseState);

		return (m_mouseState.pressed && MouseL.down());
	}

	void SimpleTooltip::draw() const {}

	void SimpleTooltip::drawOverlay() const
	{
		if (not isVisible())
		{
			return;
		}

		const GUIColorStyle& colorStyle = getTheme().getColorStyle();
		const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
		const RectF rect = regionOverlay();

		// 吹き出し描画
		{
			const ColorF tooltipColor = colorStyle.getTooltipColor(isEnabled(), isHovered(), isPressed());

			// 背景描画
			{
				rect.rounded(shapeStyle[Theme::Constant::TooltipCornerRadius]).draw(tooltipColor);
			}

			// 矢印描画
			if (const Anchor anchor = getAnchor(); (anchor != Anchor::Center))
			{
				const double triangleHeight = shapeStyle[Theme::Constant::TooltipArrowHeight];
				const double triangleBaseHalfWidth = (triangleHeight * Math::Constants::Sqrt3 / 3.0);

				Triangle triangle{ getPos(), Vec2{ 0, 0 }, Vec2{ 0, 0 } };

				if (anchor.isTop())
				{
					triangle.p1 = triangle.p0.movedBy(triangleBaseHalfWidth, triangleHeight);
					triangle.p2 = triangle.p0.movedBy(-triangleBaseHalfWidth, triangleHeight);
				}
				else if (anchor.isBottom())
				{
					triangle.p1 = triangle.p0.movedBy(-triangleBaseHalfWidth, -triangleHeight);
					triangle.p2 = triangle.p0.movedBy(triangleBaseHalfWidth, -triangleHeight);
				}
				else if (anchor == Anchor::MiddleLeft)
				{
					triangle.p1 = triangle.p0.movedBy(triangleHeight, -triangleBaseHalfWidth);
					triangle.p2 = triangle.p0.movedBy(triangleHeight, triangleBaseHalfWidth);
				}
				else if (anchor == Anchor::MiddleRight)
				{
					triangle.p1 = triangle.p0.movedBy(-triangleHeight, triangleBaseHalfWidth);
					triangle.p2 = triangle.p0.movedBy(-triangleHeight, -triangleBaseHalfWidth);
				}

				triangle.draw(tooltipColor);
			}
		}

		// テキスト描画
		{
			const Font& font = SimpleGUI::GetFont();
			const double fontSize = shapeStyle[Theme::Constant::TooltipFontSize];
			const double scale = (fontSize / SimpleGUI::DefaultFontSize);
			const double fontYOffset = scale;

			const Vec2 textBasePos{ (rect.x + shapeStyle[Theme::Constant::TooltipTextHorizontalPadding]),
				(rect.y + shapeStyle[Theme::Constant::TooltipTextVerticalPadding] - fontYOffset) };
			{
				const ColorF textColor = colorStyle.getTooltipTextColor(isEnabled(), isHovered(), isPressed());

				for (size_t i = 0; i < m_drawableTexts.size(); ++i)
				{
					const Vec2 textPos = textBasePos.withOffsetY(font.height(fontSize) * i);
					m_drawableTexts[i].draw(fontSize, textPos, textColor);
				}
			}
		}
	}
}
