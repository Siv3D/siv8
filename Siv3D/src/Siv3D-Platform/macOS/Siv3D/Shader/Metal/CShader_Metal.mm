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

# include "CShader_Metal.hpp"
# include <Siv3D/ShaderStage.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CShader_Metal::~CShader_Metal()
	{
		LOG_SCOPED_DEBUG("CShader_Metal::~CShader_Metal()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CShader_Metal::init()
	{
		LOG_SCOPED_DEBUG("CShader_Metal::init()");

		// null VS を管理に登録
		{

		}

		// null PS を管理に登録
		{

		}
	}

	VertexShader::IDType CShader_Metal::createVSFromFile(const FilePathView path, StringView entryPoint)
	{
		return(VertexShader::IDType::Null());
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
		return(PixelShader::IDType::Null());
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
		//m_vertexShaders.erase(handleID);
	}

	void CShader_Metal::releasePS(const PixelShader::IDType handleID)
	{
		//m_pixelShaders.erase(handleID);
	}

	void CShader_Metal::setVS(const VertexShader::IDType handleID)
	{
		//m_context->VSSetShader(m_vertexShaders[handleID]->getShader(), nullptr, 0);
	}

	void CShader_Metal::setPS(const PixelShader::IDType handleID)
	{
		//m_context->PSSetShader(m_pixelShaders[handleID]->getShader(), nullptr, 0);
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
}
