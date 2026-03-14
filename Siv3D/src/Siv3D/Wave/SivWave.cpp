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
# include <Siv3D/AudioEncoder.hpp>
# include <Siv3D/AudioFormat/WAVEncoder.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Wave::Wave(const FilePathView path, const AudioFormat format)
	{
		*this = AudioDecoder::Decode(path, format);
	}

	Wave::Wave(std::unique_ptr<IReader> reader, const AudioFormat format)
	{
		*this = AudioDecoder::Decode(std::move(reader), format);
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool Wave::save(const FilePathView path, const AudioFormat format) const
	{
		return AudioEncoder::Save(*this, format, path);
	}

	////////////////////////////////////////////////////////////////
	//
	//	saveWAV
	//
	////////////////////////////////////////////////////////////////

	bool Wave::saveWAV(const FilePathView path, const WAVSaveFormat format) const
	{
		return WAVEncoder{}.save(*this, path, format);
	}



	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	Blob Wave::encode(const AudioFormat format) const
	{
		return AudioEncoder::Encode(*this, format);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeWAV
	//
	////////////////////////////////////////////////////////////////

	Blob Wave::encodeWAV(const WAVSaveFormat format) const
	{
		return WAVEncoder{}.encode(*this, format);
	}

}
