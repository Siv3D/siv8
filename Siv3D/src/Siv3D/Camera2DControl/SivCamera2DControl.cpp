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

# include <Siv3D/Camera2DControl.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Camera2DControl::Camera2DControl(
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
		: minScale{ _minScale }
		, maxScale{ _maxScale }
		, scaleSmoothTime{ _scaleSmoothTime }
		, positionSmoothTime{ _positionSmoothTime }
		, wheelScaleFactor{ _wheelScaleFactor }
		, grabSpeedFactor{ _grabSpeedFactor }
		, controlScaleFactor{ _controlScaleFactor }
		, controlSpeedFactor{ _controlSpeedFactor }
		, moveToUp{ std::move(_moveToUp) }
		, moveToRight{ std::move(_moveToRight) }
		, moveToDown{ std::move(_moveToDown) }
		, moveToLeft{ std::move(_moveToLeft) }
		, zoomIn{ std::move(_zoomIn) }
		, zoomOut{ std::move(_zoomOut) } {}

	Camera2DControl::Camera2DControl(const Camera2DControlBuilder& builder)
		: minScale{ builder.minScale() }
		, maxScale{ builder.maxScale() }
		, scaleSmoothTime{ builder.scaleSmoothTime() }
		, positionSmoothTime{ builder.positionSmoothTime() }
		, wheelScaleFactor{ builder.wheelScaleFactor() }
		, grabSpeedFactor{ builder.grabSpeedFactor() }
		, controlScaleFactor{ builder.controlScaleFactor() }
		, controlSpeedFactor{ builder.controlSpeedFactor() }
		, moveToUp{ builder.moveToUp() }
		, moveToRight{ builder.moveToRight() }
		, moveToDown{ builder.moveToDown() }
		, moveToLeft{ builder.moveToLeft() }
		, zoomIn{ builder.zoomIn() }
		, zoomOut{ builder.zoomOut() } {}
}
