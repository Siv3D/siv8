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

# include <Siv3D/Parse.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/Error/ParseError.hpp>

namespace s3d
{
	namespace detail
	{
		void ThrowParseError(const char* type, const std::string_view s)
		{
			throw ParseError{ fmt::format("Parse<{}>(\"{}\") failed", DemangleUTF8(type), s) };
		}
	}
}
