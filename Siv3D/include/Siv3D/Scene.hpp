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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "ResizeMode.hpp"
# include "TextureFilter.hpp"
# include "MSAASampleCount.hpp"

namespace s3d
{
	namespace Scene
	{
		/// @brief シーンの論理的な幅と高さのデフォルト値です。
		inline constexpr s3d::Size DefaultSceneSize{ 800, 600 };

		/// @brief シーンの背景色のデフォルト色です。
		inline constexpr ColorF DefaultBackgroundColor = Palette::DefaultBackground;

		/// @brief ウィンドウのクライアント領域がシーンよりも大きい場合に余白となるスペース「レターボックス」のデフォルト色です。
		inline constexpr ColorF DefaultLetterboxColor = Palette::DefaultLetterbox;

		/// @brief `Scene::DeltaTime()` が返す最大の時間（秒）のデフォルト値です。
		inline constexpr double DefaultMaxDeltaTime = 0.1;

		/// @brief ウィンドウをリサイズしたときのシーンのサイズの扱いのデフォルト値です。
		inline constexpr ResizeMode DefaultResizeMode = ResizeMode::Virtual;

		/// @brief ウィンドウのフレームバッファのサイズがシーンのサイズと異なる際、シーンの拡大縮小描画に用いるテクスチャフィルタのデフォルト値です。
		inline constexpr TextureFilter DefaultTextureFilter = TextureFilter::Linear;

		/// @brief シーンのマルチサンプリングのデフォルト値です。
		inline constexpr MSAASampleCount DefaultMSAASampleCount = MSAASampleCount::X4MSAA;
	}

	namespace Scene
	{
		////////////////////////////////////////////////////////////////
		//
		//	Resize
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの論理的な幅と高さを変更します。
		/// @remark シーンのリサイズモードが `ResizeMode::Keep` でなければ、ウィンドウのリサイズ時に自動的にシーンのサイズも変更されます。
		/// @remark サイズは即座に反映されます。リサイズモードは変更しません。
		/// @remark 論理サイズまたは描画解像度が変わると、実行済みの描画内容をクリアします。未実行の描画は保持され、実行時の描画解像度と高精細化倍率で描かれます。
		/// @remark 記録済みの座標や図形の分割数は再計算しません。描画前の呼び出しを推奨します。
		/// @throws Error 論理サイズまたは描画解像度が範囲外の場合、または描画バッファを確保できない場合。以前のシーンを維持します。
		/// @param size 新しい論理サイズ。各成分は 1～16384
		void Resize(s3d::Size size);

		/// @brief シーンの論理的な幅と高さを変更します。
		/// @remark シーンのリサイズモードが `ResizeMode::Keep` でなければ、ウィンドウのリサイズ時に自動的にシーンのサイズも変更されます。
		/// @remark サイズは即座に反映されます。リサイズモードは変更しません。
		/// @remark 論理サイズまたは描画解像度が変わると、実行済みの描画内容をクリアします。未実行の描画は保持され、実行時の描画解像度と高精細化倍率で描かれます。
		/// @remark 記録済みの座標や図形の分割数は再計算しません。描画前の呼び出しを推奨します。
		/// @throws Error 論理サイズまたは描画解像度が範囲外の場合、または描画バッファを確保できない場合。以前のシーンを維持します。
		/// @param width 新しい論理的な幅。1～16384
		/// @param height 新しい論理的な高さ。1～16384
		void Resize(int32 width, int32 height);

		////////////////////////////////////////////////////////////////
		//
		//	Size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの論理的な幅と高さを返します。
		/// @return シーンの論理的な幅と高さ
		[[nodiscard]]
		s3d::Size Size() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	High resolution rendering
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの高精細描画を有効または無効にします。
		/// @param enabled 高精細描画を有効にする場合 true
		/// @remark デフォルトは false です。表示領域のピクセル数に合わせて描画解像度を増やします。論理サイズ未満には下げません。
		/// @remark 論理サイズ、リサイズモード、カーソル座標は変更しません。高精細化用の Transformer2D は不要です。
		/// @remark 設定は即座に反映されます。描画解像度が変わると、実行済みの描画内容をクリアします。未実行の描画は新しい解像度と倍率で実行されます。
		/// @remark 記録済みの図形の分割数は再計算しません。描画前の呼び出しを推奨します。
		/// @throws Error 描画解像度が上限（各軸 16384）を超える場合、または描画バッファを確保できない場合。以前の設定とシーンを維持します。
		void SetHighResolutionEnabled(bool enabled);

		/// @brief シーンの高精細描画が有効かを返します。
		/// @return 有効な場合 true
		[[nodiscard]]
		bool IsHighResolutionEnabled() noexcept;

		/// @brief シーンの描画バッファのサイズ（ピクセル）を返します。
		/// @return 実際の描画解像度。高精細描画が無効な場合は Size() と同じ
		[[nodiscard]]
		s3d::Size RenderSize() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Width
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの論理的な幅を返します。
		/// @return シーンの論理的な幅
		[[nodiscard]]
		int32 Width() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Height
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの論理的な高さを返します。
		/// @return シーンの論理的な高さ
		[[nodiscard]]
		int32 Height() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Center, CenterF
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの中心の座標を `Point` 型で返します。
		/// @return シーンの中心の座標
		[[nodiscard]]
		Point Center() noexcept;

		/// @brief シーンの中心の座標を `Vec2` 型で返します。
		/// @return シーンの中心の座標
		[[nodiscard]]
		Vec2 CenterF() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Rect
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上が (0, 0) で現在のシーンと同じ大きさの `Rect` を返します。
		/// @return シーンと同じ大きさの `Rect`
		[[nodiscard]]
		s3d::Rect Rect() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	HorizontalAspectRatio
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンのアスペクト比（幅 / 高さ）を返します。
		/// @tparam Float アスペクト比の型
		/// @return シーンのアスペクト比（幅 / 高さ）
		template <Concept::FloatingPoint Float = double>
		[[nodiscard]]
		Float HorizontalAspectRatio() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetResizeMode
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのサイズを変更したときに、シーンをどのようにリサイズするかを設定します。
		/// @remark デフォルトは `Scene::DefaultResizeMode` です。
		/// @remark 現在のウィンドウに合わせて即座に論理サイズと描画解像度を更新します。サイズ変更時の描画の扱いは Resize() と同じです。
		/// @throws Error 論理サイズまたは描画解像度が上限（各軸 16384）を超える場合、または描画バッファを確保できない場合。以前の設定とシーンを維持します。
		/// @param resizeMode シーンのリサイズモード
		void SetResizeMode(ResizeMode resizeMode);

		////////////////////////////////////////////////////////////////
		//
		//	GetResizeMode
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのサイズを変更したときに、シーンをどのようにリサイズするかの現在の設定を返します。
		/// @return シーンのリサイズモード
		[[nodiscard]]
		ResizeMode GetResizeMode() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetBackground
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの背景色を設定します。色のアルファ成分は無視されます。
		/// @remark デフォルトは `Scene::DefaultBackgroundColor` です。
		/// @param color 色
		void SetBackground(const ColorF& color);

		////////////////////////////////////////////////////////////////
		//
		//	GetBackground
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンをクリアする色（背景色）を返します。
		/// @return シーンをクリアする色
		[[nodiscard]]
		const ColorF& GetBackground() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetLetterbox
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンとウィンドウのアスペクト比が異なる際に、余白となるスペース「レターボックス」の色を設定します。
		/// @remark デフォルトは `Scene::DefaultLetterboxColor` です。
		/// @param color レターボックスの色
		void SetLetterbox(const ColorF& color);

		////////////////////////////////////////////////////////////////
		//
		//	GetLetterbox
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のレターボックスの色を返します。
		/// @return レターボックスの色
		[[nodiscard]]
		const ColorF& GetLetterbox() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetTextureFilter
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアント領域がシーンのサイズと異なる場合にシーンを拡大縮小描画するために使うテクスチャフィルタを設定します。
		/// @remark デフォルトは `Scene::DefaultTextureFilter` です。	
		/// @param textureFilter シーンを拡大縮小描画する際に使うテクスチャフィルタ
		void SetTextureFilter(TextureFilter textureFilter);

		////////////////////////////////////////////////////////////////
		//
		//	GetTextureFilter
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのクライアント領域がシーンのサイズと異なる場合にシーンを拡大縮小描画するために使うテクスチャフィルタの現在の設定を返します。
		/// @return シーンを拡大縮小描画する際に使うテクスチャフィルタ
		[[nodiscard]]
		TextureFilter GetTextureFilter() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

		//void SetMSAASampleCount(MSAASampleCount sampleCount);

		////////////////////////////////////////////////////////////////
		//
		//	GetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

		//[[nodiscard]]
		//MSAASampleCount GetMSAASampleCount() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetMaxDeltaTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief `Scene::DeltaTime()` が返す最大の時間（秒）を設定します。
		/// @param timeSec 最大の時間（秒）
		void SetMaxDeltaTime(double timeSec);

		////////////////////////////////////////////////////////////////
		//
		//	GetMaxDeltaTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief `Scene::DeltaTime()` が返す最大の時間（秒）の現在の設定を返します。
		/// @return `Scene::DeltaTime()` が返す最大の時間（秒）
		[[nodiscard]]
		double GetMaxDeltaTime() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	DeltaTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief 前回の System::Update() からの経過時間（秒）を、`Scene::GetMaxDeltaTime()` を超えない値で返します。
		/// @remark この値をもとにアニメーションやイベントの処理などを行うことで、フレームレートが上下しても対応できます。
		/// @remark `System::Update()` を呼ぶことで値が更新されます。
		/// @return 前回のフレームからの経過時間（秒）と `Scene::GetMaxDeltaTime()` のうち、小さいほうの値
		[[nodiscard]]
		double DeltaTime() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Time
		//
		////////////////////////////////////////////////////////////////

		/// @brief アプリケーションが起動してからのシーンの経過時間（秒）を返します。
		/// @return アプリケーションが起動してからのシーンの経過時間（秒）
		/// @remark `System::Update()` を呼ぶことで値が更新されます。
		/// @remark `GetMaxDeltaTime()` の設定によっては、実際のアプリケーション起動時間よりも小さくなることがあります。
		[[nodiscard]]
		double Time() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	FrameCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief `System::Update()` が呼ばれた回数（= フレームカウント）を返します。
		/// @remark ユーザの環境によってフレームレートが変わるため、この値をアニメーション等の制御に使ってはいけません。
		/// @return `System::Update()` が呼ばれた回数
		[[nodiscard]]
		int32 FrameCount() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	ClientToScene
		//
		////////////////////////////////////////////////////////////////

		/// @brief クライアント座標をシーンの座標に変換します。
		/// @remark `Cursor::PosRaw()` が返す座標をシーンの座標に変換できます。
		/// @param pos クライアント座標（ピクセル）
		/// @return シーンの論理座標
		[[nodiscard]]
		Vec2 ClientToScene(Vec2 pos) noexcept;
	}
}

# include "detail/Scene.ipp"
