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
	constexpr Theme::Theme(const Predefined predefined) noexcept
		: m_colorID{ predefined.colorID }
		, m_shapeID{ predefined.shapeID } {}

	constexpr uint8 Theme::getColorID() const noexcept
	{
		return m_colorID;
	}

	constexpr uint8 Theme::getShapeID() const noexcept
	{
		return m_shapeID;
	}
}
