//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
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
		/// @brief 整数型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept Integral = std::integral<Type>;

		/// @brief 符号付整数型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept SignedIntegral = std::signed_integral<Type>;

		/// @brief 符号なし整数型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept UnsignedIntegral = std::unsigned_integral<Type>;

		/// @brief 浮動小数点数型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept FloatingPoint = std::floating_point<Type>;

		/// @brief 符号付数値型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept Signed = std::is_signed_v<Type>;

		/// @brief 数値型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept Arithmetic = std::is_arithmetic_v<Type>;

		/// @brief スカラー型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept Scalar = std::is_scalar_v<Type>;

		/// @brief 非スカラー型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept NonScalar = std::negation_v<std::is_scalar<Type>>;

		/// @brief 列挙型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept Enum = std::is_enum_v<Type>;

		/// @brief スコープ付き列挙型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept ScopedEnum = std::conjunction_v<std::is_enum<Type>, std::negation<std::is_convertible<Type, int>>>;

		/// @brief トリビアルコピー可能型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept TriviallyCopyable = std::is_trivially_copyable_v<Type>;

		/// @brief 一様ランダムビットジェネレータ型のコンセプト
		/// @tparam Type 型
		template <class Type>
		concept UniformRandomBitGenerator = (std::invocable<Type&> && std::unsigned_integral<std::invoke_result_t<Type&>>);
	}
}
