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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse::SuperEllipse(const value_type _x, const value_type _y, const value_type _a, const value_type _b, const value_type _n) noexcept
		: center{ _x, _y }
		, axes{ _a, _b }
		, n{ _n } {}

	constexpr SuperEllipse::SuperEllipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b, const Concept::Arithmetic auto _n) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const position_type& _center, const value_type _a, const value_type _b, const value_type _n) noexcept
		: center{ _center }
		, axes{ _a, _b }
		, n{ _n } {}

	constexpr SuperEllipse::SuperEllipse(const position_type& _center, const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b, const Concept::Arithmetic auto _n) noexcept
		: center{ _center }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const value_type _x, const value_type _y, const size_type& _axes, const value_type _n) noexcept
		: center{ _x, _y }
		, axes{ _axes }
		, n{ _n } {}

	constexpr SuperEllipse::SuperEllipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type& _axes, const Concept::Arithmetic auto _n) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ _axes }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const position_type& _center, const size_type& _axes, const Concept::Arithmetic auto _n) noexcept
		: center{ _center }
		, axes{ _axes }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const Ellipse& ellipse, const Concept::Arithmetic auto _n) noexcept
		: center{ ellipse.center }
		, axes{ ellipse.axes }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const RectF& rect, const Concept::Arithmetic auto _n) noexcept
		: center{ rect.center()}
		, axes{ (rect.size.x * 0.5), (rect.size.y * 0.5) }
		, n{ static_cast<value_type>(_n) } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withX(const value_type _x) const noexcept
	{
		return{ _x, center.y, axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withY(const value_type _y) const noexcept
	{
		return{ center.x, _y, axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withCenter(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, axes, n };
	}

	constexpr SuperEllipse SuperEllipse::withCenter(const position_type _center) const noexcept
	{
		return{ _center, axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withA
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withA(const value_type _a) const noexcept
	{
		return{ center, _a, axes.y, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withB
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withB(const value_type _b) const noexcept
	{
		return{ center, axes.x, _b, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withAxes
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withAxes(const value_type _a, const value_type _b) const noexcept
	{
		return{ center, _a, _b, n };
	}

	constexpr SuperEllipse SuperEllipse::withAxes(const size_type _axes) const noexcept
	{
		return{ center, _axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withN
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withN(const value_type _n) const noexcept
	{
		return{ center, axes, _n };
	}

}
