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

# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/Image.hpp>

# define STB_IMAGE_RESIZE_IMPLEMENTATION
# include <ThirdParty/stb/stb_image_resize2.h>

namespace s3d
{
	namespace ImageProcessing
	{
		[[nodiscard]]
		Image Resize(const Image& src, const Size& size)
		{
			Image dst{ size };

			stbir_resize(
				static_cast<const void*>(src.data()),
				src.width(), src.height(), 0,
				static_cast<void*>(dst.data()),
				dst.width(), dst.height(), 0,
				STBIR_RGBA_PM, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT);

			return dst;
		}
	}
}
