//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/IntFormatter.hpp>
# include <Siv3D/HalfFloat.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		uint32 PackUnsignedFloat(const uint8 channel, const uint32 mantissaBits) noexcept
		{
			if (channel == 0)
			{
				return 0;
			}

			const float value = (channel / 255.0f);
			const uint32 bits = std::bit_cast<uint32>(value);
			uint32 exponent = static_cast<uint32>(static_cast<int32>((bits >> 23) & 0xFFu) - 127 + 15);
			const uint32 significand = ((bits & 0x7FFFFFu) | 0x800000u);
			const uint32 shift = (23u - mantissaBits);
			const uint32 halfway = (1u << (shift - 1));
			const uint32 remainder = (significand & ((1u << shift) - 1u));
			uint32 rounded = (significand >> shift);

			if ((halfway < remainder) || ((remainder == halfway) && (rounded & 1u)))
			{
				++rounded;
			}

			if (rounded == (1u << (mantissaBits + 1u)))
			{
				++exponent;
				rounded >>= 1;
			}

			return ((exponent << mantissaBits) | (rounded & ((1u << mantissaBits) - 1u)));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hueShifted
	//
	////////////////////////////////////////////////////////////////

	Color Color::hueShifted(const double degrees) const noexcept
	{
		HSV hsv{ *this };
		hsv.h += degrees;
		return hsv.toColor();
	}

	////////////////////////////////////////////////////////////////
	//
	//	complemented
	//
	////////////////////////////////////////////////////////////////

	Color Color::complemented() const noexcept
	{
		HSV hsv{ *this };
		hsv.h += 180.0;
		return hsv.toColor();
	}

	////////////////////////////////////////////////////////////////
	//
	//	toHexRGB, toHexRGBA
	//
	////////////////////////////////////////////////////////////////

	String Color::toHexRGB() const
	{
		const uint32 rgb = (toRGBA8888() >> 8);
		return U"{:0>6X}"_fmt(rgb);
	}

	String Color::toHexRGBA() const
	{
		return U"{:0>8X}"_fmt(toRGBA8888());
	}

	////////////////////////////////////////////////////////////////
	//
	//	toHSV
	//
	////////////////////////////////////////////////////////////////

	HSV Color::toHSV() const noexcept
	{
		return HSV{ *this };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toFloat4
	//
	////////////////////////////////////////////////////////////////

	Float4 Color::toFloat4() const noexcept
	{
		return{ (r / 255.0f), (g / 255.0f), (b / 255.0f), (a / 255.0f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16_Float
	//
	////////////////////////////////////////////////////////////////

	HalfFloat Color::toR16_Float() const noexcept
	{
		return HalfFloat{ r / 255.0f };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16G16_Float
	//
	////////////////////////////////////////////////////////////////

	uint32 Color::toR16G16_Float() const noexcept
	{
		return ((static_cast<uint32>(HalfFloat{ g / 255.0f }.getBits()) << 16) | HalfFloat{ r / 255.0f }.getBits());
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR11G11B10_UFloat
	//
	////////////////////////////////////////////////////////////////

	uint32 Color::toR11G11B10_UFloat() const noexcept
	{
		const uint32 rBits = PackUnsignedFloat(r, 6);
		const uint32 gBits = PackUnsignedFloat(g, 6);
		const uint32 bBits = PackUnsignedFloat(b, 5);
		return (rBits | (gBits << 11) | (bBits << 22));
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16G16B16A16_Float
	//
	////////////////////////////////////////////////////////////////

	uint64 Color::toR16G16B16A16_Float() const noexcept
	{
		const uint16 r16 = HalfFloat{ r / 255.0f }.getBits();
		const uint16 g16 = HalfFloat{ g / 255.0f }.getBits();
		const uint16 b16 = HalfFloat{ b / 255.0f }.getBits();
		const uint16 a16 = HalfFloat{ a / 255.0f }.getBits();
		return ((static_cast<uint64>(a16) << 48)
			| (static_cast<uint64>(b16) << 32)
			| (static_cast<uint64>(g16) << 16)
			| r16);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR32G32_Float
	//
	////////////////////////////////////////////////////////////////

	Float2 Color::toR32G32_Float() const noexcept
	{
		return{ (r / 255.0f), (g / 255.0f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR32G32B32A32_Float
	//
	////////////////////////////////////////////////////////////////

	Float4 Color::toR32G32B32A32_Float() const noexcept
	{
		return toFloat4();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Color& value)
	{
		constexpr size_t BufferSize = ((3 * 4) + 8 + 1);
		char32 buffer[BufferSize];
		char32* p = buffer;

		*(p++) = U'(';
		detail::AppendInt32(&p, value.r);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.g);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.b);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.a);
		*(p++) = U')';

		formatData.string.append(buffer, (p - buffer));
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Color>::format(const s3d::Color& value, fmt::format_context& ctx) const
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", value.r, value.g, value.b, value.a);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.r, value.g, value.b, value.a));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Color, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Color, s3d::char32>::format(const s3d::Color& value, s3d::BufferContext& ctx) const
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", value.r, value.g, value.b, value.a);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.r, value.g, value.b, value.a);
	}
}
