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

# include "CShader_Metal.hpp"
# include <Siv3D/ShaderStage.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CShader_Metal::CShader_Metal() {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CShader_Metal::~CShader_Metal()
	{
		LOG_SCOPED_DEBUG("CShader_Metal::~CShader_Metal()");

		// PS の管理を破棄
		m_pixelShaders.destroy();

		// VS の管理を破棄
		m_vertexShaders.destroy();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CShader_Metal::init()
	{
		LOG_SCOPED_DEBUG("CShader_Metal::init()");

		CRenderer_Metal* pRenderer = static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		m_device = pRenderer->getDevice();
		
		m_defaultLibrary = NS::TransferPtr(m_device->newDefaultLibrary());

		if (not m_defaultLibrary)
		{
			throw InternalEngineError{ "Metal::newDefaultLibrary() failed" };
		}

		NS::Array* functionNames = m_defaultLibrary->functionNames();
		
		LOG_TRACE(fmt::format("{} functions in the default library", functionNames->count()));

		for (uint32 i = 0; i < functionNames->count(); ++i)
		{
			LOG_TRACE(fmt::format("- {}", static_cast<NS::String*>(functionNames->object(i))->utf8String()));
		}
		
		// null VS を管理に登録
		{
			auto nullVertexShader = std::make_unique<MetalVertexShader>(MetalVertexShader::Null{});

			if (not nullVertexShader->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null vertex shader" };
			}

			m_vertexShaders.setNullData(std::move(nullVertexShader));
		}

		// null PS を管理に登録
		{
			auto nullPixelShader = std::make_unique<MetalPixelShader>(MetalPixelShader::Null{});

			if (not nullPixelShader->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null pixel shader" };
			}

			m_pixelShaders.setNullData(std::move(nullPixelShader));
		}
	}

	VertexShader::IDType CShader_Metal::createVSFromFile(const FilePathView path, StringView entryPoint)
	{
		// [Siv3D ToDo] ファイルからの読み込みは今後実装
		if (path)
		{
			return VertexShader::IDType::Null();
		}

		auto vertexShader = std::make_unique<MetalVertexShader>(m_defaultLibrary.get(), Unicode::ToUTF8(entryPoint));

		if (not vertexShader->isInitialized())
		{
			return VertexShader::IDType::Null();
		}

		return m_vertexShaders.add(std::move(vertexShader));
	}

	VertexShader::IDType CShader_Metal::createVSFromSource(const StringView source, StringView entryPoint)
	{
		return(VertexShader::IDType::Null());
	}

	VertexShader::IDType CShader_Metal::createVSFromBytecode(const Blob& bytecode)
	{
		return(VertexShader::IDType::Null());
	}

	PixelShader::IDType CShader_Metal::createPSFromFile(const FilePathView path, StringView entryPoint)
	{
		// [Siv3D ToDo] ファイルからの読み込みは今後実装
		if (path)
		{
			return PixelShader::IDType::Null();
		}

		auto pixelShader = std::make_unique<MetalPixelShader>(m_defaultLibrary.get(), Unicode::ToUTF8(entryPoint));

		if (not pixelShader->isInitialized())
		{
			return PixelShader::IDType::Null();
		}

		return m_pixelShaders.add(std::move(pixelShader));
	}

	PixelShader::IDType CShader_Metal::createPSFromSource(const StringView source, StringView entryPoint)
	{
		return(PixelShader::IDType::Null());
	}

	PixelShader::IDType CShader_Metal::createPSFromBytecode(const Blob& bytecode)
	{
		return(PixelShader::IDType::Null());
	}

	void CShader_Metal::releaseVS(const VertexShader::IDType handleID)
	{
		m_vertexShaders.erase(handleID);
	}

	void CShader_Metal::releasePS(const PixelShader::IDType handleID)
	{
		m_pixelShaders.erase(handleID);
	}

	void CShader_Metal::setVS(const VertexShader::IDType handleID)
	{
		//m_context->VSSetShader(m_vertexShaders[handleID]->getShader(), nullptr, 0);
	}

	void CShader_Metal::setVSNull()
	{
		//m_context->VSSetShader(m_vertexShaders[handleID]->getShader(), nullptr, 0);
	}

	void CShader_Metal::setPS(const PixelShader::IDType handleID)
	{
		//m_context->PSSetShader(m_pixelShaders[handleID]->getShader(), nullptr, 0);
	}

	void CShader_Metal::setPSNull()
	{
		//m_context->VSSetShader(m_vertexShaders[handleID]->getShader(), nullptr, 0);
	}

	const Blob& CShader_Metal::getBytecodeVS(const VertexShader::IDType handleID)
	{
		static const Blob blob;
		return blob;
	}

	const Blob& CShader_Metal::getBytecodePS(const PixelShader::IDType handleID)
	{
		static const Blob blob;
		return blob;
	}

	void CShader_Metal::setConstantBufferVS(const uint32 slot, IConstantBuffer* cb)
	{

	}

	void CShader_Metal::setConstantBufferPS(const uint32 slot, IConstantBuffer* cb)
	{

	}

	MTL::Function* CShader_Metal::getShaderVS(const VertexShader::IDType handleID)
	{
		return m_vertexShaders[handleID]->getShader();
	}

	MTL::Function* CShader_Metal::getShaderPS(const PixelShader::IDType handleID)
	{
		return m_pixelShaders[handleID]->getShader();
	}
}
