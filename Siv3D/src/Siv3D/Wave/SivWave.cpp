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

# include <Siv3D/Wave.hpp>
# include <Siv3D/AudioDecoder.hpp>

namespace s3d
{
	Wave::Wave(const FilePathView path, const AudioFormat format)
	{
		*this = AudioDecoder::Decode(path, format);
	}

	Wave::Wave(std::unique_ptr<IReader> reader, const AudioFormat format)
	{
		*this = AudioDecoder::Decode(std::move(reader), format);
	}
}
