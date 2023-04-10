//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <concepts>

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
		concept Signed = std::is_signed_v<Type>;

		/// @brief 数値型のコンセプト | Numeric type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Arithmetic = std::is_arithmetic_v<Type>;

		/// @brief スカラー型のコンセプト | Scalar type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Scalar = std::is_scalar_v<Type>;

		/// @brief 非スカラー型のコンセプト | Non-scalar type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept NonScalar = std::negation_v<std::is_scalar<Type>>;

		/// @brief 列挙型のコンセプト | Enum type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept Enum = std::is_enum_v<Type>;

		/// @brief スコープ付き列挙型のコンセプト | Scoped enum type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept ScopedEnum = std::is_scoped_enum_v<Type>;

		/// @brief トリビアルコピー可能型のコンセプト | Trivially copyable type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept TriviallyCopyable = std::is_trivially_copyable_v<Type>;

		/// @brief 一様ランダムビットジェネレータ型のコンセプト | Uniform random bit generator type concept
		/// @tparam Type 型 | Type
		template <class Type>
		concept UniformRandomBitGenerator = (std::invocable<Type&> && std::unsigned_integral<std::invoke_result_t<Type&>>);
	}
}
