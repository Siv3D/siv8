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

			return (blob[0] == Byte{ 'D' })
				&& (blob[1] == Byte{ 'X' })
				&& (blob[2] == Byte{ 'B' })
				&& (blob[3] == Byte{ 'C' });
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
		static Blob CompileHLSLFromFile(ShaderCompiler& shaderCompiler, const FilePathView path, const ShaderStage stage, const StringView entryPoint, const uint32 flags)
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
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CShader_D3D11::~CShader_D3D11()
	{
		LOG_SCOPED_DEBUG("CShader_D3D11::~CShader_D3D11()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CShader_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CShader_D3D11::init()");

		pRenderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
		m_device = pRenderer->getDevice().getDevice();
		m_context = pRenderer->getDevice().getContext();

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

	VertexShader::IDType CShader_D3D11::createVSFromSource(const StringView source, StringView entryPoint)
	{
		return VertexShader::IDType::Null();
	}

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

	PixelShader::IDType CShader_D3D11::createPSFromSource(const StringView source, StringView entryPoint)
	{
		return PixelShader::IDType::Null();
	}

	void CShader_D3D11::releaseVS(const VertexShader::IDType handleID)
	{
		m_vertexShaders.erase(handleID);
	}

	void CShader_D3D11::releasePS(const PixelShader::IDType handleID)
	{
		m_pixelShaders.erase(handleID);
	}

	void CShader_D3D11::setVS(const VertexShader::IDType handleID)
	{
		m_context->VSSetShader(m_vertexShaders[handleID]->getShader(), nullptr, 0);
	}

	void CShader_D3D11::setPS(const PixelShader::IDType handleID)
	{
		m_context->PSSetShader(m_pixelShaders[handleID]->getShader(), nullptr, 0);
	}

	const Blob& CShader_D3D11::getBinaryVS(const VertexShader::IDType handleID)
	{
		static const Blob blob;
		return blob;
	}

	const Blob& CShader_D3D11::getBinaryPS(const PixelShader::IDType handleID)
	{
		static const Blob blob;
		return blob;
	}

	Blob CShader_D3D11::compileHLSL(const std::string_view source, const std::string& sourceName, const std::string& entryPoint, const ShaderStage shaderStage, const HLSL::CompileOption option, std::string& message)
	{
		return m_shaderCompiler.compile(source, sourceName, entryPoint, ToTargetName(shaderStage), FromEnum(option), message);
	}

	VertexShader::IDType CShader_D3D11::createVS(Blob&& binary)
	{
		auto vertexShader = std::make_unique<D3D11VertexShader>(std::move(binary), m_device);

		if (not vertexShader->isInitialized())
		{
			return VertexShader::IDType::Null();
		}

		return m_vertexShaders.add(std::move(vertexShader));
	}

	PixelShader::IDType CShader_D3D11::createPS(Blob&& binary)
	{
		auto pixelShader = std::make_unique<D3D11PixelShader>(std::move(binary), m_device);

		if (not pixelShader->isInitialized())
		{
			return PixelShader::IDType::Null();
		}

		return m_pixelShaders.add(std::move(pixelShader));
	}

	Blob ShaderCompiler::compile(const std::string_view source, const std::string& sourceName, const std::string& entryPoint, const char* target, const uint32 flags, std::string& message)
	{
		message.clear();

		if (not p_D3DCompile)
		{
			d3dCompiler		= DLL::LoadSystemLibrary(L"d3dcompiler_47.dll");
			p_D3DCompile	= DLL::GetFunction(d3dCompiler, "D3DCompile");
		}

		ComPtr<ID3DBlob> binary, error;
		const HRESULT hr = p_D3DCompile(source.data(), source.size(),
			sourceName.c_str(), nullptr, nullptr,
			entryPoint.c_str(),
			target,
			flags, 0, &binary, &error);

		if (error)
		{
			message.assign(static_cast<const char*>(error->GetBufferPointer()));
		}

		if (FAILED(hr)) // 失敗したら
		{
			LOG_FAIL("CShader_D3D11::ShaderCompiler::compileShader(): D3DCompile(): " + message);

			return{};
		}
		else if (error)
		{
			LOG_WARN("CShader_D3D11::ShaderCompiler::compileShader(): D3DCompile(): " + message);
		}

		return Blob{ binary->GetBufferPointer(), binary->GetBufferSize() };
	}
}
