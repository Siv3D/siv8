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

# include <Siv3D/TextToSpeech.hpp>
# include <Siv3D/TextToSpeech/ITextToSpeech.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace TextToSpeech
	{
		void Speak(const StringView text)
		{
			//SIV3D_ENGINE(TextToSpeech)->speak(text);
		}

		Wave Synthesize(const StringView text)
		{
			Wave wave;
			SIV3D_ENGINE(TextToSpeech)->synthesizeToWave(text, wave);
			return wave;
		}
	}
}
