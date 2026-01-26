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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Bezier3Walker::Bezier3Walker(const Bezier3& bezier) noexcept
		: m_d0{ (-bezier.p0 + 3.0 * bezier.p1 - 3.0 * bezier.p2 + bezier.p3) * 3.0 } // 3 * (-p0 + 3p1 - 3p2 + p3)
		, m_d1{ (bezier.p0 - 2.0 * bezier.p1 + bezier.p2) * 6.0 } // 6 * (p0 - 2p1 + p2)
		, m_d2{ (bezier.p1 - bezier.p0) * 3.0 } // 3 * (p1 - p0)
		, m_t{ 0.0 } {}

	////////////////////////////////////////////////////////////////
	//
	//	setT, t
	//
	////////////////////////////////////////////////////////////////

	constexpr void Bezier3Walker::setT(const double t) noexcept
	{
		m_t = Clamp(t, 0.0, 1.0);
	}

	constexpr double Bezier3Walker::t() const noexcept
	{
		return m_t;
	}

}
