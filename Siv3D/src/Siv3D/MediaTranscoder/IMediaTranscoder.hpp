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
# include <Siv3D/Common.hpp>

namespace s3d
{
	class Wave;
	class IWriter;
	enum class AACQuality : uint8;
	enum class MP3Quality : uint8;

	class SIV3D_NOVTABLE ISiv3DMediaTranscoder
	{
	public:

		[[nodiscard]]
		static ISiv3DMediaTranscoder* Create();

		virtual ~ISiv3DMediaTranscoder() = default;

		virtual void init() = 0;

		virtual bool encodeAAC(const Wave& wave, IWriter& writer, AACQuality quality) const = 0;

		virtual Array<uint32> getAvailableAACSampleRates(uint32 channels = 2) const = 0;

		virtual bool encodeMP3(const Wave& wave, IWriter& writer, MP3Quality quality) const = 0;

		virtual Array<uint32> getAvailableMP3SampleRates(uint32 channels = 2) const = 0;
	};
}
