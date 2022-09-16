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

# if defined(_WIN32)

	# if defined(_DEBUG)
	#	define SIV3D_DEBUG_LIB_POSTFIX(s) #s
	# else
	#	define SIV3D_DEBUG_LIB_POSTFIX(s)
	# endif

# pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
# pragma comment (lib, "Siv3D" SIV3D_DEBUG_LIB_POSTFIX(_d))

# undef SIV3D_DEBUG_LIB_POSTFIX

# endif // defined(_WIN32)
