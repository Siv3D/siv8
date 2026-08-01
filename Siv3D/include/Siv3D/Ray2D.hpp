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
# include <cassert>
# include "PointVector.hpp"

namespace s3d
{
	struct Ray2D
	{
		Vec2 origin{ 0.0, 0.0 };
		
		Vec2 direction{ 1.0, 0.0 };

		[[nodiscard]]
		constexpr Ray2D() noexcept = default;

		[[nodiscard]]
		Ray2D(const Vec2& _origin, const Vec2& _direction) noexcept;

		[[nodiscard]]
		constexpr Vec2 pointAt(const double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Ray2D& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Ray2D, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Ray2D& value, s3d::BufferContext& ctx) const;
};

# include "detail/Ray2D.ipp"
