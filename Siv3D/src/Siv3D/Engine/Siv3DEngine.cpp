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

# include "Siv3DEngine.hpp"
# include <Siv3D/Console/IConsole.hpp>

namespace s3d
{
	namespace detail
	{
		template <size_t I, class Tuple>
		static void Release(Tuple&& t)
		{
			std::get<I>(t).release();

			if constexpr (I > 0)
			{
				Release<I - 1>(t);
			}
		}

		template <class Tuple>
		static void ReleaseAll(Tuple&& t)
		{
			return Release<std::tuple_size_v<std::remove_reference_t<Tuple>> -1>(std::forward<Tuple>(t));
		}
	}

	Siv3DEngine::Siv3DEngine() noexcept
	{
		pEngine = this;
	}

	Siv3DEngine::~Siv3DEngine()
	{
		detail::ReleaseAll(m_components);
		
		pEngine = nullptr;
	}

	bool Siv3DEngine::isActive() noexcept
	{
		return (pEngine != nullptr);
	}
}
