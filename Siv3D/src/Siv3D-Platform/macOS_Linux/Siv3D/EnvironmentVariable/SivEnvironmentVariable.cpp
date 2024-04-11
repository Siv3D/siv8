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

# include <cstdlib>
# include <Siv3D/EnvironmentVariable.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace EnvironmentVariable
	{
		String Get(const StringView name)
		{
			const char* value = std::getenv(Unicode::ToUTF8(name).c_str());

			if (not value)
			{
				return{};
			}

			return Unicode::FromUTF8(value);
		}
	}
}
