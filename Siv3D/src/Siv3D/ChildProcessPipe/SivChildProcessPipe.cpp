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

# include <array>
# include <Siv3D/ChildProcessPipe.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array ChildProcessPipeStrings =
		{
			U"None"_sv,
			U"StdIn"_sv,
			U"StdOut"_sv,
			U"StdInOut"_sv,
		};
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const ChildProcessPipe value)
	{
		formatData.string.append(ChildProcessPipeStrings[FromEnum(value)]);
	}
}
