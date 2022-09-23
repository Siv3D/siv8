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

# include <Siv3D/ParseBool.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Error.hpp>

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		void ThrowParseBoolError(const StringView s)
		{
			throw ParseError{ U"ParseBool(\"" + s + U"\") failed" };
		}
	}
}
