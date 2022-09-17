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

# pragma once
# include "../Platform.hpp"

# if (SIV3D_PLATFORM(WINDOWS) && !defined(SIV3D_LIBRARY_BUILD))

	// デバッグビルドの場合、ライブラリ名にサフィックスを付ける
	# if SIV3D_BUILD(DEBUG)
	#	define SIV3D_DEBUG_LIB_SUFFIX(s) #s
	# else
	#	define SIV3D_DEBUG_LIB_SUFFIX(s)
	# endif

	# pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	# pragma comment (lib, "Siv3D" SIV3D_DEBUG_LIB_SUFFIX(_d))

	# if (SIV3D_USE_MIMALLOC)
	#	pragma comment (lib, "mimalloc/mimalloc-static" SIV3D_DEBUG_LIB_SUFFIX(_d))
	# endif

	# undef SIV3D_DEBUG_LIB_SUFFIX

# endif // (SIV3D_PLATFORM(WINDOWS) && !defined(SIV3D_LIBRARY_BUILD))
