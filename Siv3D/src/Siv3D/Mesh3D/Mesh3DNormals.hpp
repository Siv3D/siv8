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
# include <Siv3D/Array.hpp>
# include <Siv3D/TriangleIndex32.hpp>
# include <Siv3D/Vertex3D.hpp>
# include <ThirdParty/DirectXMath/DirectXMath.h>

namespace s3d
{
    enum CNORM_FLAGS : uint32_t
    {
        CNORM_DEFAULT = 0,
        // Default is to compute normals using weight-by-angle

        CNORM_WEIGHT_BY_AREA = 0x1,
        // Computes normals using weight-by-area

        CNORM_WEIGHT_EQUAL = 0x2,
        // Compute normals with equal weights

        CNORM_WIND_CW = 0x4,
        // Vertices are clock-wise (defaults to CCW)
    };

    bool __cdecl ComputeNormals(
        _In_reads_(nFaces * 3) const uint32_t* indices, _In_ size_t nFaces,
        _In_reads_(nVerts) const DirectX::XMFLOAT3* positions, _In_ size_t nVerts,
        _In_ CNORM_FLAGS flags,
        _Out_writes_(nVerts) DirectX::XMFLOAT3* normals) noexcept;
}
