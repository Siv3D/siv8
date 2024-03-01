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

# include <Siv3D/Rect.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/IntFormatter.hpp>

namespace s3d
{
	void Formatter(FormatData& formatData, const Rect& value)
	{
		constexpr size_t BufferSize = ((detail::Int32Width * 4) + 8 + 1);
		char32 buffer[BufferSize];
		char32* p = buffer;

		*(p++) = U'(';
		detail::AppendInt32(&p, value.x);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.y);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.w);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.h);
		*(p++) = U')';

		formatData.string.append(buffer, (p - buffer));
	}
}
