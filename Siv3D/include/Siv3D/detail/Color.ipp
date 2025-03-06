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

# pragma once

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		constexpr uint8 HexToColor(const char32 c) noexcept
		{
			if (not IsXdigit(c))
			{
				return 0;
			}

			return static_cast<uint8>(((c & 0xF) + ((c & 0x40) >> 6) * 9) * 17);
		}

		[[nodiscard]]
		constexpr uint8 HexToColor(const char32 c0, const char32 c1) noexcept
		{
			if ((not IsXdigit(c0)) || (not IsXdigit(c1)))
			{
				return 0;
			}

			const uint32 high = ((c0 & 0xF) + ((c0 & 0x40) >> 6) * 9);
			const uint32 low = ((c1 & 0xF) + ((c1 & 0x40) >> 6) * 9);
			return static_cast<uint8>((high << 4) | low);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Color::Color(const value_type rgb) noexcept
		: r{ rgb }
		, g{ rgb }
		, b{ rgb }
		, a{ 255 } {}

	constexpr Color::Color(const Concept::Integral auto rgb) noexcept
		: r{ static_cast<value_type>(rgb) }
		, g{ static_cast<value_type>(rgb) }
		, b{ static_cast<value_type>(rgb) }
		, a{ 255 } {}

	constexpr Color::Color(const value_type rgb, const value_type _a) noexcept
		: r{ rgb }
		, g{ rgb }
		, b{ rgb }
		, a{ _a } {}

	constexpr Color::Color(const Concept::Integral auto rgb, const Concept::Integral auto _a) noexcept
		: r{ static_cast<value_type>(rgb) }
		, g{ static_cast<value_type>(rgb) }
		, b{ static_cast<value_type>(rgb) }
		, a{ static_cast<value_type>(_a) } {}

	constexpr Color::Color(const value_type _r, const value_type _g, const value_type _b) noexcept
		: r{ _r }
		, g{ _g }
		, b{ _b }
		, a{ 255 } {}

	constexpr Color::Color(const Concept::Integral auto _r, const Concept::Integral auto _g, const Concept::Integral auto _b) noexcept
		: r{ static_cast<value_type>(_r) }
		, g{ static_cast<value_type>(_g) }
		, b{ static_cast<value_type>(_b) }
		, a{ 255 } {}

	constexpr Color::Color(const value_type _r, const value_type _g, const value_type _b, const value_type _a) noexcept
		: r{ _r }
		, g{ _g }
		, b{ _b }
		, a{ _a } {}

	constexpr Color::Color(const Concept::Integral auto _r, const Concept::Integral auto _g, const Concept::Integral auto _b, const Concept::Integral auto _a) noexcept
		: r{ static_cast<value_type>(_r) }
		, g{ static_cast<value_type>(_g) }
		, b{ static_cast<value_type>(_b) }
		, a{ static_cast<value_type>(_a) } {}

	constexpr Color::Color(const Color rgb, const value_type _a) noexcept
		: r{ rgb.r }
		, g{ rgb.g }
		, b{ rgb.b }
		, a{ _a } {}

	constexpr Color::Color(const Color rgb, const Concept::Integral auto _a) noexcept
		: r{ rgb.r }
		, g{ rgb.g }
		, b{ rgb.b }
		, a{ static_cast<value_type>(_a) } {}

	constexpr Color::Color(const ColorF& color) noexcept
		: r{ Color::ToUint8(color.r) }
		, g{ Color::ToUint8(color.g) }
		, b{ Color::ToUint8(color.b) }
		, a{ Color::ToUint8(color.a) } {}

	constexpr Color::Color(const ColorF& color, const value_type _a) noexcept
		: r{ Color::ToUint8(color.r) }
		, g{ Color::ToUint8(color.g) }
		, b{ Color::ToUint8(color.b) }
		, a{ _a } {}

	constexpr Color::Color(const ColorF& color, const Concept::Integral auto _a) noexcept
		: r{ Color::ToUint8(color.r) }
		, g{ Color::ToUint8(color.g) }
		, b{ Color::ToUint8(color.b) }
		, a{ static_cast<value_type>(_a) } {}

	inline Color::Color(const HSV& hsv) noexcept
	{
		*this = hsv.toColor();
	}

	inline Color::Color(const HSV& hsv, const value_type _a) noexcept
	{
		*this = hsv.toColor(_a);
	}

	inline Color::Color(const HSV& hsv, const Concept::Integral auto _a) noexcept
	{
		*this = hsv.toColor(static_cast<value_type>(_a));
	}

	constexpr Color::Color(const StringView code) noexcept
		: r{ 0 }
		, g{ 0 }
		, b{ 0 }
		, a{ 255 }
	{
		if (code.size() == 4) // #RGB
		{
			r = detail::HexToColor(code[1]);
			g = detail::HexToColor(code[2]);
			b = detail::HexToColor(code[3]);
			a = 255;
		}
		else if (code.size() == 5) // #RGBA
		{
			r = detail::HexToColor(code[1]);
			g = detail::HexToColor(code[2]);
			b = detail::HexToColor(code[3]);
			a = detail::HexToColor(code[4]);
		}
		else if (code.size() == 7) // #RRGGBB
		{
			r = detail::HexToColor(code[1], code[2]);
			g = detail::HexToColor(code[3], code[4]);
			b = detail::HexToColor(code[5], code[6]);
			a = 255;
		}
		else if (code.size() == 8) // RRGGBBAA
		{
			r = detail::HexToColor(code[0], code[1]);
			g = detail::HexToColor(code[2], code[3]);
			b = detail::HexToColor(code[4], code[5]);
			a = detail::HexToColor(code[6], code[7]);
		}
		else if (code.size() == 9) // #RRGGBBAA
		{
			r = detail::HexToColor(code[1], code[2]);
			g = detail::HexToColor(code[3], code[4]);
			b = detail::HexToColor(code[5], code[6]);
			a = detail::HexToColor(code[7], code[8]);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	constexpr Color& Color::operator =(const ColorF& color) noexcept
	{
		r = Color::ToUint8(color.r);
		g = Color::ToUint8(color.g);
		b = Color::ToUint8(color.b);
		a = Color::ToUint8(color.a);
		return *this;
	}

	inline Color& Color::operator =(const HSV& hsva) noexcept
	{
		return (*this = hsva.toColor());
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ~
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::operator ~() const noexcept
	{
		return{ static_cast<uint8>(255 - r),
				static_cast<uint8>(255 - g),
				static_cast<uint8>(255 - b),
				a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withR, withG, withB, withA
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::withR(const uint32 _r) const noexcept
	{
		return{ static_cast<uint8>(_r), g, b, a };
	}

	constexpr Color Color::withG(const uint32 _g) const noexcept
	{
		return{ r, static_cast<uint8>(_g), b, a };
	}

	constexpr Color Color::withB(const uint32 _b) const noexcept
	{
		return{ r, g, static_cast<uint8>(_b), a };
	}

	constexpr Color Color::withA(const uint32 _a) const noexcept
	{
		return{ r, g, b, static_cast<uint8>(_a) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::withAlpha(const uint32 _a) const noexcept
	{
		return{ r, g, b, static_cast<uint8>(_a) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setR, setG, setB, setA
	//
	////////////////////////////////////////////////////////////////

	constexpr Color& Color::setR(const uint32 _r) noexcept
	{
		r = static_cast<uint8>(_r);
		return *this;
	}

	constexpr Color& Color::setG(const uint32 _g) noexcept
	{
		g = static_cast<uint8>(_g);
		return *this;
	}

	constexpr Color& Color::setB(const uint32 _b) noexcept
	{
		b = static_cast<uint8>(_b);
		return *this;
	}

	constexpr Color& Color::setA(const uint32 _a) noexcept
	{
		a = static_cast<uint8>(_a);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setRGB
	//
	////////////////////////////////////////////////////////////////

	constexpr Color& Color::setRGB(const uint32 rgb) noexcept
	{
		r = static_cast<uint8>(rgb);
		g = static_cast<uint8>(rgb);
		b = static_cast<uint8>(rgb);
		return *this;
	}

	constexpr Color& Color::setRGB(const uint32 _r, const uint32 _g, const uint32 _b) noexcept
	{
		r = static_cast<uint8>(_r);
		g = static_cast<uint8>(_g);
		b = static_cast<uint8>(_b);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Color& Color::set(const uint32 rgb, const uint32 _a) noexcept
	{
		r = static_cast<uint8>(rgb);
		g = static_cast<uint8>(rgb);
		b = static_cast<uint8>(rgb);
		a = static_cast<uint8>(_a);
		return *this;
	}

	constexpr Color& Color::set(const uint32 _r, const uint32 _g, const uint32 _b, const uint32 _a) noexcept
	{
		r = static_cast<uint8>(_r);
		g = static_cast<uint8>(_g);
		b = static_cast<uint8>(_b);
		a = static_cast<uint8>(_a);
		return *this;
	}

	constexpr Color& Color::set(const Color color) noexcept
	{
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	grayscale0_255
	//
	////////////////////////////////////////////////////////////////

	constexpr uint8 Color::grayscale0_255() const noexcept
	{
		return static_cast<uint8>((0.299 * r) + (0.587 * g) + (0.114 * b));
	}

	////////////////////////////////////////////////////////////////
	//
	//	grayscale
	//
	////////////////////////////////////////////////////////////////

	constexpr double Color::grayscale() const noexcept
	{
		return ((0.299 / 255.0 * r) + (0.587 / 255.0 * g) + (0.114 / 255.0 * b));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minRGBComponent, maxRGBComponent
	//
	////////////////////////////////////////////////////////////////

	constexpr uint8 Color::minRGBComponent() const noexcept
	{
		return Min(Min(r, g), b);
	}

	constexpr uint8 Color::maxRGBComponent() const noexcept
	{
		return Max(Max(r, g), b);
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	constexpr uint8 Color::minComponent() const noexcept
	{
		return Min(Min(r, g), Min(b, a));
	}

	constexpr uint8 Color::maxComponent() const noexcept
	{
		return Max(Max(r, g), Max(b, a));
	}

	////////////////////////////////////////////////////////////////
	//
	//	asUint32
	//
	////////////////////////////////////////////////////////////////

	constexpr uint32 Color::asUint32() const noexcept
	{
		return std::bit_cast<uint32>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::lerp(const Color other, const double f) const noexcept
	{
		return{ static_cast<uint8>(int32(r) + (int32(other.r) - int32(r)) * f),
				static_cast<uint8>(int32(g) + (int32(other.g) - int32(g)) * f),
				static_cast<uint8>(int32(b) + (int32(other.b) - int32(b)) * f),
				static_cast<uint8>(int32(a) + (int32(other.a) - int32(a)) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lightened, darkened
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::lightened(const double amount) const noexcept
	{
		return{ static_cast<uint8>(r + (255 - r) * amount),
				static_cast<uint8>(g + (255 - g) * amount),
				static_cast<uint8>(b + (255 - b) * amount),
				a };
	}

	constexpr Color Color::darkened(const double amount) const noexcept
	{
		return{ static_cast<uint8>(r * (1.0 - amount)),
				static_cast<uint8>(g * (1.0 - amount)),
				static_cast<uint8>(b * (1.0 - amount)),
				a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	inverted
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::inverted() const noexcept
	{
		return{ static_cast<uint8>(255 - r),
				static_cast<uint8>(255 - g),
				static_cast<uint8>(255 - b),
				a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	gamma
	//
	////////////////////////////////////////////////////////////////

	inline Color Color::gamma(const double gamma) const noexcept
	{
		if (gamma <= 0.0)
		{
			return{ 0, 0, 0, a };
		}

		const double ig = (1.0 / gamma);

		return{ static_cast<uint8>(255 * std::pow(r / 255.0, ig)),
				static_cast<uint8>(255 * std::pow(g / 255.0, ig)),
				static_cast<uint8>(255 * std::pow(b / 255.0, ig)),
				a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeSRGBCurve, applySRGBCurve
	//
	////////////////////////////////////////////////////////////////

	inline ColorF Color::removeSRGBCurve() const noexcept
	{
		return ColorF{ *this }.removeSRGBCurve();
	}

	inline ColorF Color::applySRGBCurve() const noexcept
	{
		return ColorF{ *this }.applySRGBCurve();
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Color::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	abgr
	//
	////////////////////////////////////////////////////////////////

	inline constexpr Color Color::abgr() const noexcept
	{
		return{ a, b, g, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::Zero() noexcept
	{
		return{ 0, 0, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	ToUint8
	//
	////////////////////////////////////////////////////////////////

	constexpr uint8 Color::ToUint8(const float x) noexcept
	{
		if (x <= 0.0f)
		{
			return 0;
		}
		else if (1.0f <= x)
		{
			return 255;
		}
		else
		{
			return static_cast<uint8>(x * 255.0f + 0.5f);
		}
	}

	constexpr uint8 Color::ToUint8(const double x) noexcept
	{
		if (x <= 0.0)
		{
			return 0;
		}
		else if (1.0 <= x)
		{
			return 255;
		}
		else
		{
			return static_cast<uint8>(x * 255.0 + 0.5);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromRGBA
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::FromRGBA(const uint32 rgba) noexcept
	{
		return{ static_cast<uint8>((rgba >> 24) & 0xFF),
				static_cast<uint8>((rgba >> 16) & 0xFF),
				static_cast<uint8>((rgba >> 8) & 0xFF),
				static_cast<uint8>(rgba & 0xFF) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromABGR
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::FromABGR(const uint32 abgr) noexcept
	{
		return{ static_cast<uint8>(abgr & 0xFF),
				static_cast<uint8>((abgr >> 8) & 0xFF),
				static_cast<uint8>((abgr >> 16) & 0xFF),
				static_cast<uint8>((abgr >> 24) & 0xFF) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	PremultiplyAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::PremultiplyAlpha(const Color color) noexcept
	{
		const uint8 r = static_cast<uint8>((static_cast<uint16>(color.r) * color.a) / 255);
		const uint8 g = static_cast<uint8>((static_cast<uint16>(color.g) * color.a) / 255);
		const uint8 b = static_cast<uint8>((static_cast<uint16>(color.b) * color.a) / 255);	
		return{ r, g, b, color.a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnpremultiplyAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Color::UnpremultiplyAlpha(const Color color) noexcept
	{
		if (color.a == 0)
		{
			return Color{ 0, 0, 0, 0 };
		}

		const uint8 r = static_cast<uint8>((static_cast<uint16>(color.r) * 255) / color.a);
		const uint8 g = static_cast<uint8>((static_cast<uint16>(color.g) * 255) / color.a);
		const uint8 b = static_cast<uint8>((static_cast<uint16>(color.b) * 255) / color.a);
		return{ r, g, b, color.a };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Alpha
	//
	////////////////////////////////////////////////////////////////

	constexpr Color Alpha(const uint32 alpha) noexcept
	{
		return Color{ 255, static_cast<uint8>(alpha) };
	}
}
