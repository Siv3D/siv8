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
	namespace TextEffect
	{
		constexpr QuadWarp::QuadWarp(const double leftOffset, const double rightOffset, const ColorF& color) noexcept
			: BasicTextEffect{ color }, m_leftOffset{ leftOffset }, m_rightOffset{ rightOffset } {}
	}
}
