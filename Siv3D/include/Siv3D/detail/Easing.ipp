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
	namespace detail
	{
		inline double ElasticAP(double t, double a, const double p) noexcept
		{
			if (t == 0)
			{
				return 0.0;
			}
			else if (t == 1)
			{
				return 1.0;
			}

			double s;

			if (a < 1.0)
			{
				a = 1.0;
				s = (p / 4.0f);
			}
			else
			{
				s = (p * Math::InvTwoPi * std::asin(1.0 / a));
			}

			t -= 1.0;

			return -(a * std::exp2(10.0 * t) * std::sin((t - s) * Math::TwoPi / p));
		}
	}

	namespace Easing
	{
		constexpr double Linear(const double t) noexcept
		{
			return t;
		}

		inline double Sine(const double t) noexcept
		{
			return (1.0 - std::cos(t * Math::HalfPi));
		}

		constexpr double Quad(const double t) noexcept
		{
			return (t * t);
		}

		constexpr double Cubic(const double t) noexcept
		{
			return (t * t * t);
		}

		constexpr double Quart(const double t) noexcept
		{
			return (t * t) * (t * t);
		}

		constexpr double Quint(const double t) noexcept
		{
			return (t * t) * (t * t) * t;
		}

		inline double Expo(const double t) noexcept
		{
			if (t == 0.0)
			{
				return 0.0;
			}
			else
			{
				return std::exp2(10.0 * (t - 1.0));
			}
		}

		inline double Circ(const double t) noexcept
		{
			return (1.0 - std::sqrt(1.0 - t * t));
		}

		constexpr double Back(const double t) noexcept
		{
			return t * t * (2.70158 * t - 1.70158);
		}

		inline double Elastic(const double t) noexcept
		{
			return detail::ElasticAP(t, 1.0, 0.3);
		}

		constexpr double Bounce(double t) noexcept
		{
			t = (1.0 - t);

			if (t < (1 / 2.75))
			{
				return (1.0 - (7.5625 * t * t));
			}
			else if (t < (2 / 2.75))
			{
				t -= 1.5 / 2.75;
				return (1.0 - (7.5625 * t * t + 0.75));
			}
			else if (t < (2.5 / 2.75))
			{
				t -= 2.25 / 2.75;
				return (1.0 - (7.5625 * t * t + 0.9375));
			}
			else
			{
				t -= (2.625 / 2.75);
				return (1.0 - (7.5625 * t * t + 0.984375));
			}
		}

		constexpr double Zero(double) noexcept
		{
			return 0.0;
		}

		constexpr double One(double) noexcept
		{
			return 1.0;
		}
	}

	constexpr double EaseIn(double f(double), const double t) noexcept
	{
		return f(t);
	}

	template <HasLerp Type>
	constexpr auto EaseIn(double f(double), const Type& start, const Type& end, const double t)
	{
		return Math::Lerp(start, end, f(t));
	}

	constexpr double EaseOut(double f(double), const double t) noexcept
	{
		return (1.0 - f(1.0 - t));
	}

	template <HasLerp Type>
	constexpr auto EaseOut(double f(double), const Type& start, const Type& end, const double t)
	{
		return Math::Lerp(start, end, EaseOut(f, t));
	}

	constexpr double EaseInOut(double f(double), const double t) noexcept
	{
		return ((t < 0.5) ? (f(2.0 * t) * 0.5) : (0.5 + EaseOut(f, (2.0 * t - 1.0)) * 0.5));
	}

	template <HasLerp Type>
	constexpr auto EaseInOut(double f(double), const Type& start, const Type& end, const double t)
	{
		return Math::Lerp(start, end, EaseInOut(f, t));
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInLinear, EaseOutLinear, EaseInOutLinear
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInLinear(const double t) noexcept
	{
		return Easing::Linear(t);
	}

	constexpr double EaseOutLinear(const double t) noexcept
	{
		return EaseOut(Easing::Linear, t);
	}

	constexpr double EaseInOutLinear(const double t) noexcept
	{
		return EaseInOut(Easing::Linear, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInSine, EaseOutSine, EaseInOutSine
	//
	////////////////////////////////////////////////////////////////

	inline double EaseInSine(const double t) noexcept
	{
		return Easing::Sine(t);
	}

	inline double EaseOutSine(const double t) noexcept
	{
		return EaseOut(Easing::Sine, t);
	}

	inline double EaseInOutSine(const double t) noexcept
	{
		return EaseInOut(Easing::Sine, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInQuad, EaseOutQuad, EaseInOutQuad
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInQuad(const double t) noexcept
	{
		return Easing::Quad(t);
	}

	constexpr double EaseOutQuad(const double t) noexcept
	{
		return EaseOut(Easing::Quad, t);
	}

	constexpr double EaseInOutQuad(const double t) noexcept
	{
		return EaseInOut(Easing::Quad, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInCubic, EaseOutCubic, EaseInOutCubic
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInCubic(const double t) noexcept
	{
		return Easing::Cubic(t);
	}

	constexpr double EaseOutCubic(const double t) noexcept
	{
		return EaseOut(Easing::Cubic, t);
	}

	constexpr double EaseInOutCubic(const double t) noexcept
	{
		return EaseInOut(Easing::Cubic, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInQuart, EaseOutQuart, EaseInOutQuart
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInQuart(const double t) noexcept
	{
		return Easing::Quart(t);
	}

	constexpr double EaseOutQuart(const double t) noexcept
	{
		return EaseOut(Easing::Quart, t);
	}

	constexpr double EaseInOutQuart(const double t) noexcept
	{
		return EaseInOut(Easing::Quart, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInQuint, EaseOutQuint, EaseInOutQuint
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInQuint(const double t) noexcept
	{
		return Easing::Quint(t);
	}

	constexpr double EaseOutQuint(const double t) noexcept
	{
		return EaseOut(Easing::Quint, t);
	}

	constexpr double EaseInOutQuint(const double t) noexcept
	{
		return EaseInOut(Easing::Quint, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInExpo, EaseOutExpo, EaseInOutExpo
	//
	////////////////////////////////////////////////////////////////

	inline double EaseInExpo(const double t) noexcept
	{
		return Easing::Expo(t);
	}

	inline double EaseOutExpo(const double t) noexcept
	{
		return EaseOut(Easing::Expo, t);
	}

	inline double EaseInOutExpo(const double t) noexcept
	{
		return EaseInOut(Easing::Expo, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInCirc, EaseOutCirc, EaseInOutCirc
	//
	////////////////////////////////////////////////////////////////

	inline double EaseInCirc(const double t) noexcept
	{
		return Easing::Circ(t);
	}

	inline double EaseOutCirc(const double t) noexcept
	{
		return EaseOut(Easing::Circ, t);
	}

	inline double EaseInOutCirc(const double t) noexcept
	{
		return EaseInOut(Easing::Circ, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInBack, EaseOutBack, EaseInOutBack
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInBack(const double t) noexcept
	{
		return Easing::Back(t);
	}

	constexpr double EaseOutBack(const double t) noexcept
	{
		return EaseOut(Easing::Back, t);
	}

	constexpr double EaseInOutBack(const double t) noexcept
	{
		return EaseInOut(Easing::Back, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInElastic, EaseOutElastic, EaseInOutElastic
	//
	////////////////////////////////////////////////////////////////

	inline double EaseInElastic(const double t) noexcept
	{
		return Easing::Elastic(t);
	}

	inline double EaseOutElastic(const double t) noexcept
	{
		return EaseOut(Easing::Elastic, t);
	}

	inline double EaseInOutElastic(const double t) noexcept
	{
		return EaseInOut(Easing::Elastic, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseInBounce, EaseOutBounce, EaseInOutBounce
	//
	////////////////////////////////////////////////////////////////

	constexpr double EaseInBounce(const double t) noexcept
	{
		return Easing::Bounce(t);
	}

	constexpr double EaseOutBounce(const double t) noexcept
	{
		return EaseOut(Easing::Bounce, t);
	}

	constexpr double EaseInOutBounce(const double t) noexcept
	{
		return EaseInOut(Easing::Bounce, t);
	}
}
