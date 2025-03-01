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

# pragma once
# include "Common.hpp"
# include "FormatData.hpp"
# include "Format.hpp"

namespace s3d
{
	namespace Concept
	{
		template <class Type>
		concept Escapable = (
			std::same_as<std::decay_t<Type>, char> ||
			std::same_as<std::decay_t<Type>, char8_t> ||
			std::same_as<std::decay_t<Type>, char16_t> ||
			std::same_as<std::decay_t<Type>, wchar_t> ||
			std::same_as<std::decay_t<Type>, char32_t> ||
			std::same_as<std::decay_t<Type>, const char32_t*> ||
			std::same_as<std::decay_t<Type>, std::u32string> ||
			std::same_as<std::decay_t<Type>, String> ||
			std::same_as<std::decay_t<Type>, StringView>
			);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EscapedFormatter
	//
	////////////////////////////////////////////////////////////////

	void EscapedFormatter(FormatData& formatData, char ch);

	void EscapedFormatter(FormatData& formatData, char8_t ch);

	void EscapedFormatter(FormatData& formatData, char16_t ch);

	void EscapedFormatter(FormatData& formatData, wchar_t ch);

	void EscapedFormatter(FormatData& formatData, char32_t ch);

	void EscapedFormatter(FormatData& formatData, const char32_t* s);

	void EscapedFormatter(FormatData& formatData, const std::u32string& s);

	void EscapedFormatter(FormatData& formatData, const String& s);

	void EscapedFormatter(FormatData& formatData, StringView s);

	template <Concept::Formattable Type>
	void DebugFormatter(FormatData& formatData, const Type& value);
}

# include "detail/EscapedFormatter.ipp"
