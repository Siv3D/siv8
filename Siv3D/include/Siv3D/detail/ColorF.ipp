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

	constexpr ColorF::ColorF(const double _r, const double _g, const double _b, const double _a) noexcept
		: r{ _r }
		, g{ _g }
		, b{ _b }
		, a{ _a } {}

	constexpr ColorF::ColorF(const double rgb, const double _a) noexcept
		: r{ rgb }
		, g{ rgb }
		, b{ rgb }
		, a{ _a } {}

	constexpr ColorF::ColorF(const ColorF& rgb, const double _a) noexcept
		: r{ rgb.r }
		, g{ rgb.g }
		, b{ rgb.b }
		, a{ _a } {}

	constexpr ColorF::ColorF(const Vec3& rgb, const double _a) noexcept
		: r{ rgb.x }
		, g{ rgb.y }
		, b{ rgb.z }
		, a{ _a } {}

	constexpr ColorF::ColorF(const Vec4& rgba) noexcept
		: r{ rgba.x }
		, g{ rgba.y }
		, b{ rgba.z }
		, a{ rgba.w } {}

	constexpr ColorF::ColorF(const Color color) noexcept
		: r{ color.r / 255.0 }
		, g{ color.g / 255.0 }
		, b{ color.b / 255.0 }
		, a{ color.a / 255.0 } {}

	constexpr ColorF::ColorF(const Color rgb, const double _a) noexcept
		: r{ rgb.r / 255.0 }
		, g{ rgb.g / 255.0 }
		, b{ rgb.b / 255.0 }
		, a{ _a } {}

	inline ColorF::ColorF(const HSV& hsva) noexcept
	{
		*this = hsva.toColorF();
	}

	inline ColorF::ColorF(const HSV& hsv, const double _a) noexcept
	{
		*this = hsv.toColorF(_a);
	}

	constexpr ColorF::ColorF(const StringView code) noexcept
		: ColorF{ Color{ code } } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	constexpr double ColorF::elem(const size_t index) const noexcept
	{
		if (index == 0)
		{
			return r;
		}
		else if (index == 1)
		{
			return g;
		}
		else if (index == 2)
		{
			return b;
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

	constexpr double* ColorF::getPointer() noexcept
	{
		return &r;
	}

	constexpr const double* ColorF::getPointer() const noexcept
	{
		return &r;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF& ColorF::operator =(const Color color) noexcept
	{
		r = (color.r / 255.0);
		g = (color.g / 255.0);
		b = (color.b / 255.0);
		a = (color.a / 255.0);
		return *this;
	}

	inline ColorF& ColorF::operator =(const HSV& hsva) noexcept
	{
		return *this = hsva.toColorF();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::operator +(const ColorF& rgb) const noexcept
	{
		return{ (r + rgb.r), (g + rgb.g), (b + rgb.b), a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF& ColorF::operator +=(const ColorF& rgb) noexcept
	{
		r += rgb.r;
		g += rgb.g;
		b += rgb.b;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::operator -(const ColorF& rgb) const noexcept
	{
		return{ (r - rgb.r), (g - rgb.g), (b - rgb.b), a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF& ColorF::operator -=(const ColorF& rgb) noexcept
	{
		r -= rgb.r;
		g -= rgb.g;
		b -= rgb.b;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::operator *(const double s) const noexcept
	{
		return{ (r * s), (g * s), (b * s), a };
	}

	constexpr ColorF ColorF::operator *(const ColorF& rgba) const noexcept
	{
		return{ (r * rgba.r), (g * rgba.g), (b * rgba.b), (a * rgba.a) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF& ColorF::operator *=(const double s) noexcept
	{
		r *= s;
		g *= s;
		b *= s;
		return *this;
	}

	constexpr ColorF& ColorF::operator *=(const ColorF& rgba) noexcept
	{
		r *= rgba.r;
		g *= rgba.g;
		b *= rgba.b;
		a *= rgba.a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withR, withG, withB, withA
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::withR(const double _r) const noexcept
	{
		return{ _r, g, b, a };
	}

	constexpr ColorF ColorF::withG(const double _g) const noexcept
	{
		return{ r, _g, b, a };
	}

	constexpr ColorF ColorF::withB(const double _b) const noexcept
	{
		return{ r, g, _b, a };
	}

	constexpr ColorF ColorF::withA(const double _a) const noexcept
	{
		return{ r, g, b, _a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::withAlpha(const double _a) const noexcept
	{
		return{ r, g, b, _a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setR, setG, setB, setA
	//
	////////////////////////////////////////////////////////////////

	inline constexpr ColorF& ColorF::setR(const double _r) noexcept
	{
		r = _r;
		return *this;
	}

	constexpr ColorF& ColorF::setG(const double _g) noexcept
	{
		g = _g;
		return *this;
	}

	constexpr ColorF& ColorF::setB(const double _b) noexcept
	{
		b = _b;
		return *this;
	}

	constexpr ColorF& ColorF::setA(const double _a) noexcept
	{
		a = _a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setRGB
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF& ColorF::setRGB(const double rgb) noexcept
	{
		r = rgb;
		g = rgb;
		b = rgb;
		return *this;
	}

	constexpr ColorF& ColorF::setRGB(const double _r, const double _g, const double _b) noexcept
	{
		r = _r;
		g = _g;
		b = _b;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF& ColorF::set(const double rgb, const double _a) noexcept
	{
		r = rgb;
		g = rgb;
		b = rgb;
		a = _a;
		return *this;
	}

	constexpr ColorF& ColorF::set(const double _r, const double _g, const double _b, const double _a) noexcept
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
		return *this;
	}

	constexpr ColorF& ColorF::set(const ColorF& color) noexcept
	{
		return *this = color;
	}

	////////////////////////////////////////////////////////////////
	//
	//	grayscale
	//
	////////////////////////////////////////////////////////////////

	constexpr double ColorF::grayscale() const noexcept
	{
		return ((0.299 * r) + (0.587 * g) + (0.114 * b));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minRGBComponent, maxRGBComponent
	//
	////////////////////////////////////////////////////////////////

	constexpr double ColorF::minRGBComponent() const noexcept
	{
		return Min({ r, g, b });
	}

	constexpr double ColorF::maxRGBComponent() const noexcept
	{
		return Max({ r, g, b });
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	constexpr double ColorF::minComponent() const noexcept
	{
		return Min({ r, g, b ,a });
	}

	constexpr double ColorF::maxComponent() const noexcept
	{
		return Max({ r, g, b ,a });
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::lerp(const ColorF& other, const double f) const noexcept
	{
		return{ (r + (other.r - r) * f),
				(g + (other.g - g) * f),
				(b + (other.b - b) * f),
				(a + (other.a - a) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lightened, darkened
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::lightened(const double amount) const noexcept
	{
		return{ (r + (1.0 - r) * amount),
				(g + (1.0 - g) * amount),
				(b + (1.0 - b) * amount),
				a };
	}

	constexpr ColorF ColorF::darkened(const double amount) const noexcept
	{
		return{ (r * (1.0 - amount)),
				(g * (1.0 - amount)),
				(b * (1.0 - amount)),
				a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	gamma
	//
	////////////////////////////////////////////////////////////////

	inline ColorF ColorF::gamma(const double gamma) const noexcept
	{
		if (gamma <= 0.0)
		{
			return{ 0.0, 0.0, 0.0, a };
		}

		const double ig = (1.0 / gamma);

		return{ std::pow(r, ig), std::pow(g, ig), std::pow(b, ig), a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 ColorF::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toColor
	//
	////////////////////////////////////////////////////////////////

	constexpr Color ColorF::toColor() const noexcept
	{
		return{ Color::ToUint8(r),
				Color::ToUint8(g),
				Color::ToUint8(b),
				Color::ToUint8(a) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat4
	//
	////////////////////////////////////////////////////////////////

	constexpr Float4 ColorF::toFloat4() const noexcept
	{
		return{ static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toVec4
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec4 ColorF::toVec4() const noexcept
	{
		return{ r, g, b, a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rg, gb, ba
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 ColorF::rg() const noexcept
	{
		return{ r, g };
	}

	constexpr Vec2 ColorF::gb() const noexcept
	{
		return{ g, b };
	}

	constexpr Vec2 ColorF::ba() const noexcept
	{
		return{ b, a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rgb, gba, bgr
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec3 ColorF::rgb() const noexcept
	{
		return{ r, g, b };
	}

	constexpr Vec3 ColorF::gba() const noexcept
	{
		return{ g, b, a };
	}

	constexpr Vec3 ColorF::bgr() const noexcept
	{
		return{ b, g, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rgba, rgb0, rgb1, argb, abgr
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec4 ColorF::rgba() const noexcept
	{
		return{ r, g, b, a };
	}

	constexpr Vec4 ColorF::rgb0() const noexcept
	{
		return{ r, g, b, 0.0 };
	}

	constexpr Vec4 ColorF::rgb1() const noexcept
	{
		return{ r, g, b, 1.0 };
	}

	constexpr Vec4 ColorF::argb() const noexcept
	{
		return{ a, r, g, b };
	}

	constexpr Vec4 ColorF::abgr() const noexcept
	{
		return{ a, b, g, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::Zero() noexcept
	{
		return{ 0.0, 0.0, 0.0, 0.0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF ColorF::One() noexcept
	{
		return{ 1.0, 1.0, 1.0, 1.0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AlphaF
	//
	////////////////////////////////////////////////////////////////

	constexpr ColorF AlphaF(const double alpha) noexcept
	{
		return ColorF{ 1.0, alpha };
	}
}
