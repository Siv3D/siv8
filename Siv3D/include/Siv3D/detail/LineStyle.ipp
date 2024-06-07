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
		return Parameters{ _dotOffset, hasCap, isRound, isDotted, false };
	}

	constexpr LineStyle::Parameters LineStyle::Parameters::offset(const double _dotOffset) const noexcept
	{
		return Parameters{ _dotOffset, hasCap, isRound, isDotted, false };
	}

	constexpr LineStyle::LineStyle(const Parameters& params) noexcept
		: dotOffset{ params.dotOffset }
		, hasCap{ params.hasCap }
		, isRound{ params.isRound }
		, isDotted{ params.isDotted }
		, hasAlignedDot{ params.hasAlignedDot } {}

	constexpr bool LineStyle::hasSquareCap() const noexcept
	{
		return (hasCap && (not isRound) && (not isDotted));
	}

	constexpr bool LineStyle::hasRoundCap() const noexcept
	{
		return (hasCap && isRound && (not isDotted));
	}

	constexpr bool LineStyle::hasNoCap() const noexcept
	{
		return ((not hasCap) && (not isRound) && (not isDotted));
	}

	constexpr bool LineStyle::hasSquareDot() const noexcept
	{
		return (hasCap && (not isRound) && isDotted);
	}

	constexpr bool LineStyle::hasRoundDot() const noexcept
	{
		return (hasCap && isRound && isDotted);
	}
}
