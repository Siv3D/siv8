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
		static Point GetScreenPos()
		{
			@autoreleasepool
			{
				const NSScreen* mainScreen = [NSScreen mainScreen];
				const int32 mainScreenHeight = [mainScreen frame].size.height;
				const float mainScaleFactor = [mainScreen backingScaleFactor];
				const NSPoint mouseLocation = [NSEvent mouseLocation];
				
				for (NSScreen* screen in [NSScreen screens])
				{
					const NSRect frame = [screen frame];
					
					if ((frame.origin.x <= mouseLocation.x)
						&& (mouseLocation.x <= (frame.origin.x + frame.size.width))
						&& (frame.origin.y <= mouseLocation.y)
						&& (mouseLocation.y <= (frame.origin.y + frame.size.height)))
					{
						const float scaleFactor = [screen backingScaleFactor];
						return Math::Round(Vec2{ (mouseLocation.x * scaleFactor),
							((mainScreenHeight * scaleFactor) - (mouseLocation.y * scaleFactor)) }).asPoint();
					}
				}
				
				// fallback
				return Math::Round(Vec2{ (mouseLocation.x * mainScaleFactor), ((mainScreenHeight * mainScaleFactor) - (mouseLocation.y * mainScaleFactor)) }).asPoint();
			}
		}

		[[nodiscard]]
		static Point GetCursorRawClientPos(GLFWwindow* window)
		{
			double clientX, clientY;
			::glfwGetCursorPos(window, &clientX, &clientY);
			return Vec2{ clientX, clientY }.asPoint();
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
			const Point screenPos = GetScreenPos();
			const Point rawClientPos = GetCursorRawClientPos(m_window);
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
			const Point screenPos = GetScreenPos();
			const Point rawClientPos = GetCursorRawClientPos(m_window);
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
