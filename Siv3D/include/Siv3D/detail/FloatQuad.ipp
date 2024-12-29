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

	constexpr FloatQuad::FloatQuad(const Float2& p0, const Float2& p1, const Float2& p2, const Float2& p3) noexcept
		: p{ p0, p1, p2, p3 } {}

	constexpr FloatQuad::FloatQuad(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3) noexcept
		: p{ p0, p1, p2, p3 } {}

	constexpr FloatQuad::FloatQuad(const Quad& q) noexcept
		: p{ q.p0, q.p1, q.p2, q.p3 } {}

	constexpr FloatQuad::FloatQuad(const FloatRect& rect) noexcept
		: p{ { rect.left, rect.top }, { rect.right, rect.top }, { rect.right, rect.bottom }, { rect.left, rect.bottom } } {}

	////////////////////////////////////////////////////////////////
	//
	//	FromRect
	//
	////////////////////////////////////////////////////////////////

	constexpr FloatQuad FloatQuad::FromRect(double x, double y, double w, double h) noexcept
	{
		return{ Float2{ x, y }, Float2{ (x + w), y }, Float2{ (x + w), (y + h) }, Float2{ x, (y + h) } };
	}
}
