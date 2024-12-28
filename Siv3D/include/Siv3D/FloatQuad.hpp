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
# include "Common.hpp"
# include "PointVector.hpp"
# include "2DShapes.hpp"
# include "FloatRect.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FloatQuad
	//
	////////////////////////////////////////////////////////////////

	struct FloatQuad
	{
		Float2 p[4];

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		FloatQuad() = default;

		[[nodiscard]]
		constexpr FloatQuad(const Float2& p0, const Float2& p1, const Float2& p2, const Float2& p3) noexcept;

		[[nodiscard]]
		constexpr FloatQuad(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3) noexcept;

		[[nodiscard]]
		explicit constexpr FloatQuad(const Quad& q) noexcept;

		[[nodiscard]]
		explicit constexpr FloatQuad(const FloatRect& rect) noexcept;
	};
}

# include "detail/FloatQuad.ipp"
