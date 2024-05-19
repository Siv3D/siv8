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

# include <Siv3D/HLSL.hpp>
# include <Siv3D/VertexSHader.hpp>
# include <Siv3D/PixelShader.hpp>

namespace s3d
{
	HLSL::HLSL(FilePath _path)
		: path{ std::move(_path) } {}

	HLSL::HLSL(FilePath _path, String _entryPoint)
		: path{ std::move(_path) }
		, entryPoint{ std::move(_entryPoint) } {}

	HLSL::operator VertexShader() const
	{
		if (entryPoint)
		{
			return VertexShader::HLSL(path, entryPoint);
		}
		else
		{
			return VertexShader::HLSL(path);
		}
	}

	HLSL::operator PixelShader() const
	{
		if (entryPoint)
		{
			return PixelShader::HLSL(path, entryPoint);
		}
		else
		{
			return PixelShader::HLSL(path);
		}
	}
}
