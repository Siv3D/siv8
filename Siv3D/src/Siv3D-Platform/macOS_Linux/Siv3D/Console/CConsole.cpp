//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <ios>
# include "CConsole.hpp"

namespace s3d
{
	CConsole::CConsole()
	{
		std::ios_base::sync_with_stdio(false);
	}

	CConsole::~CConsole() {}

	void CConsole::open() {}

	void CConsole::close() {}

	void CConsole::setSystemDefaultCodePage() {}

	void CConsole::setUTF8CodePage() {}
}
