﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "../Common.hpp"
# include "PatternParameters.hpp"

namespace s3d
{
	namespace Pattern
	{
		struct Checker
		{
			ColorF primary{ 1.0, 1.0 };

			ColorF background{ 0.0, 0.0 };

			double scale = 16.0;

			double horizontalIntensity = 1.0;

			double verticalIntensity = 1.0;

			double angle = 0.0_deg;

			Vec2 offset{ 0, 0 };

			[[nodiscard]]
			operator PatternParameters() const noexcept
			{
				return{
					.primaryColor = primary.toFloat4(),
					.backgroundColor = background.toFloat4(),
					.uvTransform = Mat3x2::Rotate(-angle).scaled(1.0 / scale).translated(-offset),
					.param0 = static_cast<float>(verticalIntensity),
					.param1 = static_cast<float>(horizontalIntensity),
					.type = PatternType::Checker
				};
			}
		};
	}
}
