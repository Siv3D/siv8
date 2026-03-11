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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	TextFileReader::TextFileReader(Reader&& reader, const Optional<TextEncoding>& encoding)
		: TextFileReader{}
	{
		open(std::forward<Reader>(reader), encoding);
	}

	template <ReaderObject Reader>
	bool TextFileReader::open(Reader&& reader, const Optional<TextEncoding>& encoding)
	{
		return open(std::make_unique<Reader>(std::forward<Reader>(reader)), encoding);
	}
}
