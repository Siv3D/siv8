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
# include "Common.hpp"
# include "Blob.hpp"
# include "TextureFormat.hpp"

namespace s3d
{
	struct BCnData
	{
		Size size{ 0, 0 };

		TextureFormat format = TextureFormat::Unknown;

		Array<Blob> textures;

		[[nodiscard]]
		bool isEmpty() const noexcept;

		[[nodiscard]]
		explicit operator bool() const noexcept;
	};
}

# include "detail/BCnData.ipp"
