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

# include <Siv3D/GUIColorStyle.hpp>
# include <Siv3D/GUIShapeStyle.hpp>
# include <Siv3D/Geometry2D/Misc.hpp>
# include <Siv3D/SimpleGUI/SimpleProgressBar.hpp>

namespace s3d
{
	SimpleProgressBar::SimpleProgressBar(const double progress, const Vec2& pos, const Theme theme)
		: SimpleProgressBar{ progress, pos, Anchor::TopLeft, DefaultWidth, theme } {}

	SimpleProgressBar::SimpleProgressBar(const double progress, const Vec2& pos, const double width, const Theme theme)
		: SimpleProgressBar{ progress, pos, Anchor::TopLeft, width,  theme } {}

	SimpleProgressBar::SimpleProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const Theme theme)
		: SimpleProgressBar{ progress, pos, anchor, DefaultWidth, theme } {}

	SimpleProgressBar::SimpleProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		: ISimpleGUIElement{ pos, anchor, true, theme }
		, m_width{ width }
		, m_state{ .progress = Clamp(progress, 0.0, 1.0) } {}

	SimpleProgressBar& SimpleProgressBar::setWidth(const double width)
	{
		if (const auto oldWidth = std::exchange(m_width, width);
			oldWidth != m_width)
		{
			setDirty();
		}

		return *this;
	}

	SimpleProgressBar& SimpleProgressBar::setProgress(const double progress)
	{
		if (const auto oldProgress = std::exchange(m_state.progress, Clamp(progress, 0.0, 1.0));
			oldProgress != m_state.progress)
		{
			setDirty();
		}

		return *this;
	}

	RectF SimpleProgressBar::region() const
	{
		const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
		const double height = (shapeStyle[Theme::Constant::ProgressBarHeight] + shapeStyle[Theme::Constant::ProgressBarVerticalPadding] * 2.0);
		const RectF backgroundRect{ getAnchor(), getPos(), m_width, height };

		if (isDirty())
		{
			if ((m_state.progress <= 0.0) || (not IsFinite(m_state.progress)))
			{
				m_cache.bar.clear();
			}
			else
			{
				const double barWidth = Max(1.0, (m_width - shapeStyle[Theme::Constant::ProgressBarPaddingLeft] - shapeStyle[Theme::Constant::ProgressBarPaddingRight]));
				const RectF barRect{ 0, 0, barWidth, shapeStyle[Theme::Constant::ProgressBarHeight] };
				const double cornerRadius = Min(shapeStyle[Theme::Constant::ProgressBarCornerRadius], (barRect.h / 2.0));
				const RoundRect roundRect{ barRect, cornerRadius };
				const RectF progressRect = barRect.withW(barRect.w * m_state.progress);

				if (auto polygons = Geometry2D::And(roundRect.asPolygon(QualityFactor{ 1.0 }), progressRect))
				{
					m_cache.bar = std::move(polygons.front());
				}
				else
				{
					m_cache.bar.clear();
				}
			}

			clearDirty();
		}

		return{ getAnchor(), getPos(), backgroundRect.size };
	}

	bool SimpleProgressBar::update()
	{
		return false;
	}

	void SimpleProgressBar::draw() const
	{
		if (not isVisible())
		{
			return;
		}

		const GUIColorStyle& colorStyle = getTheme().getColorStyle();
		const GUIShapeStyle& shapeStyle = getTheme().getShapeStyle();
		const RectF backgroundRect = region();

		// 背景描画
		if (const ColorF backgroundColor = colorStyle.getBackgroundColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
			(0.0 < backgroundColor.a))
		{
			backgroundRect.draw(backgroundColor);
		}

		// プログレスバー描画
		{
			const double barWidth = Max(1.0, (m_width - shapeStyle[Theme::Constant::ProgressBarPaddingLeft] - shapeStyle[Theme::Constant::ProgressBarPaddingRight]));
			const RectF barRect{
					(backgroundRect.x + shapeStyle[Theme::Constant::ProgressBarPaddingLeft]),
					backgroundRect.y + shapeStyle[Theme::Constant::ProgressBarVerticalPadding],
					barWidth,
					shapeStyle[Theme::Constant::ProgressBarHeight]
			};
			const double cornerRadius = Min(shapeStyle[Theme::Constant::ProgressBarCornerRadius], (barRect.h / 2.0));
			const RoundRect box{ barRect, cornerRadius };

			const ColorF innerShadowColor = colorStyle.getInnerShadowColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
			box.draw(innerShadowColor);

			const double innerShadowThickness = shapeStyle[Theme::Constant::InnerShadowThickness];
			const double innerRadius = (((box.w - innerShadowThickness) / box.w) * box.r);
			const RoundRect innerRect = box.stretched(-innerShadowThickness).withR(innerRadius);
			const ColorF containerColor = colorStyle.getContainerColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
			innerRect.draw(containerColor);

			const ColorF fillColor = colorStyle.getFillColor(isEnabled(), m_mouseState.hovered, m_mouseState.pressed);
			m_cache.bar.draw(barRect.pos, fillColor);
		}
	}
}
