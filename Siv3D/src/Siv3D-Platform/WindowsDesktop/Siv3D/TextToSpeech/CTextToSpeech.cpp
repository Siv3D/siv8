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

# include "CTextToSpeech.hpp"
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Wave.hpp>
# include <Siv3D/EngineLog.hpp>

#include <Siv3D/BinaryFileWriter.hpp>

namespace s3d
{
	CTextToSpeech::~CTextToSpeech() // 必ず owner thread で呼び出される
	{
		LOG_SCOPED_DEBUG("CTextToSpeech::~CTextToSpeech()");
	}

	void CTextToSpeech::init()
	{
		LOG_SCOPED_DEBUG("CTextToSpeech::init()");
		assert(m_state == State::Uninitialized);
		
		m_ownerThreadID = std::this_thread::get_id();	
		m_state = State::Idle;

		//using namespace winrt::Windows::Media::SpeechSynthesis;

		//const auto voices = SpeechSynthesizer::AllVoices();

		//for (const auto& voice : voices)
		//{
		//	const auto id = voice.Id();
		//	const auto name = voice.DisplayName();
		//	const auto lang = voice.Language();
		//	const auto desc = voice.Description();
		//	const auto gender = voice.Gender();

		//	LOG_DEBUG(fmt::format("Voice: id={}, name={}, lang={}, desc={}, gender={}", Unicode::FromWstring(id), Unicode::FromWstring(name), Unicode::FromWstring(lang), Unicode::FromWstring(desc), static_cast<int32>(gender)));
		//}
	}

	bool CTextToSpeech::synthesizeToWave(const StringView text, Wave& wave)
	{
		assert(m_state != State::Uninitialized);

		wave.clear();

		if (text.isEmpty())
		{
			return false;
		}

		if (not isOwnerThread())
		{
			LOG_FAIL("CTextToSpeech::synthesizeToWave() was not called from the owner thread.");
			return false;
		}

		if (m_state == State::Unavailable)
		{
			return false;
		}
		
		if (m_state == State::Idle)
		{
			setup();
		}

		if (m_state != State::Ready)
		{
			return false;
		}

		return synthesizeToWaveImpl(text, wave);
	}

	void CTextToSpeech::setup()
	{
		assert(m_state == State::Idle);
		assert(isOwnerThread());

		try
		{
			m_synth = winrt::Windows::Media::SpeechSynthesis::SpeechSynthesizer{};
			m_state = State::Ready;
		}
		catch (const winrt::hresult_error& e)
		{
			LOG_FAIL(fmt::format("Failed to initialize SpeechSynthesizer: {}", Unicode::FromWstring(e.message())));
			m_synth = nullptr;
			m_state = State::Unavailable;
		}
	}

	bool CTextToSpeech::isOwnerThread() const noexcept
	{
		return (std::this_thread::get_id() == m_ownerThreadID);
	}

	bool CTextToSpeech::synthesizeToWaveImpl(const StringView text, Wave& wave)
	{
		assert(m_state == State::Ready);
		assert(isOwnerThread());

		try
		{
			const std::wstring utf16 = Unicode::ToWstring(text);
			auto asyncOp = m_synth.SynthesizeTextToStreamAsync(utf16);
			auto stream = asyncOp.get();
			if (not stream)
			{
				return false;
			}

			return loadWaveFromSpeechStream(stream, wave);
		}
		catch (const winrt::hresult_error& e)
		{
			LOG_FAIL(fmt::format("Speech synthesis failed: {}", Unicode::FromWstring(e.message())));
			wave.clear();
			return false;
		}
	}

	bool CTextToSpeech::loadWaveFromSpeechStream(const winrt::Windows::Media::SpeechSynthesis::SpeechSynthesisStream& stream, Wave& wave)
	{
		assert(isOwnerThread());

		try
		{
			if (stream.ContentType() != L"audio/wav")
			{
				LOG_FAIL("Unexpected speech stream content type.");
				return false;
			}

			const uint64 size64 = stream.Size();
			if ((size64 == 0) || (UINT32_MAX < size64))
			{
				return false;
			}

			using namespace winrt::Windows::Storage::Streams;

			auto input = stream.GetInputStreamAt(0);
			DataReader reader{ input };

			const uint32 size = static_cast<uint32>(size64);
			reader.LoadAsync(size).get();

			Array<uint8> bytes(size);
			reader.ReadBytes(winrt::array_view<uint8>(bytes.data(), bytes.size()));

			BinaryFileWriter writer{ U"test.wav" };
			writer.write(bytes.data(), bytes.size());

			LOG_DEBUG(fmt::format("Read {} bytes from speech stream.", bytes.size()));

			return(false);// decodeWavBytesToWave(bytes, wave);
		}
		catch (const winrt::hresult_error& e)
		{
			LOG_FAIL(fmt::format("Failed to read speech stream: {}", Unicode::FromWstring(e.message())));
			wave.clear();
			return false;
		}
	}
}
