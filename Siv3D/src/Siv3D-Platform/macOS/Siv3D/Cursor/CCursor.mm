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

# include "CCursor.hpp"
# include <Siv3D/Time.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Point GetCursorScreenPos()
		{
			@autoreleasepool
			{
				const NSArray<NSScreen*>* screens = [NSScreen screens];
				
				if (screens.count < 1)
				{
					return{ 0, 0 };
				}
				
				const float zeroScreenHeight = [screens[0] frame].size.height;
				const float zeroScreenScaleFactor = [screens[0] backingScaleFactor];
				const NSPoint mouseLocation = [NSEvent mouseLocation];
				//NSLog(@"%f %f", mouseLocation.x, mouseLocation.y);

				for (NSScreen* screen in screens)
				{
					const NSRect screenRect = [screen frame];
					const NSRect retinaScreenRect = [screen convertRectToBacking: screenRect];
					
					const NSRect placeholderRect = NSMakeRect(mouseLocation.x, mouseLocation.y, 0, 0);
					const NSRect retinaPlaceholderRect = [screen convertRectToBacking: placeholderRect];
					const NSPoint retinaMouseLocation = retinaPlaceholderRect.origin;
					//NSLog(@"%f %f", retinaMouseLocation.x, retinaMouseLocation.y);
					
					// マウスカーソルが retinaScreenRect 上にあるか
					if ((retinaScreenRect.origin.x <= retinaMouseLocation.x)
						&& (retinaMouseLocation.x <= (retinaScreenRect.origin.x + retinaScreenRect.size.width))
						&& (retinaScreenRect.origin.y <= retinaMouseLocation.y)
						&& (retinaMouseLocation.y <= (retinaScreenRect.origin.y + retinaScreenRect.size.height)))
					{
						const float scaleFactor = [screen backingScaleFactor];
						
						// スクリーンの左上の座標（macOS 座標系）
						const float retinaAx = retinaScreenRect.origin.x;
						const float retinaAy = (retinaScreenRect.origin.y + retinaScreenRect.size.height);
						
						// スクリーンの左上の座標（Siv3D 座標系）
						const float retinaSx = retinaAx;
						const float retinaSy = (zeroScreenHeight * scaleFactor - retinaAy);
						
						// スクリーン左上からのマウスカーソル位置オフセット
						const float retinaOx = (retinaMouseLocation.x - retinaAx);
						const float retinaOy = (retinaScreenRect.size.height - (retinaMouseLocation.y - retinaScreenRect.origin.y));

						return Math::Round(Vec2{ (retinaSx + retinaOx), (retinaSy + retinaOy) }).asPoint();
					}
				}
				
				// fallback
				return Math::Round(Vec2{ (mouseLocation.x * zeroScreenScaleFactor), ((zeroScreenHeight * zeroScreenScaleFactor) - (mouseLocation.y * zeroScreenScaleFactor)) }).asPoint();
			}
		}

		[[nodiscard]]
		static Point GetCursorRawClientPos(const Point screenPos)
		{
			const WindowState windowState = SIV3D_ENGINE(Window)->getState();
			return (screenPos - windowState.bounds.pos - Point{ 0, windowState.titleBarHeight });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	~CCursor
	//
	////////////////////////////////////////////////////////////////

	CCursor::~CCursor()
	{
		LOG_SCOPED_DEBUG("CCursor::~CCursor()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CCursor::init()
	{
		LOG_SCOPED_DEBUG("CCursor::init()");

		m_window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());

		{
			const Point screenPos = GetCursorScreenPos();
			const Point rawClientPos = GetCursorRawClientPos(screenPos);
			updateHighTemporalResolutionCursorPos(rawClientPos);
			
			const double windowScaling = SIV3D_ENGINE(Window)->getState().scaling;
			const Vec2 clientPos = (rawClientPos / windowScaling);
			m_state.update(screenPos, rawClientPos, clientPos);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateHighTemporalResolutionCursorPos
	//
	////////////////////////////////////////////////////////////////

	void CCursor::updateHighTemporalResolutionCursorPos(const Point rawClientPos)
	{
		m_highTemporalResolutionCursor.add(rawClientPos);
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CCursor::update()
	{
		if (m_clippedToWindow)
		{
			// [Siv3D ToDo]
		}
		
		m_highTemporalResolutionCursor.update();
	
		{
			m_transform.setBaseWindow(SIV3D_ENGINE(Renderer)->getLetterboxComposition());
		}
	
		{
			const Point screenPos = GetCursorScreenPos();
			const Point rawClientPos = GetCursorRawClientPos(screenPos);
			updateHighTemporalResolutionCursorPos(rawClientPos);
			
			const Vec2 clientPos = m_transform.allInv.transformPoint(rawClientPos);
			m_state.update(screenPos, rawClientPos, clientPos);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getState
	//
	////////////////////////////////////////////////////////////////

	const CursorState& CCursor::getState() const
	{
		return m_state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getHighTemporalResolutionCursorPos
	//
	////////////////////////////////////////////////////////////////

	Array<std::pair<int64, Point>> CCursor::getHighTemporalResolutionCursorPos() const
	{
		return m_highTemporalResolutionCursor.get();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	void CCursor::setPos(const Point pos)
	{
		const Vec2 rawPos = m_transform.all.transformPoint(pos);
		
		::glfwSetCursorPos(m_window, rawPos.x, rawPos.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBaseWindowTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CCursor::getBaseWindowTransform() const noexcept
	{
		return m_transform.baseWindow;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCameraTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CCursor::getCameraTransform() const noexcept
	{
		return m_transform.camera;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLocalTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& CCursor::getLocalTransform() const noexcept
	{
		return m_transform.local;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCameraTransform
	//
	////////////////////////////////////////////////////////////////

	void CCursor::setCameraTransform(const Mat3x2& matrix)
	{
		if (m_transform.camera == matrix)
		{
			return;
		}

		m_transform.setCamera(matrix);

		m_state.vec2.previous	= m_transform.allInv.transformPoint(m_state.raw.previous);
		m_state.vec2.current	= m_transform.allInv.transformPoint(m_state.raw.current);
		m_state.vec2.delta		= (m_state.vec2.current - m_state.vec2.previous);

		m_state.point.previous	= m_state.vec2.previous.asPoint();
		m_state.point.current	= m_state.vec2.current.asPoint();
		m_state.point.delta		= m_state.vec2.delta.asPoint();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setLocalTransform
	//
	////////////////////////////////////////////////////////////////

	void CCursor::setLocalTransform(const Mat3x2& matrix)
	{
		if (m_transform.local == matrix)
		{
			return;
		}

		m_transform.setLocal(matrix);

		m_state.vec2.previous	= m_transform.allInv.transformPoint(m_state.raw.previous);
		m_state.vec2.current	= m_transform.allInv.transformPoint(m_state.raw.current);
		m_state.vec2.delta		= (m_state.vec2.current - m_state.vec2.previous);

		m_state.point.previous	= m_state.vec2.previous.asPoint();
		m_state.point.current	= m_state.vec2.current.asPoint();
		m_state.point.delta		= m_state.vec2.delta.asPoint();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isClippedToWindow
	//
	////////////////////////////////////////////////////////////////

	bool CCursor::isClippedToWindow() const noexcept
	{
		return m_clippedToWindow;
	}

	////////////////////////////////////////////////////////////////
	//
	//	clipToWindow
	//
	////////////////////////////////////////////////////////////////

	void CCursor::clipToWindow(const bool clip)
	{
		if (clip == m_clippedToWindow)
		{
			return;
		}

		m_clippedToWindow = clip;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCapture
	//
	////////////////////////////////////////////////////////////////

	void CCursor::setCapture(const bool captured) noexcept
	{
		m_captured = captured;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isCaptured
	//
	////////////////////////////////////////////////////////////////

	bool CCursor::isCaptured() const noexcept
	{
		return m_captured;
	}
}
