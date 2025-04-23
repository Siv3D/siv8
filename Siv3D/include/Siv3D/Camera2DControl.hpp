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
		static constexpr double DefaultMinZoom = Camera2DControlBuilder::DefaultMinZoom;

		static constexpr double DefaultMaxZoom = Camera2DControlBuilder::DefaultMaxZoom;

		static constexpr double DefaultZoomSmoothTime = Camera2DControlBuilder::DefaultZoomSmoothTime;

		static constexpr double DefaultPositionSmoothTime = Camera2DControlBuilder::DefaultPositionSmoothTime;

		static constexpr double DefaultWheelZoomSpeed = Camera2DControlBuilder::DefaultWheelZoomSpeed;

		static constexpr double NoWheelZoom = Camera2DControlBuilder::NoWheelZoom;

		static constexpr double DefaultMouseDragPanningSpeed = Camera2DControlBuilder::DefaultMouseDragPanningSpeed;

		static constexpr double NoMouseDragPanning = Camera2DControlBuilder::NoMouseDragPanning;

		/// @brief 最小カメラズーム
		double minZoom = DefaultMinZoom;

		/// @brief 最大カメラズーム
		double maxZoom = DefaultMaxZoom;

		/// @brief カメラズームの平滑化時間（秒）
		double zoomSmoothTime = DefaultZoomSmoothTime;

		/// @brief 位置変更の平滑化時間（秒）
		double positionSmoothTime = DefaultPositionSmoothTime;

		/// @brief ホイールによるカメラズームのスピード
		double wheelZoomSpeed = DefaultWheelZoomSpeed;

		/// @brief マウスによる位置変更の速さ
		double mouseDragPanningSpeed = DefaultMouseDragPanningSpeed;

		/// @brief キー入力による拡大倍率変更のスケールファクター
		double controlScaleFactor = 2.0;

		/// @brief キー入力による位置変更の速さ
		double controlSpeedFactor = 400.0;

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
			double _controlScaleFactor = 2.0,
			double _controlSpeedFactor = 400.0,
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

		inline static const Camera2DControlBuilder None{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder WASD{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, 2.0, 400.0,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			{}, {} };

		inline static const Camera2DControlBuilder UpDown{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, 2.0, 400.0,
			{}, {}, {}, {},
			Camera2DControlBuilder::DefaultZoomIn, Camera2DControlBuilder::DefaultZoomOut };

		inline static const Camera2DControlBuilder WASDUpDown{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, NoMouseDragPanning, 2.0, 400.0,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			Camera2DControlBuilder::DefaultZoomIn, Camera2DControlBuilder::DefaultZoomOut };

		inline static const Camera2DControlBuilder Wheel{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, DefaultWheelZoomSpeed, NoMouseDragPanning, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder RightClick{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, NoWheelZoom, DefaultMouseDragPanningSpeed, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder Mouse{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, DefaultWheelZoomSpeed, DefaultMouseDragPanningSpeed, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder WASDMouse{ DefaultMinZoom, DefaultMaxZoom, DefaultZoomSmoothTime, DefaultPositionSmoothTime, DefaultWheelZoomSpeed, DefaultMouseDragPanningSpeed, 2.0, 400.0,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			{}, {} };

		inline static const Camera2DControlBuilder Default{};
	};
}
