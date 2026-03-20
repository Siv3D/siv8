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

# include "CMediaTranscoder.hpp"
# include "Transcoder/AACEncoder_MF.hpp"
# include "Transcoder/MP3Encoder_MF.hpp"
# include <Siv3D/Wave.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <mfapi.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CMediaTranscoder::~CMediaTranscoder()
	{
		LOG_SCOPED_DEBUG("CMediaTranscoder::~CMediaTranscoder()");

		if (FAILED(::MFShutdown()))
		{
			LOG_FAIL("Failed to shutdown Media Foundation.");
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CMediaTranscoder::init()
	{
		LOG_SCOPED_DEBUG("CMediaTranscoder::init()");

		if (FAILED(::MFStartup(MF_VERSION, MFSTARTUP_LITE)))
		{
			throw InternalEngineError{ "Failed to initialize Media Foundation." };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeAAC
	//
	////////////////////////////////////////////////////////////////

	bool CMediaTranscoder::encodeAAC(const Wave& wave, IWriter& writer, const AACQuality quality) const
	{
		return AACEncoder_MF::EncodeWaveToAAC(wave.data(), wave.size(), wave.sampleRate(), writer, quality);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAvailableAACSampleRates
	//
	////////////////////////////////////////////////////////////////

	Array<uint32> CMediaTranscoder::getAvailableAACSampleRates(const uint32 channels) const
	{
		return AACEncoder_MF::GetAvailableSampleRates(channels);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeMP3
	//
	////////////////////////////////////////////////////////////////

	bool CMediaTranscoder::encodeMP3(const Wave& wave, IWriter& writer, const MP3Quality quality) const
	{
		return MP3Encoder_MF::EncodeWaveToMP3(wave.data(), wave.size(), wave.sampleRate(), writer, quality);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAvailableMP3SampleRates
	//
	////////////////////////////////////////////////////////////////

	Array<uint32> CMediaTranscoder::getAvailableMP3SampleRates(const uint32 channels) const
	{
		return MP3Encoder_MF::GetAvailableSampleRates(channels);
	}
}
