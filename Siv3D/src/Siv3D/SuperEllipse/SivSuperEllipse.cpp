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

# include <Siv3D/2DShapes.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	double SuperEllipse::area() const noexcept
	{
		if (n <= 0)
		{
			return 0.0;
		}

		const long double _a = Abs(axes.x);
		const long double _b = Abs(axes.y);

		const long double x1 = (1.0L + 1.0L / n);
		const long double x2 = (1.0L + 2.0L / n);
		const long double lnA = (std::logl(4.0L * _a * _b) + 2.0L * std::lgammal(x1) - std::lgammal(x2));
		return static_cast<double>(std::expl(lnA));
	}
}
