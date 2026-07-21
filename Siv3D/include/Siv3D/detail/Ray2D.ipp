//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	inline Ray2D::Ray2D(const Vec2& _origin, const Vec2& _direction) noexcept
		: origin{ _origin }
	{
		const double lengthSq = _direction.dot(_direction);
		
		assert((0.0 < lengthSq) && std::isfinite(lengthSq));

		if ((0.0 < lengthSq) && std::isfinite(lengthSq))
		{
			direction = (_direction / std::sqrt(lengthSq));
		}
		else
		{
			// A zero-length or non-finite direction makes the Ray2D invalid.
			// Keep the supplied value to avoid manufacturing a valid direction.
			direction = _direction;
		}
	}

	constexpr Vec2 Ray2D::pointAt(const double distance) const noexcept
	{
		return (origin + direction * distance);
	}
}
