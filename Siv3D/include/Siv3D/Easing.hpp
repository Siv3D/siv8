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
# include "Common.hpp"
# include "MathConstants.hpp"
# include "Interpolation.hpp"

namespace s3d
{
	namespace Easing
	{
		[[nodiscard]]
		constexpr double Linear(double t) noexcept;

		[[nodiscard]]
		double Sine(double t) noexcept;

		[[nodiscard]]
		constexpr double Quad(double t) noexcept;

		[[nodiscard]]
		constexpr double Cubic(double t) noexcept;

		[[nodiscard]]
		constexpr double Quart(double t) noexcept;

		[[nodiscard]]
		constexpr double Quint(double t) noexcept;

		[[nodiscard]]
		double Expo(double t) noexcept;

		[[nodiscard]]
		double Circ(double t) noexcept;

		[[nodiscard]]
		constexpr double Back(double t) noexcept;

		[[nodiscard]]
		double Elastic(double t) noexcept;

		[[nodiscard]]
		constexpr double Bounce(double t) noexcept;

		[[nodiscard]]
		constexpr double Zero(double) noexcept;

		[[nodiscard]]
		constexpr double One(double) noexcept;
	}

	////////////////////////////////////////////////////////////////
	//
	//	EaseIn
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseIn(double f(double), double t) noexcept;

	template <HasLerp Type>
	[[nodiscard]]
	constexpr auto EaseIn(double f(double), const Type& start, const Type& end, double t);

	////////////////////////////////////////////////////////////////
	//
	//	EaseOut
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseOut(double f(double), double t) noexcept;

	template <HasLerp Type>
	[[nodiscard]]
	constexpr auto EaseOut(double f(double), const Type& start, const Type& end, double t);

	////////////////////////////////////////////////////////////////
	//
	//	EaseInOut
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInOut(double f(double), double t) noexcept;

	template <HasLerp Type>
	[[nodiscard]]
	constexpr auto EaseInOut(double f(double), const Type& start, const Type& end, double t);

	////////////////////////////////////////////////////////////////
	//
	//	EaseInLinear, EaseOutLinear, EaseInOutLinear
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInLinear(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutLinear(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutLinear(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInSine, EaseOutSine, EaseInOutSine
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	double EaseInSine(double t) noexcept;

	[[nodiscard]]
	double EaseOutSine(double t) noexcept;

	[[nodiscard]]
	double EaseInOutSine(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInQuad, EaseOutQuad, EaseInOutQuad
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInQuad(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutQuad(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutQuad(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInCubic, EaseOutCubic, EaseInOutCubic
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInCubic(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutCubic(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutCubic(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInQuart, EaseOutQuart, EaseInOutQuart
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInQuart(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutQuart(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutQuart(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInQuint, EaseOutQuint, EaseInOutQuint
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInQuint(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutQuint(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutQuint(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInExpo, EaseOutExpo, EaseInOutExpo
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	double EaseInExpo(double t) noexcept;

	[[nodiscard]]
	double EaseOutExpo(double t) noexcept;

	[[nodiscard]]
	double EaseInOutExpo(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInCirc, EaseOutCirc, EaseInOutCirc
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	double EaseInCirc(double t) noexcept;

	[[nodiscard]]
	double EaseOutCirc(double t) noexcept;

	[[nodiscard]]
	double EaseInOutCirc(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInBack, EaseOutBack, EaseInOutBack
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInBack(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutBack(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutBack(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInElastic, EaseOutElastic, EaseInOutElastic
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	double EaseInElastic(double t) noexcept;

	[[nodiscard]]
	double EaseOutElastic(double t) noexcept;

	[[nodiscard]]
	double EaseInOutElastic(double t) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	EaseInBounce, EaseOutBounce, EaseInOutBounce
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr double EaseInBounce(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseOutBounce(double t) noexcept;

	[[nodiscard]]
	constexpr double EaseInOutBounce(double t) noexcept;
}

# include "detail/Easing.ipp"
