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
# include "Common.hpp"
# include "SimpleGUI/ISimpleGUIElement.hpp"

namespace s3d
{
	class Font;

	namespace SimpleGUI
	{
		inline constexpr double DefaultFontSize = 20;

		/// @brief SimpleGUI のデフォルトフォントを取得します。
		/// @remark ベースが CJK_Regular で、利用可能であれば Icon_MaterialDesign, ColorEmoji がフォールバックとして追加されています。
		/// @return デフォルトフォント
		[[nodiscard]]
		const Font& GetFont() noexcept;


		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Theme theme = Theme::Light);

		/// @brief 誤用防止用コンストラクタ（= delete）
		bool Button(StringView text, const Vec2& pos, Concept::ExactlyBool auto param, Theme theme = Theme::Light) = delete;

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width = unspecified, bool enabled = true, Theme theme = Theme::Light);

		/// @brief 誤用防止用コンストラクタ（= delete）
		bool Button(StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto param, bool enabled = true, Theme theme = Theme::Light) = delete;
	}
}
