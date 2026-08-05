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
# include "SimdFloat4.hpp"
# include "Quaternion.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Mat4x4
	//
	////////////////////////////////////////////////////////////////

	/// @brief 4x4 行列クラス | 4x4 matrix class
	struct Mat4x4
	{
		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Mat4x4& value);
	};
}

# include "detail/Mat4x4.ipp"
