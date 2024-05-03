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

# include "CMouse.hpp"
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	~CMouse
	//
	////////////////////////////////////////////////////////////////

	CMouse::~CMouse()
	{
		LOG_SCOPED_DEBUG("CMouse::~CMouse()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CMouse::init()
	{
		LOG_SCOPED_DEBUG("CMouse::init()");

		m_hWnd = static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());

		if (::GetSystemMetrics(SM_DIGITIZER) & NID_MULTI_INPUT)
		{
			m_touchAvailable = true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	bool CMouse::update()
	{



		{
			std::lock_guard lock{ m_wheelMutex };
			m_wheel = std::exchange(m_wheelInternal, Vec2{ 0, 0 });
		}


		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	wheel
	//
	////////////////////////////////////////////////////////////////

	Vec2 CMouse::wheel() const noexcept
	{
		return m_wheel;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onMouseButtonUpdated
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onMouseButtonUpdated(const int32, const bool) {}

	////////////////////////////////////////////////////////////////
	//
	//	onScroll
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onScroll(const double x, const double y)
	{
		std::lock_guard lock{ m_wheelMutex };
		m_wheelInternal.moveBy(x, y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPrimaryTouchPos
	//
	////////////////////////////////////////////////////////////////

	Optional<Point> CMouse::getPrimaryTouchPos()
	{
		std::lock_guard lock{ m_touchMutex };
		return m_primaryTouchScreenPos;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onTouchInput
	//
	////////////////////////////////////////////////////////////////

	void CMouse::onTouchInput(const Array<TOUCHINPUT>& touchInputs)
	{
		std::lock_guard lock{ m_touchMutex };

		for (size_t i = 0; i < touchInputs.size(); ++i)
		{
			if (!m_currentPrimaryTouchID
				&& (not (touchInputs[i].dwFlags & TOUCHEVENTF_UP)))
			{
				m_currentPrimaryTouchID = touchInputs[i].dwID;
				break;
			}

			if (touchInputs[0].dwFlags & TOUCHEVENTF_UP)
			{
				m_currentPrimaryTouchID.reset();
			}
		}

		m_primaryTouchScreenPos.reset();

		if (m_currentPrimaryTouchID)
		{
			for (const auto& touchInput : touchInputs)
			{
				if (touchInput.dwID == m_currentPrimaryTouchID)
				{
					const POINT screenPos{ (touchInput.x / 100), (touchInput.y / 100) };
					m_primaryTouchScreenPos.emplace(screenPos.x, screenPos.y);
				}
			}
		}
	}
}
