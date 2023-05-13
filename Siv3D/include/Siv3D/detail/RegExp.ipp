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
	namespace Literals
	{
		namespace RegExpLiterals
		{
			inline RegExp operator ""_re(const char32_t* s, const size_t length) noexcept
			{
				return RegExp{ StringView{ s, length } };
			}
		}
	}
}
