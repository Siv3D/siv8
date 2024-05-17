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

		return(VertexShader::IDType::Null());

		//Blob binary = compileHLSLFromFile(path, ShaderStage::Vertex, entryPoint);

		//return createVS(std::move(binary), bindings);
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

		return PixelShader::IDType::Null();

		//Blob binary = compileHLSLFromFile(path, ShaderStage::Pixel, entryPoint);

		//return createPS(std::move(binary), bindings);
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

	}

	void CShader_D3D11::setPS(const PixelShader::IDType handleID)
	{

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
}
