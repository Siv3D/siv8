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

# include "MetalVertexShader.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalVertexShader::MetalVertexShader(Null)
		: m_initialized{ true } {}

	MetalVertexShader::MetalVertexShader(const std::string& name)
	{
		//if (FAILED(device->CreateVertexShader(m_bytecode.data(), static_cast<size_t>(m_bytecode.size()), nullptr, &m_shader)))
		{
			return;
		}

		m_initialized = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool MetalVertexShader::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getShader
	//
	////////////////////////////////////////////////////////////////

	MTL::Function* MetalVertexShader::getShader() const
	{
		return m_shader.get();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBytecode
	//
	////////////////////////////////////////////////////////////////

	const Blob& MetalVertexShader::getBytecode() const noexcept
	{
		return m_bytecode;
	}
}
