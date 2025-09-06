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

# include <Siv3D/Print.hpp>
# include <Siv3D/Print/IPrint.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		void Print_impl::write(const char32_t* s) const
		{
			SIV3D_ENGINE(Print)->write(s);
		}

		void Print_impl::write(const StringView s) const
		{
			SIV3D_ENGINE(Print)->write(String{ s });
		}

		void Print_impl::write(const String& s) const
		{
			SIV3D_ENGINE(Print)->write(s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	writeln
		//
		////////////////////////////////////////////////////////////////

		void Print_impl::writeln(const char32_t* s) const
		{
			SIV3D_ENGINE(Print)->writeln(s);
		}

		void Print_impl::writeln(const StringView s) const
		{
			SIV3D_ENGINE(Print)->writeln(String{ s });
		}

		void Print_impl::writeln(const String& s) const
		{
			SIV3D_ENGINE(Print)->writeln(s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		void Print_impl::operator ()(const char32_t* s) const
		{
			writeln(s);
		}

		void Print_impl::operator ()(const StringView s) const
		{
			writeln(s);
		}

		void Print_impl::operator ()(const String& s) const
		{
			writeln(s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		PrintBuffer Print_impl::operator <<(const char32* s) const
		{
			return PrintBuffer{ s };
		}

		PrintBuffer Print_impl::operator <<(const StringView s) const
		{
			return PrintBuffer{ s };
		}

		PrintBuffer Print_impl::operator <<(const String& s) const
		{
			return PrintBuffer{ s };
		}

		PrintBuffer Print_impl::operator <<(String&& s) const
		{
			return PrintBuffer{ std::move(s) };
		}
	}

	void ClearPrint()
	{
		SIV3D_ENGINE(Print)->clear();
	}
}
