﻿//-----------------------------------------------
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
	class String;

	/// @brief C++ のシンボル名をデマングルします。 | Demangle a C++ symbol name
	/// @param name デマングルするシンボル名 | The symbol name to demangle
	/// @return デマングルされた文字列 | The demangled string
	[[nodiscard]]
	String Demangle(const char* name);
}
