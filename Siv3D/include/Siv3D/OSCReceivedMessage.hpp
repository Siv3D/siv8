//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"
# include "OSCArgument.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	OSCReceivedMessage
	//
	////////////////////////////////////////////////////////////////

	/// @brief OSC 受信メッセージ
	struct OSCReceivedMessage
	{
		/// @brief アドレスパターン
		String addressPattern;

		/// @brief 引数
		Array<OSCArgument> arguments;
	};
}
