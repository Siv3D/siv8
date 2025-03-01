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

# include <memory>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Unicode.hpp>

# if __has_include(<cxxabi.h>)
#	define SIV3D_HAS_CXXABI
#	include <cxxabi.h>
# endif

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Demangle
	//
	////////////////////////////////////////////////////////////////

	String Demangle(const char* name)
	{
	# if defined(SIV3D_HAS_CXXABI)

		int status = 0;

		std::unique_ptr<char, void(*)(void*)> demangledName(
			abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);

		if (status == 0)
		{
			return Unicode::FromUTF8(demangledName.get());
		}
		else
		{
			return Unicode::FromUTF8(name); // デマングルに失敗した場合、元の名前を返す
		}

	# else

		return Unicode::FromUTF8(name);

	# endif
	}
			
	////////////////////////////////////////////////////////////////
	//
	//	DemangleUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string DemangleUTF8(const char* name)
	{
	# if defined(SIV3D_HAS_CXXABI)

		int status = 0;

		std::unique_ptr<char, void(*)(void*)> demangledName(
			abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);

		if (status == 0)
		{
			return demangledName.get();
		}
		else
		{
			return name; // デマングルに失敗した場合、元の名前を返す
		}

	# else

		return name;

	# endif
	}
}
