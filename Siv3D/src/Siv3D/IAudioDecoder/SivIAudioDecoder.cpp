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

# include <Siv3D/IAudioDecoder.hpp>
# include <Siv3D/BinaryFileReader.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Wave IAudioDecoder::decode(const FilePathView path) const
	{
		std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

		if (not reader->isOpen())
		{
			return{};
		}

		return decode(std::move(reader), path);
	}
}
