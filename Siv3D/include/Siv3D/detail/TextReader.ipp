//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	TextReader::TextReader(Reader&& reader, const Optional<TextEncoding>& encoding)
		: TextReader{}
	{
		open(std::forward<Reader>(reader), encoding);
	}

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	bool TextReader::open(Reader&& reader, const Optional<TextEncoding>& encoding)
	{
		return open(std::make_unique<Reader>(std::forward<Reader>(reader)), encoding);
	}
}
