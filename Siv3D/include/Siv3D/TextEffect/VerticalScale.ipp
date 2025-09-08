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
		constexpr VerticalScale::VerticalScale(const double scale, const ColorF& color) noexcept
			: VerticalScale{ scale, 0.5, color } {}

		constexpr VerticalScale::VerticalScale(const double scale, const double center, const ColorF& color) noexcept
			: BasicTextEffect{ color }, m_scale{ scale }, m_center{ center } {}
	}
}
