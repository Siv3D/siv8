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
# include "StringView.hpp"
# include "Format.hpp"
# include "MonitorInfo.hpp"
# include "PointVector.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	struct WindowState;
	enum class WindowStyle : uint8;
	struct Point;

	namespace Window
	{
		////////////////////////////////////////////////////////////////
		//
		//	DefaultTitle
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのデフォルトのタイトル
		inline constexpr StringView DefaultTitle = U"Siv3D App";

		////////////////////////////////////////////////////////////////
		//
		//	SetTitle
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのタイトルを変更します。
		/// @param title 新しいタイトル
		void SetTitle(const String& title);

		/// @brief ウィンドウのタイトルを変更します。
		/// @tparam ...Args 新しいタイトルの値の型
		/// @param ...args 新しいタイトル
		void SetTitle(const Concept::Formattable auto&... args);

		////////////////////////////////////////////////////////////////
		//
		//	GetTitle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のウィンドウのタイトルを返します。
		/// @return 現在のウィンドウのタイトル
		[[nodiscard]]
		const String& GetTitle() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetState
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のウィンドウの状態を返します。
		/// @return 現在のウィンドウの状態
		[[nodiscard]]
		const WindowState& GetState() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetStyle
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウスタイルを変更します。
		/// @param style 新しいウィンドウスタイル
		void SetStyle(WindowStyle style);

		////////////////////////////////////////////////////////////////
		//
		//	GetStyle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のウィンドウスタイルを返します。
		/// @return 現在のウィンドウスタイル
		[[nodiscard]]
		WindowStyle GetStyle() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウを指定したスクリーン座標に移動させます。
		/// @param screenPos 移動先のスクリーン座標
		void SetPos(Point screenPos);

		/// @brief ウィンドウを指定したスクリーン座標に移動させます。
		/// @param screenX 移動先のスクリーン X 座標
		/// @param screenY 移動先のスクリーン Y 座標
		void SetPos(int32 screenX, int32 screenY);

		////////////////////////////////////////////////////////////////
		//
		//	GetPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のウィンドウの位置をスクリーン座標で返します。
		/// @return 現在のウィンドウの位置
		[[nodiscard]]
		Point GetPos() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	CenterToScreen
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のモニターのワークエリアの中央にウィンドウを移動させます。
		/// @return 移動に成功した場合 true, それ以外の場合は false
		bool CenterToScreen();

		/// @brief 指定したモニターのワークエリアの中央にウィンドウを移動させます。
		/// @param monitorIndex モニターのインデックス
		/// @return 移動に成功した場合 true, それ以外の場合は false
		bool CenterToScreen(size_t monitorIndex);

		////////////////////////////////////////////////////////////////
		//
		//	Maximize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウを最大化します。
		void Maximize();

		////////////////////////////////////////////////////////////////
		//
		//	Restore
		//
		////////////////////////////////////////////////////////////////

		/// @brief 最大・最小化されたウィンドウを元のサイズに戻します。
		void Restore();

		////////////////////////////////////////////////////////////////
		//
		//	Minimize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウを最小化します。
		void Minimize();

		////////////////////////////////////////////////////////////////
		//
		//	Resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param size 新しいサイズ（ピクセル）
		/// @param centering ウィンドウを画面の中心に移動させるか
		/// @return サイズの変更に成功した場合 true, それ以外の場合は false
		/// @remark `ResizeVirtual(size, centering)` と同じです。
		bool Resize(Size size, Centering centering = Centering::Yes);

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param centering ウィンドウを画面の中心に移動させるか
		/// @return サイズの変更に成功した場合 true, それ以外の場合は false
		/// @remark `ResizeVirtual(width, height, centering)` と同じです。
		bool Resize(int32 width, int32 height, Centering centering = Centering::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	ResizeVirtual
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param size 新しいサイズ（ピクセル）
		/// @param centering ウィンドウを画面の中心に移動させるか
		/// @return サイズの変更に成功した場合 true, それ以外の場合は false
		/// @remark `Resize(size, centering)` と同じです。
		bool ResizeVirtual(Size size, Centering centering = Centering::Yes);

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param centering ウィンドウを画面の中心に移動させるか
		/// @return サイズの変更に成功した場合 true, それ以外の場合は false
		/// @remark `Resize(width, height, centering)` と同じです。
		bool ResizeVirtual(int32 width, int32 height, Centering centering = Centering::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	ResizeActual
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアントサイズを実サイズ基準で変更します。
		/// @param size 新しいサイズ（ピクセル）
		/// @param centering ウィンドウを画面の中心に移動させるか
		/// @return サイズの変更に成功した場合 true, それ以外の場合は false
		bool ResizeActual(Size size, Centering centering = Centering::Yes);

		/// @brief ウィンドウのクライアントサイズを実サイズ基準で変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param centering ウィンドウを画面の中心に移動させるか
		/// @return サイズの変更に成功した場合 true, それ以外の場合は false
		bool ResizeActual(int32 width, int32 height, Centering centering = Centering::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	SetMinimumFrameBufferSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウを手動でリサイズするときの最小サイズを実サイズ基準で設定します。
		/// @param size 設定するサイズ（ピクセル）
		void SetMinimumFrameBufferSize(Size size);

		////////////////////////////////////////////////////////////////
		//
		//	SetFullscreen
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウモードとフルスクリーンモードを切り替えます。
		/// @param fullscreen フルスクリーンモードにする場合 true, ウィンドウモードにする場合 false
		/// @param monitorIndex 表示するモニタのインデックス
		void SetFullscreen(bool fullscreen, size_t monitorIndex = System::GetCurrentMonitorIndex());

		////////////////////////////////////////////////////////////////
		//
		//	SetToggleFullscreenEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief Windows 版において、Alt + Enter キーによるウィンドウモードとフルスクリーンモードの切り替えを有効にするかを設定します。
		/// @param enabled Alt + Enter キーによるウィンドウモードとフルスクリーンモードの切り替えを有効にする場合は true, 無効にする場合は false
		/// @remark Windows 版ではデフォルトで有効、それ以外のプラットフォームでは常に無効です。
		void SetToggleFullscreenEnabled(bool enabled);

		////////////////////////////////////////////////////////////////
		//
		//	IsToggleFullscreenEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief Alt + Enter キーによるウィンドウモードとフルスクリーンモードの切り替えが有効であるかを返します。
		/// @return Alt + Enter キーによるウィンドウモードとフルスクリーンモードの切り替えが有効である場合 true, それ以外の場合は false
		/// @remark Windows 版以外では常に false を返します。
		[[nodiscard]]
		bool IsToggleFullscreenEnabled();
	}

# if SIV3D_PLATFORM(WINDOWS)

	namespace Platform::Windows::Window
	{
		/// @brief ウィンドウハンドルを返します。
		/// @return ウィンドウハンドル
		/// @remark この関数の戻り値を HWND にキャストして使用します。
		[[nodiscard]]
		void* GetHWND();

		///// @brief タスクバー上のプログレスバーの進行度を設定します。1.0 を渡すと終了します。
		///// @param progress0_1 進行度。[0.0, 1.0] の範囲。
		//void SetTaskbarProgressBar(double progress0_1);
	}

# endif
}

# include "detail/Window.ipp"
