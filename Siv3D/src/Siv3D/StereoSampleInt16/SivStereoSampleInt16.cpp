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

# include <Siv3D/WaveSample.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/IntFormatter.hpp>

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const StereoSampleInt16 value)
	{
		constexpr size_t BufferSize = ((detail::Int32Width * 2) + 4 + 1);
		char32 buffer[BufferSize];
		char32* p = buffer;

		*(p++) = U'(';
		detail::AppendInt32(&p, value.left);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.right);
		*(p++) = U')';

		formatData.string.append(buffer, (p - buffer));
	}
}
