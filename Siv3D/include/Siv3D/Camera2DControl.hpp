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
# include "Camera2DControlBuilder.hpp"

namespace s3d
{
	/// @brief 2D カメラの操作設定
	struct Camera2DControl
	{
		/// @brief 拡大倍率の最小値
		double minScale = (1.0 / 128.0);

		/// @brief 拡大倍率の最大値
		double maxScale = 128.0;

		/// @brief 拡大率変更の平滑化時間（秒）
		double scaleSmoothTime = 0.2;

		/// @brief 位置変更の平滑化時間（秒）
		double positionSmoothTime = 0.2;

		/// @brief マウスホイールによる拡大率変更のスケールファクタ
		double wheelScaleFactor = 1.125;

		/// @brief マウスによる位置変更の速さ
		double grabSpeedFactor = 4.0;

		/// @brief キー入力による拡大率変更のスケールファクタ
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

		/// @brief 拡大率を増加させる（ズームインする）条件を指定する関数
		std::function<bool()> zoomIn = Camera2DControlBuilder::DefaultZoomIn;

		/// @brief 拡大率を減少させる（ズームアウトする）条件を指定する関数
		std::function<bool()> zoomOut = Camera2DControlBuilder::DefaultZoomOut;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit Camera2DControl(
			double _minScale = (1.0 / 128.0),
			double _maxScale = 128.0,
			double _scaleSmoothTime = 0.2,
			double _positionSmoothTime = 0.2,
			double _wheelScaleFactor = 1.125,
			double _grabSpeedFactor = 4.0,
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

		inline static const Camera2DControlBuilder None{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.0, 0.0, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder WASD{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.0, 0.0, 2.0, 400.0,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			{}, {} };

		inline static const Camera2DControlBuilder UpDown{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.0, 0.0, 2.0, 400.0,
			{}, {}, {}, {},
			Camera2DControlBuilder::DefaultZoomIn, Camera2DControlBuilder::DefaultZoomOut };

		inline static const Camera2DControlBuilder WASDUpDown{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.0, 0.0, 2.0, 400.0,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			Camera2DControlBuilder::DefaultZoomIn, Camera2DControlBuilder::DefaultZoomOut };

		inline static const Camera2DControlBuilder Wheel{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.125, 0.0, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder Mouse{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.125, 4.0, 2.0, 400.0,
			{}, {}, {}, {},
			{}, {} };

		inline static const Camera2DControlBuilder WASDMouse{ (1.0 / 128.0), 128.0, 0.2, 0.2, 1.125, 4.0, 2.0, 400.0,
			Camera2DControlBuilder::DefaultMoveToUp, Camera2DControlBuilder::DefaultMoveToRight, Camera2DControlBuilder::DefaultMoveToDown, Camera2DControlBuilder::DefaultMoveToLeft,
			{}, {} };

		inline static const Camera2DControlBuilder Default{};
	};
}
