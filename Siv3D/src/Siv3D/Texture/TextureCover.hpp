//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/Anchor.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/RectF.hpp>
# include <Siv3D/Utility.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d::detail
{
	inline RectF DrawTextureCover(const Texture& texture, const double width, const double height,
		const FloatRect& uvRect, const RectF& rect, const Anchor anchor, const ColorF& diffuse)
	{
		if ((width == 0) || (height == 0) || (rect.w <= 0) || (rect.h <= 0))
		{
			return{ rect.pos, 0, 0 };
		}

		// A positive destination keeps the visible orientation of signed source sizes.
		FloatRect uv{
			((width < 0) ? uvRect.right : uvRect.left),
			((height < 0) ? uvRect.bottom : uvRect.top),
			((width < 0) ? uvRect.left : uvRect.right),
			((height < 0) ? uvRect.top : uvRect.bottom) };
		const double scaleX = (rect.w / Abs(width));
		const double scaleY = (rect.h / Abs(height));

		if (scaleX < scaleY)
		{
			const double crop = ((static_cast<double>(uv.right) - uv.left) * (1.0 - scaleX / scaleY));
			const double alignment = (anchor.isLeft() ? 0.0 : (anchor.isRight() ? 1.0 : 0.5));
			uv.left = static_cast<float>(uv.left + crop * alignment);
			uv.right = static_cast<float>(uv.right - crop * (1.0 - alignment));
		}
		else if (scaleY < scaleX)
		{
			const double crop = ((static_cast<double>(uv.bottom) - uv.top) * (1.0 - scaleY / scaleX));
			const double alignment = (anchor.isTop() ? 0.0 : (anchor.isBottom() ? 1.0 : 0.5));
			uv.top = static_cast<float>(uv.top + crop * alignment);
			uv.bottom = static_cast<float>(uv.bottom - crop * (1.0 - alignment));
		}

		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture,
			FloatQuad{ rect.tl(), rect.tr(), rect.br(), rect.bl() }, uv, diffuse.toFloat4());
		return rect;
	}
}
