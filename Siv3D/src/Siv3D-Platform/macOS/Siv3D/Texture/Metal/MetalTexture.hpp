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
# include <Siv3D/Common.hpp>
# include <Siv3D/Image.hpp>
# include "MetalTexture2DDesc.hpp"

namespace s3d
{
	class MetalTexture
	{
	public:

		struct NoMipmap {};

		struct GenerateMipmap {};

		[[nodiscard]]
		MetalTexture(const Image& image, const Array<Image>& mipmaps, TextureDesc desc);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		const MetalTexture2DDesc& getDesc() const noexcept;

	private:

		MetalTexture2DDesc m_desc;

		bool m_initialized = false;
	};
}
