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

// Visual Studio 2022 17.12 以降が必要です。
// Visual Studio 2022 17.12 or later is required.

# if (SIV3D_COMPILER(MSVC) && (_MSC_VER < 1942))

	# pragma message("Siv3D は Visual Studio 2022 version 17.12 以降が必要です。Visual Studio をアップデートしてください。")
	# error "Siv3D requires Visual Studio 2022 version 17.12 or later. Please update your Visual Studio installation."

# endif
