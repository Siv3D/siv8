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

# include <Siv3D/FreestandingMessageBox/FreestandingMessageBox.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	namespace
	{
		static void ShowFreestandingMessageBox(const std::string_view text, const uint32 type)
		{
			::MessageBoxW(nullptr, Unicode::ToWstring(text).c_str(), L"Siv3D Engine", type);
		}
	}

	namespace FreestandingMessageBox
	{
		////////////////////////////////////////////////////////////////
		//
		//	ShowInfo
		//
		////////////////////////////////////////////////////////////////

		void ShowInfo(const std::string_view text)
		{
			ShowFreestandingMessageBox(text, (MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL));
		}

		////////////////////////////////////////////////////////////////
		//
		//	ShowError
		//
		////////////////////////////////////////////////////////////////

		void ShowError(const std::string_view text)
		{
			ShowFreestandingMessageBox(text, (MB_OK | MB_ICONERROR | MB_SYSTEMMODAL));
		}
	}
}
