﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <cassert>

namespace s3d
{
	template <class Interface>
	class Siv3DComponent
	{
	public:

		~Siv3DComponent()
		{
			// must be released prior to the destructor executing
			assert(pInterface == nullptr);
		}

		[[nodiscard]]
		Interface* get() noexcept
		{
			return pInterface;
		}

		void release()
		{
			delete pInterface;
			pInterface = nullptr;
		}

	private:

		Interface* pInterface = Interface::Create();
	};
}
