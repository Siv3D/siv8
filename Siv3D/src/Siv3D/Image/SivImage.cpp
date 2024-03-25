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

# include <Siv3D/Image.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	void Image::fill(const Color color) noexcept
	{
		std::fill(m_pixels.begin(), m_pixels.end(), color); // 明示的に SIMD にしなくても早いコードが生成される
	}
}
