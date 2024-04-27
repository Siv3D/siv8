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

				for (NSScreen* screen in screens)
				{
					const NSRect screenRect = [screen frame];

					// マウスカーソルが screenRect 上にあるか
					if ((screenRect.origin.x <= mouseLocation.x)
						&& (mouseLocation.x <= (screenRect.origin.x + screenRect.size.width))
						&& (screenRect.origin.y <= mouseLocation.y)
						&& (mouseLocation.y <= (screenRect.origin.y + screenRect.size.height)))
					{
						// スクリーンの左上の座標（macOS 座標系）
						const float ax = screenRect.origin.x;
						const float ay = (screenRect.origin.y + screenRect.size.height);
						
						// スクリーンの左上の座標（Siv3D 座標系）
						const float sx = ax;
						const float sy = (zeroScreenHeight - ay);
						
						// スクリーン左上からのマウスカーソル位置オフセット
						const float ox = (mouseLocation.x - ax);
						const float oy = (screenRect.size.height - (mouseLocation.y - screenRect.origin.y));
						
						const float scaleFactor = [screen backingScaleFactor];
						return Math::Round(Vec2{ (sx + ox * scaleFactor), (sy + oy * scaleFactor) }).asPoint();
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

	CCursor::~CCursor()
	{
		LOG_SCOPED_DEBUG("CCursor::~CCursor()");
	}

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

	void CCursor::updateHighTemporalResolutionCursorPos(const Point rawClientPos)
	{
		m_highTemporalResolutionCursor.add(rawClientPos);
	}
	
	bool CCursor::update()
	{
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

	const CursorState& CCursor::getState() const
	{
		return m_state;
	}

	Array<std::pair<int64, Point>> CCursor::getHighTemporalResolutionCursorPos() const
	{
		return m_highTemporalResolutionCursor.get();
	}
}
