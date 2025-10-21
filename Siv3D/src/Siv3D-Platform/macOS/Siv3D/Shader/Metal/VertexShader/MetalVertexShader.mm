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
		m_shader = NS::TransferPtr(library->newFunction(NS::String::string(name.c_str(), NS::UTF8StringEncoding)));

		if (not m_shader)
		{
			LOG_FAIL(fmt::format("MetalVertexShader: Failed to create a vertex shader `{}`", name));
			return;
		}

		m_initialized = true;
	}

	MetalVertexShader::MetalVertexShader(MTL::Device* device, const std::string& source, const std::string& entryPoint)
	{
		NS::Error* error = nullptr;
		NS::SharedPtr<MTL::CompileOptions> opts = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
		NS::SharedPtr<MTL::Library> library = NS::TransferPtr(device->newLibrary(NS::String::string(source.c_str(), NS::UTF8StringEncoding), opts.get(), &error));
		
		if (not library)
		{
			LOG_FAIL(fmt::format("MetalVertexShader: Failed to create a library for vertex shader `{}`. {}", entryPoint, error->localizedDescription()->utf8String()));
			return;
		}
		
		m_shader = NS::TransferPtr(library->newFunction(NS::String::string(entryPoint.c_str(), NS::UTF8StringEncoding)));
		
		if (not m_shader)
		{
			LOG_FAIL(fmt::format("MetalVertexShader: Failed to create a vertex shader `{}`", entryPoint));
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
}
