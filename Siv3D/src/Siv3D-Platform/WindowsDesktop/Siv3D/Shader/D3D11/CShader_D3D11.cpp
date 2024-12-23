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

# include "CShader_D3D11.hpp"
# include <Siv3D/ShaderStage.hpp>
# include <Siv3D/TextReader.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/ConstantBuffer/D3D11/ConstantBuffer_D3D11.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		// 4 バイトの FourCC が、コンパイル済みシェーダ "DXBC" かを調べる
		[[nodiscard]]
		static bool IsPrecompiledHLSL(const Blob& blob) noexcept
		{
			if (blob.size() < 4)
			{
				return false;
			}

			return ((blob[0] == Byte{ 'D' })
				 && (blob[1] == Byte{ 'X' })
				 && (blob[2] == Byte{ 'B' })
				 && (blob[3] == Byte{ 'C' }));
		}

		static constexpr const char* ToTargetName(const ShaderStage stage) noexcept
		{
			switch (stage)
			{
			case ShaderStage::Vertex:
				return "vs_4_0";
			case ShaderStage::Pixel:
				return "ps_4_0";
			default:
				return "";
			}
		}

		[[nodiscard]]
		static Blob CompileHLSLFromFile(HLSLCompiler& shaderCompiler, const FilePathView path, const ShaderStage stage, const StringView entryPoint, const uint32 flags)
		{
			LOG_DEBUG(fmt::format("CompileHLSLFromFile(path = {}, stage = {}, entryPoint = {}, flags = {:#X})", path, ToTargetName(stage), entryPoint, flags));

			TextReader reader{ path };

			if (not reader)
			{
				LOG_FAIL(fmt::format("CompileHLSLFromFile: Failed to open file `{}`", path));
				
				return{};
			}

			const std::string source = reader.readAllUTF8();
			std::string message;

			return shaderCompiler.compile(source, Unicode::ToUTF8(path), Unicode::ToUTF8(entryPoint), ToTargetName(stage), flags, message);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CShader_D3D11::CShader_D3D11() {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CShader_D3D11::~CShader_D3D11()
	{
		LOG_SCOPED_DEBUG("CShader_D3D11::~CShader_D3D11()");

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

	void CShader_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CShader_D3D11::init()");

		pRenderer	= static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
		m_device	= pRenderer->getDevice().getDevice();
		m_context	= pRenderer->getDevice().getContext();

		// null VS を管理に登録
		{
			auto nullVertexShader = std::make_unique<D3D11VertexShader>(D3D11VertexShader::Null{});

			if (not nullVertexShader->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null vertex shader" };
			}

			m_vertexShaders.setNullData(std::move(nullVertexShader));
		}

		// null PS を管理に登録
		{
			auto nullPixelShader = std::make_unique<D3D11PixelShader>(D3D11PixelShader::Null{});

			if (not nullPixelShader->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null pixel shader" };
			}

			m_pixelShaders.setNullData(std::move(nullPixelShader));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	createVSFromFile
	//
	////////////////////////////////////////////////////////////////

	VertexShader::IDType CShader_D3D11::createVSFromFile(const FilePathView path, StringView entryPoint)
	{
		Blob blob{ path };

		if (not blob)
		{
			LOG_FAIL(fmt::format("CShader_D3D11::createVSFromFile(): failed to load `{}`", path));
			
			return VertexShader::IDType::Null();
		}

		if (IsPrecompiledHLSL(blob))
		{
			LOG_DEBUG(fmt::format("CShader_D3D11::createVSFromFile(): `{}` is a precompiled shader file", path));
			
			return createVS(std::move(blob));
		}

		if (Blob byteCode = CompileHLSLFromFile(m_shaderCompiler, path, ShaderStage::Vertex, entryPoint, (D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS)))
		{
			return createVS(std::move(byteCode));
		}
		else
		{
			return VertexShader::IDType::Null();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	createVSFromSource
	//
	////////////////////////////////////////////////////////////////

	VertexShader::IDType CShader_D3D11::createVSFromSource(const StringView source, StringView entryPoint)
	{
		return(VertexShader::IDType::Null());
	}

	////////////////////////////////////////////////////////////////
	//
	//	createVSFromBytecode
	//
	////////////////////////////////////////////////////////////////

	VertexShader::IDType CShader_D3D11::createVSFromBytecode(const Blob& bytecode)
	{
		return createVS(Blob{ bytecode });
	}

	////////////////////////////////////////////////////////////////
	//
	//	createPSFromFile
	//
	////////////////////////////////////////////////////////////////

	PixelShader::IDType CShader_D3D11::createPSFromFile(const FilePathView path, StringView entryPoint)
	{
		Blob blob{ path };

		if (not blob)
		{
			LOG_FAIL(fmt::format("CShader_D3D11::createPSFromFile(): failed to load `{}`", path));
			
			return PixelShader::IDType::Null();
		}

		if (IsPrecompiledHLSL(blob))
		{
			LOG_DEBUG(fmt::format("CShader_D3D11::createPSFromFile(): `{}` is a precompiled shader file", path));
			
			return createPS(std::move(blob));
		}

		if (Blob byteCode = CompileHLSLFromFile(m_shaderCompiler, path, ShaderStage::Pixel, entryPoint, (D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS)))
		{
			return createPS(std::move(byteCode));
		}
		else
		{
			return PixelShader::IDType::Null();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	createPSFromSource
	//
	////////////////////////////////////////////////////////////////

	PixelShader::IDType CShader_D3D11::createPSFromSource(const StringView source, StringView entryPoint)
	{
		return(PixelShader::IDType::Null());
	}

	////////////////////////////////////////////////////////////////
	//
	//	createPSFromBytecode
	//
	////////////////////////////////////////////////////////////////

	PixelShader::IDType CShader_D3D11::createPSFromBytecode(const Blob& bytecode)
	{
		return createPS(Blob{ bytecode });
	}

	////////////////////////////////////////////////////////////////
	//
	//	releaseVS
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::releaseVS(const VertexShader::IDType handleID)
	{
		m_vertexShaders.erase(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	releasePS
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::releasePS(const PixelShader::IDType handleID)
	{
		m_pixelShaders.erase(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVS
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::setVS(const VertexShader::IDType handleID)
	{
		m_context->VSSetShader(m_vertexShaders[handleID]->getShader(), nullptr, 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVSNull
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::setVSNull()
	{
		m_context->VSSetShader(nullptr, nullptr, 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPS
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::setPS(const PixelShader::IDType handleID)
	{
		m_context->PSSetShader(m_pixelShaders[handleID]->getShader(), nullptr, 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPSNull
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::setPSNull()
	{
		m_context->PSSetShader(nullptr, nullptr, 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBytecodeVS
	//
	////////////////////////////////////////////////////////////////

	const Blob& CShader_D3D11::getBytecodeVS(const VertexShader::IDType handleID)
	{
		return m_vertexShaders[handleID]->getBytecode();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBytecodePS
	//
	////////////////////////////////////////////////////////////////

	const Blob& CShader_D3D11::getBytecodePS(const PixelShader::IDType handleID)
	{
		return m_pixelShaders[handleID]->getBytecode();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setConstantBufferVS
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::setConstantBufferVS(const uint32 slot, IConstantBuffer* cb)
	{
		assert(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

		if (auto pCB = static_cast<ConstantBuffer_D3D11*>(cb))
		{
			ID3D11Buffer* buffer = pCB->getBuffer();
			m_context->VSSetConstantBuffers(slot, 1, &buffer);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setConstantBufferPS
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::setConstantBufferPS(const uint32 slot, IConstantBuffer* cb)
	{
		assert(slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

		if (auto pCB = static_cast<ConstantBuffer_D3D11*>(cb))
		{
			ID3D11Buffer* buffer = pCB->getBuffer();
			m_context->PSSetConstantBuffers(slot, 1, &buffer);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	compileHLSL
	//
	////////////////////////////////////////////////////////////////

	Blob CShader_D3D11::compileHLSL(const std::string_view source, const std::string& sourceName, const std::string& entryPoint, const ShaderStage shaderStage, const HLSL::CompileOption option, std::string& message)
	{
		return m_shaderCompiler.compile(source, sourceName, entryPoint, ToTargetName(shaderStage), FromEnum(option), message);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	VertexShader::IDType CShader_D3D11::createVS(Blob&& bytecode)
	{
		auto vertexShader = std::make_unique<D3D11VertexShader>(std::move(bytecode), m_device);

		if (not vertexShader->isInitialized())
		{
			return VertexShader::IDType::Null();
		}

		return m_vertexShaders.add(std::move(vertexShader));
	}

	PixelShader::IDType CShader_D3D11::createPS(Blob&& bytecode)
	{
		auto pixelShader = std::make_unique<D3D11PixelShader>(std::move(bytecode), m_device);

		if (not pixelShader->isInitialized())
		{
			return PixelShader::IDType::Null();
		}

		return m_pixelShaders.add(std::move(pixelShader));
	}
}
