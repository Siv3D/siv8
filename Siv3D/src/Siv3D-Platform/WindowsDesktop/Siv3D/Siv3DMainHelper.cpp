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

# include "Siv3DMainHelper.hpp"
# include <Siv3D/Error.hpp>
# include <Siv3D/UserInfo.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Window.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/FreestandingMessageBox/FreestandingMessageBox.hpp>

// ユーザが実装するエントリーポイント | User-defined entry point
void Main();

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		constexpr static std::string_view ExceptionToString(const DWORD code) noexcept
		{
			switch (code)
			{
			case EXCEPTION_ACCESS_VIOLATION:
				return "EXCEPTION_ACCESS_VIOLATION";
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
			case EXCEPTION_BREAKPOINT:
				return "EXCEPTION_BREAKPOINT";
			case EXCEPTION_DATATYPE_MISALIGNMENT:
				return "EXCEPTION_DATATYPE_MISALIGNMENT";
			case EXCEPTION_FLT_DENORMAL_OPERAND:
				return "EXCEPTION_FLT_DENORMAL_OPERAND";
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
			case EXCEPTION_FLT_INEXACT_RESULT:
				return "EXCEPTION_FLT_INEXACT_RESULT";
			case EXCEPTION_FLT_INVALID_OPERATION:
				return "EXCEPTION_FLT_INVALID_OPERATION";
			case EXCEPTION_FLT_OVERFLOW:
				return "EXCEPTION_FLT_OVERFLOW";
			case EXCEPTION_FLT_STACK_CHECK:
				return "EXCEPTION_FLT_STACK_CHECK";
			case EXCEPTION_FLT_UNDERFLOW:
				return "EXCEPTION_FLT_UNDERFLOW";
			case EXCEPTION_ILLEGAL_INSTRUCTION:
				return "EXCEPTION_ILLEGAL_INSTRUCTION";
			case EXCEPTION_IN_PAGE_ERROR:
				return "EXCEPTION_IN_PAGE_ERROR";
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				return "EXCEPTION_INT_DIVIDE_BY_ZERO";
			case EXCEPTION_INT_OVERFLOW:
				return "EXCEPTION_INT_OVERFLOW";
			case EXCEPTION_INVALID_DISPOSITION:
				return "EXCEPTION_INVALID_DISPOSITION";
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
			case EXCEPTION_PRIV_INSTRUCTION:
				return "EXCEPTION_PRIV_INSTRUCTION";
			case EXCEPTION_SINGLE_STEP:
				return "EXCEPTION_SINGLE_STEP";
			case EXCEPTION_STACK_OVERFLOW:
				return "EXCEPTION_STACK_OVERFLOW";
			default:
				return "Unknown Exception";
			}
		}

		static int ShowException(const DWORD code, struct _EXCEPTION_POINTERS*)
		{
			const std::string message{ ExceptionToString(code) };

			LOG_ERROR(fmt::format("🛑 Application terminated due to an exception `{}`", message));

			Window::Minimize();

			FreestandingMessageBox::ShowError(fmt::format("Application terminated due to an exception `{}`", message));

			return EXCEPTION_EXECUTE_HANDLER;
		}

		static std::string MakeErrorMessage(const std::string_view error, const std::string_view what)
		{
			std::string errorMessage;

			const bool isJapanese = System::GetUserInfo().defaultLanguage.lowercased().contains(U"ja-jp");

			if (isJapanese) // 日本語
			{
				errorMessage = "プログラムでエラーが発生しました。\n内容: ";
			}
			else
			{
				errorMessage = "An error occurred in the program.\n";
			}

			errorMessage += fmt::format("[{}] {}\n\n", error, what);
	
			if (isJapanese)
			{
				errorMessage += fmt::format("（開発者向け情報）例外の発生場所を特定するには、Visual Studio メニュー［デバッグ］→［ウィンドウ］→［例外設定］→［スローされたときに中断］→［C++ Exceptions］において、`{}` を追加または有効にしてください。", error);
			}
			else
			{
				errorMessage += fmt::format("(For developers) To identify the location where the exception occurred, add or enable `{}` in [Debug] -> [Windows] -> [Exception Settings] -> [Break When Thrown] -> [C++ Exceptions].", error);
			}

			return errorMessage;
		}

		static void TryMain()
		{
			std::string errorMessage;

			try
			{
				LOG_SCOPED_DEBUG("Main()");
				Main();
			}
			catch (const Error& error)
			{
				errorMessage = MakeErrorMessage("s3d::Error", error.messageUTF8());
			}
			catch (const fmt::format_error& error)
			{
				errorMessage = MakeErrorMessage("fmt::format_error", error.what());
			}
			catch (const std::runtime_error& error)
			{
				errorMessage = MakeErrorMessage("std::runtime_error", error.what());
			}
			catch (const std::out_of_range& error)
			{
				errorMessage = MakeErrorMessage("std::out_of_range", error.what());
			}
			catch (const std::exception& error)
			{
				errorMessage = MakeErrorMessage("std::exception", error.what());
			}

			if (not errorMessage.empty())
			{
				//static_cast<void>(Window::SetFullscreen(false)); // メッセージボックスを表示するためにフルスクリーンモードを解除

				//errorMessage += "For more information, [Debug] -> [Windows] -> [Exception Settings] -> Tick the C++ Exceptions checkbox under the [Break When Thrown] heading.";

				FreestandingMessageBox::ShowError(errorMessage);
			}
		}
	}

	void SetWorkingDirectory()
	{
		if (System::IsRunningInVisualStudio())
		{
			return;
		}

		if (const FilePath workingDirectory = FileSystem::ParentPath(FileSystem::ModulePath()))
		{
			FileSystem::ChangeCurrentDirectory(workingDirectory);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	WinMainInit
	//
	////////////////////////////////////////////////////////////////

	void WinMainInit()
	{
		// タイマーの解像度を上げることで Sleep() の精度を上げる
		::timeBeginPeriod(1);

		if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		{
			::OutputDebugStringW(L"CoInitializeEx() failed\n");
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	WinMainExit
	//
	////////////////////////////////////////////////////////////////

	void WinMainExit()
	{
		::CoUninitialize();

		// タイマーの解像度を戻す
		::timeEndPeriod(1);
	}

	////////////////////////////////////////////////////////////////
	//
	//	MainSEH
	//
	////////////////////////////////////////////////////////////////

	void MainSEH()
	{
		__try
		{
			TryMain();
		}
		__except (ShowException(GetExceptionCode(), GetExceptionInformation())) {}
	}

	////////////////////////////////////////////////////////////////
	//
	//	PumpMessages
	//
	////////////////////////////////////////////////////////////////

	void PumpMessages()
	{
		for (int32 i = 0; i < 100; ++i)
		{
			MSG message{};

			if (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessageW(&message);
			}
			else
			{
				return;
			}
		}
	}
}
