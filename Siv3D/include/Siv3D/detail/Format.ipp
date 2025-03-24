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

namespace s3d
{
	namespace detail
	{		
		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		template <Concept::Formattable... Args>
		inline String Format_impl::operator ()(const Args&... args)
		{
			FormatData formatData;
			Apply(formatData, args...);
			return std::move(formatData.string);
		}

		inline String Format_impl::operator ()(const char32 ch)
		{
			return String(1, ch);
		}

		inline String Format_impl::operator ()(const char32* s) noexcept
		{
			return String{ s };
		}

		inline String Format_impl::operator ()(const StringView s) noexcept
		{
			return String{ s };
		}

		inline String Format_impl::operator ()(const String& s)
		{
			return s;
		}

		inline String Format_impl::operator ()(String&& s) noexcept
		{
			return std::move(s);
		}

		constexpr void Format_impl::Apply(const FormatData&) noexcept {}

		template <class Type, class... Args>
		inline void Format_impl::Apply(FormatData& formatData, const Type& value, const Args&... args)
		{
			Formatter(formatData, value);
			Apply(formatData, args...);
		}
	}
}
