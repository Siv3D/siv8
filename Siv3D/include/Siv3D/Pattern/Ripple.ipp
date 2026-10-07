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

namespace s3d::Pattern
{
	inline Ripple::operator PatternParameters() const noexcept
	{
		const double invPitch = (1.0 / pitch);
		return{
			.primaryColor = primary.toFloat4(),
			.backgroundColor = background.toFloat4(),
			.uvTransform = {
				static_cast<float>(invPitch), 0.0f, 0.0f, static_cast<float>(invPitch),
				static_cast<float>(-center.x * invPitch), static_cast<float>(-center.y * invPitch) },
			.param0 = static_cast<float>(thickness * invPitch),
			.param1 = static_cast<float>(radiusOffset * invPitch),
			.type = PatternType::Ripple,
		};
	}
}
