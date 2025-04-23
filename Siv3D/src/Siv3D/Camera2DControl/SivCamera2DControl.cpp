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
		: minZoom{ _minZoom }
		, maxZoom{ _maxZoom }
		, zoomSmoothTime{ _zoomSmoothTime }
		, positionSmoothTime{ _positionSmoothTime }
		, wheelZoomSpeed{ _wheelZoomSpeed }
		, mouseDragPanningSpeed{ _mouseDragPanningSpeed }
		, controlScaleFactor{ _controlScaleFactor }
		, controlSpeedFactor{ _controlSpeedFactor }
		, moveToUp{ std::move(_moveToUp) }
		, moveToRight{ std::move(_moveToRight) }
		, moveToDown{ std::move(_moveToDown) }
		, moveToLeft{ std::move(_moveToLeft) }
		, zoomIn{ std::move(_zoomIn) }
		, zoomOut{ std::move(_zoomOut) } {}

	Camera2DControl::Camera2DControl(const Camera2DControlBuilder& builder)
		: minZoom{ builder.minZoom() }
		, maxZoom{ builder.maxZoom() }
		, zoomSmoothTime{ builder.zoomSmoothTime() }
		, positionSmoothTime{ builder.positionSmoothTime() }
		, wheelZoomSpeed{ builder.wheelZoomSpeed() }
		, mouseDragPanningSpeed{ builder.mouseDragPanningSpeed() }
		, controlScaleFactor{ builder.controlScaleFactor() }
		, controlSpeedFactor{ builder.controlSpeedFactor() }
		, moveToUp{ builder.moveToUp() }
		, moveToRight{ builder.moveToRight() }
		, moveToDown{ builder.moveToDown() }
		, moveToLeft{ builder.moveToLeft() }
		, zoomIn{ builder.zoomIn() }
		, zoomOut{ builder.zoomOut() } {}
}
