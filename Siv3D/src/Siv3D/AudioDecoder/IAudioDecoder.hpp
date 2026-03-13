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
# include <Siv3D/IAudioDecoder.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DAudioDecoder
	{
	public:

		[[nodiscard]]
		static ISiv3DAudioDecoder* Create();

		virtual ~ISiv3DAudioDecoder() = default;

		virtual void init() = 0;

		virtual bool add(std::unique_ptr<IAudioDecoder> decoder) = 0;

		virtual void remove(StringView decoderName) = 0;

		virtual const Array<std::unique_ptr<IAudioDecoder>>& enumDecoder() const noexcept = 0;

		virtual AudioFormat getAudioFormat(const IReader& reader, FilePathView pathHint, StringView decoderName) = 0;

		virtual Wave decode(std::unique_ptr<IReader> reader, FilePathView pathHint, StringView decoderName) = 0;
	};
}
