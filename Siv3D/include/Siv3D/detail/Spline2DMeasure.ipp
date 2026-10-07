//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once

namespace s3d
{
	inline Spline2DMeasure& Spline2DMeasure::operator =(Spline2DMeasure other) noexcept
	{
		swap(other);
		return *this;
	}

	inline bool Spline2DMeasure::isEmpty() const noexcept
	{
		return m_spline.isEmpty();
	}

	inline double Spline2DMeasure::length() const noexcept
	{
		return (m_prefixLengths.isEmpty() ? 0.0 : m_prefixLengths.back());
	}

	inline const Spline2D& Spline2DMeasure::spline() const& noexcept
	{
		return m_spline;
	}
}
