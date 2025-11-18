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
# include "ISimpleGUIElement.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SimpleTooltip
	//
	////////////////////////////////////////////////////////////////

	class SimpleTooltip : public ISimpleGUIElement
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		SimpleTooltip() = default;

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~SimpleTooltip() override = default;

		////////////////////////////////////////////////////////////////
		//
		//	setText, getText
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleTooltip& setText(StringView text);

		[[nodiscard]]
		virtual const String& getText() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setWidth, getWidth
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleTooltip& setWidth(const Optional<double>& width);

		[[nodiscard]]
		virtual const Optional<double>& getWidth() const;

		////////////////////////////////////////////////////////////////
		//
		//	region, regionOverlay
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual RectF region() const override;

		[[nodiscard]]
		virtual RectF regionOverlay() const;

	protected:

		////////////////////////////////////////////////////////////////
		//
		//	updateBase, updateOverlay
		//
		////////////////////////////////////////////////////////////////

		virtual bool updateBase() override;

		virtual bool updateOverlay() override;

		////////////////////////////////////////////////////////////////
		//
		//	drawBase, drawOverlay
		//
		////////////////////////////////////////////////////////////////

		virtual void drawBase() const override;

		virtual void drawOverlay() const override;

		Array<DrawableText> m_drawableTexts;

		String m_text;

		Optional<double> m_width;

		mutable struct Cache
		{
			double textWidth = 0.0;
		} m_cache;
	};
}

# include "SimpleTooltip.ipp"
