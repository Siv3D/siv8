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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Mat3x3.hpp>

namespace s3d
{
	struct VSConstants2D
	{
		Float4 transform[2] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
		
		Float4 colorMul{ 1.0f, 1.0f, 1.0f, 1.0f };
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

	struct PSEffectConstants2D
	{
		Float4 patternUVTransform[2] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
		
		Float4 patternBackgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };

		Float4 quadWarpInvHomography[3] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };

		Float4 quadWarpUVTransform;

		void setPattern(const std::array<Float4, 3>& params)
		{
			patternUVTransform[0]	= params[0];
			patternUVTransform[1]	= params[1];
			patternBackgroundColor	= params[2];
		}

		void setQuadWarp(const Mat3x3& mat, const Float4& uvTransform)
		{
			quadWarpInvHomography[0]	= { mat._11, mat._12, mat._13, 0.0f };
			quadWarpInvHomography[1]	= { mat._21, mat._22, mat._23, 0.0f };
			quadWarpInvHomography[2]	= { mat._31, mat._32, mat._33, 0.0f };
			quadWarpUVTransform			= uvTransform;
		}
	};
}
