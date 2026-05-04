//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/TOMLParseErrorReason.hpp>

namespace s3d
{
	String TOMLParseErrorReason::format() const
	{
		String result = title;
	
		for (const auto& location : locations)
		{
			if (result)
			{
				result.push_back(U'\n');
			}

			result.append(location);
		}
		
		if (hint)
		{
			if (result)
			{
				result.push_back(U'\n');
			}

			result.append(hint);
		}
		
		return result;
	}
}
