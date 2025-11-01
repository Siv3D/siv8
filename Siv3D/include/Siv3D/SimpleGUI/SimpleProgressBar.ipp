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
	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, Anchor::TopLeft, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const double width, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, Anchor::TopLeft, width,  theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, anchor, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, anchor, width, theme } {}

	SimpleProgressBar& SimpleProgressBar::setProgress(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total)
	{
		return setProgress(static_cast<double>(current) / static_cast<double>(total));
	}
}
