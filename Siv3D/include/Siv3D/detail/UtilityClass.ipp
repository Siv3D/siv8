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

namespace s3d::detail
{
	////////////////////////////////////////////////////////////////
	//
	//	Abs
	//
	////////////////////////////////////////////////////////////////

	struct Abs_impl
	{
		[[nodiscard]]
		constexpr auto operator ()(const Concept::Signed auto x) const noexcept
		{
			return ((x < 0) ? -x : x);
		}
	};

	////////////////////////////////////////////////////////////////
	//
	//	FromEnum
	//
	////////////////////////////////////////////////////////////////

	struct FromEnum_impl
	{
		[[nodiscard]]
		constexpr auto operator ()(const Concept::Enum auto x) const noexcept
		{
			return static_cast<std::underlying_type_t<decltype(x)>>(x);
		}
	};
		
	////////////////////////////////////////////////////////////////
	//
	//	ToEnum
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Enum Enum>
	struct ToEnum_impl
	{
		[[nodiscard]]
		SIV3D_MSVC_INTRINSIC
		constexpr auto operator ()(const std::underlying_type_t<Enum> x) const noexcept
		{
			return Enum{ x };
		}
	};
		
	////////////////////////////////////////////////////////////////
	//
	//	IsOdd
	//
	////////////////////////////////////////////////////////////////

	struct IsOdd_impl
	{
		/// @brief 数が奇数であるかを返します。 | Returns true if the number is odd.
		/// @param x 数 | Number
		/// @return 数が奇数の場合 true, それ以外の場合は false | Returns true if the number is odd, false otherwise
		[[nodiscard]]
		constexpr bool operator()(const Concept::Integral auto& x) const noexcept
		{
			return ((x % 2) != 0);
		}
	};
		
	////////////////////////////////////////////////////////////////
	//
	//	IsEven
	//
	////////////////////////////////////////////////////////////////

	struct IsEven_impl
	{
		/// @brief 数が偶数であるかを返します。 | Returns true if the number is even.
		/// @param x 数 | Number
		/// @return 数が偶数の場合 true, それ以外の場合は false | Returns true if the number is even, false otherwise
		[[nodiscard]]
		constexpr bool operator()(const Concept::Integral auto& x) const noexcept
		{
			return ((x % 2) == 0);
		}
	};
		
	////////////////////////////////////////////////////////////////
	//
	//	Identity
	//
	////////////////////////////////////////////////////////////////

	struct Identity_impl
	{
		using is_transparent = void;

		template <class Type>
		[[nodiscard]]
		SIV3D_MSVC_INTRINSIC
		constexpr Type&& operator()(Type&& x) const noexcept
		{
			return std::forward<Type>(x);
		}
	};
}
