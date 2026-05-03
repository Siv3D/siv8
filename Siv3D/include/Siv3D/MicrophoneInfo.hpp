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
# include "String.hpp"
# include "Array.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MicrophoneInfo
	//
	////////////////////////////////////////////////////////////////

	struct MicrophoneInfo
	{
		/// @brief 名前
		String name;
		
		/// @brief ID
		String id;

		/// @brief デフォルトのマイクであるか
		bool isDefault = false;
	};

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	EnumerateMicrophones
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<MicrophoneInfo> EnumerateMicrophones();
	}
}
