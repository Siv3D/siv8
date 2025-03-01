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

# include <Siv3D/HLSL.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	HLSL::HLSL(FilePath _path)
		: path{ std::move(_path) } {}

	HLSL::HLSL(FilePath _path, String _entryPoint)
		: path{ std::move(_path) }
		, entryPoint{ std::move(_entryPoint) } {}

	HLSL::HLSL(const Blob& bytecode)
		: bytecode{ bytecode } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator VertexShader
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	operator PixelShader
	//
	////////////////////////////////////////////////////////////////

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
