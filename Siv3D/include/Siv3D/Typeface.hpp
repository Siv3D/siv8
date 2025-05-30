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
# include "Common.hpp"

namespace s3d
{
	enum class Typeface : uint8
	{
		///// @brief 日本向けの　CJK フォント
		//CJK_Regular_JP,

		///// @brief 韓国向けの CJK フォント
		//CJK_Regular_KR,

		///// @brief 中国向けの CJK フォント
		//CJK_Regular_SC,

		///// @brief 台湾向けの CJK フォント
		//CJK_Regular_TC,

		///// @brief 香港向けの CJK フォント
		//CJK_Regular_HK,

		///// @brief モノクロ絵文字フォント
		//MonochromeEmoji,

		/// @brief カラー絵文字フォント
		ColorEmoji,

		/// @brief 非常に細いサイズの M PLUS 2 フォント
		MPlus2_Thin,

		/// @brief 細いサイズの M PLUS 2 フォント	
		MPlus2_ExtraLight,

		/// @brief やや細いサイズの M PLUS 2 フォント	
		MPlus2_Light,

		/// @brief 通常サイズの M PLUS 2 フォント	
		MPlus2_Regular,

		/// @brief 少し太いサイズの M PLUS 2 フォント	
		MPlus2_Medium,

		/// @brief やや太いサイズの M PLUS 2 フォント	
		MPlus2_SemiBold,

		/// @brief 太いサイズの M PLUS 2 フォント
		MPlus2_Bold,

		/// @brief とても太いサイズの M PLUS 2 フォント
		MPlus2_ExtraBold,

		/// @brief 最も太いサイズの M PLUS 2 フォント	
		MPlus2_Black,

		///// @brief マテリアルデザイン・アイコンフォント
		//Icon_MaterialDesign,

		/// @brief 非常に細いサイズの M PLUS 2 フォント
		Thin		= MPlus2_Thin,

		/// @brief 細いサイズの M PLUS 2 フォント	
		ExtraLight	= MPlus2_ExtraLight,

		/// @brief やや細いサイズの M PLUS 2 フォント
		Light		= MPlus2_Light,

		/// @brief 通常サイズの M PLUS 2 フォント
		Regular		= MPlus2_Regular,

		/// @brief 少し太いサイズの M PLUS 2 フォント
		Medium		= MPlus2_Medium,

		/// @brief やや太いサイズの M PLUS 2 フォント	
		SemiBold	= MPlus2_SemiBold,

		/// @brief 太いサイズの M PLUS 2 フォント
		Bold		= MPlus2_Bold,

		/// @brief とても太いサイズの M PLUS 2 フォント
		Heavy		= MPlus2_ExtraBold,

		/// @brief 最も太いサイズの M PLUS 2 フォント
		Black		= MPlus2_Black,
	};
}
