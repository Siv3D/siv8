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
	inline Halftone::operator PatternParameters() const noexcept
	{
		const double invPitch = (1.0 / pitch);
		const double c = std::cos(angle);
		const double s = std::sin(angle);
		const Vec2 direction = (end - start);
		const Vec2 gradient = (direction / direction.lengthSq());
		// Express the drawing-space linear field in the lattice's UV coordinates.
		const double gx = (pitch * (c * gradient.x + s * gradient.y));
		const double gy = (pitch * (-s * gradient.x + c * gradient.y));
		const double bias = ((origin - start).dot(gradient) - 0.5 * (gx + gy));

		return{
			.primaryColor = primary.toFloat4(),
			.backgroundColor = background.toFloat4(),
			.uvTransform = {
				static_cast<float>(c * invPitch), static_cast<float>(-s * invPitch),
				static_cast<float>(s * invPitch), static_cast<float>(c * invPitch),
				static_cast<float>(0.5 - (origin.x * c + origin.y * s) * invPitch),
				static_cast<float>(0.5 - (-origin.x * s + origin.y * c) * invPitch) },
			.param0 = static_cast<float>(2.0 * minRadius * invPitch),
			.param1 = static_cast<float>(2.0 * maxRadius * invPitch),
			.type = PatternType::Halftone,
			.extraParams = { static_cast<float>(gx), static_cast<float>(gy), static_cast<float>(bias), 0.0f },
		};
	}
}
