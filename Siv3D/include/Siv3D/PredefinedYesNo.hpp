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
	/// @brief 即座に処理を開始するかどうかを指定 | Whether to start processing immediately
	using StartImmediately = YesNo<struct StartImmediately_tag>;

	/// @brief 大文字小文字の区別を保持するかどうかを指定 | Whether to preserve case sensitivity
	using PreserveCase = YesNo<struct PreserveCase_tag>;

	/// @brief サブディレクトリを含め再帰的に処理するかどうかを指定 | Whether to process subdirectories recursively
	using Recursive = YesNo<struct Recursive_tag>;

	/// @brief 操作の取り消しを許可するかどうかを指定 | Whether to allow operation undoing
	using AllowUndo = YesNo<struct AllowUndo_tag>;

	/// @brief 小数点以下の桁数を固定するかどうかを指定 | Whether to fix the number of decimal places
	using FixDecimalPlace = YesNo<struct FixDecimalPlace_tag>;

	/// @brief エラー時に例外を発生させるかどうかを指定 | Whether to throw exceptions on errors
	using AllowExceptions = YesNo<struct AllowExceptions_tag>;

	/// @brief ASCII 文字のみに変換するかどうかを指定 | Whether to convert to ASCII characters only
	using EnsureAscii = YesNo<struct EnsureAscii_tag>;

	/// @brief 乗算済みアルファを使用するかどうかを指定 | Whether to use premultiplied alpha
	using PremultiplyAlpha = YesNo<struct PremultiplyAlpha_tag>;

	/// @brief 乗算済みアルファが使用されているかどうかを指定 | Whether premultiplied alpha is being used
	using IsAlphaPremultiplied = YesNo<struct IsAlphaPremultiplied_tag>;

	/// @brief 要素を中心に配置するかどうかを指定 | Whether to center elements
	using Centering = YesNo<struct Centering_tag>;

	/// @brief カーソル座標を変換するかどうかを指定 | Whether to transform cursor coordinates
	using TransformCursor = YesNo<struct TransformCursor_tag>;

	/// @brief 距離が近い順にソートするかどうかを指定 | Whether to sort by proximity
	using SortByDistance = YesNo<struct SortByDistance_tag>;

	/// @brief 回転を許可するかどうかを指定 | Whether to allow rotation
	using AllowRotation = YesNo<struct AllowRotation_tag>;

	/// @brief 入力の妥当性チェックをスキップするかどうかを指定 | Whether to skip input validation
	using SkipValidation = YesNo<struct SkipValidation_tag>;

	/// @brief 先頭と終端を接続して閉じた輪にするかどうかを指定 | Whether to connect endpoints to form a closed ring
	using CloseRing = YesNo<struct CloseRing_tag>;

	/// @brief 画像の拡大処理を許可するかどうかを指定 | Whether to allow image upscaling
	using AllowUpscale = YesNo<struct AllowUpscale_tag>;

	/// @brief ミップマップを生成するかどうかを指定 | Whether to generate mipmaps
	using GenerateMipmaps = YesNo<struct GenerateMipmaps_tag>;
}
