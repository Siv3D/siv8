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
# include <Siv3D/FreestandingMessageBox/FreestandingMessageBox.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace detail
	{
		static void ShowFreestandingMessageBox(const std::wstring& text, const uint32 type)
		{
			::MessageBoxW(nullptr, text.c_str(), L"Engine notification", type);
		}
	}

	namespace FreestandingMessageBox
	{
		void ShowInfo(const std::string_view text)
		{
			detail::ShowFreestandingMessageBox(Unicode::ToWstring(text), (MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL));
		}

		void ShowError(const std::string_view text)
		{
			detail::ShowFreestandingMessageBox(Unicode::ToWstring(text), (MB_OK | MB_ICONERROR | MB_SYSTEMMODAL));
		}
	}
}
