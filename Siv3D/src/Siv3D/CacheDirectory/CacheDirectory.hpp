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

namespace s3d
{
	namespace CacheDirectory
	{
		[[nodiscard]]
		FilePath Engine();

		[[nodiscard]]
		FilePath Apps(StringView applicationName);
	}
}
