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
	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	Lerp
		//
		////////////////////////////////////////////////////////////////

		constexpr auto Lerp(const Concept::Arithmetic auto v1, const Concept::Arithmetic auto v2, const Concept::FloatingPoint auto f) noexcept
		{
			return (v1 + (v2 - v1) * f);
		}

		template <HasLerp Type>
		constexpr auto Lerp(const Type& v1, const Type& v2, const Concept::FloatingPoint auto f) noexcept
		{
			return v1.lerp(v2, f);
		}

		////////////////////////////////////////////////////////////////
		//
		//	InvLerp
		//
		////////////////////////////////////////////////////////////////

		constexpr auto InvLerp(const Concept::FloatingPoint auto a, const Concept::FloatingPoint auto b, const Concept::FloatingPoint auto value) noexcept
		{
			return ((value - a) / (b - a));
		}

		////////////////////////////////////////////////////////////////
		//
		//	LerpAngle
		//
		////////////////////////////////////////////////////////////////

		auto LerpAngle(const Concept::Arithmetic auto from, const Concept::Arithmetic auto to, const Concept::FloatingPoint auto t) noexcept
		{
			using Ret = CommonFloat_t<decltype(from), decltype(to)>;

			const auto diff = std::fmod((to - from), Math::TwoPi_v<Ret>);

			return (from + (std::fmod((2 * diff), Math::TwoPi_v<Ret>) - diff) * t);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Eerp
		//
		////////////////////////////////////////////////////////////////

		auto Eerp(const Concept::Arithmetic auto a, const Concept::Arithmetic auto b, const Concept::FloatingPoint auto t) noexcept
		{
			using Ret = CommonFloat_t<decltype(a), decltype(b)>;

			assert(a != 0);
			assert(0 <= (b / a));

			return (a * std::pow((static_cast<Ret>(b) / a), t));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Map
		//
		////////////////////////////////////////////////////////////////

		constexpr auto Map(const double value, const double a, const double b, const Concept::Arithmetic auto v1, const Concept::Arithmetic auto v2) noexcept
		{
			return Lerp(v1, v2, InvLerp(a, b, value));
		}

		template <HasLerp Type>
		constexpr auto Map(const double value, const double a, const double b, const Type& v1, const Type& v2) noexcept
		{
			return Lerp(v1, v2, InvLerp(a, b, value));
		}

		////////////////////////////////////////////////////////////////
		//
		//	MoveTowards
		//
		////////////////////////////////////////////////////////////////

		constexpr float MoveTowards(const float current, const float target, const float maxSpeed) noexcept
		{
			return (current + Clamp((target - current), -maxSpeed, maxSpeed));
		}

		constexpr double MoveTowards(const double current, const double target, const double maxSpeed) noexcept
		{
			return (current + Clamp((target - current), -maxSpeed, maxSpeed));
		}
	}
}
