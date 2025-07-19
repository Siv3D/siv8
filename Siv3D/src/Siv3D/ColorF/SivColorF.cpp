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

# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/HalfFloat.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{
	namespace
	{
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
	//	adjustHue
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::adjustHue(const double amount) const noexcept
	{
		HSV hsv{ *this };
		hsv.h += amount;
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
	//	removeSRGBCurve, applySRGBCurve
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::removeSRGBCurve() const noexcept
	{
		return{	RemoveSRGBCurve(r),
				RemoveSRGBCurve(g),
				RemoveSRGBCurve(b),
				a
		};
	}

	ColorF ColorF::applySRGBCurve() const noexcept
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
		// 負の値は 0 にクランプ（unsigned float 形式のため）
		const float rf = Clamp(static_cast<float>(r), 0.0f, 1.0f);
		const float gf = Clamp(static_cast<float>(g), 0.0f, 1.0f);
		const float bf = Clamp(static_cast<float>(b), 0.0f, 1.0f);

		// R の 11-bit 浮動小数点変換 (5 ビット指数, 6 ビット仮数)
		uint32 rBits = 0;
		if (0.0f < rf)
		{
			int32 rExp = static_cast<int32>(std::floor(std::log2f(rf)));
			float rMantissa = (rf / std::powf(2.0f, static_cast<float>(rExp)) - 1.0f);

			rExp += 15; // バイアス調整

			if (rExp <= 0)
			{
				// デノーマル数またはゼロ
				rBits = 0;
			}
			else if (31 <= rExp)
			{
				// オーバーフロー - 最大値に設定
				rBits = 0x7FF;
			}
			else
			{
				// 6ビット仮数に変換 (0-63)
				uint32 rMantissaBits = static_cast<uint32>(std::roundf(rMantissa * 64.0f));
				rBits = ((rExp << 6) | (rMantissaBits & 0x3F));
			}
		}

		// G の 11-bit 浮動小数点変換
		uint32 gBits = 0;
		if (0.0f < gf)
		{
			int32 gExp = static_cast<int32>(std::floor(std::log2f(gf)));
			float gMantissa = (gf / std::powf(2.0f, static_cast<float>(gExp)) - 1.0f);

			gExp += 15; // バイアス調整

			if (gExp <= 0)
			{
				gBits = 0;
			}
			else if (31 <= gExp)
			{
				gBits = 0x7FF;
			}
			else
			{
				uint32 gMantissaBits = static_cast<uint32>(std::roundf(gMantissa * 64.0f));
				gBits = ((gExp << 6) | (gMantissaBits & 0x3F));
			}
		}

		// B の 10-bit 浮動小数点変換 (5 ビット指数, 5 ビット仮数)
		uint32 bBits = 0;
		if (0.0f < bf)
		{
			int32 bExp = static_cast<int32>(std::floor(std::log2f(bf)));
			float bMantissa = (bf / std::powf(2.0f, static_cast<float>(bExp)) - 1.0f);

			bExp += 15; // バイアス調整

			if (bExp <= 0)
			{
				bBits = 0;
			}
			else if (31 <= bExp)
			{
				bBits = 0x3FF;
			}
			else
			{
				uint32 bMantissaBits = static_cast<uint32>(std::roundf(bMantissa * 32.0f));
				bBits = ((bExp << 5) | (bMantissaBits & 0x1F));
			}
		}

		// R11G11B10 形式にパック
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

fmt::format_context::iterator fmt::formatter<s3d::ColorF>::format(const s3d::ColorF& value, fmt::format_context& ctx)
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

s3d::BufferContext::iterator fmt::formatter<s3d::ColorF, s3d::char32>::format(const s3d::ColorF& value, s3d::BufferContext& ctx)
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
