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

	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleButton{ text, pos, unspecified, enabled, theme } {}

	SimpleButton::SimpleButton(const StringView text, const Vec2& pos, Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleButton{ text, pos, anchor, unspecified, enabled, theme } {}
}
