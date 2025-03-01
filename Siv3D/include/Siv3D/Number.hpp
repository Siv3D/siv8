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
# include <limits>
# include <cmath>
# include "Common.hpp"
# include "Concepts.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Largest
	//
	////////////////////////////////////////////////////////////////

	/// @brief 表現できる最大の数値
	/// @tparam Type 数値型
	template <class Type>
	constexpr Type Largest = std::numeric_limits<Type>::max();

	////////////////////////////////////////////////////////////////
	//
	//	Smallest
	//
	////////////////////////////////////////////////////////////////

	/// @brief 表現できる最小の数値
	/// @tparam Type 数値型
	template <class Type>
	constexpr Type Smallest = std::numeric_limits<Type>::lowest();

	////////////////////////////////////////////////////////////////
	//
	//	Inf
	//
	////////////////////////////////////////////////////////////////

	/// @brief 正の無限大
	template <Concept::FloatingPoint Float>
	constexpr Float Inf = std::numeric_limits<Float>::infinity();

	////////////////////////////////////////////////////////////////
	//
	//	QNaN
	//
	////////////////////////////////////////////////////////////////

	/// @brief qNaN
	template <Concept::FloatingPoint Float>
	constexpr Float QNaN = std::numeric_limits<Float>::quiet_NaN();

	////////////////////////////////////////////////////////////////
	//
	//	SNaN
	//
	////////////////////////////////////////////////////////////////

	/// @brief sNaN
	template <Concept::FloatingPoint Float>
	constexpr Float SNaN = std::numeric_limits<Float>::signaling_NaN();

	////////////////////////////////////////////////////////////////
	//
	//	IsNaN
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数値が非数 (NaN) であるかを示します。
	/// @param value 数値
	/// @remark 無限は非数ではありません。
	/// @return 数値が非数 (NaN) である場合 true, それ以外の場合は false
	[[nodiscard]]
	bool IsNaN(Concept::FloatingPoint auto value) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	IsFinite
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数値が有限値であるかを示します。
	/// @param value 数値
	/// @return 数値が有限値である場合 true, それ以外の場合は false
	[[nodiscard]]
	bool IsFinite(Concept::FloatingPoint auto value) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	IsInfinity
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数値が無限であるかを示します。
	/// @param value 数値
	/// @return 数値が無限である場合 true, それ以外の場合は false
	[[nodiscard]]
	bool IsInfinity(Concept::FloatingPoint auto value) noexcept;
}

# include "detail/Number.ipp"
