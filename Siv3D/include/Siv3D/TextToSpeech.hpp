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
# include "StringView.hpp"
# include "Wave.hpp"

namespace s3d
{
	namespace TextToSpeech
	{
		/// @brief テキストを読み上げます。
		/// @param text 読み上げるテキスト
		void Speak(StringView text);

		Wave Synthesize(StringView text);
	}
}
