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

# include "CTexture_D3D11.hpp"
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static std::array<bool, CTexture_D3D11::RenderTextureFormatCount> CheckMultiSampleSupport(ID3D11Device* device)
		{
			static constexpr std::array<TextureFormat, CTexture_D3D11::RenderTextureFormatCount> formats =
			{
				TextureFormat::Unknown,
				TextureFormat::R8_Unorm,
				TextureFormat::R8G8_Unorm,
				TextureFormat::R16_Float,
				TextureFormat::R8G8B8A8_Unorm,
				TextureFormat::R8G8B8A8_Unorm_SRGB,
				TextureFormat::R16G16_Unorm,
				TextureFormat::R16G16_Float,
				TextureFormat::R32_Float,
				TextureFormat::R10G10B10A2_Unorm,
				TextureFormat::R11G11B10_UFloat,
				TextureFormat::R16G16B16A16_Float,
				TextureFormat::R32G32_Float,
				TextureFormat::R32G32B32A32_Float,
			};

			std::array<bool, CTexture_D3D11::RenderTextureFormatCount> multiSampleSupportTable{};

			std::string logText = "4x MSAA support:\n";

			for (size_t i = 1; i < formats.size(); ++i)
			{
				const StringView name = formats[i].name();
				const DXGI_FORMAT format = DXGI_FORMAT(formats[i].DXGIFormat());

				if (UINT quality = 0; SUCCEEDED(device->CheckMultisampleQualityLevels(format, 4, &quality)) && (0 < quality))
				{
					multiSampleSupportTable[i] = true;
					logText += fmt::format("{} ✔\n", name);
				}
				else
				{
					logText += fmt::format("{} ✘\n", name);
				}
			}

			logText.pop_back();

			LOG_INFO(logText);

			return multiSampleSupportTable;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CTexture_D3D11::CTexture_D3D11() {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CTexture_D3D11::~CTexture_D3D11()
	{
		LOG_SCOPED_DEBUG("CTexture_D3D11::~CTexture_D3D11()");

		m_textures.destroy();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CTexture_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CTexture_D3D11::init()");

		m_pRenderer	= static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));
		m_device	= m_pRenderer->getDevice().getDevice();
		m_context	= m_pRenderer->getDevice().getContext();

		// 4x MSAA サポート状況を取得
		m_multiSampleSupportTable = CheckMultiSampleSupport(m_device);

		// null Texture を管理に登録
		{
			const Image image{ 16, Palette::Yellow };
			const Array<Image> mipmaps = {
				Image{ 8, Palette::Yellow }, Image{ 4, Palette::Yellow },
				Image{ 2, Palette::Yellow }, Image{ 1, Palette::Yellow }
			};

			// null Texture を作成
			auto nullTexture = std::make_unique<D3D11Texture>(m_device, image, mipmaps, TextureDesc::Mipmap);

			if (not nullTexture->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null texture" };
			}

			// 管理に登録
			m_textures.setNullData(std::move(nullTexture));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	create
	//
	////////////////////////////////////////////////////////////////

	Texture::IDType CTexture_D3D11::create(const Image& image, const TextureDesc desc)
	{
		if (not image)
		{
			return Texture::IDType::Null();
		}

		std::unique_ptr<D3D11Texture> texture;

		if ((not desc.hasMipmap) || (image.size() == Size{ 1, 1 }))
		{
			texture = std::make_unique<D3D11Texture>(D3D11Texture::NoMipmap{}, m_device, image, desc);
		}
		else
		{
			texture = std::make_unique<D3D11Texture>(D3D11Texture::GenerateMipmap{}, m_device, m_context, image, desc);
		}

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_D3D11::create(const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
	{
		if (not image)
		{
			return Texture::IDType::Null();
		}

		auto texture = std::make_unique<D3D11Texture>(m_device, image, mipmaps, desc);

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	void CTexture_D3D11::release(const Texture::IDType handleID)
	{
		m_textures.erase(handleID);
	}
}
