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

# include <Siv3D/Console.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/Console/IConsole.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		void Console_impl::open() const
		{
			SIV3D_ENGINE(Console)->open();
		}

		////////////////////////////////////////////////////////////////
		//
		//	attach
		//
		////////////////////////////////////////////////////////////////

		bool Console_impl::attach() const
		{
			return SIV3D_ENGINE(Console)->attach();
		}

		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		void Console_impl::write(const char32* s) const
		{
			write(StringView{ s });
		}

		void Console_impl::write(const StringView s) const
		{
			open();

			std::cout << Unicode::ToUTF8(s);
		}

		void Console_impl::write(const String& s) const
		{
			write(StringView{ s });
		}

		////////////////////////////////////////////////////////////////
		//
		//	writeln
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

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
