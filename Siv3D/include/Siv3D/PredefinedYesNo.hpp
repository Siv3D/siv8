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
# include "YesNo.hpp"

namespace s3d
{
	/// @brief 即座に開始するか | Start immediately
	using StartImmediately		= YesNo<struct StartImmediately_tag>;

	/// @brief 大文字小文字を保持するか | Preserve case
	using PreserveCase			= YesNo<struct PreserveCase_tag>;

	/// @brief 再帰的に処理するか | Process recursively
	using Recursive				= YesNo<struct Recursive_tag>;

	/// @brief 取り消しを許可するか | Allow undo
	using AllowUndo				= YesNo<struct AllowUndo_tag>;

	/// @brief 固定されているか | Fixed
	using Fixed					= YesNo<struct Fixed_tag>;

	/// @brief 例外を発生させるか | Raise exceptions
	using AllowExceptions		= YesNo<struct AllowExceptions_tag>;
	
	/// @brief アスキー文字のみになるよう変換するか | Convert to ASCII characters
	using EnsureAscii			= YesNo<struct EnsureAscii_tag>;

	/// @brief 乗算済みアルファを使用するか | Use premultiplied alpha
	using PremultiplyAlpha		= YesNo<struct PremultiplyAlpha_tag>;

	/// @brief 乗算済みアルファが使用されているか | Premultiplied alpha is used
	using AlphaPremultiplied	= YesNo<struct AlphaPremultiplied_tag>;

	/// @brief 中心にそろえるか | Centering
	using Centering				= YesNo<struct Centering_tag>;

	/// @brief カーソルの座標を変換するか | Convert cursor coordinates
	using TransformCursor		= YesNo<struct TransformCursor_tag>;
	
	/// @brief 距離が近い順にソートするか | Sort by distance
	using SortByDistance		= YesNo<struct SortByDistance_tag>;

	/// @brief 回転を許可するか | Allow rotation
	using AllowFlip				= YesNo<struct AllowFlip_tag>;

	/// @brief 妥当性をチェックするか | Skip validation
	using SkipValidation		= YesNo<struct SkipValidation_tag>;

	/// @brief 先頭と終端をつなげて輪を閉じるか | Close ring
	using CloseRing				= YesNo<struct CloseRing_tag>;
	
	/// @brief 画像の拡大を許可するか | Allow upscaling
	using AllowUpscale			= YesNo<struct AllowUpscale_tag>;
}
