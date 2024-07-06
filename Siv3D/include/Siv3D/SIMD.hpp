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
#	include <xmmintrin.h> // SSE
# endif

# if __has_include(<emmintrin.h>)
#	include <emmintrin.h> // SSE2
# endif

# if __has_include(<pmmintrin.h>)
#	include <pmmintrin.h> // SSE3
# endif

# if __has_include(<tmmintrin.h>)
#	include <tmmintrin.h> // SSSE3
# endif

# if __has_include(<smmintrin.h>)
#	include <smmintrin.h> // SSE4.1
# endif

# if __has_include(<nmmintrin.h>)
#	include <nmmintrin.h> // SSE4.2
# endif

# if __has_include(<immintrin.h>)
#	include <immintrin.h> // AVX, AVX2, FMA
# endif

# elif SIV3D_INTRINSIC(NEON)

# if __has_include(<arm_neon.h>)
#	include <arm_neon.h>
# endif

# endif
