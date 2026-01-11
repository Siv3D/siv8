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

# include <Siv3D/FrameMetrics.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Logger.hpp>
# include <Siv3D/Console.hpp>
# include <Siv3D/Print.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String FrameMetrics::format() const
	{
		String result;
		result += U"Frame [{}]\n"_fmt(frameIndex);
		result += U"Draw Calls: {}\n"_fmt(drawCalls);
		result += U"Triangle Count: {}\n"_fmt(triangleCount);
		result += U"Engine Begin Time: {} μs\n"_fmt(engineBeginTimeUs);
		result += U"User Update Time: {} μs\n"_fmt(userUpdateTimeUs);
		result += U"Engine End Time: {} μs\n"_fmt(engineEndTimeUs);
		result += U"GPU Wait Time: {} μs\n"_fmt(gpuWaitTimeUs);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const FrameMetrics& value)
	{
		formatData.string += value.format();
	}
}
