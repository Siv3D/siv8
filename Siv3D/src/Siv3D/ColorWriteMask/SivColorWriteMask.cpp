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

# include <Siv3D/ColorWriteMask.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const ColorWriteMask value)
	{
		if (value & ColorWriteMask::Red)
		{
			formatData.string.push_back(U'R');
		}

		if (value & ColorWriteMask::Green)
		{
			formatData.string.push_back(U'G');
		}

		if (value & ColorWriteMask::Blue)
		{
			formatData.string.push_back(U'B');
		}

		if (value & ColorWriteMask::Alpha)
		{
			formatData.string.push_back(U'A');
		}
	}
}
