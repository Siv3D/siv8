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
	class SimpleButton : public ISimpleGUIElement
	{
	public:

		[[nodiscard]]
		SimpleButton() = default;

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, Concept::ExactlyBool auto param, Theme theme = Theme::Light)
			: SimpleButton{ text, pos, unspecified, param, theme } {}

		[[nodiscard]]
		SimpleButton(StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto param, bool enabled = true, Theme theme = Theme::Light)
			: SimpleButton{ text, pos, anchor, unspecified, param, theme } {}

		virtual ~SimpleButton() override = default;

		virtual SimpleButton& setText(StringView text);

		virtual SimpleButton& setWidth(const Optional<double>& width);

		[[nodiscard]]
		virtual RectF region() const override;

		/// @brief 状態を更新します。
		/// @return このフレームでボタンが押下された場合 true, それ以外は false
		virtual bool update() override;

		virtual void draw() const override;

	private:

		DrawableText m_drawableText;

		Optional<double> m_width;

		mutable struct Cache
		{
			double textWidth = 0.0;
		} m_cache;
	};
}
