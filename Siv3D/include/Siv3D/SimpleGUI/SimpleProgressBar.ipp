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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	SimpleProgressBar::SimpleProgressBar(const double progress, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleProgressBar{ progress, pos, DefaultWidth, enabled, theme } {}

	SimpleProgressBar::SimpleProgressBar(const double progress, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleProgressBar{ progress, pos, anchor, DefaultWidth, enabled, theme } {}


	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, Anchor::TopLeft, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, Anchor::TopLeft, enabled, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const double width, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, Anchor::TopLeft, width,  theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const double width, const bool enabled, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, Anchor::TopLeft, width,  enabled, theme } {}


	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, anchor, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, anchor, enabled, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const double width, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, anchor, width, theme } {}

	SimpleProgressBar::SimpleProgressBar(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total, const Vec2& pos, const Anchor anchor, const double width, const bool enabled, const Theme theme)
		: SimpleProgressBar{ (static_cast<double>(current) / static_cast<double>(total)), pos, anchor, width, enabled, theme } {}

	////////////////////////////////////////////////////////////////
	//
	//	setProgress
	//
	////////////////////////////////////////////////////////////////

	SimpleProgressBar& SimpleProgressBar::setProgress(const Concept::Arithmetic auto current, const Concept::Arithmetic auto total)
	{
		return setProgress(static_cast<double>(current) / static_cast<double>(total));
	}
}
