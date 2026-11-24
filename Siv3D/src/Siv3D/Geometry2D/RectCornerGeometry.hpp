// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.

# pragma once
# include <Siv3D/Utility.hpp>

namespace s3d::detail::RectCornerGeometry
{
	inline constexpr void ScaleCornerSizes(double& tl, double& tr, double& br, double& bl, const double w, const double h) noexcept
	{
		double scale = 1.0;

		if (const double top = (tl + tr); w < top)
		{
			scale = Min(scale, (w / top));
		}

		if (const double right = (tr + br); h < right)
		{
			scale = Min(scale, (h / right));
		}

		if (const double bottom = (br + bl); w < bottom)
		{
			scale = Min(scale, (w / bottom));
		}

		if (const double left = (bl + tl); h < left)
		{
			scale = Min(scale, (h / left));
		}

		if (scale < 1.0)
		{
			tl *= scale;
			tr *= scale;
			br *= scale;
			bl *= scale;
		}
	}

	inline constexpr void NormalizeCornerRadii(double& tl, double& tr, double& br, double& bl, const double w, const double h) noexcept
	{
		tl = Max(tl, 0.0);
		tr = Max(tr, 0.0);
		br = Max(br, 0.0);
		bl = Max(bl, 0.0);
		ScaleCornerSizes(tl, tr, br, bl, w, h);
	}
}
