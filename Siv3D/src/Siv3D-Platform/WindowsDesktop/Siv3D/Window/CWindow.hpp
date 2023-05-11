//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/DLL.hpp>
# include <Siv3D/Window.hpp>

namespace s3d
{
	class CWindow final : public ISiv3DWindow
	{
	public:

		CWindow();

		~CWindow() override;

		void init() override;

		void update() override;
		
		void setWindowTitle(const String& title) override;

		const String& getWindowTitle() const noexcept override;

		void* getHandle() const noexcept override;

		void destroy();

	private:

		HWND m_hWnd = nullptr;

		HINSTANCE m_hInstance = nullptr;

		struct User32Lib
		{
			LibraryHandle library = nullptr;
			
			decltype(GetSystemMetricsForDpi)* pGetSystemMetricsForDpi = nullptr;
			
			decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi = nullptr;

			void load();

			void unload();
		
		} m_user32;

		struct WindowClass
		{
			std::wstring name;

			void registerClass(HINSTANCE hInstance);

			void unregisterClass(HINSTANCE hInstance);

		} m_windowClass;

		String m_title{ Window::DefaultTitle };

		String m_actualTitle{ SIV3D_BUILD(DEBUG) ? U"Siv3D App (Debug Build)"_sv : Window::DefaultTitle };
	};
}
