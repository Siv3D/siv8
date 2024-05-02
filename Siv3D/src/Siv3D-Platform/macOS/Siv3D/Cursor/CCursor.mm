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

# include "CCursor.hpp"
# include <Siv3D/Time.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Window/IWindow.hpp>
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
			//const Vec2 clientPos = m_transformAllInv.transformPoint(lastClientPos / scaling);
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

	bool CCursor::update()
	{
		if (m_clippedToWindow)
		{
			// [Siv3D ToDo]
		}
		
		m_highTemporalResolutionCursor.update();
		
		{
			const Point screenPos = GetCursorScreenPos();
			const Point rawClientPos = GetCursorRawClientPos(screenPos);
			updateHighTemporalResolutionCursorPos(rawClientPos);
			
			const double windowScaling = SIV3D_ENGINE(Window)->getState().scaling;
			//const Vec2 clientPos = m_transformAllInv.transformPoint(lastClientPos / scaling);
			const Vec2 clientPos = (rawClientPos / windowScaling);
			m_state.update(screenPos, rawClientPos, clientPos);
		}
		
		return true;
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
		//const Vec2 rawPos = m_transformAll.transformPoint(pos);
		const Vec2 rawPos = pos;
		
		::glfwSetCursorPos(m_window, rawPos.x, rawPos.y);
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
