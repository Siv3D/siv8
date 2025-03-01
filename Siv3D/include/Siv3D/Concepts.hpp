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
# include <concepts>
# include <type_traits>

namespace s3d
{
	namespace Concept
	{
		////////////////////////////////////////////////////////////////
		//
		//	Integral
		//
		////////////////////////////////////////////////////////////////

		/// @brief 整数型のコンセプト | Integer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Integral = std::integral<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	SignedIntegral
		//
		////////////////////////////////////////////////////////////////

		/// @brief 符号付整数型のコンセプト | Signed integer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept SignedIntegral = std::signed_integral<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	UnsignedIntegral
		//
		////////////////////////////////////////////////////////////////

		/// @brief 符号なし整数型のコンセプト | Unsigned integer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept UnsignedIntegral = std::unsigned_integral<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	FloatingPoint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 浮動小数点数型のコンセプト | Floating point type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept FloatingPoint = std::floating_point<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	Signed
		//
		////////////////////////////////////////////////////////////////

		/// @brief 符号付数値型のコンセプト | Signed numeric type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Signed = (SignedIntegral<Type> || FloatingPoint<Type>);

		////////////////////////////////////////////////////////////////
		//
		//	Arithmetic
		//
		////////////////////////////////////////////////////////////////

		/// @brief 数値型のコンセプト | Numeric type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Arithmetic = (Integral<Type> || FloatingPoint<Type>);

		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		/// @brief 列挙型のコンセプト | Enum type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Enum = std::is_enum_v<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	ScopedEnum
		//
		////////////////////////////////////////////////////////////////

		/// @brief スコープ付き列挙型のコンセプト | Scoped enum type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept ScopedEnum = (Enum<Type> && (not std::is_convertible_v<Type, int>));

		////////////////////////////////////////////////////////////////
		//
		//	Pointer
		//
		////////////////////////////////////////////////////////////////

		/// @brief ポインタ型のコンセプト | Pointer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Pointer = std::is_pointer_v<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	Scalar
		//
		////////////////////////////////////////////////////////////////

		/// @brief スカラー型のコンセプト | Scalar type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Scalar = (Arithmetic<Type> || Enum<Type> || Pointer<Type> || std::is_member_pointer_v<Type> || std::is_null_pointer_v<Type>);

		////////////////////////////////////////////////////////////////
		//
		//	TriviallyCopyable
		//
		////////////////////////////////////////////////////////////////

		/// @brief トリビアルコピー可能型のコンセプト | Trivially copyable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept TriviallyCopyable = std::is_trivially_copyable_v<Type>;

		////////////////////////////////////////////////////////////////
		//
		//	UniformRandomBitGenerator
		//
		////////////////////////////////////////////////////////////////

		/// @brief 一様ランダムビットジェネレータ型のコンセプト | Uniform random bit generator type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept UniformRandomBitGenerator = (std::invocable<Type&> && std::unsigned_integral<std::invoke_result_t<Type&>>);

		////////////////////////////////////////////////////////////////
		//
		//	LessThanComparable
		//
		////////////////////////////////////////////////////////////////

		/// @brief `<` 演算子による比較が可能な型のコンセプト | Less-than comparable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept LessThanComparable = requires(const Type& a, const Type& b)
		{
			{ a < b } -> std::convertible_to<bool>;
		};

		////////////////////////////////////////////////////////////////
		//
		//	GreaterThanComparable
		//
		////////////////////////////////////////////////////////////////

		/// @brief `>` 演算子による比較が可能な型のコンセプト | Greater-than comparable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept GreaterThanComparable = requires(const Type& a, const Type& b)
		{
			{ a > b } -> std::convertible_to<bool>;
		};

		////////////////////////////////////////////////////////////////
		//
		//	Addable
		//
		////////////////////////////////////////////////////////////////

		/// @brief `+` 演算子による加算が可能な型のコンセプト | Addable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Addable = requires(const Type& a, const Type& b)
		{
			{ a + b } -> std::convertible_to<Type>;
		};

		////////////////////////////////////////////////////////////////
		//
		//	AddAssignable
		//
		////////////////////////////////////////////////////////////////

		/// @brief `+=` 演算子による加算が可能な型のコンセプト | Add-assignable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept AddAssignable = requires(Type& a, const Type & b)
		{
			{ a += b } -> std::convertible_to<Type>;
		};
	}
}
