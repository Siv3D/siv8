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
		constexpr VerticalGradient::VerticalGradient(const ColorF& startColor, const ColorF& endColor, const double startOffset, const double endOffset) noexcept
			: m_startColor{ startColor }, m_endColor{ endColor }, m_startOffset{ startOffset }, m_endOffset{ endOffset } {}
	}
}
