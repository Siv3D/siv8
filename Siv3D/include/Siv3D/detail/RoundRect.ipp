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

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect::RoundRect(const value_type _x, const value_type _y, const value_type _w, const value_type _h, const value_type _r) noexcept
		: rect{ _x, _y, _w, _h }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h, const Concept::Arithmetic auto _r) noexcept
		: rect{ static_cast<value_type>(_x), static_cast<value_type>(_y), static_cast<value_type>(_w), static_cast<value_type>(_h) }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const position_type pos, const value_type _w, const value_type _h, const value_type _r) noexcept
		: rect{ pos, _w, _h }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const position_type pos, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h, const Concept::Arithmetic auto _r) noexcept
		: rect{ pos, static_cast<value_type>(_w), static_cast<value_type>(_h) }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const value_type _x, const value_type _y, const size_type size, const value_type _r) noexcept
		: rect{ _x, _y, size }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type size, const Concept::Arithmetic auto _r) noexcept
		: rect{ static_cast<value_type>(_x), static_cast<value_type>(_y), size }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const position_type pos, const size_type size, const value_type _r) noexcept
		: rect{ pos, size }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const position_type pos, const size_type size, const Concept::Arithmetic auto _r) noexcept
		: rect{ pos, size }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const value_type _w, const value_type _h, const value_type _r) noexcept
		: rect{ _center, _w, _h }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h, const Concept::Arithmetic auto _r) noexcept
		: rect{ _center, static_cast<value_type>(_w), static_cast<value_type>(_h) }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const size_type size, const value_type _r) noexcept
		: rect{ _center, size }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(Arg::center_<position_type> _center, const size_type size, const Concept::Arithmetic auto _r) noexcept
		: rect{ _center, size }
		, r{ static_cast<value_type>(_r) } {}

	constexpr RoundRect::RoundRect(const RectF& _rect, const value_type _r) noexcept
		: rect{ _rect }
		, r{ _r } {}

	constexpr RoundRect::RoundRect(const RectF& _rect, const Concept::Arithmetic auto _r) noexcept
		: rect{ _rect }
		, r{ static_cast<value_type>(_r) } {}
}
