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

	template <ReaderObject Reader>
	bool SVG::load(Reader&& reader)
	{
		return load(std::make_unique<Reader>(std::forward<Reader>(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	SVG SVG::Load(Reader&& reader)
	{
		SVG svg;
		svg.load(std::make_unique<Reader>(std::forward<Reader>(reader)));
		return svg;
	}
}
