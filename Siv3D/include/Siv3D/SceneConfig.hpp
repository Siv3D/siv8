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
# include "TextureFilter.hpp"

namespace s3d
{
	struct FormatData;

	/// @brief シーンサイズの決め方
	enum class SceneSizeMode : uint8
	{
		/// @brief 指定サイズに固定する
		Fixed,
		/// @brief DPI 倍率を除いたウィンドウの描画領域のサイズに合わせる
		Window,
		/// @brief ウィンドウの描画領域の実ピクセル数に合わせる
		WindowPixels,
	};

	/// @brief シーンの描画解像度の決め方
	enum class SceneResolution : uint8
	{
		/// @brief シーンサイズと同じピクセル数で描く
		SceneSize,
		/// @brief シーンの表示領域に合わせて描画解像度を増やす。シーンサイズ未満には下げない
		Display,
	};

	/// @brief 全画面中のシーンサイズの扱い
	enum class FullscreenSceneMode : uint8
	{
		/// @brief シーンサイズの自動変更を止める
		KeepSize,
		/// @brief シーンサイズの方針に従って変更する
		FollowWindow,
	};

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

		/// @brief シーンサイズを固定する設定を作ります。
		/// @param size シーンサイズ
		/// @param resolution 描画解像度の決め方
		/// @return 指定サイズに固定し、Linear で表示する設定
		[[nodiscard]]
		static constexpr SceneConfig Fixed(Size size, SceneResolution resolution = SceneResolution::SceneSize) noexcept;

		/// @brief ウィンドウサイズに追従する設定を作ります。
		/// @param resolution 描画解像度の決め方
		/// @return DPI 倍率を除いたウィンドウサイズに追従し、Linear で表示する設定。全画面中はシーンサイズを維持する
		[[nodiscard]]
		static constexpr SceneConfig Window(SceneResolution resolution = SceneResolution::SceneSize) noexcept;

		/// @brief ドット絵向けの設定を作ります。
		/// @param size シーンサイズと描画解像度
		/// @return 指定サイズに固定し、Nearest で表示する設定。表示倍率は整数倍に制限しない
		[[nodiscard]]
		static constexpr SceneConfig PixelArt(Size size) noexcept;

		/// @brief ウィンドウの実ピクセル数に追従する設定を作ります。
		/// @return 全画面中も描画領域の実ピクセル数にシーンサイズと描画解像度を合わせる設定
		[[nodiscard]]
		static constexpr SceneConfig WindowPixels() noexcept;

		friend bool operator ==(const SceneConfig&, const SceneConfig&) = default;
	};

	/// @brief シーンサイズの方針を文字列に変換します。
	/// @param formatData 出力先
	/// @param value 変換する値
	void Formatter(FormatData& formatData, SceneSizeMode value);
	/// @brief 描画解像度の方針を文字列に変換します。
	/// @param formatData 出力先
	/// @param value 変換する値
	void Formatter(FormatData& formatData, SceneResolution value);
	/// @brief 全画面中のシーンサイズの方針を文字列に変換します。
	/// @param formatData 出力先
	/// @param value 変換する値
	void Formatter(FormatData& formatData, FullscreenSceneMode value);
}

# include "detail/SceneConfig.ipp"
