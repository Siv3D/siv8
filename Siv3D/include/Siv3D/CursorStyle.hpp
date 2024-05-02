//-----------------------------------------------
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
# include "Common.hpp"
# include "PointVector.hpp"

namespace s3d
{
	class Image;

	////////////////////////////////////////////////////////////////
	//
	//	CursorStyle
	//
	////////////////////////////////////////////////////////////////

	/// @brief マウスカーソルの形状
	enum class CursorStyle : uint8
	{
		/// @brief 矢印
		Arrow,

		/// @brief I 形
		IBeam,

		/// @brief 十字型
		Cross,

		/// @brief 手のアイコン
		Hand,

		/// @brief 禁止のアイコン
		NotAllowed,

		/// @brief 上下のリサイズ
		ResizeUpDown,

		/// @brief 左右のリサイズ
		ResizeLeftRight,

		/// @brief 左上-右下のリサイズ
		ResizeNWSE,

		/// @brief 右上-左下のリサイズ
		ResizeNESW,

		/// @brief 上下左右方向のリサイズ
		ResizeAll,

		/// @brief 非表示
		Hidden,

		/// @brief デフォルト（矢印）
		Default = Arrow,
	};

	namespace Cursor
	{
		////////////////////////////////////////////////////////////////
		//
		//	SetDefaultStyle
		//
		////////////////////////////////////////////////////////////////

		/// @brief アプリケーションのデフォルトのマウスカーソルを設定します。
		/// @param style  カーソルのスタイル
		void SetDefaultStyle(CursorStyle style);

		////////////////////////////////////////////////////////////////
		//
		//	RegisterCustomCursorStyle
		//
		////////////////////////////////////////////////////////////////

		/// @brief カスタムのマウスカーソルを登録します。
		/// @param name カスタムマウスカーソルの名前
		/// @param image カスタムマウスカーソルの画像
		/// @param hotSpot 画像中のクリック位置
		/// @return カスタムのマウスカーソルの登録に成功した場合 true, それ以外の場合は false
		bool RegisterCustomCursorStyle(StringView name, const Image& image, Point hotSpot = Point{ 0, 0 });

		////////////////////////////////////////////////////////////////
		//
		//	RequestStyle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のフレームのマウスカーソルを、指定したスタイルにするようリクエストします。
		/// @param style カーソルのスタイル
		void RequestStyle(CursorStyle style);

		/// @brief 現在のフレームのマウスカーソルを、指定したカスタムマウスカーソルにするようリクエストします。
		/// @param name カスタムマウスカーソルの名前
		void RequestStyle(StringView name);
	}
}
