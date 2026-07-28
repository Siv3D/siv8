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
	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	Lerp
		//
		////////////////////////////////////////////////////////////////

		constexpr auto Lerp(const Concept::Arithmetic auto v1, const Concept::Arithmetic auto v2, const Concept::FloatingPoint auto f) noexcept
		{
			using Ret = decltype(v1 + (v2 - v1) * f);

			const Ret v1F = static_cast<Ret>(v1);
			const Ret v2F = static_cast<Ret>(v2);
			const Ret fF = static_cast<Ret>(f);

			return (v1F + (v2F - v1F) * fF);
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

			const Ret fromF = static_cast<Ret>(from);
			const Ret toF = static_cast<Ret>(to);
			const Ret diff = std::fmod((toF - fromF), Math::TwoPi_v<Ret>);

			return (fromF + (std::fmod((2 * diff), Math::TwoPi_v<Ret>) - diff) * t);
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
			const Ret ratio = (static_cast<Ret>(b) / static_cast<Ret>(a));
			assert(0 <= ratio);

			return (a * std::pow(ratio, t));
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
			if (maxSpeed <= 0.0f)
			{
				return current;
			}

			return (current + Clamp((target - current), -maxSpeed, maxSpeed));
		}

		constexpr double MoveTowards(const double current, const double target, const double maxSpeed) noexcept
		{
			if (maxSpeed <= 0.0)
			{
				return current;
			}

			return (current + Clamp((target - current), -maxSpeed, maxSpeed));
		}
	}
}
