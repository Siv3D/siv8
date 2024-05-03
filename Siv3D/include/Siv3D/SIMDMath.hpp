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
# include "Common.hpp"
# include "FormatData.hpp"

# if SIV3D_INTRINSIC(SSE)
#	define _XM_SSE4_INTRINSICS_
# endif

# ifdef __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
# endif

# include <ThirdParty/DirectXMath/DirectXMath.h>
# include <ThirdParty/DirectXMath/DirectXPackedVector.h>

# ifdef __clang__
#	pragma clang diagnostic pop
# endif

# if !SIV3D_PLATFORM(WINDOWS)
#	undef __in
#	undef __out
#	undef __valid
# endif

namespace s3d
{
    ////////////////////////////////////////////////////////////////
    //
    //	aligned_float4
    //
    ////////////////////////////////////////////////////////////////

#if defined(_XM_SSE_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)

    using aligned_float4 = __m128;

#elif defined(_XM_ARM_NEON_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
    
    using aligned_float4 = float32x4_t;

#else
    
    using aligned_float4 = DirectX::__vector4;

#endif

# define SIV3D_VECTOR_CALL XM_CALLCONV

    ////////////////////////////////////////////////////////////////
    //
    //	Formatter
    //
    ////////////////////////////////////////////////////////////////

    void Formatter(FormatData& formatData, aligned_float4 value);
}
