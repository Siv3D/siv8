//-----------------------------------------------
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
# include "../PointVector.hpp"
# include "../Mat3x2.hpp"
# include "PatternType.hpp"

namespace s3d
{
	struct PatternParameters
	{
		Float4 primaryColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		Float4 backgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };

		Mat3x2 uvTransform = Mat3x2::Identity();

		float param0 = 0.0f;

		float param1 = 0.0f;

		PatternType type = PatternType::PolkaDot;
	};
}
