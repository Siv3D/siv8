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

# include "CCursorStyle.hpp"
# include <Siv3D/PointVector.hpp>
# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static GLFWcursor* CreateCursor(const Image& image, const Point hotSpot, const AlphaPremultiplied alphaPremultiplied)
		{
			Image pixels = image.cloned();

			if (alphaPremultiplied)
			{
				for (auto& pixel : pixels)
				{
					pixel = Color::UnpremultiplyAlpha(pixel);
				}
			}

			GLFWimage cursorImage;
			cursorImage.width	= image.width();
			cursorImage.height	= image.height();
			cursorImage.pixels	= reinterpret_cast<uint8*>(pixels.data());

			return ::glfwCreateCursor(&cursorImage, hotSpot.x, hotSpot.y);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	~CCursorStyle
	//
	////////////////////////////////////////////////////////////////

	CCursorStyle::~CCursorStyle()
	{
		LOG_SCOPED_DEBUG("CCursorStyle::~CCursorStyle()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::init()
	{
		LOG_SCOPED_DEBUG("CCursorStyle::init()");

		m_window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());

		m_systemCursors[FromEnum(CursorStyle::Arrow)]			= ::glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::IBeam)]			= ::glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::Cross)]			= ::glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::Hand)]			= ::glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::NotAllowed)]		= ::glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::ResizeUpDown)]	= ::glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::ResizeLeftRight)]	= ::glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::ResizeNWSE)]		= ::glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::ResizeNESW)]		= ::glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::ResizeAll)]		= ::glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
		m_systemCursors[FromEnum(CursorStyle::Hidden)]			= CreateCursor(Image{ 16, 16, Color{ 0, 0 } }, Point::Zero(), AlphaPremultiplied::No);

		m_currentCursor		= m_systemCursors[FromEnum(CursorStyle::Arrow)];
		m_defaultCursor		= m_currentCursor;
		m_requestedCursor	= m_defaultCursor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	bool CCursorStyle::update()
	{
		if (not Cursor::OnClientRect())
		{
			m_requestedCursor = m_systemCursors[FromEnum(CursorStyle::Arrow)];
		}
		
		{
			m_currentCursor = m_requestedCursor;

			::glfwSetCursor(m_window, m_currentCursor);
		}

		m_requestedCursor = m_defaultCursor;

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setDefaultStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::setDefaultStyle(const CursorStyle style)
	{
		m_defaultCursor = m_systemCursors[FromEnum(style)];
	}

	////////////////////////////////////////////////////////////////
	//
	//	registerCursor
	//
	////////////////////////////////////////////////////////////////

	bool CCursorStyle::registerCustomStyle(const StringView name, const Image& image, const Point hotSpot, const AlphaPremultiplied alphaPremultiplied)
	{
		if (m_customCursors.contains(name))
		{
			LOG_FAIL(fmt::format("❌ CCursorStyle::registerCustomStyle(): `{}` is already registered", name));
			return false;
		}

		if (GLFWcursor* cursor = CreateCursor(image, hotSpot, alphaPremultiplied))
		{
			m_customCursors.emplace(name, UniqueResource{ cursor, CursorDeleter });
			return true;
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	unregisterCustomStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::unregisterCustomStyle(const StringView name)
	{
		::glfwSetCursor(m_window, m_systemCursors[FromEnum(CursorStyle::Arrow)]);
		
		if (auto it = m_customCursors.find(name);
			it != m_customCursors.end())
		{
			const auto cursor = it->second.get();

			if (m_defaultCursor == cursor)
			{
				m_defaultCursor = m_systemCursors[FromEnum(CursorStyle::Arrow)];
			}

			if (m_currentCursor == cursor)
			{
				m_currentCursor = m_defaultCursor;
			}

			if (m_requestedCursor == cursor)
			{
				m_requestedCursor = m_defaultCursor;
			}

			m_customCursors.erase(it);
		}		
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearCustomStyles
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::clearCustomStyles()
	{
		::glfwSetCursor(m_window, m_systemCursors[FromEnum(CursorStyle::Arrow)]);
		
		for (const auto& [name, cursor] : m_customCursors)
		{
			if (m_defaultCursor == cursor.get())
			{
				m_defaultCursor = m_systemCursors[FromEnum(CursorStyle::Arrow)];
			}

			if (m_currentCursor == cursor.get())
			{
				m_currentCursor = m_defaultCursor;
			}

			if (m_requestedCursor == cursor.get())
			{
				m_requestedCursor = m_defaultCursor;
			}
		}

		m_customCursors.clear();		
	}

	////////////////////////////////////////////////////////////////
	//
	//	requestStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::requestStyle(const CursorStyle style)
	{
		m_requestedCursor = m_systemCursors[FromEnum(style)];
	}

	////////////////////////////////////////////////////////////////
	//
	//	requestStyle
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::requestStyle(const StringView name)
	{
		if (auto it = m_customCursors.find(name);
			it != m_customCursors.end())
		{
			m_requestedCursor = it->second.get();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onSetCursor
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::onSetCursor() {}

	////////////////////////////////////////////////////////////////
	//
	//	(private functions)
	//
	////////////////////////////////////////////////////////////////

	void CCursorStyle::CursorDeleter(GLFWcursor* h)
	{
		::glfwDestroyCursor(h);
	}
}
