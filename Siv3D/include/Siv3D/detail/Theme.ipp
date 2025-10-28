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
	constexpr Theme::Theme(const Predefined predefined) noexcept
		: m_themeID{ predefined.themeID } {}

	constexpr uint8 Theme::getID() const noexcept
	{
		return m_themeID;
	}
}
