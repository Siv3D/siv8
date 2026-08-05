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
# include "PointVector.hpp"
# include "SIMDMath.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	SimdFloat4
	//
	////////////////////////////////////////////////////////////////

	/// @brief SIMD 対応の 4 要素ベクトル | SIMD-enabled 4-element vector
	struct SimdFloat4
	{
		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const SimdFloat4& value);
	};
}

# include "detail/SimdFloat4.ipp"
