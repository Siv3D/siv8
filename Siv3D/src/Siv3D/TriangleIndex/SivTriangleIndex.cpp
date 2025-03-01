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

# include <Siv3D/TriangleIndex.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/IntFormatter.hpp>

namespace s3d
{
	void Formatter(FormatData& formatData, const TriangleIndex& value)
	{
		constexpr size_t BufferSize = ((detail::Uint32Width * 3) + 6 + 1);
		char32 buffer[BufferSize];
		char32* p = buffer;

		*(p++) = U'(';
		detail::AppendInt32(&p, value.i0);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.i1);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.i2);
		*(p++) = U')';

		formatData.string.append(buffer, (p - buffer));
	}
}
