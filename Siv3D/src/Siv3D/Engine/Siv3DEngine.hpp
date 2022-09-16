//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <tuple>
# include "Siv3DComponent.hpp"

namespace s3d
{
	class ISiv3DConsole;

	class Siv3DEngine
	{
	private:

		inline static Siv3DEngine* pEngine = nullptr;

		std::tuple<
			Siv3DComponent<ISiv3DConsole>> m_components;

	public:

		Siv3DEngine() noexcept;

		~Siv3DEngine();

		[[nodiscard]]
		static bool isActive() noexcept;

		template <class Interface>
		[[nodiscard]]
		static auto* Get() noexcept
		{
			return std::get<Siv3DComponent<Interface>>(pEngine->m_components).get();
		}
	};

	# define SIV3D_ENGINE(COMPONENT) Siv3DEngine::Get<ISiv3D##COMPONENT>()
}
