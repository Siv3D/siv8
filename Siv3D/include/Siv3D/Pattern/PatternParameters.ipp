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

namespace s3d
{
	constexpr std::array<Float4, 3> PatternParameters::toFloat4Array(const float maxScalingInv) const noexcept
	{
		Mat3x2 mat = uvTransform.scaled(maxScalingInv);

		return{ {
			{ mat._11, mat._12, mat._31, mat._32 },
			{ mat._21, mat._22, param0, param1 },
			backgroundColor
		} };
	}
}
