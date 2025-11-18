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

# include "CNativeShare.hpp"
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CNativeShare::~CNativeShare()
	{
		LOG_SCOPED_DEBUG("CNativeShare::~CNativeShare()");

		if (FileSystem::Exists(m_imagePath))
		{
			FileSystem::Remove(m_imagePath);
		}
	}

	void CNativeShare::init()
	{
		LOG_SCOPED_DEBUG("CNativeShare::init()");

		m_imagePath = (FileSystem::TemporaryDirectoryPath() + U"Siv3D/NativeShare/image.png");
	}

	bool CNativeShare::show(const Image& image)
	{
		if (FileSystem::Exists(m_imagePath))
		{
			FileSystem::Remove(m_imagePath);
		}

		if (not image.save(m_imagePath))
		{
			return false;
		}

		return(false);
	}
}
