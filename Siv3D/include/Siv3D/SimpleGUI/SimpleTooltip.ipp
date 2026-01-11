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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleTooltip{ text, pos, unspecified, enabled, theme } {}

	SimpleTooltip::SimpleTooltip(const StringView text, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleTooltip{ text, pos, anchor, unspecified, enabled, theme } {}
}
