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
# include "Platform.hpp"

# if (SIV3D_COMPILER(MSVC) && (_MSC_VER < 1940))

	// Visual Studio の更新が必要
	# error "Please update your Visual Studio"

# endif
