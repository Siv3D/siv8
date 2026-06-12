//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/INIItem.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace detail
	{
		void ThrowINIItemParseError(const char* type, const StringView value)
		{
			throw Error{ fmt::format("INIItem::get<{}>(): Failed to parse value `{}`", DemangleUTF8(type), value.toUTF8()) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getString
	//
	////////////////////////////////////////////////////////////////

	const String& INIItem::getString() const noexcept
	{
		return value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string INIItem::getUTF8() const
	{
		return Unicode::ToUTF8(value);
	}
}
