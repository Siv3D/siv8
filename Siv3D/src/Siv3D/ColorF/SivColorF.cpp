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
# include <Siv3D/HalfFloat.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{
	namespace
	{
		template <uint32 MantissaBits>
		[[nodiscard]]
		uint32 PackUnsignedFloat(const float value) noexcept
		{
			static_assert((MantissaBits == 5) || (MantissaBits == 6));

			const uint32 bits = std::bit_cast<uint32>(value);

			if (bits & 0x80000000u)
			{
				return 0;
			}

			const uint32 sourceExponent = ((bits >> 23) & 0xFFu);

			if (sourceExponent == 0)
			{
				return 0;
			}

			const int32 exponent = (static_cast<int32>(sourceExponent) - 127);
			constexpr uint32 MantissaMask = ((1u << MantissaBits) - 1u);

			if (15 < exponent)
			{
				return ((30u << MantissaBits) | MantissaMask);
			}

			const uint32 significand = ((bits & 0x7FFFFFu) | 0x800000u);

			if (exponent < -14)
			{
				if (exponent < (-15 - static_cast<int32>(MantissaBits)))
				{
					return 0;
				}

				const uint32 shift = static_cast<uint32>(9 - static_cast<int32>(MantissaBits) - exponent);
				const uint32 halfway = (1u << (shift - 1));
				const uint32 remainder = (significand & ((1u << shift) - 1u));
				uint32 rounded = (significand >> shift);

				if ((halfway < remainder) || ((remainder == halfway) && (rounded & 1u)))
				{
					++rounded;
				}

				return rounded;
			}

			constexpr uint32 Shift = (23u - MantissaBits);
			constexpr uint32 Halfway = (1u << (Shift - 1));
			const uint32 remainder = (significand & ((1u << Shift) - 1u));
			uint32 rounded = (significand >> Shift);

			if ((Halfway < remainder) || ((remainder == Halfway) && (rounded & 1u)))
			{
				++rounded;
			}

			uint32 targetExponent = static_cast<uint32>(exponent + 15);

			if (rounded == (1u << (MantissaBits + 1u)))
			{
				++targetExponent;
				rounded >>= 1;
			}

			if (31u <= targetExponent)
			{
				return ((30u << MantissaBits) | MantissaMask);
			}

			return ((targetExponent << MantissaBits) | (rounded & MantissaMask));
		}

		[[nodiscard]]
		inline static double RemoveSRGBCurve(const double x) noexcept
		{
			return ((x < 0.04045) ? (x / 12.92) : std::pow((x + 0.055) / 1.055, 2.4));
		}

		[[nodiscard]]
		inline static double ApplySRGBCurve(const double x) noexcept
		{
			return ((x < 0.0031308) ? (12.92 * x) : (1.055 * std::pow(x, (1.0 / 2.4)) - 0.055));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hueShifted
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::hueShifted(const double degrees) const noexcept
	{
		HSV hsv{ *this };
		hsv.h += degrees;
		return hsv.toColorF();
	}

	////////////////////////////////////////////////////////////////
	//
	//	complemented
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::complemented() const noexcept
	{
		HSV hsv{ *this };
		hsv.h += 180.0;
		return hsv.toColorF();
	}

	////////////////////////////////////////////////////////////////
	//
	//	srgbToLinear, linearToSRGB
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::srgbToLinear() const noexcept
	{
		return{	RemoveSRGBCurve(r),
				RemoveSRGBCurve(g),
				RemoveSRGBCurve(b),
				a
		};
	}

	ColorF ColorF::linearToSRGB() const noexcept
	{
		return{	ApplySRGBCurve(r),
				ApplySRGBCurve(g),
				ApplySRGBCurve(b),
				a
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	toHSV
	//
	////////////////////////////////////////////////////////////////

	HSV ColorF::toHSV() const noexcept
	{
		return HSV{ *this };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16_Float
	//
	////////////////////////////////////////////////////////////////

	HalfFloat ColorF::toR16_Float() const noexcept
	{
		return HalfFloat{ r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16G16_Float
	//
	////////////////////////////////////////////////////////////////

	uint32 ColorF::toR16G16_Float() const noexcept
	{
		return ((static_cast<uint32>(HalfFloat{ g }.getBits()) << 16) | HalfFloat{ r }.getBits());
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR11G11B10_UFloat
	//
	////////////////////////////////////////////////////////////////

	uint32 ColorF::toR11G11B10_UFloat() const noexcept
	{
		const uint32 rBits = PackUnsignedFloat<6>(static_cast<float>(r));
		const uint32 gBits = PackUnsignedFloat<6>(static_cast<float>(g));
		const uint32 bBits = PackUnsignedFloat<5>(static_cast<float>(b));
		return (rBits | (gBits << 11) | (bBits << 22));
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16G16B16A16_Float
	//
	////////////////////////////////////////////////////////////////

	uint64 ColorF::toR16G16B16A16_Float() const noexcept
	{
		const uint16 r16 = HalfFloat{ r }.getBits();
		const uint16 g16 = HalfFloat{ g }.getBits();
		const uint16 b16 = HalfFloat{ b }.getBits();
		const uint16 a16 = HalfFloat{ a }.getBits();
		return ((static_cast<uint64>(a16) << 48) | (static_cast<uint64>(b16) << 32) | (static_cast<uint64>(g16) << 16) | r16);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const ColorF& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.r);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.g);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.b);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.a);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::ColorF>::format(const s3d::ColorF& value, fmt::format_context& ctx) const
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

s3d::ParseContext::iterator fmt::formatter<s3d::ColorF, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::ColorF, s3d::char32>::format(const s3d::ColorF& value, s3d::BufferContext& ctx) const
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
