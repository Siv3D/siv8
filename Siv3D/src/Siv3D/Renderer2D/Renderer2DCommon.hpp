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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Mat3x3.hpp>

namespace s3d
{
	struct VSConstants2D
	{
		Float4 transform[3] = {};
		
		Float4 colorMul{ 1.0f, 1.0f, 1.0f, 1.0f };

		void setTransform(const Mat3x3& matrix)
		{
			transform[0] = { matrix._11, matrix._12, 0.0f, matrix._13 };
			transform[1] = { matrix._21, matrix._22, 0.0f, matrix._23 };
			transform[2] = { matrix._31, matrix._32, 0.0f, matrix._33 };
		}
	};

	static_assert(sizeof(VSConstants2D) == 64);
	static_assert(offsetof(VSConstants2D, colorMul) == 48);

	struct PSConstants2D
	{
		Float4 patternBackgroundColorMul{ 1.0f, 1.0f, 1.0f, 1.0f };

		Float4 colorAdd{ 0.0f, 0.0f, 0.0f, 0.0f };

		Float4 sdfParam{ 0.5f, 0.5f, 0.0f, 0.0f };

		Float4 sdfOuterColorPMA{ 0.0f, 0.0f, 0.0f, 1.0f };

		Float4 sdfShadowColorPMA{ 0.0f, 0.0f, 0.0f, 0.5f };

		//Float4 internal{ 0.0f, 0.0f, 0.0f, 0.0f };

		void setSDFParameters(const std::array<Float4, 3>& params)
		{
			sdfParam			= params[0];
			sdfOuterColorPMA	= params[1];
			sdfShadowColorPMA	= params[2];
		}
	};

	struct PSEffectConstants2D
	{
		Float4 patternUVTransform[2] = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f } };
		
		Float4 patternBackgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };

		Float4 patternExtraParams{ 0.0f, 0.0f, 0.0f, 0.0f };

		void setPattern(const std::array<Float4, 4>& params)
		{
			patternUVTransform[0]	= params[0];
			patternUVTransform[1]	= params[1];
			patternBackgroundColor	= params[2];
			patternExtraParams		= params[3];
		}

	};
}
