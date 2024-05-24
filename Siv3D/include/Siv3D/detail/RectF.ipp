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

	constexpr RectF::RectF(const value_type _size) noexcept
		: pos{ 0, 0 }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _size) noexcept
		: pos{ 0, 0 }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const value_type _w, const value_type _h) noexcept
		: pos{ 0, 0 }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: pos{ 0, 0 }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const size_type _size) noexcept
		: pos{ 0, 0 }
		, size{ _size } {}

	constexpr RectF::RectF(const value_type _x, const value_type _y, const value_type _size) noexcept
		: pos{ _x, _y }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _size) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const value_type _x, const value_type _y, const value_type _w, const value_type _h) noexcept
		: pos{ _x, _y }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const value_type _x, const value_type _y, const size_type _size) noexcept
		: pos{ _x, _y }
		, size{ _size } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type _size) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ _size } {}

	constexpr RectF::RectF(const position_type _pos, const value_type _size) noexcept
		: pos{ _pos }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const position_type _pos, const Concept::Arithmetic auto _size) noexcept
		: pos{ _pos }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const position_type _pos, const value_type _w, const value_type _h) noexcept
		: pos{ _pos }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const position_type _pos, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: pos{ _pos }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const position_type _pos, const size_type _size) noexcept
		: pos{ _pos }
		, size{ _size } {}
}
