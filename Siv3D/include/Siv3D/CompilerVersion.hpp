//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Platform.hpp"

# if (SIV3D_COMPILER(MSVC) && (_MSC_VER < 1936))

	// Visual Studio の更新が必要
	# error "Building Siv3D requires a newer version of Visual Studio."

# endif
