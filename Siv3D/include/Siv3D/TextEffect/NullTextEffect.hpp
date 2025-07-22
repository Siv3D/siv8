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
# include "../ITextEffect.hpp"

namespace s3d
{
	namespace TextEffect
	{
		class NullTextEffect : public ITextEffect
		{
		public:

			constexpr void draw(const TextureRegion&, const Vec2&, int32, double, double, bool) const override {}
		};
	}
}
