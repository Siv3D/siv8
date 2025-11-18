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
	//	SimpleCheckBox
	//
	////////////////////////////////////////////////////////////////

	class SimpleCheckBox : public ISimpleGUIElement
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		SimpleCheckBox() = default;

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);
		
		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);
		
		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~SimpleCheckBox() override = default;

		////////////////////////////////////////////////////////////////
		//
		//	setChecked, isChecked
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleCheckBox& setChecked(bool checked);

		[[nodiscard]]
		virtual bool isChecked() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setText, getText
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleCheckBox& setText(StringView text);

		[[nodiscard]]
		virtual const String& getText() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setWidth, getWidth
		//
		////////////////////////////////////////////////////////////////

		virtual SimpleCheckBox& setWidth(const Optional<double>& width);

		[[nodiscard]]
		virtual const Optional<double>& getWidth() const;

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

	protected:

		////////////////////////////////////////////////////////////////
		//
		//	drawBase
		//
		////////////////////////////////////////////////////////////////

		virtual void drawBase() const override;

		DrawableText m_drawableText;

		Optional<double> m_width;

		struct State
		{
			bool checked = false;
		} m_state;

		mutable struct Cache
		{
			double textWidth = 0.0;
		} m_cache;
	};
}

# include "SimpleCheckBox.ipp"
