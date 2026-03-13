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

namespace s3d
{
	namespace AudioDecoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	Decode
		//
		////////////////////////////////////////////////////////////////

		template <ReaderObject Reader>
		Wave Decode(Reader&& reader, const AudioFormat audioFormat)
		{
			return Decode(std::make_unique<Reader>(std::forward<Reader>(reader)), audioFormat);
		}

		template <ReaderObject Reader>
		Wave Decode(Reader&& reader, const StringView decoderName)
		{
			return Decode(std::make_unique<Reader>(std::forward<Reader>(reader)), decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		template <class AudioDecoder>
			requires std::derived_from<AudioDecoder, IAudioDecoder>
		bool Add()
		{
			return Add(std::make_unique<AudioDecoder>());
		}
	}
}
