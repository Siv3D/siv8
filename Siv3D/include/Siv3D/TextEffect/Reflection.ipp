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
		constexpr Reflection::Reflection(const double start, const double startAlpha, const double offsetScaleY, const ColorF& color) noexcept
			: m_color{ color }, m_start{ start }, m_startAlpha{ startAlpha }, m_offsetScaleY{ offsetScaleY } {}
	}
}
