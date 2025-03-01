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
		//	writeln
		//
		////////////////////////////////////////////////////////////////

		template <Concept::Formattable... Args>
		void Logger_impl::writeln(const Args&... args) const
		{
			return writeln(Format(args...));
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		template <Concept::Formattable... Args>
		void Logger_impl::operator ()(const Args&... args) const
		{
			return writeln(args...);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		LoggerBuffer Logger_impl::operator <<(const Concept::Formattable auto& value) const
		{
			LoggerBuffer buf;
			Formatter(*buf.formatData, value);
			return buf;
		}
	}
}
