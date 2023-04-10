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

namespace s3d
{
	//namespace detail
	//{
	//	template <Concept::Formattable... Args>
	//	inline void Console_impl::write(const Args&... args) const
	//	{
	//		return write(Format(args...));
	//	}

	//	template <Concept::Formattable... Args>
	//	inline void Console_impl::writeln(const Args&... args) const
	//	{
	//		return write(Format(args..., U'\n'));
	//	}

	//	template <Concept::Formattable... Args>
	//	inline void Console_impl::operator ()(const Args&... args) const
	//	{
	//		return writeln(args...);
	//	}

	//	template <Concept::Formattable Formattable>
	//	inline ConsoleBuffer Console_impl::operator <<(const Formattable& value) const
	//	{
	//		ConsoleBuffer buf;
	//		Formatter(*buf.formatData, value);
	//		return buf;
	//	}

	//	template <class Type>
	//	inline Type Console_impl::read() const
	//	{
	//		open();
	//		Type t{};
	//		std::cin >> t;
	//		return t;
	//	}

	//	//template <class Type>
	//	//inline Type Console_impl::readLine() const
	//	//{
	//	//	open();
	//	//	std::string s;
	//	//	std::getline(std::cin, s);
	//	//	return Parse<Type>(Unicode::Widen(s));
	//	//}

	//	template <class Type>
	//	const Console_impl& Console_impl::operator >>(Type& value) const
	//	{
	//		value = read<Type>();
	//		return *this;
	//	}
	//}
}
