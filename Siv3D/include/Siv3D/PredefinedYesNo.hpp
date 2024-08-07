﻿//-----------------------------------------------
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
# include "YesNo.hpp"

namespace s3d
{
	/// @brief 即座に開始する | Start immediately
	using StartImmediately		= YesNo<struct StartImmediately_tag>;

	/// @brief 大文字小文字を保持する | Preserve case
	using PreserveCase			= YesNo<struct PreserveCase_tag>;

	/// @brief 再帰的に処理する
	using Recursive				= YesNo<struct Recursive_tag>;

	/// @brief 取り消しを許可する | Allow undo
	using AllowUndo				= YesNo<struct AllowUndo_tag>;

	/// @brief 固定されている | Fixed
	using Fixed					= YesNo<struct Fixed_tag>;

	/// @brief 例外を発生させる
	using AllowExceptions		= YesNo<struct AllowExceptions_tag>;
	
	/// @brief アスキー文字のみになるよう変換する
	using EnsureAscii			= YesNo<struct EnsureAscii_tag>;

	/// @brief 乗算済みアルファを使用する
	using PremultiplyAlpha		= YesNo<struct PremultiplyAlpha_tag>;

	/// @brief 乗算済みアルファが使用されている
	using AlphaPremultiplied	= YesNo<struct AlphaPremultiplied_tag>;

	/// @brief 中心にそろえる
	using Centering				= YesNo<struct Centering_tag>;

	/// @brief カーソルの座標を変換する
	using TransformCursor		= YesNo<struct TransformCursor_tag>;
	
	/// @brief 距離が近い順にソートする
	using SortByDistance		= YesNo<struct SortByDistance_tag>;
	
}
