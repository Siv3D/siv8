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
# include "BasicTextEffect.hpp"

namespace s3d
{
	namespace TextEffect
	{
		class Mirror : public BasicTextEffect
		{
		public:

			Mirror() = default;

			explicit constexpr Mirror(const ColorF& color) noexcept;

			explicit constexpr Mirror(bool miror, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, const int32, double, double, bool) const override;

		private:

			bool m_mirror = true;
		};
	}
}

# include "Mirror.ipp"
