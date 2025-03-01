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

# include <ios>
# include "CConsole.hpp"

namespace s3d
{
	CConsole::CConsole()
	{
		std::ios_base::sync_with_stdio(false);
	}

	void CConsole::open() {}

	bool CConsole::attach()
	{	
		return true;
	}

	void CConsole::clearSelection() {}

	void CConsole::close() {}
}
