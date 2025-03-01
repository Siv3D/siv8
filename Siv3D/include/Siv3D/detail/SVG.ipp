//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
	SVG::SVG(Reader&& reader)
		: SVG{}
	{
		load(std::make_unique<Reader>(std::move(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	bool SVG::load(Reader&& reader)
	{
		return load(std::make_unique<Reader>(std::forward<Reader>(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	SVG SVG::Load(Reader&& reader)
	{
		SVG svg;
		svg.load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		return svg;
	}
}
