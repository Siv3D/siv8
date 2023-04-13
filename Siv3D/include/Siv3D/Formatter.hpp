//-----------------------------------------------
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
# include <compare>
# include "FormatData.hpp"

namespace s3d
{
	void Formatter(FormatData& formatData, bool value);

	void Formatter(FormatData& formatData, int8 value);

	void Formatter(FormatData& formatData, uint8 value);

	void Formatter(FormatData& formatData, int16 value);

	void Formatter(FormatData& formatData, uint16 value);

	void Formatter(FormatData& formatData, int32 value);

	void Formatter(FormatData& formatData, uint32 value);

	void Formatter(FormatData& formatData, long value);

	void Formatter(FormatData& formatData, unsigned long value);

	void Formatter(FormatData& formatData, long long value);

	void Formatter(FormatData& formatData, unsigned long long value);

	void Formatter(FormatData& formatData, float value);

	void Formatter(FormatData& formatData, double value);

	void Formatter(FormatData& formatData, long double value);

	void Formatter(FormatData& formatData, char ch);

	void Formatter(FormatData& formatData, char8_t ch);

	void Formatter(FormatData& formatData, char16_t ch);

	void Formatter(FormatData& formatData, wchar_t ch);

	void Formatter(FormatData& formatData, char32_t ch);

	void Formatter(FormatData& formatData, std::nullptr_t);

	void Formatter(FormatData& formatData, const void* value);

	void Formatter(FormatData& formatData, const char*) = delete;

	void Formatter(FormatData& formatData, const char8_t*) = delete;

	void Formatter(FormatData& formatData, const char16_t*) = delete;

	void Formatter(FormatData& formatData, const wchar_t*) = delete;

	void Formatter(FormatData& formatData, const char32_t*);

	void Formatter(FormatData& formatData, const std::u32string& s);

	void Formatter(FormatData& formatData, std::strong_ordering value);

	void Formatter(FormatData& formatData, std::weak_ordering value);

	void Formatter(FormatData& formatData, std::partial_ordering value);



	//template <class Fitrst, class Second>
	//inline void Formatter(FormatData& formatData, const std::pair<Fitrst, Second>& pair);

	namespace Concept
	{
		template <class Type>
		concept Formattable = requires (FormatData& f, Type x) { Formatter(f, x); };
	}
}
