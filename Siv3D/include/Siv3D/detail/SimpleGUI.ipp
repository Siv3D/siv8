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

namespace s3d
{
	namespace SimpleGUI
	{
		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Theme theme)
		{
			return ProgressBar(current, total, pos, Anchor::TopLeft, SimpleProgressBar::DefaultWidth, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const double width, const Theme theme)
		{
			return ProgressBar(current, total, pos, Anchor::TopLeft, width, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const Theme theme)
		{
			return ProgressBar(current, total, pos, anchor, SimpleProgressBar::DefaultWidth, theme);
		}

		void ProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		{
			const double progress = (static_cast<double>(current) / static_cast<double>(total));
			return ProgressBar(progress, pos, anchor, width, theme);
		}
	}
}
