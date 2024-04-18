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

# if SIV3D_INTRINSIC(SSE)

# if __has_include(<xmmintrin.h>)
#	include <xmmintrin.h>
# endif

# if __has_include(<emmintrin.h>)
#	include <emmintrin.h>
# endif

# if __has_include(<pmmintrin.h>)
#	include <pmmintrin.h>
# endif

# if __has_include(<tmmintrin.h>)
#	include <tmmintrin.h>
# endif

# if __has_include(<smmintrin.h>)
#	include <smmintrin.h>
# endif

# if __has_include(<nmmintrin.h>)
#	include <nmmintrin.h>
# endif

# else


# endif
