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
# include <thread>
# include <Siv3D/TextToSpeech/ITextToSpeech.hpp>

namespace s3d
{
	class CTextToSpeech final : public ISiv3DTextToSpeech
	{
	public:

		~CTextToSpeech() override;

		void init() override;

		bool synthesizeToWave(StringView text, Wave& wave) override;

	private:

		// init() を呼び出したスレッド ID（このスレッドのみ使う）
		std::thread::id m_ownerThreadID{};
	};
}
