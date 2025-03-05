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
# include "PointVector.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	class Image;

	////////////////////////////////////////////////////////////////
	//
	//	CursorStyle
	//
	////////////////////////////////////////////////////////////////

	/// @brief マウスカーソルの形状 | Cursor style
	enum class CursorStyle : uint8
	{
		/// @brief 矢印 (デフォルト) | Arrow (default)
		Arrow,

		/// @brief I 形 | I-beam
		IBeam,

		/// @brief 十字型 | Cross
		Cross,

		/// @brief 手のアイコン | Hand
		Hand,

		/// @brief 禁止のアイコン | Not allowed
		NotAllowed,

		/// @brief 上下のリサイズ | Resize up-down
		ResizeUpDown,

		/// @brief 左右のリサイズ | Resize left-right
		ResizeLeftRight,

		/// @brief 左上-右下のリサイズ | Resize top-left to bottom-right
		ResizeNWSE,

		/// @brief 右上-左下のリサイズ | Resize top-right to bottom-left
		ResizeNESW,

		/// @brief 上下左右方向のリサイズ | Resize all directions
		ResizeAll,

		/// @brief 非表示 | Hidden
		Hidden,

		/// @brief デフォルト（矢印） | Default (Arrow)
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
		//	Register
		//
		////////////////////////////////////////////////////////////////

		/// @brief カスタムのマウスカーソルを登録します。
		/// @param name カスタムマウスカーソルの名前
		/// @param image カスタムマウスカーソルの画像
		/// @param hotSpot 画像中のクリック位置
		/// @param isAlphaPremultiplied 画像がアルファ乗算済みの場合は `IsAlphaPremultiplied::Yes`, それ以外の場合は `IsAlphaPremultiplied::No`
		/// @return カスタムのマウスカーソルの登録に成功した場合 true, それ以外の場合は false
		bool Register(StringView name, const Image& image, Point hotSpot = Point{ 0, 0 }, IsAlphaPremultiplied isAlphaPremultiplied = IsAlphaPremultiplied::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	Unregister
		//
		////////////////////////////////////////////////////////////////

		/// @brief カスタムのマウスカーソルを登録解除します。
		/// @param name カスタムマウスカーソルの名前
		void Unregister(StringView name);

		////////////////////////////////////////////////////////////////
		//
		//	UnregisterAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief 登録されているすべてのカスタムマウスカーソルを登録解除します。
		void UnregisterAll();

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
