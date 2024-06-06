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

	constexpr Circle::Circle(const size_type _r) noexcept
		: center{ 0.0, 0.0 }
		, r{ _r } {}

	constexpr Circle::Circle(const Concept::Arithmetic auto _r) noexcept
		: center{ 0.0, 0.0 }
		, r{ static_cast<value_type>(_r) } {}

	constexpr Circle::Circle(const value_type _x, const value_type _y, const size_type _r) noexcept
		: center{ _x, _y }
		, r{ _r } {}

	constexpr Circle::Circle(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _r) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const position_type _center, const size_type _r) noexcept
		: center{ _center }
		, r{ _r } {}

	constexpr Circle::Circle(const position_type _center, const Concept::Arithmetic auto _r) noexcept
		: center{ _center }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::center_<position_type> _center, const size_type _r) noexcept
		: center{ *_center }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::center_<position_type> _center, const Concept::Arithmetic auto _r) noexcept
		: center{ *_center }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::topLeft_<position_type> _topLeft, const size_type _r) noexcept
		: center{ (_topLeft->x + _r), (_topLeft->y + _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::topLeft_<position_type> _topLeft, const Concept::Arithmetic auto _r) noexcept
		: center{ (_topLeft->x + static_cast<value_type>(_r)), (_topLeft->y + static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::topCenter_<position_type> _topCenter, const size_type _r) noexcept
		: center{ _topCenter->x, (_topCenter->y + _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::topCenter_<position_type> _topCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ _topCenter->x, (_topCenter->y + static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::topRight_<position_type> _topRight, const size_type _r) noexcept
		: center{ (_topRight->x - _r), (_topRight->y + _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::topRight_<position_type> _topRight, const Concept::Arithmetic auto _r) noexcept
		: center{ (_topRight->x - static_cast<value_type>(_r)), (_topRight->y + static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::rightCenter_<position_type> _rightCenter, const size_type _r) noexcept
		: center{ (_rightCenter->x - _r), _rightCenter->y }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::rightCenter_<position_type> _rightCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ (_rightCenter->x - static_cast<value_type>(_r)), _rightCenter->y }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::bottomRight_<position_type> _bottomRight, const size_type _r) noexcept
		: center{ (_bottomRight->x - _r), (_bottomRight->y - _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::bottomRight_<position_type> _bottomRight, const Concept::Arithmetic auto _r) noexcept
		: center{ (_bottomRight->x - static_cast<value_type>(_r)), (_bottomRight->y - static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::bottomCenter_<position_type> _bottomCenter, const size_type _r) noexcept
		: center{ _bottomCenter->x, (_bottomCenter->y - _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::bottomCenter_<position_type> _bottomCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ _bottomCenter->x, (_bottomCenter->y - static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::bottomLeft_<position_type> _bottomLeft, const size_type _r) noexcept
		: center{ (_bottomLeft->x + _r), (_bottomLeft->y - _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::bottomLeft_<position_type> _bottomLeft, const Concept::Arithmetic auto _r) noexcept
		: center{ (_bottomLeft->x + static_cast<value_type>(_r)), (_bottomLeft->y - static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::leftCenter_<position_type> _leftCenter, const size_type _r) noexcept
		: center{ (_leftCenter->x + _r), _leftCenter->y }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::leftCenter_<position_type> _leftCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ (_leftCenter->x + static_cast<value_type>(_r)), _leftCenter->y }
		, r{ static_cast<size_type>(_r) } {}
}
