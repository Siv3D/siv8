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
# include "Common.hpp"

namespace s3d
{
	/// @brief 多項式方程式の実数解を格納する固定容量コンテナです。
	/// @remark 3 次以下の方程式で使うため、最大 3 個の有限実数解を保持します。
	/// @remark hasInfiniteSolutions が true の場合、恒等式により実数全体が解です。このとき count は 0 です。
	struct PolynomialRoots
	{
		/// @brief 有限実数解の配列です。先頭 count 個が有効です。
		double roots[3] = {};

		/// @brief 有限実数解の個数です。
		uint32 count = 0;

		/// @brief 実数全体が解である場合 true です。
		bool hasInfiniteSolutions = false;

		/// @brief 実数全体が解である結果を返します。
		/// @return 実数全体が解である結果
		[[nodiscard]]
		static constexpr PolynomialRoots Infinite() noexcept
		{
			PolynomialRoots result;
			result.hasInfiniteSolutions = true;
			return result;
		}

		/// @brief 有限実数解の個数を返します。
		/// @return 有限実数解の個数
		[[nodiscard]]
		constexpr uint32 size() const noexcept
		{
			return count;
		}

		/// @brief 有限実数解を持たないかを返します。
		/// @return 有限実数解を持たない場合 true
		[[nodiscard]]
		constexpr bool empty() const noexcept
		{
			return (count == 0);
		}

		/// @brief 有限実数解を持たないかを返します。
		/// @return 有限実数解を持たない場合 true
		[[nodiscard]]
		constexpr bool isEmpty() const noexcept
		{
			return (count == 0);
		}

		/// @brief 有限実数解、または無限個の解を持つかを返します。
		/// @return 有限実数解、または無限個の解を持つ場合 true
		[[nodiscard]]
		explicit constexpr operator bool() const noexcept
		{
			return ((0 < count) || hasInfiniteSolutions);
		}

		/// @brief 指定したインデックスの有限実数解を返します。
		/// @param index インデックス
		/// @return 指定したインデックスの有限実数解
		[[nodiscard]]
		constexpr double operator [](const size_t index) const noexcept
		{
			return roots[index];
		}

		/// @brief 指定したインデックスの有限実数解への参照を返します。
		/// @param index インデックス
		/// @return 指定したインデックスの有限実数解への参照
		[[nodiscard]]
		constexpr double& operator [](const size_t index) noexcept
		{
			return roots[index];
		}

		/// @brief 有限実数解の先頭を指すポインタを返します。
		/// @return 有限実数解の先頭を指すポインタ
		[[nodiscard]]
		constexpr const double* begin() const noexcept
		{
			return roots;
		}

		/// @brief 有限実数解の末尾の次を指すポインタを返します。
		/// @return 有限実数解の末尾の次を指すポインタ
		[[nodiscard]]
		constexpr const double* end() const noexcept
		{
			return (roots + count);
		}

		/// @brief 有限実数解の先頭を指すポインタを返します。
		/// @return 有限実数解の先頭を指すポインタ
		[[nodiscard]]
		constexpr double* begin() noexcept
		{
			return roots;
		}

		/// @brief 有限実数解の末尾の次を指すポインタを返します。
		/// @return 有限実数解の末尾の次を指すポインタ
		[[nodiscard]]
		constexpr double* end() noexcept
		{
			return (roots + count);
		}
	};
}
