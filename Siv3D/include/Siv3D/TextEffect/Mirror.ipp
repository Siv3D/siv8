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
		constexpr Mirror::Mirror(const ColorF& color) noexcept
			: Mirror{ true, color } {}

		constexpr Mirror::Mirror(const bool miror, const ColorF& color) noexcept
			: BasicTextEffect{ color }, m_mirror{ miror } {}
	}
}
