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

# pragma once

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		template <Concept::Formattable... Args>
		void Console_impl::write(const Args&... args) const
		{
			return write(Format(args...));
		}

		////////////////////////////////////////////////////////////////
		//
		//	writeln
		//
		////////////////////////////////////////////////////////////////

		template <Concept::Formattable... Args>
		void Console_impl::writeln(const Args&... args) const
		{
			return write(Format(args..., U'\n'));
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		template <Concept::Formattable... Args>
		void Console_impl::operator ()(const Args&... args) const
		{
			return writeln(args...);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		ConsoleBuffer Console_impl::operator <<(const Concept::Formattable auto& value) const
		{
			ConsoleBuffer buf;
			Formatter(*buf.formatData, value);
			return buf;
		}

		////////////////////////////////////////////////////////////////
		//
		//	read
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		Type Console_impl::read() const
		{
			open();

			Type t;
			std::cin >> t;

			return t;
		}

		////////////////////////////////////////////////////////////////
		//
		//	readLine
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		Type Console_impl::readLine() const
		{
			open();

			std::string s;
			std::getline(std::cin, s);

			return Parse<Type>(s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class Type>
		const Console_impl& Console_impl::operator >>(Type& value) const
		{
			value = read<Type>();
			return *this;
		}
	}
}
