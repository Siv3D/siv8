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

# pragma once
# include "../DrawableText.hpp"
# include "../Polygon.hpp"
# include "ISimpleGUIElement.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SimpleProgressBar
	//
	////////////////////////////////////////////////////////////////

	class SimpleProgressBar : public ISimpleGUIElement
	{
	public:

		static constexpr double DefaultWidth = 116.0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		SimpleProgressBar() = default;

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, double width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(double progress, const Vec2& pos, Anchor anchor, double width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, double width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, double width, bool enabled, Theme theme = Theme::Light);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~SimpleProgressBar() override = default;

		////////////////////////////////////////////////////////////////
		//
		//	setWidth, getWidth
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleProgressBar& setWidth(double width);

		[[nodiscard]]
		virtual double getWidth() const;

		////////////////////////////////////////////////////////////////
		//
		//	setProgress, getProgress
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleProgressBar& setProgress(double progress);

		SimpleProgressBar& setProgress(Concept::Arithmetic auto current, Concept::Arithmetic auto total);

		[[nodiscard]]
		virtual double getProgress() const;

		////////////////////////////////////////////////////////////////
		//
		//	region
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual RectF region() const override;

	protected:

		////////////////////////////////////////////////////////////////
		//
		//	updateBase
		//
		////////////////////////////////////////////////////////////////

		virtual bool updateBase() override;

		////////////////////////////////////////////////////////////////
		//
		//	drawBase
		//
		////////////////////////////////////////////////////////////////

		virtual void drawBase() const override;

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
