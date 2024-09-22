//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	constexpr FloatRect::FloatRect(const float l, const float t, const float r, const float b) noexcept
		: left{ l }
		, top{ t }
		, right{ r }
		, bottom{ b } {}

	constexpr FloatRect::FloatRect(const Concept::Arithmetic auto l, const Concept::Arithmetic auto t, const Concept::Arithmetic auto r, const Concept::Arithmetic auto b) noexcept
		: left{ static_cast<float>(l) }
		, top{ static_cast<float>(t) }
		, right{ static_cast<float>(r) }
		, bottom{ static_cast<float>(b) } {}

	constexpr FloatRect::FloatRect(const Rect& rect) noexcept
		: left{ static_cast<float>(rect.x) }
		, top{ static_cast<float>(rect.y) }
		, right{ static_cast<float>(rect.x + rect.w) }
		, bottom{ static_cast<float>(rect.y + rect.h) } {}

	constexpr FloatRect::FloatRect(const RectF& rect) noexcept
		: left{ static_cast<float>(rect.x) }
		, top{ static_cast<float>(rect.y) }
		, right{ static_cast<float>(rect.x + rect.w) }
		, bottom{ static_cast<float>(rect.y + rect.h) } {}
}
