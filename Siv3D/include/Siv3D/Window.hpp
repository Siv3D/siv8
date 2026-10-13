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

	/// @brief ウィンドウの操作
	/// @details 位置・寸法・表示状態の操作は、関数が返る時点で取得できたウィンドウの状態をシーンサイズ・描画解像度・カーソルの座標変換に反映します。
	/// OS のアニメーションなどで後から生じる変更は System::Update() で反映します。最小化中は最後の有効なシーン寸法を維持します。
	/// シーン寸法の変更に伴う描画内容の扱いは Scene::Configure() と同じです。
	/// カーソルの新しい入力サンプルは System::Update() で取得します。
	/// 描画資源の更新で例外が発生した場合、完了済みのウィンドウ操作は取り消されません。
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
		/// @remark フルスクリーン中は変更しません。
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

		/// @brief ウィンドウ外枠の左上を指定したスクリーン座標に移動させます。
		/// @remark フルスクリーン中は移動しません。
		/// @remark 主モニターの左上が原点で、右が X の正方向、下が Y の正方向です。単位は Windows ではピクセル、macOS では point です。
		/// @param screenPos 移動先のスクリーン座標
		void SetPos(Point screenPos);

		/// @brief ウィンドウ外枠の左上を指定したスクリーン座標に移動させます。
		/// @remark フルスクリーン中は移動しません。
		/// @remark 主モニターの左上が原点で、右が X の正方向、下が Y の正方向です。単位は Windows ではピクセル、macOS では point です。
		/// @param screenX 移動先のスクリーン X 座標
		/// @param screenY 移動先のスクリーン Y 座標
		void SetPos(int32 screenX, int32 screenY);

		////////////////////////////////////////////////////////////////
		//
		//	GetPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のウィンドウ外枠の左上をスクリーン座標で返します。
		/// @return Window::SetPos() と同じ座標系での位置。Windows ではピクセル、macOS では point。
		[[nodiscard]]
		Point GetPos() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	CenterToScreen
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のモニターのワークエリアの中央にウィンドウを移動させます。
		/// @return 移動に成功した場合 true。フルスクリーン中、または対象モニターを取得できない場合は false。
		bool CenterToScreen();

		/// @brief 指定したモニターのワークエリアの中央にウィンドウを移動させます。
		/// @param monitorIndex モニターのインデックス
		/// @return 移動に成功した場合 true。フルスクリーン中、または対象モニターを取得できない場合は false。
		bool CenterToScreen(size_t monitorIndex);

		////////////////////////////////////////////////////////////////
		//
		//	Maximize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウを最大化します。
		/// @remark WindowStyle::Fixed の場合は最大化しません。
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
		/// @param centerWindow ウィンドウを画面中央へ移動させるか
		/// @return サイズ変更に成功した場合、または現在と同じサイズの場合 true。それ以外の場合は false。中央配置の成否は含みません。
		/// @remark サイズ指定・失敗時の扱いは `ResizeVirtual(size, centerWindow)` と同じです。
		bool Resize(Size size, CenterWindow centerWindow = CenterWindow::Yes);

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param centerWindow ウィンドウを画面中央へ移動させるか
		/// @return サイズ変更に成功した場合、または現在と同じサイズの場合 true。それ以外の場合は false。中央配置の成否は含みません。
		/// @remark サイズ指定・失敗時の扱いは `ResizeVirtual(width, height, centerWindow)` と同じです。
		bool Resize(int32 width, int32 height, CenterWindow centerWindow = CenterWindow::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	ResizeVirtual
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param size 新しいサイズ（ピクセル）
		/// @param centerWindow ウィンドウを画面中央へ移動させるか
		/// @return サイズ変更に成功した場合、または現在と同じサイズの場合 true。それ以外の場合は false。中央配置の成否は含みません。
		/// @remark サイズを変更する場合はフルスクリーンを解除します。現在と同じサイズならフルスクリーンを維持します。
		/// @remark Windows ではサイズ変更時に最小サイズの制限を適用し、下回る場合 false を返します。この判定の前に行った全画面・最大化からの復帰は取り消されません。
		/// @throws Error 幅または高さが [1, 8192] の範囲外の場合。ウィンドウは変更されません。
		bool ResizeVirtual(Size size, CenterWindow centerWindow = CenterWindow::Yes);

		/// @brief ウィンドウのクライアントサイズを仮想サイズ基準で変更します。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param centerWindow ウィンドウを画面中央へ移動させるか
		/// @return サイズ変更に成功した場合、または現在と同じサイズの場合 true。それ以外の場合は false。中央配置の成否は含みません。
		/// @remark サイズ指定・失敗時の扱いは `ResizeVirtual(Size{ width, height }, centerWindow)` と同じです。
		bool ResizeVirtual(int32 width, int32 height, CenterWindow centerWindow = CenterWindow::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	ResizeActual
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアントサイズを実サイズ基準で変更します。
		/// @remark 全画面の解除、サイズの範囲、Windows の最小サイズ未満の扱いは ResizeVirtual() と同じです。macOS では復帰先のモニターの倍率で point 単位に丸めます。
		/// @param size 新しいサイズ（ピクセル）
		/// @param centerWindow ウィンドウを画面中央へ移動させるか
		/// @return サイズ変更に成功した場合、または現在と同じサイズの場合 true。それ以外の場合は false。中央配置の成否は含みません。
		bool ResizeActual(Size size, CenterWindow centerWindow = CenterWindow::Yes);

		/// @brief ウィンドウのクライアントサイズを実サイズ基準で変更します。
		/// @remark 全画面の解除、サイズの範囲、Windows の最小サイズ未満の扱いは ResizeVirtual() と同じです。macOS では復帰先のモニターの倍率で point 単位に丸めます。
		/// @param width 新しい幅（ピクセル）
		/// @param height 新しい高さ（ピクセル）
		/// @param centerWindow ウィンドウを画面中央へ移動させるか
		/// @return サイズ変更に成功した場合、または現在と同じサイズの場合 true。それ以外の場合は false。中央配置の成否は含みません。
		bool ResizeActual(int32 width, int32 height, CenterWindow centerWindow = CenterWindow::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	SetMinimumFrameBufferSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウを手動でリサイズするときの最小サイズを実サイズ基準で設定します。
		/// @remark Windows では Resize()、ResizeVirtual()、ResizeActual() にも適用されます。
		/// @param size 設定するサイズ（ピクセル）
		/// @remark macOS では、指定したピクセル数を下回らないよう point 単位に切り上げます。
		void SetMinimumFrameBufferSize(Size size);

		////////////////////////////////////////////////////////////////
		//
		//	SetFullscreen
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウモードとフルスクリーンモードを切り替えます。
		/// @remark ディスプレイの表示モードを変更せず、指定したモニター全体に表示します。
		/// @remark Scene::GetConfig() が返す表示設定を維持します。
		/// @remark 全画面中も表示先を変更できます。同じモード・表示先の指定、または対象モニターを取得できない場合は切り替えません。
		/// @remark 全画面を解除すると、最初に全画面へ入る前のウィンドウ位置・サイズに戻ります。
		/// @param fullscreen フルスクリーンモードにする場合 true, ウィンドウモードにする場合 false
		/// @param monitorIndex 全画面の表示先モニターのインデックス。範囲外の場合は現在のモニターを使用します。fullscreen が false の場合は使用しません。
		void SetFullscreen(bool fullscreen, size_t monitorIndex = System::GetCurrentMonitorIndex());

		////////////////////////////////////////////////////////////////
		//
		//	SetToggleFullscreenEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief ショートカットキーやメニューによるフルスクリーン切り替えを有効にするかを設定します。
		/// @param enabled 切り替えを有効にする場合は true, 無効にする場合は false
		/// @remark Windows 版では Alt + Enter、macOS 版では Control + Command + F と全画面メニューが対象です。デフォルトで有効です。
		/// @remark 切り替えは次の System::Update() で行います。無効化すると保留中の切り替えを取り消します。
		void SetToggleFullscreenEnabled(bool enabled);

		////////////////////////////////////////////////////////////////
		//
		//	IsToggleFullscreenEnabled
		//
		////////////////////////////////////////////////////////////////

		/// @brief ショートカットキーやメニューによるフルスクリーン切り替えが有効かを返します。
		/// @return 有効である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsToggleFullscreenEnabled();

		////////////////////////////////////////////////////////////////
		//
		//	SetTaskbarProgressBar
		//
		////////////////////////////////////////////////////////////////

		/// @brief タスクバー上のプログレスバーの進行度を設定します。1.0 を渡すと終了します。
		/// @param progress0_1 進行度。[0.0, 1.0] の範囲。
		void SetTaskbarProgressBar(double progress0_1);
	}

# if SIV3D_PLATFORM(WINDOWS)

	namespace Platform::Windows::Window
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetHWND
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウハンドルを返します。
		/// @return ウィンドウハンドル
		/// @remark この関数の戻り値を HWND にキャストして使用します。
		[[nodiscard]]
		void* GetHWND();
	}

# endif
}

# include "detail/Window.ipp"
