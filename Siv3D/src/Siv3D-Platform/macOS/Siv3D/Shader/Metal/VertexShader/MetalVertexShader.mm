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

# include "MetalVertexShader.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalVertexShader::MetalVertexShader(Null)
		: m_initialized{ true } {}

	MetalVertexShader::MetalVertexShader(MTL::Library* library, const std::string& name)
	{
		m_shader = NS::TransferPtr(library->newFunction(NS::String::string(name.c_str(), NS::ASCIIStringEncoding)));

		if (not m_shader)
		{
			LOG_FAIL(fmt::format("MetalVertexShader: Failed to create a vertex shader `{}`", name));
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
