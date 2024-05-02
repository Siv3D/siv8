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
	}

	CCursor::~CCursor()
	{
		LOG_SCOPED_DEBUG("CCursor::~CCursor()");
	}

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

	void CCursor::updateHighTemporalResolutionCursorPos(const Point rawClientPos)
	{
		m_highTemporalResolutionCursor.add(rawClientPos);
	}
	
	bool CCursor::update()
	{
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

	const CursorState& CCursor::getState() const
	{
		return m_state;
	}

	Array<std::pair<int64, Point>> CCursor::getHighTemporalResolutionCursorPos() const
	{
		return m_highTemporalResolutionCursor.get();
	}

	void CCursor::setPos(const Point pos)
	{
		//const Vec2 rawPos = m_transformAll.transformPoint(pos);
		const Vec2 rawPos = pos;

		POINT point{ static_cast<int32>(rawPos.x), static_cast<int32>(rawPos.y) };
		::ClientToScreen(m_hWnd, &point);
		::SetCursorPos(point.x, point.y);

		update();
	}
}
