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

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, unspecified, enabled, theme } {}

	SimpleCheckBox::SimpleCheckBox(const bool checked, const StringView text, const Vec2& pos, const Anchor anchor, const Concept::ExactlyBool auto enabled, const Theme theme)
		: SimpleCheckBox{ checked, text, pos, anchor, unspecified, enabled, theme } {}
}
