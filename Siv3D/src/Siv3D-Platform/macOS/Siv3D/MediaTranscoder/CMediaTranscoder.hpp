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
# include <Siv3D/MediaTranscoder/IMediaTranscoder.hpp>

namespace s3d
{
	class CMediaTranscoder final : public ISiv3DMediaTranscoder
	{
	public:

		~CMediaTranscoder() override;

		void init() override;

		bool encodeAAC(const Wave& wave, IWriter& writer, AACQuality quality) const override;

		Array<uint32> getAvailableAACSampleRates(uint32 channels = 2) const override;

		bool encodeMP3(const Wave& wave, IWriter& writer, MP3Quality quality) const override;

		Array<uint32> getAvailableMP3SampleRates(uint32 channels = 2) const override;

	private:

	};
}
