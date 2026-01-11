//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
		/// @brief 何もしないテキスト描画エフェクト
		class NullTextEffect : public ITextEffect
		{
		public:

			constexpr void draw(const TextureRegion&, const GlyphContext&) const override {}
		};
	}
}
