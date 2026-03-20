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
# include <Siv3D/Array.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <mfapi.h>
# include <mftransform.h>
# include <mferror.h>
# include <objbase.h>
# include <wmcodecdsp.h>

namespace s3d
{
	class IWriter;
	struct StereoSampleFloat32;

	class AutoComMtaScope
	{
	public:
		
		[[nodiscard]]
		AutoComMtaScope();
		
		AutoComMtaScope(const AutoComMtaScope&) = delete;

		~AutoComMtaScope();

		AutoComMtaScope& operator =(const AutoComMtaScope&) = delete;

	private:

		bool m_needUninitialize = false;
	};

	namespace MF
	{
		enum class AudioQuality
		{
			Low,
			Medium,
			High,
		};

		struct AudioEncoderCapability
		{
			uint32 sampleRate = 0;
			uint32 channels = 0;
			uint32 avgBytesPerSec = 0;
		};

		struct AudioEncoderCodecConfig
		{
			const char* name = "";
			GUID encoderClsid = {};
			GUID outputSubtype = {};
			uint32 channels = 2;
			size_t defaultInputChunkFrames = 1152 * 4;

			std::function<void(IMFMediaType*)> configureOutputType;
		};

		void ThrowIfFailed(HRESULT hr, const char* what);

		Array<AudioEncoderCapability> EnumerateEncoderCapabilities(const AudioEncoderCodecConfig& codec);

		bool EncodeWaveToCompressedAudio(
			const StereoSampleFloat32* wave, size_t frameCount, uint32 sampleRate, IWriter& writer,
			const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities,
			AudioQuality quality, size_t inputChunkFrames) noexcept;

		Array<uint32> GetAvailableSampleRates(const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, uint32 channels) noexcept;

		bool IsSampleRateSupported(const AudioEncoderCodecConfig& codec, const Array<AudioEncoderCapability>& capabilities, uint32 sampleRate, uint32 channels) noexcept;
	}
}
