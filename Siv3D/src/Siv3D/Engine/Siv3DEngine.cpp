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

# include "Siv3DEngine.hpp"
# include <Siv3D/LicenseManager/ILicenseManager.hpp>
# include <Siv3D/Logger/ILogger.hpp>
# include <Siv3D/System/ISystem.hpp>
# include <Siv3D/EngineResource/IEngineResource.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/ImageDecoder/IImageDecoder.hpp>
# include <Siv3D/ImageEncoder/IImageEncoder.hpp>
# include <Siv3D/Emoji/IEmoji.hpp>
# include <Siv3D/Console/IConsole.hpp>
# include <Siv3D/Cursor/ICursor.hpp>

namespace s3d
{
	namespace
	{
		template <class Tuple, size_t Index = (std::tuple_size_v<Tuple> -1)>
		void ReleaseTupleReverse(Tuple& tuple)
		{
			if constexpr (Index != static_cast<size_t>(-1))
			{
				std::get<Index>(tuple).release();
				ReleaseTupleReverse<Tuple, (Index - 1)>(tuple);
			}
		}
	}

	Siv3DEngine::Siv3DEngine() noexcept
	{
		pEngine = this;
	}

	Siv3DEngine::~Siv3DEngine()
	{
		ReleaseTupleReverse(m_components);	
		pEngine = nullptr;
	}

	bool Siv3DEngine::isNotNull() noexcept
	{
		return (pEngine != nullptr);
	}

	bool Siv3DEngine::isAvailable() noexcept
	{
		constexpr auto LastIndex = (std::tuple_size<decltype(m_components)>::value - 1);	
		return (std::get<10>(pEngine->m_components).get() != nullptr);
	}
}
