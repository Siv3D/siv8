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
# include "TextureRegion.hpp"

namespace s3d
{
	struct GlyphContext;

	struct ITextEffect
	{
		virtual ~ITextEffect() = default;

		virtual void draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const = 0;
	};
}
