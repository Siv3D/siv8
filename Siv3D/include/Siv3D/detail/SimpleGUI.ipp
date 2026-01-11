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

namespace s3d
{
	namespace SimpleGUI
	{
		////////////////////////////////////////////////////////////////
		//
		//	Button
		//
		////////////////////////////////////////////////////////////////

		bool Button(const StringView text, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return Button(text, pos, Anchor::TopLeft, unspecified, enabled, theme);
		}

		bool Button(const StringView text, const Vec2& pos, const Anchor anchor, Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return Button(text, pos, anchor, unspecified, enabled, theme);
		}

		////////////////////////////////////////////////////////////////
		//
		//	CheckBox
		//
		////////////////////////////////////////////////////////////////

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return CheckBox(checked, text, pos, Anchor::TopLeft, unspecified, enabled, theme);
		}

		bool CheckBox(bool& checked, const StringView text, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return CheckBox(checked, text, pos, anchor, unspecified, enabled, theme);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ProgressBar
		//
		////////////////////////////////////////////////////////////////

		void ProgressBar(const double progress, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return ProgressBar(progress, pos, Anchor::TopLeft, SimpleProgressBar::DefaultWidth, enabled, theme);
		}

		void ProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return ProgressBar(progress, pos, anchor, SimpleProgressBar::DefaultWidth, enabled, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Theme theme)
		{
			return ProgressBar(current, total, pos, Anchor::TopLeft, SimpleProgressBar::DefaultWidth, true, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return ProgressBar(current, total, pos, Anchor::TopLeft, SimpleProgressBar::DefaultWidth, enabled, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const double width, const Theme theme)
		{
			return ProgressBar(current, total, pos, Anchor::TopLeft, width, true, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const double width, const bool enabled, const Theme theme)
		{
			return ProgressBar(current, total, pos, Anchor::TopLeft, width, enabled, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const Theme theme)
		{
			return ProgressBar(current, total, pos, anchor, SimpleProgressBar::DefaultWidth, true, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, Concept::ExactlyBool auto enabled, const Theme theme)
		{
			return ProgressBar(current, total, pos, anchor, SimpleProgressBar::DefaultWidth, enabled, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		{
			return ProgressBar(current, total, pos, anchor, width, true, theme);
		}
		
		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const double width, const bool enabled, const Theme theme)
		{
			const double progress = (static_cast<double>(current) / static_cast<double>(total));
			return ProgressBar(progress, pos, anchor, width, enabled, theme);
		}
	}
}
