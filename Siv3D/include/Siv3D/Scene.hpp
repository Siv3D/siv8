﻿//-----------------------------------------------
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
# include "ColorHSV.hpp"
# include "ResizeMode.hpp"
# include "TextureFilter.hpp"
# include "MSAASampleCount.hpp"

namespace s3d
{
	namespace Scene
	{
		/// @brief シーンの幅と高さ（ピクセル）のデフォルト値です。
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

		/// @brief シーンの幅と高さを変更します。
		/// @remark シーンのリサイズモードが `ResizeMode::Keep` でなければ、ウィンドウのリサイズ時に自動的にシーンのサイズも変更されます。
		/// @param size 新しいシーンの幅と高さ（ピクセル）
		void Resize(s3d::Size size);

		/// @brief シーンの幅と高さを変更します。
		/// @remark シーンのリサイズモードが `ResizeMode::Keep` でなければ、ウィンドウのリサイズ時に自動的にシーンのサイズも変更されます。
		/// @param width 新しいシーンの幅（ピクセル）
		/// @param height 新しいシーンの高さ（ピクセル）
		void Resize(int32 width, int32 height);

		////////////////////////////////////////////////////////////////
		//
		//	Size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの幅と高さ（ピクセル）を返します。
		/// @return シーンの幅と高さ（ピクセル）
		[[nodiscard]]
		s3d::Size Size() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Width
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの幅（ピクセル）を返します。
		/// @return シーンの幅（ピクセル）
		[[nodiscard]]
		int32 Width() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Height
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のシーンの高さ（ピクセル）を返します。
		/// @return シーンの高さ（ピクセル）
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
		/// @tparam Type アスペクト比の型
		/// @return シーンのアスペクト比（幅 / 高さ）
		template <class Type = double>
		[[nodiscard]]
		Type HorizontalAspectRatio() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	SetResizeMode
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのサイズを変更したときに、シーンをどのようにリサイズするかを設定します。
		/// @remark デフォルトは `Scene::DefaultResizeMode` です。	
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

		void SetMSAASampleCount(MSAASampleCount sampleCount);

		////////////////////////////////////////////////////////////////
		//
		//	GetMSAASampleCount
		//
		////////////////////////////////////////////////////////////////

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
		/// @return シーン座標（ピクセル）
		[[nodiscard]]
		Vec2 ClientToScene(Vec2 pos) noexcept;
	}
}

# include "detail/Scene.ipp"
