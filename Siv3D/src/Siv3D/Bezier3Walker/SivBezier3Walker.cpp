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

# include <Siv3D/Bezier3Walker.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	advanceBy
	//
	////////////////////////////////////////////////////////////////

	double Bezier3Walker::advanceBy(const double distance, const int32 iterations) noexcept
	{
		double remainderUnused = 0.0;
		return advanceBy(distance, remainderUnused, iterations);
	}

	double Bezier3Walker::advanceBy(const double distance, double& remainder, int32 iterations) noexcept
	{
		iterations = Max(1, iterations);

		remainder = 0.0;

		if (distance == 0.0)
		{
			return m_t;
		}

		// 端にいるなら即座に余り
		if (((m_t <= 0.0) && (distance < 0.0)) || ((1.0 <= m_t) && (0.0 < distance)))
		{
			remainder = distance;
			return m_t;
		}

		const double stepS = (distance / static_cast<double>(iterations));
		constexpr double SpeedEps = 1e-12;

		double consumed = 0.0;

		for (int32 i = 0; i < iterations; ++i)
		{
			const double oldT = m_t;

			// B'(t) = m_d0 * t^2 + m_d1 * t + m_d2
			const Vec2 d = (m_d0 * (m_t * m_t) + (m_d1 * m_t) + m_d2);
			const double speed = d.length();

			// 退化/極小速度：これ以上の更新は危険なので打ち切り
			if (!(speed > SpeedEps))
			{
				break;
			}

			const double dt = (stepS / speed);
			double newT = (oldT + dt);

			// 端に達するなら clamp し、そのぶんの距離だけ「消費」する
			if (newT <= 0.0)
			{
				newT = 0.0;
				m_t = 0.0;

				const double usedDt = (newT - oldT); // 負
				consumed += (usedDt * speed);        // 負
				break;
			}
			else if (1.0 <= newT)
			{
				newT = 1.0;
				m_t = 1.0;

				const double usedDt = (newT - oldT); // 正
				consumed += (usedDt * speed);        // 正
				break;
			}

			// 通常更新
			m_t = newT;
			consumed += stepS;
		}

		remainder = (distance - consumed);
		return m_t;
	}
}
