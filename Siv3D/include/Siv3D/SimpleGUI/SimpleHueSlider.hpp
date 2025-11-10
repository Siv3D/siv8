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
# include "../DrawableText.hpp"
# include "../Polygon.hpp"
# include "ISimpleGUIElement.hpp"

namespace s3d
{
	class SimpleHueSlider : public ISimpleGUIElement
	{
	public:

		static constexpr double DefaultWidth = 156.0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		SimpleHueSlider() = default;

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, Concept::ExactlyBool auto param, Theme theme = Theme::Light)
			: SimpleHueSlider{ hue, pos, DefaultWidth, param, theme } {}

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto param, bool enabled = true, Theme theme = Theme::Light)
			: SimpleHueSlider{ hue, pos, anchor, DefaultWidth, param, theme } {}

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, double width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleHueSlider(double hue, const Vec2& pos, Anchor anchor, double width, bool enabled, Theme theme = Theme::Light);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~SimpleHueSlider() override = default;

		////////////////////////////////////////////////////////////////
		//
		//	setWidth, getWidth
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleHueSlider& setWidth(double width);

		[[nodiscard]]
		virtual double getWidth() const;

		////////////////////////////////////////////////////////////////
		//
		//	setHue, getHue
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleHueSlider& setHue(double hue);

		[[nodiscard]]
		virtual double getHue() const;

		////////////////////////////////////////////////////////////////
		//
		//	region
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual RectF region() const override;

		////////////////////////////////////////////////////////////////
		//
		//	update
		//
		////////////////////////////////////////////////////////////////

		virtual bool update() override;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		virtual void draw() const override;

	private:

		double m_width;

		struct State
		{
			double hue = 0.0;
		} m_state;
	};
}
