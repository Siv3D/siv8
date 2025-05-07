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
# include "Camera2DControlBuilder.hpp"

namespace s3d
{
	/// @brief 2D カメラの操作設定
	struct Camera2DControl
	{
		/// @brief 最小カメラズームのデフォルト値
		static constexpr double DefaultMinZoom = Camera2DControlBuilder::DefaultMinZoom;

		/// @brief 最大カメラズームのデフォルト値
		static constexpr double DefaultMaxZoom = Camera2DControlBuilder::DefaultMaxZoom;

		/// @brief カメラズームの平滑化時間（秒）のデフォルト値
		static constexpr double DefaultZoomSmoothTime = Camera2DControlBuilder::DefaultZoomSmoothTime;

		/// @brief カメラ位置の平滑化時間（秒）のデフォルト値
		static constexpr double DefaultPositionSmoothTime = Camera2DControlBuilder::DefaultPositionSmoothTime;

		/// @brief ホイールによるカメラズームの速さのデフォルト値
		static constexpr double DefaultWheelZoomSpeed = Camera2DControlBuilder::DefaultWheelZoomSpeed;

		/// @brief ホイールによるカメラズーム無し
		static constexpr double NoWheelZoom = Camera2DControlBuilder::NoWheelZoom;

		/// @brief 右ボタンドラッグによるカメラ移動の速さのデフォルト値
		static constexpr double DefaultMouseDragPanningSpeed = Camera2DControlBuilder::DefaultMouseDragPanningSpeed;

		/// @brief 右ボタンドラッグによるカメラ移動無し
		static constexpr double NoMouseDragPanning = Camera2DControlBuilder::NoMouseDragPanning;

		/// @brief キー入力によるカメラズームの速さのデフォルト値
		static constexpr double DefaultControlZoomSpeed = Camera2DControlBuilder::DefaultControlZoomSpeed;

		/// @brief キー入力によるカメラ移動の速さのデフォルト値
		static constexpr double DefaultControlPanningSpeed = Camera2DControlBuilder::DefaultControlPanningSpeed;

		/// @brief 最小カメラズーム
		double minZoom = DefaultMinZoom;

		/// @brief 最大カメラズーム
		double maxZoom = DefaultMaxZoom;

		/// @brief カメラズームの平滑化時間（秒）
		double zoomSmoothTime = DefaultZoomSmoothTime;

		/// @brief 位置変更の平滑化時間（秒）
		double positionSmoothTime = DefaultPositionSmoothTime;

		/// @brief ホイールによるカメラズームの速さ
		double wheelZoomSpeed = DefaultWheelZoomSpeed;

		/// @brief マウスによる位置変更の速さ
		double mouseDragPanningSpeed = DefaultMouseDragPanningSpeed;

		/// @brief キー入力によるカメラズームの速さ
		double controlZoomSpeed = 2.0;

		/// @brief キー入力による位置変更の速さ
		double controlPanningSpeed = 400.0;

		/// @brief 上方向への移動を発生させる条件を指定する関数
		std::function<bool()> moveToUp = Camera2DControlBuilder::DefaultMoveToUp;

		/// @brief 右方向への移動を発生させる条件を指定する関数
		std::function<bool()> moveToRight = Camera2DControlBuilder::DefaultMoveToRight;

		/// @brief 下方向への移動を発生させる条件を指定する関数
		std::function<bool()> moveToDown = Camera2DControlBuilder::DefaultMoveToDown;

		/// @brief 左方向への移動を発生させる条件を指定する関数
		std::function<bool()> moveToLeft = Camera2DControlBuilder::DefaultMoveToLeft;

		/// @brief 拡大倍率を増加させる（ズームインする）条件を指定する関数
		std::function<bool()> zoomIn = Camera2DControlBuilder::DefaultZoomIn;

		/// @brief 拡大倍率を減少させる（ズームアウトする）条件を指定する関数
		std::function<bool()> zoomOut = Camera2DControlBuilder::DefaultZoomOut;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit Camera2DControl(
			double _minZoom = DefaultMinZoom,
			double _maxZoom = DefaultMaxZoom,
			double _zoomSmoothTime = DefaultZoomSmoothTime,
			double _positionSmoothTime = DefaultPositionSmoothTime,
			double _wheelZoomSpeed = DefaultWheelZoomSpeed,
			double _mouseDragPanningSpeed = DefaultMouseDragPanningSpeed,
			double _controlZoomSpeed = DefaultControlZoomSpeed,
			double _controlPanningSpeed = DefaultControlPanningSpeed,
			std::function<bool()> _moveToUp = Camera2DControlBuilder::DefaultMoveToUp,
			std::function<bool()> _moveToRight = Camera2DControlBuilder::DefaultMoveToRight,
			std::function<bool()> _moveToDown = Camera2DControlBuilder::DefaultMoveToDown,
			std::function<bool()> _moveToLeft = Camera2DControlBuilder::DefaultMoveToLeft,
			std::function<bool()> _zoomIn = Camera2DControlBuilder::DefaultZoomIn,
			std::function<bool()> _zoomOut = Camera2DControlBuilder::DefaultZoomOut
		);

		[[nodiscard]]
		Camera2DControl(const Camera2DControlBuilder& builder);

		////////////////////////////////////////////////////////////////
		//
		//	(constant)
		//
		////////////////////////////////////////////////////////////////

		inline static const Camera2DControlBuilder None{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder WASD{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			{}, {} };

		inline static const Camera2DControlBuilder UpDown{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			{}, {}, {}, {},
			Camera2DControlBuilder::DefaultZoomIn, Camera2DControlBuilder::DefaultZoomOut };

		inline static const Camera2DControlBuilder WASDUpDown{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			Camera2DControlBuilder::DefaultZoomIn, Camera2DControlBuilder::DefaultZoomOut };

		inline static const Camera2DControlBuilder Wheel{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, DefaultWheelZoomSpeed, NoMouseDragPanning, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder RightClick{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, DefaultMouseDragPanningSpeed, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder Mouse{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, DefaultWheelZoomSpeed, DefaultMouseDragPanningSpeed, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder WASDMouse{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, DefaultWheelZoomSpeed, DefaultMouseDragPanningSpeed, DefaultControlZoomSpeed, DefaultControlPanningSpeed,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			{}, {} };

		inline static const Camera2DControlBuilder Default{};
	};
}
