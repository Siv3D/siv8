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
	inline Weave::operator PatternParameters() const noexcept
	{
		const double c = std::cos(angle);
		const double s = std::sin(angle);
		const double invPitch = (1.0 / pitch);
		return{
			.primaryColor = primary.toFloat4(),
			.backgroundColor = background.toFloat4(),
			.uvTransform = {
				static_cast<float>(c * invPitch), static_cast<float>(-s * invPitch),
				static_cast<float>(s * invPitch), static_cast<float>(c * invPitch),
				static_cast<float>(-(origin.x * c + origin.y * s) * invPitch),
				static_cast<float>(-(-origin.x * s + origin.y * c) * invPitch) },
			.param0 = static_cast<float>(thickness * invPitch),
			.param1 = static_cast<float>((thickness + 2.0 * gap) * invPitch),
			.type = PatternType::Weave,
		};
	}
}
