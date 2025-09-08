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
		constexpr FlipX::FlipX(const ColorF& color) noexcept
			: FlipX{ true, color } {}

		constexpr FlipX::FlipX(const bool flipX, const ColorF& color) noexcept
			: BasicTextEffect{ color }, m_flipX{ flipX } {}
	}
}
