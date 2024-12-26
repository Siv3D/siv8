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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>

namespace s3d
{
	struct VSConstants2D
	{
		Float4 transform[2] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
		
		Float4 colorMul{ 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct VSQuadWarpConstants
	{
		std::array<Float4, 3> homography;
	};

	struct PSConstants2D
	{
		Float4 patternBackgroundColorMul{ 1.0f, 1.0f, 1.0f, 1.0f };

		Float4 colorAdd{ 0.0f, 0.0f, 0.0f, 0.0f };

		//Float4 sdfParam{ 0.5f, 0.5f, 0.0f, 0.0f };

		//Float4 sdfOuterColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		//Float4 sdfShadowColor{ 0.0f, 0.0f, 0.0f, 0.5f };

		//Float4 internal{ 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct PSPatternConstants2D
	{
		Float4 transform_params[2] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
		
		Float4 backgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct PSQuadWarpConstants
	{
		std::array<Float4, 3> invHomography;
		
		Float4 uvTransform;
	};
}
