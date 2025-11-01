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
	class SimpleProgressBar : public ISimpleGUIElement
	{
	public:

		static constexpr double DefaultWidth = 116.0;

		[[nodiscard]]
		SimpleProgressBar() = default;

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);
	
		virtual ~SimpleProgressBar() override = default;

		virtual SimpleProgressBar& setWidth(double width);

		virtual SimpleProgressBar& setProgress(double progress);

		SimpleProgressBar& setProgress(Concept::Arithmetic auto current, Concept::Arithmetic auto total);

		[[nodiscard]]
		virtual RectF region() const override;

		virtual bool update() override;

		virtual void draw() const override;

	private:

		double m_width;

		struct State
		{
			double progress = 0.0;
		} m_state;

		mutable struct Cache
		{
			Polygon bar;
		} m_cache;
	};
}

# include "SimpleProgressBar.ipp"
