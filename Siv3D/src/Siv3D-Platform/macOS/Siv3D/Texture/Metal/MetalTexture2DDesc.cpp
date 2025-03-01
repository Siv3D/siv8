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

namespace s3d
{

	////////////////////////////////////////////////////////////////
	//
	//	toString
	//
	////////////////////////////////////////////////////////////////

	String MetalTexture2DDesc::toString() const
	{
		return U"(Type: Default, Size: {0}x{1}, MipLevels: {2}, Format: {3}, HasDepth: {4})"_fmt(
			size.x, size.y, mipLevels, format.name(), (hasDepth ? U"Yes" : U"No"));
	}
}
