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
# include <concepts>
# include <ranges>
# include <type_traits>

namespace s3d
{
	namespace Concept
	{
		/// @brief 整数型のコンセプト | Integer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Integral = std::integral<Type>;

		/// @brief 符号付整数型のコンセプト | Signed integer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept SignedIntegral = std::signed_integral<Type>;

		/// @brief 符号なし整数型のコンセプト | Unsigned integer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept UnsignedIntegral = std::unsigned_integral<Type>;

		/// @brief 浮動小数点数型のコンセプト | Floating point type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept FloatingPoint = std::floating_point<Type>;

		/// @brief 符号付数値型のコンセプト | Signed numeric type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Signed = (SignedIntegral<Type> || FloatingPoint<Type>);

		/// @brief 数値型のコンセプト | Numeric type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Arithmetic = (Integral<Type> || FloatingPoint<Type>);

		/// @brief 列挙型のコンセプト | Enum type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Enum = std::is_enum_v<Type>;

		/// @brief スコープ付き列挙型のコンセプト | Scoped enum type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept ScopedEnum = (Enum<Type> && (not std::is_convertible_v<Type, int>));

		/// @brief ポインタ型のコンセプト | Pointer type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Pointer = std::is_pointer_v<Type>;

		/// @brief スカラー型のコンセプト | Scalar type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Scalar = (Arithmetic<Type> || Enum<Type> || Pointer<Type> || std::is_member_pointer_v<Type> || std::is_null_pointer_v<Type>);

		/// @brief トリビアルコピー可能型のコンセプト | Trivially copyable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept TriviallyCopyable = std::is_trivially_copyable_v<Type>;

		/// @brief 一様ランダムビットジェネレータ型のコンセプト | Uniform random bit generator type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept UniformRandomBitGenerator = (std::invocable<Type&> && std::unsigned_integral<std::invoke_result_t<Type&>>);

		/// @brief `<` 演算子による比較が可能な型のコンセプト | Less-than comparable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept LessThanComparable = requires(const Type& a, const Type& b)
		{
			{ a < b } -> std::convertible_to<bool>;
		};

		/// @brief `>` 演算子による比較が可能な型のコンセプト | Greater-than comparable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept GreaterThanComparable = requires(const Type& a, const Type& b)
		{
			{ a > b } -> std::convertible_to<bool>;
		};

		/// @brief `+` 演算子による加算が可能な型のコンセプト | Addable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Addable = requires(const Type& a, const Type& b)
		{
			{ a + b } -> std::convertible_to<Type>;
		};

		/// @brief コンテナに対応した Range のコンセプト | Container compatible range concept
		template <class Range, class Elem>
		concept ContainerCompatibleRange = (std::ranges::input_range<Range> && std::convertible_to<std::ranges::range_reference_t<Range>, Elem>);
	}
}
