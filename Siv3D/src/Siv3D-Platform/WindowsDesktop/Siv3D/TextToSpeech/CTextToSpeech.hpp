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
# include <Siv3D/Windows/MinWindows.hpp>
# include <winrt/base.h>
# include <winrt/Windows.Foundation.h>
# include <winrt/Windows.Foundation.Collections.h>
# include <winrt/Windows.Media.SpeechSynthesis.h>
# include <winrt/Windows.Storage.Streams.h>

namespace s3d
{
	class CTextToSpeech final : public ISiv3DTextToSpeech
	{
	public:

		~CTextToSpeech() override;

		void init() override;

		bool synthesizeToWave(StringView text, Wave& wave) override;

	private:

		enum class State
		{
			Uninitialized, // init() 前
			
			Idle,          // init() 済み、owner thread は確定済み、ただし SpeechSynthesizer 未生成
			
			Ready,         // setup() 成功、m_synth 生成済み
			
			Unavailable,   // setup() 失敗、今後使用不可
		
		} m_state = State::Uninitialized;

		// init() を呼び出したスレッド ID（このスレッドのみ使う）
		std::thread::id m_ownerThreadID{};

		winrt::Windows::Media::SpeechSynthesis::SpeechSynthesizer m_synth{ nullptr };

		void setup();

		[[nodiscard]]
		bool isOwnerThread() const noexcept;

		bool synthesizeToWaveImpl(StringView text, Wave& wave);

		bool loadWaveFromSpeechStream(const winrt::Windows::Media::SpeechSynthesis::SpeechSynthesisStream& stream, Wave& wave);
	};
}
