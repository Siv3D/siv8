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
	//	Min
	//
	////////////////////////////////////////////////////////////////

	constexpr auto Min(const Concept::Scalar auto a, const Concept::Scalar auto b) noexcept
	{
		return ((b < a) ? b : a);
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr const Type& Min(const Type& a, const Type& b) noexcept(noexcept(b < a))
	{
		return ((b < a) ? b : a);
	}

	template <class Type>
	constexpr Type Min(std::initializer_list<Type> values)
	{
		return *std::min_element(values.begin(), values.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Max
	//
	////////////////////////////////////////////////////////////////

	constexpr auto Max(const Concept::Scalar auto a, const Concept::Scalar auto b) noexcept
	{
		return ((a < b) ? b : a);
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr const Type& Max(const Type& a, const Type& b) noexcept(noexcept(a < b))
	{
		return ((a < b) ? b : a);
	}

	template <class Type>
	constexpr Type Max(std::initializer_list<Type> values)
	{
		return *std::max_element(values.begin(), values.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Clamp
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Scalar Scalar>
	constexpr auto Clamp(const Scalar v, const Scalar min, const Scalar max) noexcept
	{
		if (max < v)
		{
			return max;
		}

		if (v < min)
		{
			return min;
		}

		return v;
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr const Type& Clamp(const Type& v, const Type& min, const Type& max) noexcept(noexcept(max < v))
	{
		if (max < v)
		{
			return max;
		}

		if (v < min)
		{
			return min;
		}

		return v;
	}

	////////////////////////////////////////////////////////////////
	//
	//	InRange
	//
	////////////////////////////////////////////////////////////////

	constexpr bool InRange(const Concept::Scalar auto v, const Concept::Scalar auto min, const Concept::Scalar auto max) noexcept
	{
		return ((min <= v) && (v <= max));
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr bool InRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min))
	{
		return ((min <= v) && (v <= max));
	}

	////////////////////////////////////////////////////////////////
	//
	//	InOpenRange
	//
	////////////////////////////////////////////////////////////////

	constexpr bool InOpenRange(const Concept::Scalar auto v, const Concept::Scalar auto min, const Concept::Scalar auto max) noexcept
	{
		return ((min < v) && (v < max));
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr bool InOpenRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min))
	{
		return ((min < v) && (v < max));
	}

	////////////////////////////////////////////////////////////////
	//
	//	AbsDiff
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Arithmetic Arithmetic>
	constexpr auto AbsDiff(const Arithmetic a, const Arithmetic b) noexcept
	{
		if constexpr (std::is_integral_v<Arithmetic>)
		{
			using U = std::make_unsigned_t<Arithmetic>;
			return ((a > b) ? (static_cast<U>(a) - static_cast<U>(b))
				: (static_cast<U>(b) - static_cast<U>(a)));
		}
		else
		{
			return detail::Abs_impl{}(a - b);
		}
	}
}
