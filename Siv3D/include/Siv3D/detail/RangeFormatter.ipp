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

# pragma once

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Concept::FormattableRange auto& range)
	{
		formatData.string.push_back(U'[');

		auto it = std::ranges::begin(range);
		const auto end = std::ranges::end(range);
		bool isFirst = true;

		while (it != end)
		{
			if (isFirst)
			{
				isFirst = false;
			}
			else
			{
				formatData.string.append(U", "_sv);
			}

			DebugFormatter(formatData, *it);

			++it;
		}

		formatData.string.push_back(U']');
	}
}
