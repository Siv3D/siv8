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

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Quaternion
	//
	////////////////////////////////////////////////////////////////

	/// @brief クォータニオン | Quaternion
	struct Quaternion
	{
		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Quaternion& value);
	};
}

# include "detail/Quaternion.ipp"
