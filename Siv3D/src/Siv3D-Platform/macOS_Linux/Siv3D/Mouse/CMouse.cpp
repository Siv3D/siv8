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

		m_window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());

		if (m_window)
		{
			::glfwSetMouseButtonCallback(m_window, OnMouseButtonUpdated);
			::glfwSetScrollCallback(m_window, OnScroll);
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

	void CMouse::onMouseButtonUpdated(const int32, const bool)
	{

	}

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
		return none;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void CMouse::OnMouseButtonUpdated(GLFWwindow*, const int button, const int action, int)
	{
		SIV3D_ENGINE(Mouse)->onMouseButtonUpdated(button, (action == GLFW_PRESS));
	}

	void CMouse::OnScroll(GLFWwindow*, const double xOffset, const double yOffset)
	{
		SIV3D_ENGINE(Mouse)->onScroll(-xOffset, -yOffset);
	}
}
