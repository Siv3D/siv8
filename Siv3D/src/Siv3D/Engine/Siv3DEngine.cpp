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
# include <Siv3D/RegExp/IRegExp.hpp>
# include <Siv3D/System/ISystem.hpp>
# include <Siv3D/EngineResource/IEngineResource.hpp>
# include <Siv3D/Profiler/IProfiler.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Scene/IScene.hpp>
# include <Siv3D/ImageDecoder/IImageDecoder.hpp>
# include <Siv3D/ImageEncoder/IImageEncoder.hpp>
# include <Siv3D/Emoji/IEmoji.hpp>
# include <Siv3D/Console/IConsole.hpp>
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/Mouse/IMouse.hpp>
# include <Siv3D/Keyboard/IKeyboard.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Shader/IShader.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>

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

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Siv3DEngine::Siv3DEngine() noexcept
	{
		pEngine = this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Siv3DEngine::~Siv3DEngine()
	{
		ReleaseTupleReverse(m_components);	
		pEngine = nullptr;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNull
	//
	////////////////////////////////////////////////////////////////

	bool Siv3DEngine::isNull() noexcept
	{
		return (pEngine == nullptr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isAvailable
	//
	////////////////////////////////////////////////////////////////

	bool Siv3DEngine::isAvailable() noexcept
	{
		constexpr auto LastIndex = (std::tuple_size<decltype(m_components)>::value - 1);	
		return (pEngine && (std::get<LastIndex>(pEngine->m_components).get() != nullptr));
	}
}
