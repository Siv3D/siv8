//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "MetalPixelShader.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalPixelShader::MetalPixelShader(Null)
		: m_initialized{ true } {}

	MetalPixelShader::MetalPixelShader(MTL::Library* library, const std::string& name)
	{
		loadFunction(library, name);
	}

	MetalPixelShader::MetalPixelShader(MTL::Device* device, const std::string& source, const std::string& entryPoint)
	{
		NS::Error* error = nullptr;
		NS::SharedPtr<MTL::CompileOptions> opts = NS::TransferPtr(MTL::CompileOptions::alloc()->init());
		NS::SharedPtr<MTL::Library> library = NS::TransferPtr(device->newLibrary(NS::String::string(source.c_str(), NS::UTF8StringEncoding), opts.get(), &error));
														  
		if (not library)
		{
			LOG_FAIL(fmt::format("MetalPixelShader: Failed to create a library for pixel shader `{}`. {}", entryPoint, error->localizedDescription()->utf8String()));
			return;
		}
		
		loadFunction(library.get(), entryPoint);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool MetalPixelShader::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getShader
	//
	////////////////////////////////////////////////////////////////

	MTL::Function* MetalPixelShader::getShader() const
	{
		return m_shader.get();
	}

	////////////////////////////////////////////////////////////////
	//
	//	loadFunction
	//
	////////////////////////////////////////////////////////////////

	void MetalPixelShader::loadFunction(MTL::Library* library, const std::string& name)
	{
		auto function = NS::TransferPtr(library->newFunction(NS::String::string(name.c_str(), NS::UTF8StringEncoding)));
		if (not function)
		{
			LOG_FAIL(fmt::format("MetalPixelShader: Failed to create a pixel shader `{}`", name));
			return;
		}
		if (function->functionType() != MTL::FunctionTypeFragment)
		{
			LOG_FAIL(fmt::format("MetalPixelShader: Function `{}` is not a fragment function", name));
			return;
		}

		m_shader = std::move(function);
		m_initialized = true;
	}
}
