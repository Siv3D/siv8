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

# pragma once

namespace s3d
{
	namespace ImageEncoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		template <class ImageEncoder> requires std::derived_from<ImageEncoder, IImageEncoder>
		bool Add()
		{
			return Add(std::make_unique<ImageEncoder>());
		}
	}
}
