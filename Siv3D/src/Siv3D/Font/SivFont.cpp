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

# include <Siv3D/Font.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static void CheckEngine(const StringView type = U"Font")
		{
			if (Siv3DEngine::isNull())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, type);
				std::exit(EXIT_FAILURE);
			}
		}
	}

	template <>
	AssetIDWrapper<AssetHandle<Font>>::AssetIDWrapper()
	{
		CheckEngine();
	}

	template <>
	AssetIDWrapper<AssetHandle<Font>>::~AssetIDWrapper()
	{
		if (not Siv3DEngine::isAvailable())
		{
			return;
		}

		SIV3D_ENGINE(Font)->release(m_id);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Font::Font() {}

	Font::Font(const FontMethod fontMethod, const int32 baseSize, const FilePathView path, const size_t faceIndex, const StringView styleName, const FontStyle style)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Font)->create(path, faceIndex, styleName, fontMethod, baseSize, style))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Font::~Font() {}

	////////////////////////////////////////////////////////////////
	//
	//	properties
	//
	////////////////////////////////////////////////////////////////

	const FontFaceProperties& Font::properties() const
	{
		return SIV3D_ENGINE(Font)->getProperties(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void Font::swap(Font& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}

}
