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
# include "2DShapes.hpp"
# include "WindowStyle.hpp"

namespace s3d
{
	namespace Window
	{
		/// @brief デフォルトのクライアントサイズ | Default client size
		inline constexpr Size DefaultClientSize{ 800, 600 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	WindowState
	//
	////////////////////////////////////////////////////////////////

	/// @brief ウィンドウの状態 | Window state
	struct WindowState
	{
		/// @brief スクリーン座標におけるウィンドウの矩形
		Rect bounds = Rect::Empty();

		/// @brief フレームの幅
		Size frameThickness{ 0, 0 };

		/// @brief フレームバッファのサイズ
		Size frameBufferSize = Window::DefaultClientSize;

		/// @brief クライアントの仮想サイズ
		Size virtualSize = Window::DefaultClientSize;

		/// @brief 最小のフレームバッファのサイズ
		Size minFrameBufferSize{ 1, 1 };

		/// @brief ユーザが設定しているデスクトップのスケーリング
		double scaling = 1.0;

		/// @brief タイトルバーの高さ
		int32 titleBarHeight = 0;

		/// @brief スタイル
		WindowStyle style = WindowStyle::Fixed;

		/// @brief フルスクリーンであるか
		bool fullscreen = false;

		/// @brief 最小化されているか
		bool minimized = false;

		/// @brief 最大化されているか
		bool maximized = false;

		/// @brief アクティブであるか
		bool focused = true;

		/// @brief 移動やリサイズの途中であるか
		bool sizeMove = false;
	};
}
