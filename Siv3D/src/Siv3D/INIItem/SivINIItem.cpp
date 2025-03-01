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

# include <Siv3D/INIItem.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Demangle.hpp>

namespace s3d
{
	namespace detail
	{
		void ThrowINIItemParseError(const char* type, const StringView value)
		{
			throw Error{ fmt::format("INIItem::get<{}>(): Failed to parse value `{}`", DemangleUTF8(type), value) };
		}
	}
}
