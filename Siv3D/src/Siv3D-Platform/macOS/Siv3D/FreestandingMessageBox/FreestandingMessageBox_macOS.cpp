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

# include <Siv3D/MessageBox.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FreestandingMessageBox/FreestandingMessageBox.hpp>

namespace s3d
{
	namespace FreestandingMessageBox
	{
		void ShowInfo(const std::string_view text)
		{
			System::MessageBoxOK(U"Siv3D Engine", Unicode::FromUTF8(text), MessageBoxStyle::Info);
		}

		void ShowError(const std::string_view text)
		{
			System::MessageBoxOK(U"Siv3D Engine", Unicode::FromUTF8(text), MessageBoxStyle::Error);
		}
	}
}
