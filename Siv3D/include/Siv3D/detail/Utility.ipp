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

	////////////////////////////////////////////////////////////////
	//
	//	LexicographicalCompareThreeWay
	//
	////////////////////////////////////////////////////////////////

   template <class InputIt1, class InputIt2, class Cmp>
   constexpr auto LexicographicalCompareThreeWay(InputIt1 f1, InputIt1 l1, InputIt2 f2, InputIt2 l2, Cmp comp)
	   -> decltype(comp(*f1, *f2))
   {
	   bool exhaust1 = (f1 == l1);
	   bool exhaust2 = (f2 == l2);
	   
	   for (; (not exhaust1) && (not exhaust2); exhaust1 = (++f1 == l1), exhaust2 = (++f2 == l2))
	   {
		   if (auto c = comp(*f1, *f2); c != 0)
		   {
			   return c;
		   }
	   }
	   
	   return ((not exhaust1) ? std::strong_ordering::greater:
			  (not exhaust2) ? std::strong_ordering::less:
						  std::strong_ordering::equal);
   }
}
