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

# include "WindowProc.hpp"
# include <Siv3D/UserAction.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/UserAction/IUSerAction.hpp>
# include "CWindow.hpp"
# include <windowsx.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static CWindow* GetWindow()
		{
			return static_cast<CWindow*>(SIV3D_ENGINE(Window));
		}
	}

	LRESULT CALLBACK WindowProc(const HWND hWnd, const UINT message, const WPARAM wParam, LPARAM lParam)
	{
		// エンジンコンポーネントにアクセスできるか
		const bool engineAvailable = Siv3DEngine::isAvailable();

		//if (engineAvailable)
		//{
		//	if (auto textinput = static_cast<CTextInput*>(SIV3D_ENGINE(TextInput)))
		//	{
		//		if (textinput->process(message, wParam, &lParam))
		//		{
		//			return 0;
		//		}
		//	}

		//	if (auto dragDrop = static_cast<CDragDrop*>(SIV3D_ENGINE(DragDrop)))
		//	{
		//		dragDrop->process();
		//	}
		//}

		switch (message)
		{
		case WM_DESTROY:
			{
				LOG_DEBUG("WM_DESTROY");

				::PostQuitMessage(0);

				return 0;
			}
		case WM_MOVE:
			{
				LOG_DEBUG("WM_MOVE");

				if (engineAvailable)
				{
					GetWindow()->onBoundsUpdate();
				}

				return 0;
			}
		case WM_SIZE:
			{
				LOG_DEBUG("WM_SIZE");

				if (engineAvailable)
				{
					auto pWindow = GetWindow();
					pWindow->onBoundsUpdate();

					const bool minimized = (wParam == SIZE_MINIMIZED);
					const bool maximized = ((wParam == SIZE_MAXIMIZED) || (pWindow->getState().maximized && (wParam != SIZE_RESTORED)));		
					pWindow->onResize(minimized, maximized);

					const Size frameBufferSize{ LOWORD(lParam), HIWORD(lParam) };
					pWindow->onFrameBufferResize(frameBufferSize);
				}

				return 0;
			}
		case WM_SETFOCUS:
			{
				LOG_DEBUG("WM_SETFOCUS");

				if (engineAvailable)
				{
					GetWindow()->onFocus(true);
				}

				break;
			}
		case WM_KILLFOCUS:
			{
				LOG_DEBUG("WM_KILLFOCUS");

				if (engineAvailable)
				{
					GetWindow()->onFocus(false);
				}

				break;
			}
		case WM_CLOSE:
			{
				LOG_DEBUG("WM_CLOSE");

				if (engineAvailable)
				{
					SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::CloseButtonClicked);
				}

				return 0; // WM_DESTROY を発生させない
			}
		case WM_ERASEBKGND:
			{
				return true;
			}
		case WM_SETCURSOR:
		{
			if (engineAvailable)
			{
				if (const uint32 hitTest = (lParam & 0xFFFF);
					(hitTest == HTCLIENT))
				{
					SIV3D_ENGINE(CursorStyle)->onSetCursor();

					return 1;
				}
			}

			break;
		}
		case WM_GETMINMAXINFO:
			{
				if (engineAvailable)
				{
					const Size minSize = GetWindow()->getMinTrackSize();
					
					LPMINMAXINFO pMinMaxInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
					pMinMaxInfo->ptMinTrackSize.x = minSize.x;
					pMinMaxInfo->ptMinTrackSize.y = minSize.y;
				}

				break;
			}
		case WM_SYSKEYDOWN:
			{
				if (engineAvailable)
				{
					if ((wParam == VK_RETURN) && (lParam & (1 << 29))) // Alt + Enter
					{
						//static_cast<CWindow*>(SIV3D_ENGINE(Window))->requestToggleFullscreen();
						return 0;
					}
				}

				break;
			}
		case WM_SYSKEYUP:
			{
				break;
			}
		//case WM_KEYDOWN:
		//	{
		//		LOG_VERBOSE(U"WM_KEYDOWN");

				//if (engineAvailable)
				//{
				//	const bool repeatFlag = ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT);

				//	if (not repeatFlag)
				//	{
				//		static_cast<CKeyboard*>(SIV3D_ENGINE(Keyboard))->onKeyEvent(static_cast<uint8>(wParam), true, false);
				//	}
				//}

		//		break;
		//	}
		//case WM_KEYUP:
		//	{
				//if (engineAvailable)
				//{
				//	static_cast<CKeyboard*>(SIV3D_ENGINE(Keyboard))->onKeyEvent(static_cast<uint8>(wParam), false, true);
				//}

		//		break;
		//	}
		case WM_SYSCOMMAND:
			{
				LOG_DEBUG("WM_SYSCOMMAND");

				switch (wParam & 0xffF0)
				{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				case SC_KEYMENU:
					return 0;
				}

				break;
			}
		case WM_DISPLAYCHANGE:
			{
				LOG_DEBUG("WM_DISPLAYCHANGE");

				return true;
			}
		//case WM_CHAR:
		//	{
		//		LOG_DEBUG("WM_CHAR");
	
				//if (engineAvailable)
				//{
				//	if (auto p = SIV3D_ENGINE(TextInput))
				//	{
				//		p->pushChar(static_cast<uint32>(wParam));
				//	}
				//}

		//		return 0;
		//	}
		//case WM_UNICHAR:
		//	{
		//		LOG_DEBUG("WM_UNICHAR");

		//		if (wParam == UNICODE_NOCHAR)
		//		{
		//			return true;
		//		}

				//if (engineAvailable)
				//{
				//	if (auto p = SIV3D_ENGINE(TextInput))
				//	{
				//		p->pushChar(static_cast<uint32>(wParam));
				//	}
				//}
			
		//		return 0;
		//	}
		//case WM_DEVICECHANGE:
		//	{
		//		LOG_VERBOSE(U"WM_DEVICECHANGE {:#X}"_fmt(wParam));

				//if (engineAvailable)
				//{
				//	if (wParam == DBT_DEVICEARRIVAL)
				//	{
				//		LOG_DEBUG("WM_DEVICECHANGE (DBT_DEVICEARRIVAL)");

				//		if (CSystem* system = static_cast<CSystem*>(SIV3D_ENGINE(System)))
				//		{
				//			system->onDeviceChange();
				//		}

				//		const DEV_BROADCAST_HDR* dbh = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);

				//		if (dbh && (dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE))
				//		{
				//			static_cast<CGamepad*>(SIV3D_ENGINE(Gamepad))->detectJoystickConnection();
				//		}
				//	}
				//	else if (wParam == DBT_DEVICEREMOVECOMPLETE)
				//	{
				//		LOG_DEBUG("WM_DEVICECHANGE (DBT_DEVICEREMOVECOMPLETE)");

				//		if (CSystem* system = static_cast<CSystem*>(SIV3D_ENGINE(System)))
				//		{
				//			system->onDeviceChange();
				//		}

				//		const DEV_BROADCAST_HDR* dbh = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);

				//		if (dbh && (dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE))
				//		{
				//			static_cast<CGamepad*>(SIV3D_ENGINE(Gamepad))->detectJoystickDisconnection();
				//		}
				//	}
				//}

		//		break;
		//	}
		case WM_MOUSEMOVE:
			{
				if (engineAvailable)
				{
					const Point rawClientPos{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
					SIV3D_ENGINE(Cursor)->updateHighTemporalResolutionCursorPos(rawClientPos);
				}
				
				break;
			}
		case WM_ENTERSIZEMOVE:
			{
				LOG_DEBUG("WM_ENTERSIZEMOVE");
		
				if (engineAvailable)
				{
					GetWindow()->onEnterSizeMove();
				}

				break;
			}
		case WM_EXITSIZEMOVE:
			{
				LOG_DEBUG("WM_EXITSIZEMOVE");

				if (engineAvailable)
				{
					GetWindow()->onExitSizeMove();
				}

				break;
			}
		case WM_DPICHANGED:
			{
				LOG_DEBUG("WM_DPICHANGED");
				
				if (engineAvailable)
				{
					const uint32 newDPI = HIWORD(wParam);
					const RECT* rect = reinterpret_cast<const RECT*>(lParam);
					GetWindow()->onDPIChange(newDPI, Point{ rect->left, rect->top });
				}

				return true;
			}
		//case WM_MOUSEWHEEL:
		//	{
				//
				//if (engineAvailable)
				//{
				//	SIV3D_ENGINE(Mouse)->onScroll(0, static_cast<short>(HIWORD(wParam)) / -double(WHEEL_DELTA));
				//}
		
				//		return 0;
		//	}
		//case WM_MOUSEHWHEEL:
		//	{
				//if (engineAvailable)
				//{
				//	SIV3D_ENGINE(Mouse)->onScroll(static_cast<short>(HIWORD(wParam)) / double(WHEEL_DELTA), 0);
				//}

		//		return 0;
		//	}
		//case WM_TOUCH:
		//	{
		//		if (engineAvailable)
		//		{
			//		if (const size_t num_inputs = LOWORD(wParam))
			//		{
			//			Array<TOUCHINPUT> touchInputs(num_inputs);

			//			if (::GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam),
			//				static_cast<uint32>(touchInputs.size()), touchInputs.data(),
			//				sizeof(TOUCHINPUT)))
			//			{
			//				if (auto pMouse = static_cast<CMouse*>(SIV3D_ENGINE(Mouse)))
			//				{
			//					pMouse->onTouchInput(touchInputs);
			//				}

			//				::CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));

			//				return 0;
			//			}
			//		}
			//	}

		//		break;
		//	}
		}

		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	}
}
