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
		constexpr VerticalGradient::VerticalGradient(const ColorF& _startColor, const ColorF& _endColor, const double _startOffset, const double _endOffset) noexcept
			: startColor{ _startColor }, endColor{ _endColor }, startOffset{ _startOffset }, endOffset{ _endOffset } {}
	}
}
