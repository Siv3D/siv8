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
# include "CACommon.hpp"

namespace s3d
{
	struct StereoSampleFloat32;

	class AACEncoder_CA
	{
	public:

		/// @brief ステレオ波形を ADTS 形式の AAC ファイルとして Writer に書き出します。
		static bool EncodeWaveToAAC(const StereoSampleFloat32* wave, size_t frameCount, uint32 sampleRate, IWriter& writer, AACQuality quality, size_t inputChunkFrames = Codec().defaultInputChunkFrames) noexcept;

		/// @brief 指定チャンネル数の入力でエンコード可能なサンプルレート一覧を返します。
		static Array<uint32> GetAvailableSampleRates(uint32 channels = 2) noexcept;

		/// @brief 指定したサンプルレート・チャンネル数の入力がサポートされているかを返します。
		static bool IsSampleRateSupported(uint32 sampleRate, uint32 channels = 2) noexcept;

	private:

		static CA::AudioQuality ToAudioQuality(AACQuality quality) noexcept;

		static const CA::AudioEncoderCodecConfig& Codec();

		static const Array<CA::AudioEncoderCapability>& GetAllCapabilities();
	};
}
