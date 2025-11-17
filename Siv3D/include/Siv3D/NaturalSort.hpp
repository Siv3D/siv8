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
# include "StringView.hpp"

namespace s3d
{
	/// @brief 自然順で文字列を比較します。
	/// @param lhs 一方の文字列
	/// @param rhs もう一方の文字列
	/// @return lhs が rhs より小さい場合は負の値, 等しい場合は 0, lhs が rhs より大きい場合は正の値
	int32 NaturalSortCompare(StringView lhs, StringView rhs) noexcept;

	/// @brief 自然順で文字列を比較するための関数オブジェクト
	struct NaturalSortLess
	{
		/// @brief 自然順で文字列を比較します。
		/// @param lhs 一方の文字列
		/// @param rhs もう一方の文字列
		/// @return lhs が rhs より小さい場合 true, それ以外の場合は false
		static bool operator()(StringView lhs, StringView rhs) noexcept;
	};
}
