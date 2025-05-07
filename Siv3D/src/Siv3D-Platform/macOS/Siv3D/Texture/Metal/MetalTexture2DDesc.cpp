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

# include "MetalTexture2DDesc.hpp"
# include <Siv3D/ImageProcessing.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array TextureTypeStrings =
		{
			U"Default"_sv,
			U"Dynamic"_sv,
			U"Render"_sv,
			U"MSRender"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	toString
	//
	////////////////////////////////////////////////////////////////

	String MetalTexture2DDesc::toString() const
	{
		const uint8 maxMipLevels = ImageProcessing::CalculateMipmapLevel(size.x, size.y);

		return U"({0}, {1}x{2}, {3}, Mips: {4}/{5}, RowPitch: {6}, HasDepth: {7})"_fmt(
			TextureTypeStrings[FromEnum(type)],	size.x, size.y, format.name(), mipLevels, maxMipLevels, format.bytesPerRow(size.x), (hasDepth ? U"Yes" : U"No"));
	}
}
