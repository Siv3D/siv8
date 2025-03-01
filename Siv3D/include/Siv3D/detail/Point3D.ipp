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

	constexpr Point3D::Point3D(const value_type _x, const value_type _y, const value_type _z) noexcept
		: x{ _x }
		, y{ _y }
		, z{ _z } {}

	constexpr Point3D::Point3D(const Concept::Integral auto _x, const Concept::Integral auto _y, const Concept::Integral auto _z) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) }
		, z{ static_cast<value_type>(_z) } {}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Point3D::hash() const noexcept
	{
		return Hash(*this);
	}

}
