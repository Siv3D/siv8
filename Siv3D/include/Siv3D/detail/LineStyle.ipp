//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	constexpr LineStyle::Parameters LineStyle::Parameters::operator ()(const double _dotOffset) const noexcept
	{
		return Parameters{ _dotOffset, cap, isDotted, false };
	}

	constexpr LineStyle::Parameters LineStyle::Parameters::offset(const double _dotOffset) const noexcept
	{
		return Parameters{ _dotOffset, cap, isDotted, false };
	}

	constexpr LineStyle::LineStyle(const Parameters& params) noexcept
		: dotOffset{ params.dotOffset }
		, cap{ params.cap }
		, isDotted{ params.isDotted }
		, hasAlignedDot{ params.hasAlignedDot } {}

	constexpr bool LineStyle::hasSquareCap() const noexcept
	{
		return ((cap == LineCap::Square) && (not isDotted));
	}

	constexpr bool LineStyle::hasRoundCap() const noexcept
	{
		return ((cap == LineCap::Round) && (not isDotted));
	}

	constexpr bool LineStyle::hasNoCap() const noexcept
	{
		return ((cap == LineCap::Flat) && (not isDotted));
	}

	constexpr bool LineStyle::hasSquareDot() const noexcept
	{
		return ((cap == LineCap::Square) && isDotted);
	}

	constexpr bool LineStyle::hasRoundDot() const noexcept
	{
		return ((cap == LineCap::Round) && isDotted);
	}
}
