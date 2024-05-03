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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV::HSV(const double _h, const double _a) noexcept
		: h{ _h }
		, s{ 1.0 }
		, v{ 1.0 }
		, a{ _a } {}

	constexpr HSV::HSV(const Concept::Arithmetic auto _h, const double _a) noexcept
		: h{ static_cast<double>(_h) }
		, s{ 1.0 }
		, v{ 1.0 }
		, a{ _a } {}

	constexpr HSV::HSV(const double _h, const double _s, const double _v, const double _a) noexcept
		: h{ _h }
		, s{ _s }
		, v{ _v }
		, a{ _a } {}

	constexpr HSV::HSV(const Concept::Arithmetic auto _h, const double _s, const double _v, const double _a) noexcept
		: h{ static_cast<double>(_h) }
		, s{ _s }
		, v{ _v }
		, a{ _a } {}

	constexpr HSV::HSV(const HSV& hsv, const double _a) noexcept
		: h{ hsv.h }
		, s{ hsv.s }
		, v{ hsv.v }
		, a{ _a } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	constexpr double HSV::elem(const size_t index) const noexcept
	{
		if (index == 0)
		{
			return h;
		}
		else if (index == 1)
		{
			return s;
		}
		else if (index == 2)
		{
			return v;
		}
		else if (index == 3)
		{
			return a;
		}
		else
		{
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointer
	//
	////////////////////////////////////////////////////////////////

	constexpr double* HSV::getPointer() noexcept
	{
		return &h;
	}

	constexpr const double* HSV::getPointer() const noexcept
	{
		return &h;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	inline constexpr HSV HSV::operator +(const HSV& hsv) const noexcept
	{
		return{ (h + hsv.h), Clamp((s + hsv.s), 0.0, 1.0), Clamp((v + hsv.v), 0.0, 1.0), a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV HSV::operator -(const HSV& hsv) const noexcept
	{
		return{ (h - hsv.h), Clamp((s - hsv.s), 0.0, 1.0), Clamp((v - hsv.v), 0.0, 1.0), a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withH, withS, withV, withA
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV HSV::withH(const double _h) const noexcept
	{
		return{ _h, s, v, a };
	}

	constexpr HSV HSV::withS(const double _s) const noexcept
	{
		return{ h, _s, v, a };
	}

	constexpr HSV HSV::withV(const double _v) const noexcept
	{
		return{ h, s, _v, a };
	}

	constexpr HSV HSV::withA(const double _a) const noexcept
	{
		return{ h, s, v, _a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV HSV::withAlpha(const double _a) const noexcept
	{
		return{ h, s, v, _a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setH, setS, setV, setA
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV& HSV::setH(const double _h) noexcept
	{
		h = _h;
		return *this;
	}

	constexpr HSV& HSV::setS(const double _s) noexcept
	{
		s = _s;
		return *this;
	}

	constexpr HSV& HSV::setV(const double _v) noexcept
	{
		v = _v;
		return *this;
	}

	constexpr HSV& HSV::setA(const double _a) noexcept
	{
		a = _a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setHSV
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV& HSV::setHSV(const double _h, const double _s, const double _v) noexcept
	{
		h = _h;
		s = _s;
		v = _v;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV& HSV::set(const double _h, const double _s, const double _v, const double _a) noexcept
	{
		h = _h;
		s = _s;
		v = _v;
		a = _a;
		return *this;
	}

	constexpr HSV& HSV::set(const HSV& hsva) noexcept
	{
		return (*this = hsva);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV HSV::lerp(const HSV& other, const double f) const noexcept
	{
		return{ (h + (other.h - h) * f),
				(s + (other.s - s) * f),
				(v + (other.v - v) * f),
				(a + (other.a - a) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 HSV::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hsv
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec3 HSV::hsv() const noexcept
	{
		return{ h, s, v };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hsva
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec4 HSV::hsva() const noexcept
	{
		return{ h, s, v, a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	constexpr HSV HSV::Zero() noexcept
	{
		return{ 0.0, 0.0, 0.0, 0.0 };
	}
}
