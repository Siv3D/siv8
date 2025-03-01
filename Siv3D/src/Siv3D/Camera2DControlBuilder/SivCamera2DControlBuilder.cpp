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
		const double _minScale,
		const double _maxScale,
		const double _scaleSmoothTime,
		const double _positionSmoothTime,
		const double _wheelScaleFactor,
		const double _grabSpeedFactor,
		const double _controlScaleFactor,
		const double _controlSpeedFactor,
		std::function<bool()> _moveToUp,
		std::function<bool()> _moveToRight,
		std::function<bool()> _moveToDown,
		std::function<bool()> _moveToLeft,
		std::function<bool()> _zoomIn,
		std::function<bool()> _zoomOut)
		: m_minScale{ _minScale }
		, m_maxScale{ _maxScale }
		, m_scaleSmoothTime{ _scaleSmoothTime }
		, m_positionSmoothTime{ _positionSmoothTime }
		, m_wheelScaleFactor{ _wheelScaleFactor }
		, m_grabSpeedFactor{ _grabSpeedFactor }
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
	//	minScale
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::minScale(const double _minScale) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_minScale = _minScale;
		return result;
	}

	double Camera2DControlBuilder::minScale() const noexcept
	{
		return m_minScale;
	}

	////////////////////////////////////////////////////////////////
	//
	//	maxScale
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::maxScale(const double _maxScale) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_maxScale = _maxScale;
		return result;
	}

	double Camera2DControlBuilder::maxScale() const noexcept
	{
		return m_maxScale;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleSmoothTime
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::scaleSmoothTime(const double _scaleSmoothTime) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_scaleSmoothTime = _scaleSmoothTime;
		return result;
	}

	double Camera2DControlBuilder::scaleSmoothTime() const noexcept
	{
		return m_scaleSmoothTime;
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
	//	wheelScaleFactor
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::wheelScaleFactor(const double _wheelScaleFactor) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_wheelScaleFactor = _wheelScaleFactor;
		return result;
	}

	double Camera2DControlBuilder::wheelScaleFactor() const noexcept
	{
		return m_wheelScaleFactor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	grabSpeedFactor
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::grabSpeedFactor(const double _grabSpeedFactor) const noexcept
	{
		Camera2DControlBuilder result{ *this };
		result.m_grabSpeedFactor = _grabSpeedFactor;
		return result;
	}

	double Camera2DControlBuilder::grabSpeedFactor() const noexcept
	{
		return m_grabSpeedFactor;
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
			result.m_wheelScaleFactor = 0.15;
		}
		else
		{
			result.m_wheelScaleFactor = 0.0;
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClick
	//
	////////////////////////////////////////////////////////////////

	Camera2DControlBuilder Camera2DControlBuilder::rightClick(const bool enabled) const
	{
		Camera2DControlBuilder result{ *this };

		if (enabled)
		{
			result.m_grabSpeedFactor = 4.0;
		}
		else
		{
			result.m_grabSpeedFactor = 0.0;
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
