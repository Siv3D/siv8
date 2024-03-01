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

# include <Siv3D/Common.hpp>
# include <Siv3D/String.hpp>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4100)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4127)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4267)
# include <ThirdParty/levenshtein-sse/levenshtein-sse.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	String::size_type String::levenshteinDistanceFrom(const StringView other) const noexcept
	{
		return levenshteinSSE::levenshtein(m_string, other.view());
	}
}
