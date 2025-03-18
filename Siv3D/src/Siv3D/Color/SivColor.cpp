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
# include <Siv3D/IntFormatter.hpp>
# include <Siv3D/HalfFloat.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	adjustHue
	//
	////////////////////////////////////////////////////////////////

	Color Color::adjustHue(const double amount) const noexcept
	{
		HSV hsv{ *this };
		hsv.h += amount;
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
	//	toHex
	//
	////////////////////////////////////////////////////////////////

	String Color::toHex() const
	{
		const uint32 rgb = ((r << 16) | (g << 8) | b);
		return U"{:0>6X}"_fmt(rgb);
	}

	////////////////////////////////////////////////////////////////
	//
	//	toR16_Float
	//
	////////////////////////////////////////////////////////////////

	uint16 Color::toR16_Float() const noexcept
	{
		return HalfFloat{ r / 255.0f }.getBits();
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
		const float rf = (r / 255.0f);
		const float gf = (g / 255.0f);
		const float bf = (b / 255.0f);

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
		return{ (r / 255.0f), (g / 255.0f), (b / 255.0f), (a / 255.0f) };
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

fmt::format_context::iterator fmt::formatter<s3d::Color>::format(const s3d::Color& value, fmt::format_context& ctx)
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

s3d::BufferContext::iterator fmt::formatter<s3d::Color, s3d::char32>::format(const s3d::Color& value, s3d::BufferContext& ctx)
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
