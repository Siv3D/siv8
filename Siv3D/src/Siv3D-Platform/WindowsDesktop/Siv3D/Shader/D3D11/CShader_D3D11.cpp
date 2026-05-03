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

# include "CShader_D3D11.hpp"
# include <Siv3D/ShaderStage.hpp>
# include <Siv3D/TextFileReader.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/ConstantBuffer/D3D11/ConstantBuffer_D3D11.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		/// @brief Reader オブジェクトがコンパイル済みシェーダであるかを判定します。
		/// @param reader Reader オブジェクト
		/// @return Reader オブジェクトがコンパイル済みシェーダである場合 true, それ以外の場合は false
		[[nodiscard]]
		static bool IsPrecompiledHLSL(const IReader& reader) noexcept
		{
			if (reader.size() < 4)
			{
				return false;
			}

			char fourCC[4]{};
			
			if (not reader.lookahead(fourCC))
			{
				return false;
			}

			return ((fourCC[0] == 'D') && (fourCC[1] == 'X') && (fourCC[2] == 'B') && (fourCC[3] == 'C'));
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
		static Blob CompileHLSLFromReader(HLSLCompiler& shaderCompiler, std::unique_ptr<IReader> reader, const ShaderStage stage, const StringView entryPoint, const uint32 flags, const FilePathView pathHint)
		{
			if (pathHint)
			{
				LOG_DEBUG(fmt::format("CompileHLSLFromReader(path = {}, stage = {}, entryPoint = {}, flags = {:#X})", pathHint.toUTF8(), ToTargetName(stage), entryPoint.toUTF8(), flags));
			}
			else
			{
				LOG_DEBUG(fmt::format("CompileHLSLFromReader(stage = {}, entryPoint = {}, flags = {:#X})", ToTargetName(stage), entryPoint.toUTF8(), flags));
			}
			
			TextFileReader textReader{ std::move(reader) };

			if (not textReader)
			{
				if (pathHint)
				{
					LOG_FAIL(fmt::format("CompileHLSLFromReader(): failed to read `{}`", pathHint.toUTF8()));

				}
				else
				{
					LOG_FAIL("CompileHLSLFromReader(): failed to read the shader source");
				}

				return{};
			}

			const std::string source = textReader.readAllUTF8();
			const std::string sourceName = (pathHint ? Unicode::ToUTF8(pathHint) : "shader");
			std::string message;
			return shaderCompiler.compile(source, sourceName, Unicode::ToUTF8(entryPoint), ToTargetName(stage), flags, message);
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
	//	createVSFromReader
	//
	////////////////////////////////////////////////////////////////

	VertexShader::IDType CShader_D3D11::createVSFromReader(std::unique_ptr<IReader> reader, const FilePathView pathHint, StringView entryPoint)
	{
		if (not reader->isOpen())
		{
			if (pathHint) // ファイルの場合
			{
				LOG_FAIL(fmt::format("CShader_D3D11::createVSFromFile(): failed to open `{}`", pathHint.toUTF8()));
			}
			else // その他の Reader オブジェクトの場合
			{
				LOG_FAIL("CShader_D3D11::createVSFromFile(): failed to read the shader source");
			}

			return VertexShader::IDType::Null();
		}

		// コンパイル済みシェーダであれば、そのまま VS を作成して返す
		if (IsPrecompiledHLSL(*reader))
		{
			if (pathHint) // ファイルの場合
			{
				LOG_DEBUG(fmt::format("CShader_D3D11::createVSFromFile(): `{}` is a precompiled shader file", pathHint.toUTF8()));
			}
			else // その他の Reader オブジェクトの場合
			{
				LOG_DEBUG("CShader_D3D11::createVSFromFile(): the shader source is a precompiled shader");
			}

			return createVS(Blob{ std::move(reader) });
		}
		
		// ソースコードであれば、コンパイルしてから VS を作成して返す
		if (Blob byteCode = CompileHLSLFromReader(m_shaderCompiler, std::move(reader), ShaderStage::Vertex, entryPoint, (D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS), pathHint))
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

	VertexShader::IDType CShader_D3D11::createVSFromSource(const std::string& source, const StringView entryPoint)
	{
		if (auto result = Platform::Windows::CompileHLSLFromSource(source, ShaderStage::Vertex, entryPoint))
		{
			return createVS(std::move(*result));
		}
		else
		{
			return VertexShader::IDType::Null();
		}
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
	//	createPSFromReader
	//
	////////////////////////////////////////////////////////////////

	PixelShader::IDType CShader_D3D11::createPSFromReader(std::unique_ptr<IReader> reader, const FilePathView pathHint, StringView entryPoint)
	{
		if (not reader->isOpen())
		{
			if (pathHint) // ファイルの場合
			{
				LOG_FAIL(fmt::format("CShader_D3D11::createPSFromFile(): failed to open `{}`", pathHint.toUTF8()));
			}
			else // その他の Reader オブジェクトの場合
			{
				LOG_FAIL("CShader_D3D11::createPSFromFile(): failed to read the shader source");
			}

			return PixelShader::IDType::Null();
		}

		// コンパイル済みシェーダであれば、そのまま PS を作成して返す
		if (IsPrecompiledHLSL(*reader))
		{
			if (pathHint) // ファイルの場合
			{
				LOG_DEBUG(fmt::format("CShader_D3D11::createPSFromFile(): `{}` is a precompiled shader file", pathHint.toUTF8()));
			}
			else // その他の Reader オブジェクトの場合
			{
				LOG_DEBUG("CShader_D3D11::createPSFromFile(): the shader source is a precompiled shader");
			}
			
			return createPS(Blob{ std::move(reader) });
		}

		// ソースコードであれば、コンパイルしてから PS を作成して返す
		if (Blob byteCode = CompileHLSLFromReader(m_shaderCompiler, std::move(reader), ShaderStage::Pixel, entryPoint, (D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_WARNINGS_ARE_ERRORS), pathHint))
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

	PixelShader::IDType CShader_D3D11::createPSFromSource(const std::string& source, const StringView entryPoint)
	{
		if (auto result = Platform::Windows::CompileHLSLFromSource(source, ShaderStage::Pixel, entryPoint))
		{
			return createPS(std::move(*result));
		}
		else
		{
			return PixelShader::IDType::Null();
		}
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
