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
# include "IAudioDecoder.hpp"

namespace s3d
{
	class CAudioDecoder final : public ISiv3DAudioDecoder
	{
	public:
		
		CAudioDecoder() = default;

		~CAudioDecoder() override;

		void init() override;

		bool add(std::unique_ptr<IAudioDecoder> decoder) override;

		void remove(StringView decoderName) override;

		const Array<std::unique_ptr<IAudioDecoder>>& enumDecoder() const noexcept override;

		AudioFormat getAudioFormat(const IReader& reader, FilePathView pathHint, StringView decoderName) override;

		Wave decode(std::unique_ptr<IReader> reader, FilePathView pathHint,  StringView decoderName) override;

	private:

		Array<std::unique_ptr<IAudioDecoder>> m_decoders;
	};
}
