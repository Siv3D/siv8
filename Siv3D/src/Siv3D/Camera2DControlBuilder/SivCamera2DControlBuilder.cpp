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

# include <Siv3D/Camera2DControlBuilder.hpp>
# include <Siv3D/Keyboard.hpp>
# include <Siv3D/Mouse.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder::Camera2DControlBuilder(
		const double _minZoom,
		const double _maxZoom,
		const double _zoomSmoothTime,
		const double _positionSmoothTime,
		const double _wheelZoomSpeed,
		const double _mouseDragPanningSpeed,
		const double _controlScaleFactor,
		const double _controlSpeedFactor,
		std::function<bool()> _moveToUp,
		std::function<bool()> _moveToRight,
		std::function<bool()> _moveToDown,
		std::function<bool()> _moveToLeft,
		std::function<bool()> _zoomIn,
		std::function<bool()> _zoomOut)
		: m_minZoom{ _minZoom }
		, m_maxZoom{ _maxZoom }
		, m_zoomSmoothTime{ _zoomSmoothTime }
		, m_positionSmoothTime{ _positionSmoothTime }
		, m_wheelZoomSpeed{ _wheelZoomSpeed }
		, m_mouseDragPanningSpeed{ _mouseDragPanningSpeed }
		, m_controlScaleFactor{ _controlScaleFactor }
		, m_controlSpeedFactor{ _controlSpeedFactor }
		, m_moveToUp{ std::move(_moveToUp) }
		, m_moveToRight{ std::move(_moveToRight) }
		, m_moveToDown{ std::move(_moveToDown) }
		, m_moveToLeft{ std::move(_moveToLeft) }
		, m_zoomIn{ std::move(_zoomIn) }
		, m_zoomOut{ std::move(_zoomOut) } {}

	////////////////////////////////////////////////////////////////
	//
	//	minZoom
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::minZoom(const double _minZoom) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_minZoom = _minZoom;
		return result;
	}

	double Camera2DControlBuilder::minZoom() const noexcept
	{
		return m_minZoom;
	}

	////////////////////////////////////////////////////////////////
	//
	//	maxZoom
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::maxZoom(const double _maxZoom) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_maxZoom = _maxZoom;
		return result;
	}

	double Camera2DControlBuilder::maxZoom() const noexcept
	{
		return m_maxZoom;
	}

	////////////////////////////////////////////////////////////////
	//
	//	zoomSmoothTime
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::zoomSmoothTime(const double _zoomSmoothTime) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_zoomSmoothTime = _zoomSmoothTime;
		return result;
	}

	double Camera2DControlBuilder::zoomSmoothTime() const noexcept
	{
		return m_zoomSmoothTime;
	}

	////////////////////////////////////////////////////////////////
	//
	//	positionSmoothTime
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::positionSmoothTime(const double _positionSmoothTime) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_positionSmoothTime = _positionSmoothTime;
		return result;
	}

	double Camera2DControlBuilder::positionSmoothTime() const noexcept
	{
		return m_positionSmoothTime;
	}

	////////////////////////////////////////////////////////////////
	//
	//	wheelScaleSpeed
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::wheelZoomSpeed(const double _wheelZoomSpeed) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_wheelZoomSpeed = _wheelZoomSpeed;
		return result;
	}

	double Camera2DControlBuilder::wheelZoomSpeed() const noexcept
	{
		return m_wheelZoomSpeed;
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseDragPanningSpeed
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::mouseDragPanningSpeed(const double _mouseDragPanningSpeed) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_mouseDragPanningSpeed = _mouseDragPanningSpeed;
		return result;
	}

	double Camera2DControlBuilder::mouseDragPanningSpeed() const noexcept
	{
		return m_mouseDragPanningSpeed;
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlScaleFactor
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::controlScaleFactor(const double _controlScaleFactor) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_controlScaleFactor = _controlScaleFactor;
		return result;
	}

	double Camera2DControlBuilder::controlScaleFactor() const noexcept
	{
		return m_controlScaleFactor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	controlSpeedFactor
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::controlSpeedFactor(const double _controlSpeedFactor) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_controlSpeedFactor = _controlSpeedFactor;
		return result;
	}

	double Camera2DControlBuilder::controlSpeedFactor() const noexcept
	{
		return m_controlSpeedFactor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveToUp
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::moveToUp(std::function<bool()> _moveToUp) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_moveToUp = std::move(_moveToUp);
		return result;
	}

	std::function<bool()> Camera2DControlBuilder::moveToUp() const noexcept
	{
		return m_moveToUp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveToLeft
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::moveToLeft(std::function<bool()> _moveToLeft) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_moveToLeft = std::move(_moveToLeft);
		return result;
	}

	std::function<bool()> Camera2DControlBuilder::moveToLeft() const noexcept
	{
		return m_moveToLeft;
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveToDown
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::moveToDown(std::function<bool()> _moveToDown) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_moveToDown = std::move(_moveToDown);
		return result;
	}

	std::function<bool()> Camera2DControlBuilder::moveToDown() const noexcept
	{
		return m_moveToDown;
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveToRight
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::moveToRight(std::function<bool()> _moveToRight) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_moveToRight = std::move(_moveToRight);
		return result;
	}

	std::function<bool()> Camera2DControlBuilder::moveToRight() const noexcept
	{
		return m_moveToRight;
	}

	////////////////////////////////////////////////////////////////
	//
	//	zoomIn
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::zoomIn(std::function<bool()> _zoomIn) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_zoomIn = std::move(_zoomIn);
		return result;
	}

	std::function<bool()> Camera2DControlBuilder::zoomIn() const noexcept
	{
		return m_zoomIn;
	}

	////////////////////////////////////////////////////////////////
	//
	//	zoomOut
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::zoomOut(std::function<bool()> _zoomOut) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_zoomOut = std::move(_zoomOut);
		return result;
	}

	std::function<bool()> Camera2DControlBuilder::zoomOut() const noexcept
	{
		return m_zoomOut;
	}

	////////////////////////////////////////////////////////////////
	//
	//	wasd
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::wasd(const bool enabled) const
	{
		Camera2DControlBuilder result{ *this };

		if (enabled)
		{
			result.m_moveToRight	= DefaultMoveToRight;
			result.m_moveToDown		= DefaultMoveToDown;
			result.m_moveToLeft		= DefaultMoveToLeft;
			result.m_moveToUp		= DefaultMoveToUp;
		}
		else
		{
			result.m_moveToRight	= nullptr;
			result.m_moveToDown		= nullptr;
			result.m_moveToLeft		= nullptr;
			result.m_moveToUp		= nullptr;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	upDown
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::upDown(const bool enabled) const
	{
		Camera2DControlBuilder result{ *this };

		if (enabled)
		{
			result.m_zoomIn		= DefaultZoomIn;
			result.m_zoomOut	= DefaultZoomOut;
		}
		else
		{
			result.m_zoomIn		= nullptr;
			result.m_zoomOut	= nullptr;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	wheel
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::wheel(const bool enabled) const
	{
		Camera2DControlBuilder result{ *this };

		if (enabled)
		{
			result.m_wheelZoomSpeed = DefaultWheelZoomSpeed;
		}
		else
		{
			result.m_wheelZoomSpeed = NoWheelZoom;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseDragPanning
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::mouseDragPanning(const bool enabled) const
	{
		Camera2DControlBuilder result{ *this };

		if (enabled)
		{
			result.m_mouseDragPanningSpeed = DefaultMouseDragPanningSpeed;
		}
		else
		{
			result.m_mouseDragPanningSpeed = NoMouseDragPanning;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(static function)
	//
	////////////////////////////////////////////////////////////////

	bool Camera2DControlBuilder::DefaultMoveToUp()
	{
		return KeyW.pressed();
	}

	bool Camera2DControlBuilder::DefaultMoveToRight()
	{
		return KeyD.pressed();
	}

	bool Camera2DControlBuilder::DefaultMoveToDown()
	{
		return KeyS.pressed();
	}

	bool Camera2DControlBuilder::DefaultMoveToLeft()
	{
		return KeyA.pressed();
	}

	bool Camera2DControlBuilder::DefaultZoomIn()
	{
		return KeyUp.pressed();
	}

	bool Camera2DControlBuilder::DefaultZoomOut()
	{
		return KeyDown.pressed();
	}
}
