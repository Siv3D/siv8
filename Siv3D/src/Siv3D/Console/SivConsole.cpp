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

# include <Siv3D/Console.hpp>
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

		void Console_impl::write(const char32* s) const
		{
			write(StringView{ s });
		}

		void Console_impl::write(const StringView s) const
		{
			open();

			std::cout << s.toUTF8();
		}

		void Console_impl::write(const String& s) const
		{
			write(StringView{ s });
		}

		void Console_impl::writeln(const char32* s) const
		{
			writeln(StringView{ s });
		}

		void Console_impl::writeln(const StringView s) const
		{
			write(s + U'\n');
		}

		void Console_impl::writeln(const String& s) const
		{
			writeln(StringView{ s });
		}

		void Console_impl::operator()(const char32* s) const
		{
			writeln(StringView{ s });
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
	}
}
