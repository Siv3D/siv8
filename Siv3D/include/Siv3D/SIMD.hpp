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

# pragma once
# include "Platform.hpp"

# if SIV3D_INTRINSIC(SSE)

# if __has_include(<immintrin.h>)
#	include <immintrin.h> // SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2, AVX, AVX2, FMA
# endif

# elif SIV3D_INTRINSIC(NEON)

# if __has_include(<arm_neon.h>)
#	include <arm_neon.h>
# endif

# endif
