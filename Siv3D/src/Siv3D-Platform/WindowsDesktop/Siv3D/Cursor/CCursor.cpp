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
# include <Siv3D/WindowState.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		/// @brief スクリーンカーソル座標を返します。
		/// @return スクリーンカーソル座標
		[[nodiscard]]
		static Point GetCursorScreenPos()
		{
			POINT screenPos;
			::GetCursorPos(&screenPos);
			return{ screenPos.x, screenPos.y };
		}

		/// @brief 補正前のクライアントカーソル座標を返します。
		/// @param screenPos スクリーンカーソル座標
		/// @param hWnd ウィンドウハンドル
		/// @return 補正前のクライアントカーソル座標
		[[nodiscard]]
		static Point GetCursorRawClientPos(const Point screenPos, const HWND hWnd)
		{
			POINT clientPos{ screenPos.x, screenPos.y };
			::ScreenToClient(hWnd, &clientPos);	
			return{ clientPos.x, clientPos.y };
		}

		static void ConfineCursor(const HWND hWnd)
		{
			RECT clientRect;
			::GetClientRect(hWnd, &clientRect);

			POINT leftTop{ clientRect.left, clientRect.top };
			::ClientToScreen(hWnd, &leftTop);

			RECT clipRect{ leftTop.x, leftTop.y,
				leftTop.x + Max<int32>(clientRect.right - 1, 0),
				leftTop.y + Max<int32>(clientRect.bottom - 1, 0) };
			::ClipCursor(&clipRect);
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

		m_hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());

		{
			const Point screenPos = GetCursorScreenPos();
			const Point rawClientPos = GetCursorRawClientPos(screenPos, m_hWnd);
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
			ConfineCursor(m_hWnd);
		}

		m_highTemporalResolutionCursor.update();

		{
			Point screenPos;

			if (false/*const auto touchPos = static_cast<CMouse*>(SIV3D_ENGINE(Mouse))->getPrimaryTouchPos()*/)
			{
				//screenPos.x = touchPos->x;
				//screenPos.y = touchPos->y;
			}
			else
			{
				screenPos = GetCursorScreenPos();
			}

			const Point rawClientPos = GetCursorRawClientPos(screenPos, m_hWnd);
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

		POINT point{ static_cast<int32>(rawPos.x), static_cast<int32>(rawPos.y) };
		::ClientToScreen(m_hWnd, &point);
		::SetCursorPos(point.x, point.y);

		update();
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

		if (m_clippedToWindow)
		{
			ConfineCursor(m_hWnd);
		}
		else
		{
			::ClipCursor(nullptr);
		}
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
