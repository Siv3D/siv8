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
	enum class Typeface : uint8
	{
		/// @brief 日本向けの　CJK フォント
		CJK_Regular_JP,

		/// @brief 韓国向けの CJK フォント
		CJK_Regular_KR,

		/// @brief 中国向けの CJK フォント
		CJK_Regular_SC,

		/// @brief 台湾向けの CJK フォント
		CJK_Regular_TC,

		/// @brief 香港向けの CJK フォント
		CJK_Regular_HK,

		/// @brief カラー絵文字フォント
		ColorEmoji,

		/// @brief 非常に細いサイズの M+ 1p フォント
		Mplus_Thin,

		/// @brief やや細いサイズの M+ 1p フォント	
		Mplus_Light,

		/// @brief 通常サイズの M+ 1p フォント	
		Mplus_Regular,

		/// @brief 少し太いサイズの M+ 1p フォント	
		Mplus_Medium,

		/// @brief 太いサイズの M+ 1p フォント
		Mplus_Bold,

		/// @brief とても太いサイズの M+ 1p フォント
		Mplus_Heavy,

		/// @brief 最も太いサイズの M+ 1p フォント	
		Mplus_Black,

		///// @brief マテリアルデザイン・アイコンフォント
		Icon_MaterialDesign,

		/// @brief 非常に細いサイズの M+ 1p フォント
		Thin		= Mplus_Thin,

		/// @brief やや細いサイズの M+ 1p フォント
		Light		= Mplus_Light,

		/// @brief 通常サイズの M+ 1p フォント
		Regular		= Mplus_Regular,

		/// @brief 少し太いサイズの M+ 1p フォント
		Medium		= Mplus_Medium,

		/// @brief 太いサイズの M+ 1p フォント
		Bold		= Mplus_Bold,

		/// @brief とても太いサイズの M+ 1p フォント
		Heavy		= Mplus_Heavy,

		/// @brief 最も太いサイズの M+ 1p フォント
		Black		= Mplus_Black,
	};
}
