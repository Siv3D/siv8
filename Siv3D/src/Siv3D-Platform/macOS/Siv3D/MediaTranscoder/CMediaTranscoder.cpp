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
# include "Transcoder/AACEncoder_CA.hpp"
# include <Siv3D/Wave.hpp>
# include <Siv3D/EngineLog.hpp>

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
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CMediaTranscoder::init()
	{
		LOG_SCOPED_DEBUG("CMediaTranscoder::init()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeAAC
	//
	////////////////////////////////////////////////////////////////

	bool CMediaTranscoder::encodeAAC(const Wave& wave, IWriter& writer, const AACQuality quality) const
	{
		return AACEncoder_CA::EncodeWaveToAAC(wave.data(), wave.size(), wave.sampleRate(), writer, quality);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAvailableAACSampleRates
	//
	////////////////////////////////////////////////////////////////

	Array<uint32> CMediaTranscoder::getAvailableAACSampleRates(const uint32 channels) const
	{
		return AACEncoder_CA::GetAvailableSampleRates(channels);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeMP3
	//
	////////////////////////////////////////////////////////////////

	bool CMediaTranscoder::encodeMP3(const Wave& wave, IWriter& writer, const MP3Quality quality) const
	{
		return(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAvailableMP3SampleRates
	//
	////////////////////////////////////////////////////////////////

	Array<uint32> CMediaTranscoder::getAvailableMP3SampleRates(const uint32 channels) const
	{
		return{};
	}
}
