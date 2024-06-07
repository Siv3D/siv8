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

# include <Siv3D/MSL.hpp>
# include <Siv3D/VertexSHader.hpp>
# include <Siv3D/PixelShader.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MSL::MSL(String _entryPoint)
		: entryPoint{ std::move(_entryPoint) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator VertexShader
	//
	////////////////////////////////////////////////////////////////

	MSL::operator VertexShader() const
	{
		if (path)
		{
			return VertexShader::MSL(path, entryPoint);
		}
		else
		{
			return VertexShader::MSL(path);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator PixelShader
	//
	////////////////////////////////////////////////////////////////

	MSL::operator PixelShader() const
	{
		if (path)
		{
			return PixelShader::MSL(path, entryPoint);
		}
		else
		{
			return PixelShader::MSL(path);
		}
	}
}
