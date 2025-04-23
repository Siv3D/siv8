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
# include <functional>
# include "Common.hpp"
# include "Optional.hpp"

namespace s3d
{
	class Camera2DControlBuilder
	{
	public:

		/// @brief デフォルトの最小カメラズーム
		static constexpr double DefaultMinZoom = (1.0 / 128.0);

		/// @brief デフォルトの最大カメラズーム
		static constexpr double DefaultMaxZoom = 128.0;

		/// @brief デフォルトのカメラズームの平滑化時間（秒）
		static constexpr double DefaultZoomSmoothTime = 0.2;

		/// @brief デフォルトのカメラ位置の平滑化時間（秒）
		static constexpr double DefaultPositionSmoothTime = 0.2;

		/// @brief デフォルトのホイールによるカメラズームのスピード
		static constexpr double DefaultWheelZoomSpeed = 0.15;

		/// @brief ホイールによるカメラズーム無し
		static constexpr double NoWheelZoom = 0.0;

		/// @brief デフォルトの右ボタンドラッグによるカメラ移動のスピード
		static constexpr double DefaultMouseDragPanningSpeed = 4.0;

		/// @brief 右ボタンドラッグによるカメラ移動無し
		static constexpr double NoMouseDragPanning = 0.0;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit Camera2DControlBuilder(
			double _minZoom = DefaultMinZoom,
			double _maxZoom = DefaultMaxZoom,
			double _zoomSmoothTime = DefaultZoomSmoothTime,
			double _positionSmoothTime = DefaultPositionSmoothTime,
			double _wheelZoomSpeed = DefaultWheelZoomSpeed,
			double _mouseDragPanningSpeed = DefaultMouseDragPanningSpeed,
			double _controlScaleFactor = 2.0,
			double _controlSpeedFactor = 400.0,
			std::function<bool()> _moveToUp = DefaultMoveToUp,
			std::function<bool()> _moveToRight = DefaultMoveToRight,
			std::function<bool()> _moveToDown = DefaultMoveToDown,
			std::function<bool()> _moveToLeft = DefaultMoveToLeft,
			std::function<bool()> _zoomIn = DefaultZoomIn,
			std::function<bool()> _zoomOut = DefaultZoomOut
		);

		////////////////////////////////////////////////////////////////
		//
		//	minZoom
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder minZoom(double _minZoom) const noexcept;

		[[nodiscard]]
		double minZoom() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	maxZoom
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder maxZoom(double _maxZoom) const noexcept;

		[[nodiscard]]
		double maxZoom() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	zoomSmoothTime
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder zoomSmoothTime(double _zoomSmoothTime) const noexcept;

		[[nodiscard]]
		double zoomSmoothTime() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	positionSmoothTime
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder positionSmoothTime(double _positionSmoothTime) const noexcept;

		[[nodiscard]]
		double positionSmoothTime() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	wheelZoomSpeed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder wheelZoomSpeed(double _wheelZoomSpeed) const noexcept;

		[[nodiscard]]
		double wheelZoomSpeed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mouseDragPanningSpeed
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder mouseDragPanningSpeed(double _mouseDragPanningSpeed) const noexcept;

		[[nodiscard]]
		double mouseDragPanningSpeed() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	controlScaleFactor
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder controlScaleFactor(double _controlScaleFactor) const noexcept;

		[[nodiscard]]
		double controlScaleFactor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	controlSpeedFactor
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder controlSpeedFactor(double _controlSpeedFactor) const noexcept;

		[[nodiscard]]
		double controlSpeedFactor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveToUp
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder moveToUp(std::function<bool()> _moveToUp) const noexcept;

		[[nodiscard]]
		std::function<bool()> moveToUp() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveToRight
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder moveToRight(std::function<bool()> _moveToRight) const noexcept;

		[[nodiscard]]
		std::function<bool()> moveToRight() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveToDown
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder moveToDown(std::function<bool()> _moveToDown) const noexcept;

		[[nodiscard]]
		std::function<bool()> moveToDown() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	moveToLeft
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder moveToLeft(std::function<bool()> _moveToLeft) const noexcept;

		[[nodiscard]]
		std::function<bool()> moveToLeft() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	zoomIn
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder zoomIn(std::function<bool()> _zoomIn) const noexcept;

		[[nodiscard]]
		std::function<bool()> zoomIn() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	zoomOut
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder zoomOut(std::function<bool()> _zoomOut) const noexcept;

		[[nodiscard]]
		std::function<bool()> zoomOut() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	wasd
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder wasd(bool enabled) const;

		////////////////////////////////////////////////////////////////
		//
		//	upDown
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder upDown(bool enabled) const;

		////////////////////////////////////////////////////////////////
		//
		//	wheel
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder wheel(bool enabled) const;

		////////////////////////////////////////////////////////////////
		//
		//	mouseDragPanning
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder mouseDragPanning(bool enabled) const;

		////////////////////////////////////////////////////////////////
		//
		//	(static function)
		//
		////////////////////////////////////////////////////////////////

		static bool DefaultMoveToUp();

		static bool DefaultMoveToLeft();

		static bool DefaultMoveToDown();

		static bool DefaultMoveToRight();

		static bool DefaultZoomIn();

		static bool DefaultZoomOut();

	private:

		double m_minZoom = DefaultMinZoom;

		double m_maxZoom = DefaultMaxZoom;

		double m_zoomSmoothTime = DefaultZoomSmoothTime;

		double m_positionSmoothTime = DefaultPositionSmoothTime;

		double m_wheelZoomSpeed = DefaultWheelZoomSpeed;

		double m_mouseDragPanningSpeed = DefaultMouseDragPanningSpeed;

		double m_controlScaleFactor = 2.0;

		double m_controlSpeedFactor = 400.0;

		std::function<bool()> m_moveToUp = DefaultMoveToUp;

		std::function<bool()> m_moveToRight = DefaultMoveToRight;

		std::function<bool()> m_moveToDown = DefaultMoveToDown;

		std::function<bool()> m_moveToLeft = DefaultMoveToLeft;

		std::function<bool()> m_zoomIn = DefaultZoomIn;

		std::function<bool()> m_zoomOut = DefaultZoomOut;
	};
}
