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
//	Authors (OpenSiv3D challenge #20)
//	- Rac75116
//	- ラクラムシ
//	- Nachia
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "detail/IsPrime_impl.ipp"

namespace s3d
{
	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	IsPrime
		//
		////////////////////////////////////////////////////////////////

		inline constexpr auto IsPrime = detail::IsPrime_impl{};
	}
}
