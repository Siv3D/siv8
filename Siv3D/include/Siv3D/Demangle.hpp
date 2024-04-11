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
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Demangle
	//
	////////////////////////////////////////////////////////////////

	/// @brief C++ のシンボル名をデマングルします。 | Demangle a C++ symbol name
	/// @param name デマングルするシンボル名 | The symbol name to demangle
	/// @return デマングルされた文字列 | The demangled string
	[[nodiscard]]
	String Demangle(const char* name);

	////////////////////////////////////////////////////////////////
	//
	//	DemangleUTF8
	//
	////////////////////////////////////////////////////////////////

	/// @brief C++ のシンボル名をデマングルします。 | Demangle a C++ symbol name
	/// @param name デマングルするシンボル名 | The symbol name to demangle
	/// @return デマングルされた文字列 | The demangled string
	[[nodiscard]]
	std::string DemangleUTF8(const char* name);
}
