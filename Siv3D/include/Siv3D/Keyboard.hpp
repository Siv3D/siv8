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

# pragma once
# include "Common.hpp"
# include "Array.hpp"
# include "InputGroups.hpp"
//# include "KeyEvent.hpp"

namespace s3d
{
	namespace Keyboard
	{
		/// @brief 現在のフレームで何らかの入力操作があるキーの一覧を取得します。
		/// @return 現在のフレームで何らかの入力操作があるキーの一覧
		[[nodiscard]]
		const Array<Input>& GetAllInputs() noexcept;

		//[[nodiscard]]
		//Array<KeyEvent> GetEvents();
	}
	
	/// @brief Ctrl + break キー
	inline constexpr Input KeyCancel{ InputDevice::Keyboard, 0x03 };
	
	/// @brief Backspace キー	
	inline constexpr Input KeyBackspace{ InputDevice::Keyboard, 0x08 };
	
	/// @brief Tab キー	
	inline constexpr Input KeyTab{ InputDevice::Keyboard, 0x09 };
	
	/// @brief Clear(NumLock を外した状態のテンキー5) キー	
	inline constexpr Input KeyClear{ InputDevice::Keyboard, 0x0C };
	
	/// @brief Enter キー
	inline constexpr Input KeyEnter{ InputDevice::Keyboard, 0x0D };

	/// @brief Shift キー	
	inline constexpr Input KeyShift{ InputDevice::Keyboard, 0x10 };
	
	/// @brief Ctrl キー	
	inline constexpr Input KeyControl{ InputDevice::Keyboard, 0x11 };
	
	/// @brief Alt キー	
	inline constexpr Input KeyAlt{ InputDevice::Keyboard, 0x12 };
	
	/// @brief Pause キー	
	inline constexpr Input KeyPause{ InputDevice::Keyboard, 0x13 };

	/// @brief Escape キー
	inline constexpr Input KeyEscape{ InputDevice::Keyboard, 0x1B };

	/// @brief スペースキー
	inline constexpr Input KeySpace{ InputDevice::Keyboard, 0x20 };

	/// @brief PageUp キー
	inline constexpr Input KeyPageUp{ InputDevice::Keyboard, 0x21 };
	
	/// @brief PageDown キー	
	inline constexpr Input KeyPageDown{ InputDevice::Keyboard, 0x22 };
	
	/// @brief End キー
	inline constexpr Input KeyEnd{ InputDevice::Keyboard, 0x23 };
	
	/// @brief Home キー
	inline constexpr Input KeyHome{ InputDevice::Keyboard, 0x24 };

	/// @brief Left (←) キー
	inline constexpr Input KeyLeft{ InputDevice::Keyboard, 0x25 };

	/// @brief Up (↑) キー
	inline constexpr Input KeyUp{ InputDevice::Keyboard, 0x26 };

	/// @brief Right (→) キー
	inline constexpr Input KeyRight{ InputDevice::Keyboard, 0x27 };
	
	/// @brief Down (↓) キー
	inline constexpr Input KeyDown{ InputDevice::Keyboard, 0x28 };

	/// @brief PrintScreen キー	
	inline constexpr Input KeyPrintScreen{ InputDevice::Keyboard, 0x2C };

	/// @brief Insert キー
	inline constexpr Input KeyInsert{ InputDevice::Keyboard, 0x2D };

	/// @brief Delete キー
	inline constexpr Input KeyDelete{ InputDevice::Keyboard, 0x2E };

	/// @brief 0 キー
	inline constexpr Input Key0{ InputDevice::Keyboard, 0x30 };

	/// @brief 1 キー
	inline constexpr Input Key1{ InputDevice::Keyboard, 0x31 };

	/// @brief 2 キー
	inline constexpr Input Key2{ InputDevice::Keyboard, 0x32 };

	/// @brief 3 キー
	inline constexpr Input Key3{ InputDevice::Keyboard, 0x33 };

	/// @brief 4 キー
	inline constexpr Input Key4{ InputDevice::Keyboard, 0x34 };

	/// @brief 5 キー
	inline constexpr Input Key5{ InputDevice::Keyboard, 0x35 };

	/// @brief 6 キー
	inline constexpr Input Key6{ InputDevice::Keyboard, 0x36 };

	/// @brief 7 キー
	inline constexpr Input Key7{ InputDevice::Keyboard, 0x37 };

	/// @brief 8 キー
	inline constexpr Input Key8{ InputDevice::Keyboard, 0x38 };

	/// @brief 9 キー
	inline constexpr Input Key9{ InputDevice::Keyboard, 0x39 };

	/// @brief A キー
	inline constexpr Input KeyA{ InputDevice::Keyboard, 0x41 };

	/// @brief B キー
	inline constexpr Input KeyB{ InputDevice::Keyboard, 0x42 };

	/// @brief C キー
	inline constexpr Input KeyC{ InputDevice::Keyboard, 0x43 };

	/// @brief D キー
	inline constexpr Input KeyD{ InputDevice::Keyboard, 0x44 };

	/// @brief E キー
	inline constexpr Input KeyE{ InputDevice::Keyboard, 0x45 };

	/// @brief F キー
	inline constexpr Input KeyF{ InputDevice::Keyboard, 0x46 };

	/// @brief G キー
	inline constexpr Input KeyG{ InputDevice::Keyboard, 0x47 };

	/// @brief H キー
	inline constexpr Input KeyH{ InputDevice::Keyboard, 0x48 };

	/// @brief I キー
	inline constexpr Input KeyI{ InputDevice::Keyboard, 0x49 };

	/// @brief J キー
	inline constexpr Input KeyJ{ InputDevice::Keyboard, 0x4A };

	/// @brief K キー
	inline constexpr Input KeyK{ InputDevice::Keyboard, 0x4B };

	/// @brief L キー
	inline constexpr Input KeyL{ InputDevice::Keyboard, 0x4C };

	/// @brief M キー
	inline constexpr Input KeyM{ InputDevice::Keyboard, 0x4D };

	/// @brief N キー
	inline constexpr Input KeyN{ InputDevice::Keyboard, 0x4E };

	/// @brief O キー
	inline constexpr Input KeyO{ InputDevice::Keyboard, 0x4F };

	/// @brief P キー
	inline constexpr Input KeyP{ InputDevice::Keyboard, 0x50 };

	/// @brief Q キー
	inline constexpr Input KeyQ{ InputDevice::Keyboard, 0x51 };

	/// @brief R キー
	inline constexpr Input KeyR{ InputDevice::Keyboard, 0x52 };

	/// @brief S キー
	inline constexpr Input KeyS{ InputDevice::Keyboard, 0x53 };

	/// @brief T キー
	inline constexpr Input KeyT{ InputDevice::Keyboard, 0x54 };

	/// @brief U キー
	inline constexpr Input KeyU{ InputDevice::Keyboard, 0x55 };

	/// @brief V キー
	inline constexpr Input KeyV{ InputDevice::Keyboard, 0x56 };

	/// @brief W キー
	inline constexpr Input KeyW{ InputDevice::Keyboard, 0x57 };

	/// @brief X キー
	inline constexpr Input KeyX{ InputDevice::Keyboard, 0x58 };

	/// @brief Y キー
	inline constexpr Input KeyY{ InputDevice::Keyboard, 0x59 };

	/// @brief Z キー
	inline constexpr Input KeyZ{ InputDevice::Keyboard, 0x5A };

	/// @brief Numpad 0 キー
	inline constexpr Input KeyNum0{ InputDevice::Keyboard, 0x60 };

	/// @brief Numpad 1 キー
	inline constexpr Input KeyNum1{ InputDevice::Keyboard, 0x61 };

	/// @brief Numpad 2 キー
	inline constexpr Input KeyNum2{ InputDevice::Keyboard, 0x62 };

	/// @brief Numpad 3 キー
	inline constexpr Input KeyNum3{ InputDevice::Keyboard, 0x63 };

	/// @brief Numpad 4 キー
	inline constexpr Input KeyNum4{ InputDevice::Keyboard, 0x64 };

	/// @brief Numpad 5 キー
	inline constexpr Input KeyNum5{ InputDevice::Keyboard, 0x65 };

	/// @brief Numpad 6 キー
	inline constexpr Input KeyNum6{ InputDevice::Keyboard, 0x66 };

	/// @brief Numpad 7 キー
	inline constexpr Input KeyNum7{ InputDevice::Keyboard, 0x67 };

	/// @brief Numpad 8 キー
	inline constexpr Input KeyNum8{ InputDevice::Keyboard, 0x68 };

	/// @brief Numpad 9 キー
	inline constexpr Input KeyNum9{ InputDevice::Keyboard, 0x69 };

	/// @brief Numpad * キー
	inline constexpr Input KeyNumMultiply{ InputDevice::Keyboard, 0x6A };

	/// @brief Numpad + キー
	inline constexpr Input KeyNumAdd{ InputDevice::Keyboard, 0x6B };

	/// @brief Numpad Enter キー
	inline constexpr Input KeyNumEnter{ InputDevice::Keyboard, 0x6C };

	/// @brief Numpad - キー
	inline constexpr Input KeyNumSubtract{ InputDevice::Keyboard, 0x6D };

	/// @brief Numpad . キー
	inline constexpr Input KeyNumDecimal{ InputDevice::Keyboard, 0x6E };

	/// @brief Numpad / キー
	inline constexpr Input KeyNumDivide{ InputDevice::Keyboard, 0x6F };

	/// @brief F1 キー
	inline constexpr Input KeyF1{ InputDevice::Keyboard, 0x70 };

	/// @brief F2 キー
	inline constexpr Input KeyF2{ InputDevice::Keyboard, 0x71 };

	/// @brief F3 キー
	inline constexpr Input KeyF3{ InputDevice::Keyboard, 0x72 };

	/// @brief F4 キー
	inline constexpr Input KeyF4{ InputDevice::Keyboard, 0x73 };

	/// @brief F5 キー
	inline constexpr Input KeyF5{ InputDevice::Keyboard, 0x74 };

	/// @brief F6 キー
	inline constexpr Input KeyF6{ InputDevice::Keyboard, 0x75 };

	/// @brief F7 キー
	inline constexpr Input KeyF7{ InputDevice::Keyboard, 0x76 };

	/// @brief F8 キー
	inline constexpr Input KeyF8{ InputDevice::Keyboard, 0x77 };

	/// @brief F9 キー
	inline constexpr Input KeyF9{ InputDevice::Keyboard, 0x78 };

	/// @brief F10 キー
	inline constexpr Input KeyF10{ InputDevice::Keyboard, 0x79 };

	/// @brief F11 キー
	inline constexpr Input KeyF11{ InputDevice::Keyboard, 0x7A };

	/// @brief F12 キー
	inline constexpr Input KeyF12{ InputDevice::Keyboard, 0x7B };

	/// @brief F13 キー
	inline constexpr Input KeyF13{ InputDevice::Keyboard, 0x7C };

	/// @brief F14 キー
	inline constexpr Input KeyF14{ InputDevice::Keyboard, 0x7D };

	/// @brief F15 キー
	inline constexpr Input KeyF15{ InputDevice::Keyboard, 0x7E };

	/// @brief F16 キー
	inline constexpr Input KeyF16{ InputDevice::Keyboard, 0x7F };

	/// @brief F17 キー
	inline constexpr Input KeyF17{ InputDevice::Keyboard, 0x80 };

	/// @brief F18 キー
	inline constexpr Input KeyF18{ InputDevice::Keyboard, 0x81 };

	/// @brief F19 キー
	inline constexpr Input KeyF19{ InputDevice::Keyboard, 0x82 };

	/// @brief F20 キー
	inline constexpr Input KeyF20{ InputDevice::Keyboard, 0x83 };

	/// @brief F21 キー
	inline constexpr Input KeyF21{ InputDevice::Keyboard, 0x84 };

	/// @brief F22 キー
	inline constexpr Input KeyF22{ InputDevice::Keyboard, 0x85 };

	/// @brief F23 キー
	inline constexpr Input KeyF23{ InputDevice::Keyboard, 0x86 };

	/// @brief F24 キー
	inline constexpr Input KeyF24{ InputDevice::Keyboard, 0x87 };

	/// @brief NumLock キー
	inline constexpr Input KeyNumLock{ InputDevice::Keyboard, 0x90 };

	/// @brief 左 Shift キー
	inline constexpr Input KeyLShift{ InputDevice::Keyboard, 0xA0 };

	/// @brief 右 Shift キー
	inline constexpr Input KeyRShift{ InputDevice::Keyboard, 0xA1 };

	/// @brief 左 Ctrl キー
	inline constexpr Input KeyLControl{ InputDevice::Keyboard, 0xA2 };

	/// @brief 右 Ctrl キー
	inline constexpr Input KeyRControl{ InputDevice::Keyboard, 0xA3 };

	/// @brief 左 Alt キー
	inline constexpr Input KeyLAlt{ InputDevice::Keyboard, 0xA4 };

	/// @brief 右 Alt キー
	inline constexpr Input KeyRAlt{ InputDevice::Keyboard, 0xA5 };

	/// @brief メディア Next Track ボタン
	inline constexpr Input KeyNextTrack{ InputDevice::Keyboard, 0xB0 };

	/// @brief メディア Previous Track ボタン
	inline constexpr Input KeyPreviousTrack{ InputDevice::Keyboard, 0xB1 };

	/// @brief メディア Stop ボタン
	inline constexpr Input KeyStopMedia{ InputDevice::Keyboard, 0xB2 };

	/// @brief メディア Play / Pause ボタン
	inline constexpr Input KeyPlayPauseMedia{ InputDevice::Keyboard, 0xB3 };

	/// @brief JIS : キー
	inline constexpr Input KeyColon_JIS{ InputDevice::Keyboard, 0xBA };

	/// @brief US ; キー
	inline constexpr Input KeySemicolon_US{ InputDevice::Keyboard, 0xBA };

	/// @brief JIS ; キー
	inline constexpr Input KeySemicolon_JIS{ InputDevice::Keyboard, 0xBB };

	/// @brief US = キー
	inline constexpr Input KeyEqual_US{ InputDevice::Keyboard, 0xBB };

	/// @brief , キー
	inline constexpr Input KeyComma{ InputDevice::Keyboard, 0xBC };

	/// @brief - キー
	inline constexpr Input KeyMinus{ InputDevice::Keyboard, 0xBD };

	/// @brief . キー
	inline constexpr Input KeyPeriod{ InputDevice::Keyboard, 0xBE };

	/// @brief / キー
	inline constexpr Input KeySlash{ InputDevice::Keyboard, 0xBF };

	/// @brief ` キー
	inline constexpr Input KeyGraveAccent{ InputDevice::Keyboard, 0xC0 };

	/// @brief Command キー
	inline constexpr Input KeyCommand{ InputDevice::Keyboard, 0xD8 };

	/// @brief 左 Command キー
	inline constexpr Input KeyLeftCommand{ InputDevice::Keyboard, 0xD9 };

	/// @brief 右 Command キー
	inline constexpr Input KeyRightCommand{ InputDevice::Keyboard, 0xDA };

	/// @brief [ キー
	inline constexpr Input KeyLBracket{ InputDevice::Keyboard, 0xDB };

	/// @brief JIS ￥ キー
	inline constexpr Input KeyYen_JIS{ InputDevice::Keyboard, 0xDC };

	/// @brief US \ キー
	inline constexpr Input KeyBackslash_US{ InputDevice::Keyboard, 0xDC };

	/// @brief ] キー
	inline constexpr Input KeyRBracket{ InputDevice::Keyboard, 0xDD };

	/// @brief JIS ^ キー
	inline constexpr Input KeyCaret_JIS{ InputDevice::Keyboard, 0xDE };

	/// @brief US ' キー
	inline constexpr Input KeyApostrophe_US{ InputDevice::Keyboard, 0xDE };

	/// @brief JIS \ キー
	inline constexpr Input KeyUnderscore_JIS{ InputDevice::Keyboard, 0xE2 };
}
