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

# include <Siv3D/TexturedRoundRect.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	TexturedRoundRect::TexturedRoundRect(const Texture& _texture, const float l, const float t, const float r, const float b, const RoundRect& _rect)
		: rect{ _rect }
		, texture{ _texture }
		, uvRect{ l, t, r, b } {}

	TexturedRoundRect::TexturedRoundRect(const Texture& _texture, const FloatRect& _uvRect, const RoundRect& _rect)
		: rect{ _rect }
		, texture{ _texture }
		, uvRect{ _uvRect } {}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& TexturedRoundRect::draw(const ColorF& diffuse) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedRoundRect(texture,
			FloatRect{ rect.x, rect.y, (rect.x + rect.w), (rect.y + rect.h) },
			static_cast<float>(rect.w),
			static_cast<float>(rect.h),
			Abs(static_cast<float>(rect.r)),
			uvRect, diffuse.toFloat4());
		return rect;
	}

	RoundRect TexturedRoundRect::draw(const double x, const double y, const ColorF& diffuse) const
	{
		const RoundRect rr = rect.movedBy(x, y);
		SIV3D_ENGINE(Renderer2D)->addTexturedRoundRect(texture,
			FloatRect{ rr.x, rr.y, (rr.x + rect.w), (rr.y + rect.h) },
			static_cast<float>(rect.w),
			static_cast<float>(rect.h),
			Abs(static_cast<float>(rect.r)),
			uvRect, diffuse.toFloat4());
		return rr;
	}

	RoundRect TexturedRoundRect::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RoundRect TexturedRoundRect::drawAt(const double x, const double y, const ColorF& diffuse) const
	{
		return draw((x - rect.w * 0.5), (y - rect.h * 0.5), diffuse);
	}

	RoundRect TexturedRoundRect::drawAt(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw((pos.x - rect.w * 0.5), (pos.y - rect.h * 0.5), diffuse);
	}
}
