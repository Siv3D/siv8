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
# include "Types.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	WindowStyle
	//
	////////////////////////////////////////////////////////////////

	/// @brief ウィンドウスタイル | Window style
	enum class WindowStyle : uint8
	{
		/// @brief サイズ変更できない枠付きのウィンドウ | Fixed size window with a frame
		Fixed,

		/// @brief サイズ変更できる枠付きのウィンドウ | Resizable window with a frame
		Sizable,

		/// @brief 枠の無いウィンドウ | Frameless window
		Frameless,
	};
}
