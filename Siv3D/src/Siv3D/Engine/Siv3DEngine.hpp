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

# pragma once
# include <tuple>
# include "Siv3DComponent.hpp"

namespace s3d
{
	class ISiv3DLicenseManager;
	class ISiv3DLogger;
	class ISiv3DRegExp;
	class ISiv3DSystem;
	class ISiv3DEngineResource;
	class ISiv3DProfiler;
	class ISiv3DUserAction;
	class ISiv3DWindow;
	class ISiv3DScene;
	class ISiv3DImageDecoder;
	class ISiv3DImageEncoder;
	class ISiv3DEmoji;
	class ISiv3DConsole;
	class ISiv3DCursor;
	class ISiv3DCursorStyle;
	class ISiv3DKeyboard;
	class ISiv3DMouse;
	class ISiv3DRenderer;
	class ISiv3DShader;
	class ISiv3DEngineShader;
	class ISiv3DRenderer2D;
	class ISiv3DScreenCapture;

	class Siv3DEngine
	{
	private:

		inline static Siv3DEngine* pEngine = nullptr;

		std::tuple<
			Siv3DComponent<ISiv3DLicenseManager>,
			Siv3DComponent<ISiv3DLogger>,
			Siv3DComponent<ISiv3DRegExp>,
			Siv3DComponent<ISiv3DSystem>,
			Siv3DComponent<ISiv3DEngineResource>,
			Siv3DComponent<ISiv3DProfiler>,
			Siv3DComponent<ISiv3DUserAction>,
			Siv3DComponent<ISiv3DWindow>,
			Siv3DComponent<ISiv3DScene>,
			Siv3DComponent<ISiv3DImageDecoder>,
			Siv3DComponent<ISiv3DImageEncoder>,
			Siv3DComponent<ISiv3DEmoji>,
			Siv3DComponent<ISiv3DConsole>,
			Siv3DComponent<ISiv3DCursor>,
			Siv3DComponent<ISiv3DCursorStyle>,
			Siv3DComponent<ISiv3DKeyboard>,
			Siv3DComponent<ISiv3DMouse>,
			Siv3DComponent<ISiv3DRenderer>,
			Siv3DComponent<ISiv3DShader>,
			Siv3DComponent<ISiv3DEngineShader>,
			Siv3DComponent<ISiv3DRenderer2D>,
			Siv3DComponent<ISiv3DScreenCapture>
		> m_components;

	public:

		Siv3DEngine() noexcept;

		~Siv3DEngine();

		[[nodiscard]]
		static bool isNull() noexcept;

		[[nodiscard]]
		static bool isAvailable() noexcept;

		template <class Interface>
		[[nodiscard]]
		static auto* Get() noexcept
		{
			return std::get<Siv3DComponent<Interface>>(pEngine->m_components).get();
		}
	};

	# define SIV3D_ENGINE(COMPONENT) Siv3DEngine::Get<ISiv3D##COMPONENT>()
}
