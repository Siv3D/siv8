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
	INI::INI(Reader&& reader)
	{
		return load(std::make_unique<Reader>(std::move(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	bool INI::load(Reader&& reader)
	{
		return load(std::make_unique<Reader>(std::move(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	INI INI::Load(Reader&& reader)
	{
		return Load(std::make_unique<Reader>(std::move(reader)));
	}
}
