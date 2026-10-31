//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/RectF.hpp>
# include <Siv3D/MinMax.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d::detail
{
	inline RectF DrawTextureClipped(const Texture& texture, const double x, const double y,
		const double width, const double height, const FloatRect& uvRect,
		const RectF& clipRect, const ColorF& diffuse)
	{
		const double endX = (x + width);
		const double endY = (y + height);
		const double left = Max(Min(x, endX), clipRect.x);
		const double top = Max(Min(y, endY), clipRect.y);
		const double right = Min(Max(x, endX), clipRect.rightX());
		const double bottom = Min(Max(y, endY), clipRect.bottomY());

		if ((right <= left) || (bottom <= top))
		{
			return{ left, top, 0, 0 };
		}

		// Preserve the original vertex order, including negative drawing sizes.
		const double x0 = ((width < 0) ? right : left);
		const double x1 = ((width < 0) ? left : right);
		const double y0 = ((height < 0) ? bottom : top);
		const double y1 = ((height < 0) ? top : bottom);
		const double du = ((static_cast<double>(uvRect.right) - uvRect.left) / width);
		const double dv = ((static_cast<double>(uvRect.bottom) - uvRect.top) / height);
		const FloatRect clippedUV{
			static_cast<float>(uvRect.left + (x0 - x) * du),
			static_cast<float>(uvRect.top + (y0 - y) * dv),
			static_cast<float>(uvRect.left + (x1 - x) * du),
			static_cast<float>(uvRect.top + (y1 - y) * dv) };

		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(
			texture,
			FloatQuad{ Float2{ x0, y0 }, Float2{ x1, y0 }, Float2{ x1, y1 }, Float2{ x0, y1 } },
			clippedUV,
			diffuse.toFloat4());

		return{ left, top, (right - left), (bottom - top) };
	}
}
