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

# include <Siv3D/MSL.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/ShaderGroup.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MSL::MSL(String _entryPoint)
		: m_entryPoint{ std::move(_entryPoint) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	ShaderGroup MSL::operator |(const HLSL& hlsl) const
	{
		return{ hlsl, *this };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator VertexShader
	//
	////////////////////////////////////////////////////////////////

	MSL::operator VertexShader() const
	{
		if (m_path)
		{
			return VertexShader::MSL(m_path, m_entryPoint);
		}
		else
		{
			return VertexShader::MSL(m_entryPoint);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator PixelShader
	//
	////////////////////////////////////////////////////////////////

	MSL::operator PixelShader() const
	{
		if (m_path)
		{
			return PixelShader::MSL(m_path, m_entryPoint);
		}
		else
		{
			return PixelShader::MSL(m_entryPoint);
		}
	}
}
