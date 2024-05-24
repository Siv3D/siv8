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

# include <Siv3D/PixelShader.hpp>
# include <Siv3D/Shader/IShader.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static void CheckEngine()
		{
			if (not Siv3DEngine::isAvailable())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, U"PixelShader");
				std::exit(EXIT_FAILURE);
			}
		}
	}

	template <>
	AssetIDWrapper<AssetHandle<PixelShader>>::AssetIDWrapper()
	{
		CheckEngine();
	}

	template <>
	AssetIDWrapper<AssetHandle<PixelShader>>::~AssetIDWrapper()
	{
		if (not Siv3DEngine::isAvailable())
		{
			return;
		}

		SIV3D_ENGINE(Shader)->releasePS(m_id);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	PixelShader::PixelShader() {}

	PixelShader::PixelShader(const FilePathView path, const StringView entryPoint)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Shader)->createPSFromFile(path, entryPoint))) }
	{
		//SIV3D_ENGINE(AssetMonitor)->created();
	}

	PixelShader::PixelShader(const Blob& bytecode)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Shader)->createPSFromBytecode(bytecode))) }
	{
		//SIV3D_ENGINE(AssetMonitor)->created();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	PixelShader::~PixelShader() {}

	////////////////////////////////////////////////////////////////
	//
	//	getBytecode
	//
	////////////////////////////////////////////////////////////////

	const Blob& PixelShader::getBytecode() const noexcept
	{
		return SIV3D_ENGINE(Shader)->getBytecodePS(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void PixelShader::swap(PixelShader& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	HLSL
	//
	////////////////////////////////////////////////////////////////

	PixelShader PixelShader::HLSL(const FilePathView path, const StringView entryPoint)
	{
		//if (System::GetRendererType() != EngineOption::Renderer::Direct3D11)
		//{
		//	throw Error{ U"HLSL must be used with EngineOption::Renderer::Direct3D11" };
		//}

		return PixelShader{ path, entryPoint };
	}

	PixelShader PixelShader::HLSL(const Blob& bytecode)
	{
		//if (System::GetRendererType() != EngineOption::Renderer::Direct3D11)
		//{
		//	throw Error{ U"HLSL must be used with EngineOption::Renderer::Direct3D11" };
		//}

		return PixelShader{ bytecode };
	}

	////////////////////////////////////////////////////////////////
	//
	//	MSL
	//
	////////////////////////////////////////////////////////////////

	PixelShader PixelShader::MSL(const StringView entryPoint, const FilePathView path)
	{
		//if (System::GetRendererType() != EngineOption::Renderer::Metal)
		//{
		//	throw Error{ U"MSL must be used with EngineOption::Renderer::Metal" };
		//}

		return PixelShader{ path, entryPoint };
	}
}
