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
	class SimpleCheckBox : public ISimpleGUIElement
	{
	public:

		[[nodiscard]]
		SimpleCheckBox() = default;

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Theme theme = Theme::Light);

		/// @brief 誤用防止用コンストラクタ（= delete）
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Concept::ExactlyBool auto param, Theme theme = Theme::Light) = delete;

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);
		
		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);
		
		[[nodiscard]]
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width = unspecified, bool enabled = true, Theme theme = Theme::Light);

		/// @brief 誤用防止用コンストラクタ（= delete）
		SimpleCheckBox(bool checked, StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto param, bool enabled = true, Theme theme = Theme::Light) = delete;

		virtual ~SimpleCheckBox() override = default;

		virtual SimpleCheckBox& setChecked(bool checked);

		virtual SimpleCheckBox& setText(StringView text);

		virtual SimpleCheckBox& setWidth(const Optional<double>& width);

		[[nodiscard]]
		virtual RectF region() const override;

		virtual bool update() override;

		virtual void draw() const override;

		[[nodiscard]]
		virtual bool isChecked() const noexcept;

	private:

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
