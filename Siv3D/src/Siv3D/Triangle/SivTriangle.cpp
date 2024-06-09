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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	Triangle::Triangle(const value_type sides, const value_type angle) noexcept
		: p0{ Circular{ (Math::InvSqrt3 * sides), angle } }
		, p1{ Circular{ (Math::InvSqrt3 * sides), (2 * Math::OneThirdPi + angle) } }
		, p2{ Circular{ (Math::InvSqrt3 * sides), (4 * Math::OneThirdPi + angle) } } {}

	const Triangle& Triangle::draw(const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addTriangle({ p0, p1, p2 }, color.toFloat4());
		return *this;
	}

	const Triangle& Triangle::draw(const ColorF& color0, const ColorF& color1, const ColorF& color2) const
	{
		SIV3D_ENGINE(Renderer2D)->addTriangle({ p0, p1, p2 }, { color0.toFloat4(), color1.toFloat4(), color2.toFloat4() });
		return *this;
	}
}
