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

# include "D3D11/CEngineShader_D3D11.hpp"

namespace s3d
{
	ISiv3DEngineShader* ISiv3DEngineShader::Create()
	{
		return new CEngineShader_D3D11;
	}
}
