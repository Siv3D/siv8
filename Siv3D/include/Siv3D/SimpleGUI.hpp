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
# include "SimpleGUI/SimpleProgressBar.hpp"

namespace s3d
{
	class Font;

	namespace SimpleGUI
	{
		////////////////////////////////////////////////////////////////
		//
		//	DefaultFontSize
		//
		////////////////////////////////////////////////////////////////

		inline constexpr double DefaultFontSize = 20;

		////////////////////////////////////////////////////////////////
		//
		//	GetFont
		//
		////////////////////////////////////////////////////////////////

		/// @brief SimpleGUI のデフォルトフォントを取得します。
		/// @remark ベースが CJK_Regular で、利用可能であれば Icon_MaterialDesign, ColorEmoji がフォールバックとして追加されています。
		/// @return デフォルトフォント
		[[nodiscard]]
		const Font& GetFont() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetHueTexture
		//
		////////////////////////////////////////////////////////////////

		const Texture& GetHueTexture() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Button
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, Theme theme = Theme::Light);

		[[nodiscard]]
		bool Button(StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		////////////////////////////////////////////////////////////////
		//
		//	CheckBox
		//
		////////////////////////////////////////////////////////////////

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, const Optional<double>& width, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, Theme theme = Theme::Light);

		bool CheckBox(bool& checked, StringView text, const Vec2& pos, Anchor anchor, const Optional<double>& width, bool enabled, Theme theme = Theme::Light);

		////////////////////////////////////////////////////////////////
		//
		//	ProgressBar
		//
		////////////////////////////////////////////////////////////////

		void ProgressBar(double progress, const Vec2& pos, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, double width, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, double width, bool enabled, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);

		void ProgressBar(double progress, const Vec2& pos, Anchor anchor, double width, bool enabled, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, double width, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, double width, bool enabled, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, Concept::ExactlyBool auto enabled, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, double width, Theme theme = Theme::Light);

		void ProgressBar(Concept::Arithmetic auto current, Concept::Arithmetic auto total, const Vec2& pos, Anchor anchor, double width, bool enabled, Theme theme = Theme::Light);
	}
}

# include "detail/SimpleGUI.ipp"
