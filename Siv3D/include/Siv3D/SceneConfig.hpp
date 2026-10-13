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
# include "PointVector.hpp"
# include "SceneSizeMode.hpp"
# include "SceneResolution.hpp"
# include "FullscreenSceneMode.hpp"
# include "TextureFilter.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SceneConfig
	//
	////////////////////////////////////////////////////////////////

	/// @brief シーンの表示設定
	struct SceneConfig
	{
		/// @brief sizeMode が Fixed のときに使用するシーンサイズ。各成分は 1～16384
		Size fixedSceneSize{ 800, 600 };

		/// @brief シーンサイズの決め方
		SceneSizeMode sizeMode = SceneSizeMode::Window;

		/// @brief 描画解像度の決め方
		SceneResolution resolution = SceneResolution::SceneSize;

		/// @brief 自動追従する設定での全画面中のシーンサイズの扱い
		FullscreenSceneMode fullscreen = FullscreenSceneMode::KeepSize;

		/// @brief シーンの拡大・縮小表示に使用する補間方法
		TextureFilter scalingFilter = TextureFilter::Linear;

		////////////////////////////////////////////////////////////////
		//
		//	Fixed
		//
		////////////////////////////////////////////////////////////////

		/// @brief シーンサイズを固定する設定を作ります。
		/// @param size シーンサイズ
		/// @param resolution 描画解像度の決め方
		/// @return 指定サイズに固定し、Linear で表示する設定
		[[nodiscard]]
		static constexpr SceneConfig Fixed(Size size, SceneResolution resolution = SceneResolution::SceneSize) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Window
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウサイズに追従する設定を作ります。
		/// @param resolution 描画解像度の決め方
		/// @return DPI 倍率を除いたウィンドウサイズに追従し、Linear で表示する設定。全画面中はシーンサイズを維持する
		[[nodiscard]]
		static constexpr SceneConfig Window(SceneResolution resolution = SceneResolution::SceneSize) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	PixelArt
		//
		////////////////////////////////////////////////////////////////

		/// @brief ドット絵向けの設定を作ります。
		/// @param size シーンサイズと描画解像度
		/// @return 指定サイズに固定し、Nearest で表示する設定。表示倍率は整数倍に制限しない
		[[nodiscard]]
		static constexpr SceneConfig PixelArt(Size size) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	WindowPixels
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウの実ピクセル数に追従する設定を作ります。
		/// @return 全画面中も描画領域の実ピクセル数にシーンサイズと描画解像度を合わせる設定
		[[nodiscard]]
		static constexpr SceneConfig WindowPixels() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		friend bool operator ==(const SceneConfig&, const SceneConfig&) = default;
	};
}

# include "detail/SceneConfig.ipp"
