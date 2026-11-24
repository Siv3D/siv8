//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
				
				const double desktopTop = [screens[0] frame].size.height;
				const NSPoint mouseLocation = [NSEvent mouseLocation];
				return Math::Round(Vec2{ mouseLocation.x, (desktopTop - mouseLocation.y) }).asPoint();
			}
		}

		[[nodiscard]]
		static Point GetCursorRawClientPos(GLFWwindow* window)
		{
			// Siv3D's GLFW cursor query returns client backing pixels, including outside
			// the window. Desktop points must not be subtracted from these coordinates.
			double x, y;
			::glfwGetCursorPos(window, &x, &y);
			return Math::Round(Vec2{ x, y }).asPoint();
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
			const Point rawClientPos = GetCursorRawClientPos(m_window);
			updateHighTemporalResolutionCursorPos(rawClientPos);
			
			const double windowScaling = SIV3D_ENGINE(Window)->getState().scaling;
			m_transform.setBaseWindow({ windowScaling, RectF{} });
			m_state.advanceRaw(screenPos, rawClientPos);
			m_state.refreshTransformed(m_transform);
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
			const Point rawClientPos = GetCursorRawClientPos(m_window);
			updateHighTemporalResolutionCursorPos(rawClientPos);
			
			m_state.advanceRaw(screenPos, rawClientPos);
			m_state.refreshTransformed(m_transform);
		}

		m_captured = false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getState
	//
	////////////////////////////////////////////////////////////////

	void CCursor::updateBaseWindowTransform()
	{
		if (m_transform.setBaseWindow(SIV3D_ENGINE(Renderer)->getLetterboxComposition()))
		{
			m_state.refreshTransformed(m_transform);
		}
	}

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
		const Vec2 rawPos = m_transform.all.transformPoint(Vec2{ pos });
		
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
		if (m_transform.setCamera(matrix))
		{
			m_state.refreshTransformed(m_transform);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setLocalTransform
	//
	////////////////////////////////////////////////////////////////

	void CCursor::setLocalTransform(const Mat3x2& matrix)
	{
		if (m_transform.setLocal(matrix))
		{
			m_state.refreshTransformed(m_transform);
		}
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
