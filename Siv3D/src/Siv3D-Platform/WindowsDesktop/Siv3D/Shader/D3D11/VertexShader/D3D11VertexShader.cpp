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

# include "D3D11VertexShader.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	D3D11VertexShader::D3D11VertexShader(Null)
		: m_initialized{ true } {}

	D3D11VertexShader::D3D11VertexShader(Blob&& bytecode, ID3D11Device* device)
		: m_bytecode{ std::move(bytecode) }
	{
		if (FAILED(device->CreateVertexShader(m_bytecode.data(), static_cast<size_t>(m_bytecode.size()), nullptr, &m_shader)))
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

	bool D3D11VertexShader::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getShader
	//
	////////////////////////////////////////////////////////////////

	ID3D11VertexShader* D3D11VertexShader::getShader() const
	{
		return m_shader.Get();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBytecode
	//
	////////////////////////////////////////////////////////////////

	const Blob& D3D11VertexShader::getBytecode() const noexcept
	{
		return m_bytecode;
	}
}
