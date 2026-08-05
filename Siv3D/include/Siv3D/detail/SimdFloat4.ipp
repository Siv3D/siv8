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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator +=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorAdd(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator -=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorSubtract(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator *=(const float s) & noexcept
	{
		vec = DirectX::XMVectorScale(vec, s);
		return *this;
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator *=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorMultiply(vec, v.vec);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator /=(const float s) & noexcept
	{
		vec = DirectX::XMVectorDivide(vec, DirectX::XMVectorReplicate(s));
		return *this;
	}

	inline SimdFloat4& SIV3D_VECTOR_CALL SimdFloat4::operator /=(const SimdFloat4 v) & noexcept
	{
		vec = DirectX::XMVectorDivide(vec, v.vec);
		return *this;
	}
}
