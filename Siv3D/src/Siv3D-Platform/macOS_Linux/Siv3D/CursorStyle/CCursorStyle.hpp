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

# pragma once
# include <Siv3D/HashTable.hpp>
# include <Siv3D/UniqueResource.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d
{
	class CCursorStyle final : public ISiv3DCursorStyle
	{
	public:

		CCursorStyle() = default;

		~CCursorStyle() override;

		void init() override;

		void update() override;

		void setDefaultStyle(CursorStyle style) override;

		bool registerCustomStyle(StringView name, const Image& image, Point hotSpot, IsAlphaPremultiplied isAlphaPremultiplied) override;

		void unregisterCustomStyle(StringView name) override;

		void clearCustomStyles() override;
		
		void requestStyle(CursorStyle style) override;

		void requestStyle(StringView name) override;
		
		void onSetCursor() override;

	private:
		
		GLFWwindow* m_window = nullptr;

		std::array<GLFWcursor*, 11> m_systemCursors;
		
		GLFWcursor* m_currentCursor;
		
		GLFWcursor* m_defaultCursor;
		
		GLFWcursor* m_requestedCursor;

		static void CursorDeleter(GLFWcursor* h);
		
		HashTable<String, UniqueResource<GLFWcursor*, decltype(&CursorDeleter)>> m_customCursors;
	};
}
