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

namespace s3d
{
	namespace Graphics2D
	{
		namespace Internal
		{
			void SetColorMul(const Float4& color);

			void SetColorAdd(const Float3& color);

			void SetBlendState(const BlendState& blendState);

			void SetRasterizerState(const RasterizerState& rasterizerState);
		}
	}
}
