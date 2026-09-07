//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"

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
    struct FormatData;

    ////////////////////////////////////////////////////////////////
    //
    //	aligned_float4
    //
    ////////////////////////////////////////////////////////////////

	using aligned_float4 = DirectX::XMVECTOR;

# define SIV3D_VECTOR_CALL XM_CALLCONV

    ////////////////////////////////////////////////////////////////
    //
    //	Formatter
    //
    ////////////////////////////////////////////////////////////////

    void Formatter(FormatData& formatData, aligned_float4 value);
}
