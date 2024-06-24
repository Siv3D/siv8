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
# include <functional>
# include "Common.hpp"

namespace s3d
{
	class Camera2DControlBuilder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit Camera2DControlBuilder(
			double _minScale = (1.0 / 128.0),
			double _maxScale = 128.0,
			double _scaleSmoothTime = 0.2,
			double _positionSmoothTime = 0.2,
			double _wheelScaleFactor = 1.125,
			double _grabSpeedFactor = 4.0,
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
		//	minScale
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder minScale(double _minScale) const noexcept;

		[[nodiscard]]
		double minScale() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	maxScale
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder maxScale(double _maxScale) const noexcept;

		[[nodiscard]]
		double maxScale() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	scaleSmoothTime
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder scaleSmoothTime(double _scaleSmoothTime) const noexcept;

		[[nodiscard]]
		double scaleSmoothTime() const noexcept;

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
		//	wheelScaleFactor
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder wheelScaleFactor(double _wheelScaleFactor) const noexcept;

		[[nodiscard]]
		double wheelScaleFactor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	grabSpeedFactor
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Camera2DControlBuilder grabSpeedFactor(double _grabSpeedFactor) const noexcept;

		[[nodiscard]]
		double grabSpeedFactor() const noexcept;

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

		double m_minScale = (1.0 / 128.0);

		double m_maxScale = 128.0;

		double m_scaleSmoothTime = 0.2;

		double m_positionSmoothTime = 0.2;

		double m_wheelScaleFactor = 1.125;

		double m_grabSpeedFactor = 4.0;

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
