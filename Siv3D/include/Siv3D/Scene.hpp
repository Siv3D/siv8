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
# include "SceneConfig.hpp"
# include "TextureFilter.hpp"
# include "MSAASampleCount.hpp"

namespace s3d
{
	namespace Scene
	{
		/// @brief シーンの幅と高さのデフォルト値です。
		inline constexpr s3d::Size DefaultSceneSize = SceneConfig{}.fixedSceneSize;

		/// @brief シーンの背景色のデフォルト色です。
		inline constexpr ColorF DefaultBackgroundColor = Palette::DefaultBackground;

		/// @brief ウィンドウのクライアント領域がシーンよりも大きい場合に余白となるスペース「レターボックス」のデフォルト色です。
		inline constexpr ColorF DefaultLetterboxColor = Palette::DefaultLetterbox;

		/// @brief `Scene::DeltaTime()` が返す最大の時間（秒）のデフォルト値です。
		inline constexpr double DefaultMaxDeltaTime = 0.1;

		/// @brief シーンのマルチサンプリングのデフォルト値です。
		inline constexpr MSAASampleCount DefaultMSAASampleCount = MSAASampleCount::X4MSAA;
	}

	namespace Scene
	{
		////////////////////////////////////////////////////////////////
		//
		//	Configure
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの表示設定を置き換えます。
		/// @param config 新しい表示設定。既定の設定は SceneConfig{} です。
		/// @remark 現在のウィンドウ状態に合わせて即座に反映します。ウィンドウの状態、背景色、レターボックス色は変更しません。
		/// @remark `FullscreenSceneMode::KeepSize` の全画面中は、`SceneSizeMode::Fixed` 以外の設定では現在のシーンサイズを維持します。ウィンドウ復帰後はその時点の設定に従います。
		/// @remark シーンサイズまたは描画解像度が変わると、実行済みの描画内容をクリアします。未実行の描画は保持され、実行時の描画解像度で描かれます。記録済みの座標や図形の分割数は再計算しません。
		/// @remark 寸法が変わらない設定変更では、描画済み内容を維持します。
		/// @throws Error 設定値が不正な場合、シーンサイズまたは描画解像度が各軸 1～16384 の範囲外の場合、または描画バッファを確保できない場合。以前の設定とシーンを維持します。
		void Configure(const SceneConfig& config);

		////////////////////////////////////////////////////////////////
		//
		//	GetConfig
		//
		////////////////////////////////////////////////////////////////

		/// @brief 適用済みの表示設定を返します。
		/// @return 設定のコピー。追従によって変化した寸法は含みません。
		[[nodiscard]]
		SceneConfig GetConfig() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	RenderResolution
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの描画解像度を返します。
		/// @return 描画バッファのピクセル数
		[[nodiscard]]
		s3d::Size RenderResolution() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの幅と高さを返します。
		/// @return シーンの幅と高さ
		[[nodiscard]]
		s3d::Size Size() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Width
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの幅を返します。
		/// @return シーンの幅
		[[nodiscard]]
		int32 Width() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Height
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの高さを返します。
		/// @return シーンの高さ
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
		//	SetBackground
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの背景色を設定します。
		/// @param color 背景色。アルファ成分は無視されます。
		/// @remark 次の `System::Update()` が開始するフレームから反映されます。それまでに複数回設定した場合は、最後の色を使います。描画中のフレームの背景は変わりません。
		/// @remark フレームの途中で `Graphics2D::Flush()` を呼んだり、シーンサイズ、描画解像度、MSAA サンプル数を変更したりしても、そのフレームの背景には開始時の色を使います。
		/// @remark デフォルトは `Scene::DefaultBackgroundColor` です。
		void SetBackground(const ColorF& color);

		////////////////////////////////////////////////////////////////
		//
		//	GetBackground
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンの背景色の設定を取得します。
		/// @remark `SetBackground()` の変更はすぐに取得できます。描画中のフレームの背景色とは異なる場合があります。
		/// @return 背景色の設定値
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
		//	SetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルト描画先であるシーンの MSAA サンプル数を設定します。
		/// @param sampleCount サンプル数。デフォルトは Scene::DefaultMSAASampleCount です。
		/// @remark 即座に反映します。シーン以外の描画先には影響しません。
		/// @remark 値が変わると実行済みの描画内容をクリアします。未実行の描画は保持され、実行時のサンプル数で描かれます。同じ値の設定では描画内容を維持します。
		/// @throws Error 値が不正、GPU が非対応、または描画バッファを確保できない場合。以前の設定とシーンを維持します。
		void SetMSAASampleCount(MSAASampleCount sampleCount);

		////////////////////////////////////////////////////////////////
		//
		//	GetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンに適用済みの MSAA サンプル数を返します。
		/// @return シーンの MSAA サンプル数
		[[nodiscard]]
		MSAASampleCount GetMSAASampleCount() noexcept;

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
		/// @return シーン座標
		[[nodiscard]]
		Vec2 ClientToScene(Vec2 pos) noexcept;
	}
}

# include "detail/Scene.ipp"
