//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <iostream>
# include <Siv3D/Console.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Console/IConsole.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace detail
	{
		void Console_impl::open() const
		{
			SIV3D_ENGINE(Console)->open();
		}

		void Console_impl::write(const char32_t* s) const
		{
			write(StringView{ s });
		}

		void Console_impl::write(const StringView s) const
		{
			open();

			std::cout << Unicode::Narrow(s);
		}

		void Console_impl::write(const String& s) const
		{
			write(StringView{ s });
		}

		void Console_impl::writeln(const char32_t* s) const
		{
			writeln(StringView{ s });
		}

		void Console_impl::writeln(const StringView s) const
		{
			write(s);

			std::cout << std::endl;
		}

		void Console_impl::writeln(const String& s) const
		{
			writeln(StringView{ s });
		}

		void Console_impl::operator()(const char32_t* s) const
		{
			writeln(s);
		}

		void Console_impl::operator()(const StringView s) const
		{
			writeln(s);
		}

		void Console_impl::operator()(const String& s) const
		{
			writeln(s);
		}
		
		ConsoleBuffer Console_impl::operator <<(const char32* s) const
		{
			return ConsoleBuffer{ s };
		}

		ConsoleBuffer Console_impl::operator <<(const StringView s) const
		{
			return ConsoleBuffer{ s };
		}

		ConsoleBuffer Console_impl::operator <<(const String& s) const
		{
			return ConsoleBuffer{ s };
		}

		ConsoleBuffer Console_impl::operator <<(String&& s) const
		{
			return ConsoleBuffer{ std::move(s) };
		}

		void Console_impl::setSystemDefaultCodePage() const
		{
			SIV3D_ENGINE(Console)->setSystemDefaultCodePage();
		}

		void Console_impl::setUTF8CodePage() const
		{
			SIV3D_ENGINE(Console)->setUTF8CodePage();
		}
	}
}
