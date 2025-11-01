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
	class SimpleTooltip : public ISimpleGUIElement
	{
	public:

		[[nodiscard]]
		SimpleTooltip() = default;

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		/// @brief 誤用防止用コンストラクタ（= delete）
		SimpleTooltip(StringView text, const Vec2& pos, Concept::ExactlyBool auto param, Theme theme = Theme::Light) = delete;

		/// @brief 誤用防止用コンストラクタ（= delete）
		SimpleTooltip(StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto param, bool enabled = true, Theme theme = Theme::Light) = delete;

		virtual ~SimpleTooltip() override = default;

		virtual SimpleTooltip& setText(StringView text);

		virtual SimpleTooltip& setWidth(const Optional<double>& width);

		[[nodiscard]]
		virtual RectF region() const override;

		[[nodiscard]]
		virtual RectF regionOverlay() const;

		virtual bool update() override;

		virtual void draw() const override;

		virtual void drawOverlay() const override;

	private:

		Array<DrawableText> m_drawableTexts;

		String m_text;

		Optional<double> m_width;

		mutable struct Cache
		{
			double textWidth = 0.0;
		} m_cache;
	};
}
