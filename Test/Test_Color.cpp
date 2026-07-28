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

# include "Siv3DTest.hpp"
# include <sstream>
# include <type_traits>

TEST_CASE("Color.type")
{
	static_assert(sizeof(Color) == 4);
	static_assert(std::is_trivially_copyable_v<Color>);
	static_assert(std::is_standard_layout_v<Color>);
}

TEST_CASE("Color.constructor")
{
	static_assert(Color{ 12 } == Color{ 12, 12, 12, 255 });
	static_assert(Color{ 12, 34 } == Color{ 12, 12, 12, 34 });
	static_assert(Color{ 12, 34, 56 } == Color{ 12, 34, 56, 255 });
	static_assert(Color{ Color{ 12, 34, 56, 78 }, 90 } == Color{ 12, 34, 56, 90 });

	static_assert(Color{ U"#123" } == Color{ 0x11, 0x22, 0x33, 0xFF });
	static_assert(Color{ U"#1234" } == Color{ 0x11, 0x22, 0x33, 0x44 });
	static_assert(Color{ U"123456" } == Color{ 0x12, 0x34, 0x56, 0xFF });
	static_assert(Color{ U"#123456" } == Color{ 0x12, 0x34, 0x56, 0xFF });
	static_assert(Color{ U"12345678" } == Color{ 0x12, 0x34, 0x56, 0x78 });
	static_assert(Color{ U"#12345678" } == Color{ 0x12, 0x34, 0x56, 0x78 });
	static_assert(Color{ U"#aBcDeF" } == Color{ 0xAB, 0xCD, 0xEF, 0xFF });

	static_assert(Color{ U"1234" } == Color{ 0, 0, 0, 255 });
	static_assert(Color{ U"1234567" } == Color{ 0, 0, 0, 255 });
	static_assert(Color{ U"#1234567" } == Color{ 0, 0, 0, 255 });
}

TEST_CASE("Color.operator ==")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	static_assert(c0 == c0);
	static_assert(c1 == c1);
	static_assert(c2 == c2);

	static_assert(not(c0 == c1));
	static_assert(not(c0 == c2));
	static_assert(not(c1 == c2));
}

TEST_CASE("Color.operator !=")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	static_assert(c0 != c1);
	static_assert(c0 != c2);
	static_assert(c1 != c2);

	static_assert(not(c0 != c0));
	static_assert(not(c1 != c1));
	static_assert(not(c2 != c2));
}

TEST_CASE("Color.component operations")
{
	constexpr Color color{ 10, 20, 30, 40 };

	static_assert((~color) == Color{ 245, 235, 225, 40 });
	static_assert(color.inverted() == Color{ 245, 235, 225, 40 });
	static_assert(color.withR(50) == Color{ 50, 20, 30, 40 });
	static_assert(color.withG(50) == Color{ 10, 50, 30, 40 });
	static_assert(color.withB(50) == Color{ 10, 20, 50, 40 });
	static_assert(color.withA(50) == Color{ 10, 20, 30, 50 });
	static_assert(color.withAlpha(50) == Color{ 10, 20, 30, 50 });
	static_assert(color.abgr() == Color{ 40, 30, 20, 10 });

	constexpr Color modified = []
	{
		Color result{ 1, 2, 3, 4 };
		result.setR(10).setG(20).setB(30).setA(40);
		result.setRGB(50, 60, 70);
		return result;
	}();

	static_assert(modified == Color{ 50, 60, 70, 40 });
}

TEST_CASE("Color.premultiplied")
{
	constexpr Color color{ 255, 128, 64, 128 };
	constexpr Color premultiplied{ 128, 64, 32, 128 };

	static_assert(color.premultiplied() == premultiplied);
	static_assert(Color::PremultiplyAlpha(color) == premultiplied);
	static_assert(premultiplied.unpremultiplied() == color);
	static_assert(Color::UnpremultiplyAlpha(premultiplied) == color);
	static_assert(Color::UnpremultiplyAlpha(Color{ 255, 128, 1, 0 }) == Color::Zero());
	static_assert(Color::UnpremultiplyAlpha(Color{ 255, 255, 255, 1 }) == Color{ 255, 255, 255, 1 });

	static_assert(Color::Div255Round(0) == 0);
	static_assert(Color::Div255Round(127) == 0);
	static_assert(Color::Div255Round(128) == 1);
	static_assert(Color::Div255Round(65025) == 255);
}

TEST_CASE("Color.grayscale")
{
	static_assert(Color{ 0, 0, 0 }.grayscale8() == 0);
	static_assert(Color{ 255, 255, 255 }.grayscale8() == 255);
	static_assert(Color{ 255, 0, 0 }.grayscale8() == 76);
	static_assert(Color{ 0, 255, 0 }.grayscale8() == 149);
	static_assert(Color{ 0, 0, 255 }.grayscale8() == 29);

	CHECK(Color{ 12, 34, 56 }.grayscale() == doctest::Approx(29930.0 / 255000.0));
}

TEST_CASE("Color.min/max component")
{
	constexpr Color color{ 30, 10, 40, 20 };

	static_assert(color.minRGBComponent() == 10);
	static_assert(color.maxRGBComponent() == 40);
	static_assert(color.minComponent() == 10);
	static_assert(color.maxComponent() == 40);
}

TEST_CASE("Color.lerp")
{
	constexpr Color from{ 10, 20, 30, 40 };
	constexpr Color to{ 110, 120, 130, 140 };

	static_assert(from.lerp(to, 0.0) == from);
	static_assert(from.lerp(to, 0.5) == Color{ 60, 70, 80, 90 });
	static_assert(from.lerp(to, 1.0) == to);
	static_assert(from.lerp(to, -1.0) == from);
	static_assert(from.lerp(to, 2.0) == to);

	static_assert(from.lightened(-1.0) == from);
	static_assert(from.lightened(1.0) == Color{ 255, 255, 255, 40 });
	static_assert(from.lightened(2.0) == Color{ 255, 255, 255, 40 });
	static_assert(from.darkened(-1.0) == from);
	static_assert(from.darkened(1.0) == Color{ 0, 0, 0, 40 });
	static_assert(from.darkened(2.0) == Color{ 0, 0, 0, 40 });
}

TEST_CASE("Color.color conversion")
{
	const Color color{ 255, 0, 0, 128 };

	const HSV hsv = color.toHSV();
	CHECK(hsv.h == doctest::Approx(0.0));
	CHECK(hsv.s == doctest::Approx(1.0));
	CHECK(hsv.v == doctest::Approx(1.0));
	CHECK(hsv.a == doctest::Approx(128.0 / 255.0));

	CHECK(color.hueShifted(120.0) == Color{ 0, 255, 0, 128 });
	CHECK(color.complemented() == Color{ 0, 255, 255, 128 });
	CHECK(color.gamma(1.0) == color);
	CHECK(color.gamma(0.0) == Color{ 0, 0, 0, 128 });

	const Color roundTrip = Color{ 0x12, 0x34, 0x56, 0x78 }.srgbToLinear().linearToSRGB().toColor();
	CHECK(roundTrip == Color{ 0x12, 0x34, 0x56, 0x78 });

	const ColorF linearRed = HSV{ 0.0, 1.0, 1.0, 0.5 }.srgbToLinear();
	CHECK(linearRed.r == doctest::Approx(1.0));
	CHECK(linearRed.g == doctest::Approx(0.0));
	CHECK(linearRed.b == doctest::Approx(0.0));
	CHECK(linearRed.a == doctest::Approx(0.5));

	const ColorF srgbRed = HSV{ 0.0, 1.0, 1.0, 0.5 }.linearToSRGB();
	CHECK(srgbRed.r == doctest::Approx(1.0));
	CHECK(srgbRed.g == doctest::Approx(0.0));
	CHECK(srgbRed.b == doctest::Approx(0.0));
	CHECK(srgbRed.a == doctest::Approx(0.5));
}

TEST_CASE("Color.toHexRGB/toHexRGBA")
{
	const Color color{ 0x12, 0xAB, 0xF0, 0x34 };

	CHECK(color.toHexRGB() == U"12ABF0");
	CHECK(color.toHexRGBA() == U"12ABF034");
	CHECK(Color{ color.toHexRGB() } == Color{ 0x12, 0xAB, 0xF0 });
	CHECK(Color{ color.toHexRGBA() } == color);
	CHECK(Color::Zero().toHexRGB() == U"000000");
	CHECK(Color::Zero().toHexRGBA() == U"00000000");
}

TEST_CASE("Color.RGBA8888 conversion")
{
	constexpr Color color{ 0x12, 0x34, 0x56, 0x78 };

	static_assert(color.asUint32() == 0x78563412u);
	static_assert(color.toRGBA8888() == 0x12345678u);
	static_assert(color.toABGR8888() == 0x78563412u);
	static_assert(Color::FromRGBA8888(color.toRGBA8888()) == color);
	static_assert(Color::FromABGR8888(color.toABGR8888()) == color);

	static_assert(Color::Zero().toRGBA8888() == 0x00000000u);
	static_assert(Color{ 255, 255, 255, 255 }.toRGBA8888() == 0xFFFFFFFFu);
}

TEST_CASE("Color.normalized format conversion")
{
	const Color color{ 0x12, 0x34, 0x56, 0x78 };
	const Float4 expected4{ (0x12 / 255.0f), (0x34 / 255.0f), (0x56 / 255.0f), (0x78 / 255.0f) };

	CHECK(color.toFloat4() == expected4);
	CHECK(color.toR32G32B32A32_Float() == expected4);
	CHECK(color.toR32G32_Float() == Float2{ (0x12 / 255.0f), (0x34 / 255.0f) });
	CHECK(color.toR16_Float().getBits() == HalfFloat{ 0x12 / 255.0f }.getBits());

	const uint16 r16 = HalfFloat{ 0x12 / 255.0f }.getBits();
	const uint16 g16 = HalfFloat{ 0x34 / 255.0f }.getBits();
	const uint16 b16 = HalfFloat{ 0x56 / 255.0f }.getBits();
	const uint16 a16 = HalfFloat{ 0x78 / 255.0f }.getBits();

	CHECK(color.toR16G16_Float() == ((static_cast<uint32>(g16) << 16) | r16));
	CHECK(color.toR16G16B16A16_Float()
		== ((static_cast<uint64>(a16) << 48)
			| (static_cast<uint64>(b16) << 32)
			| (static_cast<uint64>(g16) << 16)
			| r16));
}

TEST_CASE("Color.packed format conversion")
{
	constexpr Color color{ 0x12, 0x34, 0x56, 0x78 };

	static_assert(color.toR8_Unorm() == 0x12);
	static_assert(color.toR8G8_Unorm() == 0x3412);
	static_assert(color.toR8G8B8A8_Unorm() == color);
	static_assert(color.toR16G16_Unorm() == 0x34341212);
	static_assert(color.toR32_Float() == (0x12 / 255.0f));
	static_assert(Color{ 255, 0, 0, 255 }.toR10G10B10A2_Unorm() == 0xC00003FFu);

	CHECK(Color{ 0, 0, 0 }.toR11G11B10_UFloat() == 0x00000000u);
	CHECK(Color{ 255, 255, 255 }.toR11G11B10_UFloat() == 0x781E03C0u);
	CHECK(Color{ 254, 254, 254 }.toR11G11B10_UFloat() == 0x781DFBBFu);
}

TEST_CASE("Color.static conversion")
{
	static_assert(Color::Zero() == Color{ 0, 0, 0, 0 });
	static_assert(Color::ToUint8(-1.0) == 0);
	static_assert(Color::ToUint8(0.0) == 0);
	static_assert(Color::ToUint8(0.5) == 128);
	static_assert(Color::ToUint8(1.0) == 255);
	static_assert(Color::ToUint8(2.0) == 255);
	static_assert(Color::FromFloat(0.5) == Color{ 128, 128, 128, 255 });
	static_assert(Color::FromRGBA8888(0x12345678) == Color{ 0x12, 0x34, 0x56, 0x78 });
	static_assert(Color::FromABGR8888(0x78563412) == Color{ 0x12, 0x34, 0x56, 0x78 });
	static_assert(Alpha(0x12) == Color{ 255, 255, 255, 0x12 });
}

TEST_CASE("Color.stream")
{
	const Color color{ 1, 23, 45, 255 };

	std::ostringstream output;
	output << color;
	CHECK(output.str() == "(1, 23, 45, 255)");

	Color rgb;
	std::istringstream rgbInput{ "(12, 34, 56)" };
	rgbInput >> rgb;
	CHECK(rgb == Color{ 12, 34, 56, 255 });

	Color rgba;
	std::istringstream rgbaInput{ "(12, 34, 56, 78)" };
	rgbaInput >> rgba;
	CHECK(rgba == Color{ 12, 34, 56, 78 });

	Color hex;
	std::istringstream hexInput{ "#12345678" };
	hexInput >> hex;
	CHECK(hex == Color{ 0x12, 0x34, 0x56, 0x78 });
}

TEST_CASE("Color.fmt")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	CHECK(U"{}"_fmt(c0) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(c1) == U"(1, 2, 3, 4)");
	CHECK(U"{}"_fmt(c2) == U"(255, 254, 253, 252)");

	CHECK(U"{:02X}"_fmt(c0) == U"(00, 00, 00, 00)");
	CHECK(U"{:02X}"_fmt(c1) == U"(01, 02, 03, 04)");
	CHECK(U"{:02X}"_fmt(c2) == U"(FF, FE, FD, FC)");
}

TEST_CASE("Color.Format")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	CHECK(Format(c0) == U"(0, 0, 0, 0)");
	CHECK(Format(c1) == U"(1, 2, 3, 4)");
	CHECK(Format(c2) == U"(255, 254, 253, 252)");
}
