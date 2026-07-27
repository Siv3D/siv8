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

TEST_CASE("ColorF.type")
{
	static_assert(sizeof(ColorF) == (sizeof(double) * 4));
	static_assert(std::is_trivially_copyable_v<ColorF>);
	static_assert(std::is_standard_layout_v<ColorF>);
}

TEST_CASE("ColorF.constructor")
{
	static_assert(ColorF{ 0.25 } == ColorF{ 0.25, 0.25, 0.25, 1.0 });
	static_assert(ColorF{ 0.25, 0.5 } == ColorF{ 0.25, 0.25, 0.25, 0.5 });
	static_assert(ColorF{ ColorF{ 0.25, 0.5, 0.75, 1.0 }, 0.125 }
		== ColorF{ 0.25, 0.5, 0.75, 0.125 });
	static_assert(ColorF{ Vec3{ 0.25, 0.5, 0.75 } } == ColorF{ 0.25, 0.5, 0.75, 1.0 });
	static_assert(ColorF{ Vec4{ 0.25, 0.5, 0.75, 0.125 } } == ColorF{ 0.25, 0.5, 0.75, 0.125 });
	static_assert(ColorF{ Color{ 0x12, 0x34, 0x56, 0x78 } }.toColor() == Color{ 0x12, 0x34, 0x56, 0x78 });
	static_assert(ColorF{ Color{ 0x12, 0x34, 0x56, 0x78 }, 0.5 }.toColor() == Color{ 0x12, 0x34, 0x56, 0x80 });

	static_assert(ColorF{ U"#123" }.toColor() == Color{ 0x11, 0x22, 0x33, 0xFF });
	static_assert(ColorF{ U"#1234" }.toColor() == Color{ 0x11, 0x22, 0x33, 0x44 });
	static_assert(ColorF{ U"123456" }.toColor() == Color{ 0x12, 0x34, 0x56, 0xFF });
	static_assert(ColorF{ U"#12345678" }.toColor() == Color{ 0x12, 0x34, 0x56, 0x78 });
}

TEST_CASE("ColorF.operator ==")
{
	constexpr ColorF c0{ 0, 0, 0, 0 };
	constexpr ColorF c1{ 0.001, 0.002, 0.003, 0.004 };
	constexpr ColorF c2{ 0.494, 0.5, 0.504, 0.506 };

	static_assert(c0 == c0);
	static_assert(c1 == c1);
	static_assert(c2 == c2);

	static_assert(not(c0 == c1));
	static_assert(not(c0 == c2));
	static_assert(not(c1 == c2));
}

TEST_CASE("ColorF.operator !=")
{
	constexpr ColorF c0{ 0, 0, 0, 0 };
	constexpr ColorF c1{ 0.001, 0.002, 0.003, 0.004 };
	constexpr ColorF c2{ 0.494, 0.5, 0.504, 0.506 };

	static_assert(c0 != c1);
	static_assert(c0 != c2);
	static_assert(c1 != c2);

	static_assert(not(c0 != c0));
	static_assert(not(c1 != c1));
	static_assert(not(c2 != c2));
}

TEST_CASE("ColorF.arithmetic")
{
	constexpr ColorF lhs{ 0.25, 0.5, 0.75, 0.125 };
	constexpr ColorF rhs{ 0.5, 0.25, 0.125, 0.5 };

	static_assert((lhs + rhs) == ColorF{ 0.75, 0.75, 0.875, 0.125 });
	static_assert((lhs - rhs) == ColorF{ -0.25, 0.25, 0.625, 0.125 });
	static_assert((lhs * 2.0) == ColorF{ 0.5, 1.0, 1.5, 0.125 });
	static_assert((2.0 * lhs) == ColorF{ 0.5, 1.0, 1.5, 0.125 });
	static_assert((lhs * rhs) == ColorF{ 0.125, 0.125, 0.09375, 0.0625 });

	constexpr ColorF added = []
	{
		ColorF result{ 0.25, 0.5, 0.75, 0.125 };
		result += ColorF{ 0.5, 0.25, 0.125, 1.0 };
		return result;
	}();

	constexpr ColorF subtracted = []
	{
		ColorF result{ 0.25, 0.5, 0.75, 0.125 };
		result -= ColorF{ 0.5, 0.25, 0.125, 1.0 };
		return result;
	}();

	constexpr ColorF scaled = []
	{
		ColorF result{ 0.25, 0.5, 0.75, 0.125 };
		result *= 2.0;
		return result;
	}();

	constexpr ColorF multiplied = []
	{
		ColorF result{ 0.25, 0.5, 0.75, 0.125 };
		result *= ColorF{ 0.5, 0.25, 0.125, 0.5 };
		return result;
	}();

	static_assert(added == ColorF{ 0.75, 0.75, 0.875, 0.125 });
	static_assert(subtracted == ColorF{ -0.25, 0.25, 0.625, 0.125 });
	static_assert(scaled == ColorF{ 0.5, 1.0, 1.5, 0.125 });
	static_assert(multiplied == ColorF{ 0.125, 0.125, 0.09375, 0.0625 });
}

TEST_CASE("ColorF.component operations")
{
	constexpr ColorF color{ 0.25, 0.5, 0.75, 0.125 };

	static_assert((~color) == ColorF{ 0.75, 0.5, 0.25, 0.125 });
	static_assert(color.inverted() == ColorF{ 0.75, 0.5, 0.25, 0.125 });
	static_assert(color.withR(1.0) == ColorF{ 1.0, 0.5, 0.75, 0.125 });
	static_assert(color.withG(1.0) == ColorF{ 0.25, 1.0, 0.75, 0.125 });
	static_assert(color.withB(1.0) == ColorF{ 0.25, 0.5, 1.0, 0.125 });
	static_assert(color.withA(1.0) == ColorF{ 0.25, 0.5, 0.75, 1.0 });
	static_assert(color.withAlpha(1.0) == ColorF{ 0.25, 0.5, 0.75, 1.0 });
	static_assert(color.elem(0) == 0.25);
	static_assert(color.elem(1) == 0.5);
	static_assert(color.elem(2) == 0.75);
	static_assert(color.elem(3) == 0.125);
	static_assert(color.elem(4) == 0.0);
	static_assert(color.rg() == Vec2{ 0.25, 0.5 });
	static_assert(color.gb() == Vec2{ 0.5, 0.75 });
	static_assert(color.ba() == Vec2{ 0.75, 0.125 });
	static_assert(color.rgb() == Vec3{ 0.25, 0.5, 0.75 });
	static_assert(color.gba() == Vec3{ 0.5, 0.75, 0.125 });
	static_assert(color.bgr() == Vec3{ 0.75, 0.5, 0.25 });
	static_assert(color.rgba() == Vec4{ 0.25, 0.5, 0.75, 0.125 });
	static_assert(color.rgb0() == Vec4{ 0.25, 0.5, 0.75, 0.0 });
	static_assert(color.rgb1() == Vec4{ 0.25, 0.5, 0.75, 1.0 });
	static_assert(color.argb() == Vec4{ 0.125, 0.25, 0.5, 0.75 });
	static_assert(color.abgr() == Vec4{ 0.125, 0.75, 0.5, 0.25 });

	constexpr ColorF modified = []
	{
		ColorF result{ 0.0 };
		result.setR(0.125).setG(0.25).setB(0.5).setA(0.75);
		result.setRGB(0.25, 0.5, 0.75);
		return result;
	}();

	static_assert(modified == ColorF{ 0.25, 0.5, 0.75, 0.75 });
}

TEST_CASE("ColorF.premultiplied")
{
	constexpr ColorF color{ 1.0, 0.5, 0.25, 0.5 };
	constexpr ColorF premultiplied{ 0.5, 0.25, 0.125, 0.5 };

	static_assert(color.premultiplied() == premultiplied);
	static_assert(ColorF::PremultiplyAlpha(color) == premultiplied);
	static_assert(premultiplied.unpremultiplied() == color);
	static_assert(ColorF::UnpremultiplyAlpha(premultiplied) == color);
	static_assert(ColorF::UnpremultiplyAlpha(ColorF{ 1.0, 0.5, 0.25, 0.0 }) == ColorF::Zero());
}

TEST_CASE("ColorF.grayscale and min/max")
{
	constexpr ColorF color{ 0.25, 0.5, 0.75, 0.125 };

	CHECK(color.grayscale() == doctest::Approx(0.45375));
	static_assert(color.minRGBComponent() == 0.25);
	static_assert(color.maxRGBComponent() == 0.75);
	static_assert(color.minComponent() == 0.125);
	static_assert(color.maxComponent() == 0.75);
}

TEST_CASE("ColorF.lerp")
{
	constexpr ColorF from{ 0.0, 0.25, 0.5, 0.75 };
	constexpr ColorF to{ 0.5, 0.75, 1.0, 0.25 };

	static_assert(from.lerp(to, 0.0) == from);
	static_assert(from.lerp(to, 0.5) == ColorF{ 0.25, 0.5, 0.75, 0.5 });
	static_assert(from.lerp(to, 1.0) == to);
	static_assert(from.lerp(to, -1.0) == ColorF{ -0.5, -0.25, 0.0, 1.25 });
	static_assert(from.lerp(to, 2.0) == ColorF{ 1.0, 1.25, 1.5, -0.25 });

	static_assert(from.lightened(-1.0) == ColorF{ -1.0, -0.5, 0.0, 0.75 });
	static_assert(from.lightened(1.0) == ColorF{ 1.0, 1.0, 1.0, 0.75 });
	static_assert(from.lightened(2.0) == ColorF{ 2.0, 1.75, 1.5, 0.75 });
	static_assert(from.darkened(-1.0) == ColorF{ 0.0, 0.5, 1.0, 0.75 });
	static_assert(from.darkened(1.0) == ColorF{ 0.0, 0.0, 0.0, 0.75 });
	static_assert(from.darkened(2.0) == ColorF{ 0.0, -0.25, -0.5, 0.75 });
}

TEST_CASE("ColorF.color conversion")
{
	const ColorF color{ 1.0, 0.0, 0.0, 0.5 };

	const HSV hsv = color.toHSV();
	CHECK(hsv.h == doctest::Approx(0.0));
	CHECK(hsv.s == doctest::Approx(1.0));
	CHECK(hsv.v == doctest::Approx(1.0));
	CHECK(hsv.a == doctest::Approx(0.5));

	CHECK(color.adjustHue(120.0).r == doctest::Approx(0.0));
	CHECK(color.adjustHue(120.0).g == doctest::Approx(1.0));
	CHECK(color.adjustHue(120.0).b == doctest::Approx(0.0));
	CHECK(color.complemented().r == doctest::Approx(0.0));
	CHECK(color.complemented().g == doctest::Approx(1.0));
	CHECK(color.complemented().b == doctest::Approx(1.0));
	CHECK(color.gamma(1.0) == color);
	CHECK(color.gamma(0.0) == ColorF{ 0.0, 0.0, 0.0, 0.5 });

	const ColorF srgb{ 0.25, 0.5, 0.75, 0.125 };
	const ColorF roundTrip = srgb.removeSRGBCurve().applySRGBCurve();
	CHECK(roundTrip.r == doctest::Approx(srgb.r));
	CHECK(roundTrip.g == doctest::Approx(srgb.g));
	CHECK(roundTrip.b == doctest::Approx(srgb.b));
	CHECK(roundTrip.a == srgb.a);
}

TEST_CASE("ColorF.normalized format conversion")
{
	constexpr ColorF color{ 0.25, 0.5, 0.75, 1.0 };

	static_assert(color.toColor() == Color{ 0x40, 0x80, 0xBF, 0xFF });
	static_assert(color.toR8_Unorm() == 0x40);
	static_assert(color.toR8G8_Unorm() == 0x8040);
	static_assert(color.toR8G8B8A8_Unorm() == Color{ 0x40, 0x80, 0xBF, 0xFF });
	static_assert(color.toR16G16_Unorm() == 0x80004000u);
	static_assert(color.toR32_Float() == 0.25f);
	static_assert(color.toR10G10B10A2_Unorm() == 0xEFF80100u);
	static_assert(ColorF{ -1.0, 2.0, 0.0, 0.5 }.toR16G16_Unorm() == 0xFFFF0000u);

	CHECK(color.toFloat3() == Float3{ 0.25f, 0.5f, 0.75f });
	CHECK(color.toVec3() == Vec3{ 0.25, 0.5, 0.75 });
	CHECK(color.toFloat4() == Float4{ 0.25f, 0.5f, 0.75f, 1.0f });
	CHECK(color.toVec4() == Vec4{ 0.25, 0.5, 0.75, 1.0 });
	CHECK(color.toR32G32_Float() == Float2{ 0.25f, 0.5f });
	CHECK(color.toR32G32B32A32_Float() == Float4{ 0.25f, 0.5f, 0.75f, 1.0f });
	CHECK(color.toR16_Float().getBits() == HalfFloat{ 0.25f }.getBits());

	const uint16 r16 = HalfFloat{ 0.25f }.getBits();
	const uint16 g16 = HalfFloat{ 0.5f }.getBits();
	const uint16 b16 = HalfFloat{ 0.75f }.getBits();
	const uint16 a16 = HalfFloat{ 1.0f }.getBits();

	CHECK(color.toR16G16_Float() == ((static_cast<uint32>(g16) << 16) | r16));
	CHECK(color.toR16G16B16A16_Float()
		== ((static_cast<uint64>(a16) << 48)
			| (static_cast<uint64>(b16) << 32)
			| (static_cast<uint64>(g16) << 16)
			| r16));
}

TEST_CASE("ColorF.toR11G11B10_UFloat")
{
	CHECK(ColorF{ 0.0, 0.0, 0.0 }.toR11G11B10_UFloat() == 0x00000000u);
	CHECK(ColorF{ 1.0, 1.0, 1.0 }.toR11G11B10_UFloat() == 0x781E03C0u);
	CHECK(ColorF{ (254.0 / 255.0) }.toR11G11B10_UFloat() == 0x781DFBBFu);
	CHECK(ColorF{ 2.0, 2.0, 2.0 }.toR11G11B10_UFloat() == 0x80200400u);
	CHECK(ColorF{ 65024.0, 65024.0, 64512.0 }.toR11G11B10_UFloat() == 0xF7FDFFBFu);
	CHECK(ColorF{ 1e100, 1e100, 1e100 }.toR11G11B10_UFloat() == 0xF7FDFFBFu);
	CHECK(ColorF{ -1.0, -0.5, -0.25 }.toR11G11B10_UFloat() == 0x00000000u);

	CHECK(ColorF{ 0x1p-20, 0.0, 0.0 }.toR11G11B10_UFloat() == 0x00000001u);
	CHECK(ColorF{ 0.0, 0x1p-20, 0.0 }.toR11G11B10_UFloat() == 0x00000800u);
	CHECK(ColorF{ 0.0, 0.0, 0x1p-19 }.toR11G11B10_UFloat() == 0x00400000u);
	CHECK(ColorF{ 0x1p-21, 0.0, 0.0 }.toR11G11B10_UFloat() == 0x00000000u);

	CHECK(ColorF{ 1.9921875, 0.0, 0.0 }.toR11G11B10_UFloat() == 0x00000400u);
	CHECK(ColorF{ 0.0, 1.9921875, 0.0 }.toR11G11B10_UFloat() == 0x00200000u);
	CHECK(ColorF{ 0.0, 0.0, 1.984375 }.toR11G11B10_UFloat() == 0x80000000u);
}

TEST_CASE("ColorF.hash")
{
	const ColorF positiveZero{ 0.0, 0.0, 0.0, 0.0 };
	const ColorF negativeZero{ -0.0, -0.0, -0.0, -0.0 };

	CHECK(positiveZero == negativeZero);
	CHECK(positiveZero.hash() == negativeZero.hash());
	CHECK(std::hash<ColorF>{}(positiveZero) == std::hash<ColorF>{}(negativeZero));
}

TEST_CASE("ColorF.stream")
{
	const ColorF color{ 0.25, 0.5, 0.75, 1.0 };

	std::ostringstream output;
	output << color;
	CHECK(output.str() == "(0.25, 0.5, 0.75, 1)");

	ColorF rgb;
	std::istringstream rgbInput{ "(0.25, 0.5, 0.75)" };
	rgbInput >> rgb;
	CHECK(rgb == ColorF{ 0.25, 0.5, 0.75, 1.0 });

	ColorF rgba;
	std::istringstream rgbaInput{ "(0.25, 0.5, 0.75, 0.125)" };
	rgbaInput >> rgba;
	CHECK(rgba == ColorF{ 0.25, 0.5, 0.75, 0.125 });

	ColorF hex;
	std::istringstream hexInput{ "#12345678" };
	hexInput >> hex;
	CHECK(hex.toColor() == Color{ 0x12, 0x34, 0x56, 0x78 });
}

TEST_CASE("ColorF.fmt")
{
	constexpr ColorF c0{ 0, 0, 0, 0 };
	constexpr ColorF c1{ 0.001, 0.002, 0.003, 0.004 };
	constexpr ColorF c2{ 0.494, 0.5, 0.504, 0.506 };
	constexpr ColorF c3{ 0.9997, 0.9998, 0.9999, 1.0 };

	CHECK(U"{}"_fmt(c0) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(c1) == U"(0.001, 0.002, 0.003, 0.004)");
	CHECK(U"{}"_fmt(c2) == U"(0.494, 0.5, 0.504, 0.506)");
	CHECK(U"{}"_fmt(c3) == U"(0.9997, 0.9998, 0.9999, 1)");

	CHECK(U"{:.2f}"_fmt(c0) == U"(0.00, 0.00, 0.00, 0.00)");
	CHECK(U"{:.2f}"_fmt(c1) == U"(0.00, 0.00, 0.00, 0.00)");
	CHECK(U"{:.2f}"_fmt(c2) == U"(0.49, 0.50, 0.50, 0.51)");
	CHECK(U"{:.2f}"_fmt(c3) == U"(1.00, 1.00, 1.00, 1.00)");
}

TEST_CASE("ColorF.Format")
{
	constexpr ColorF c0{ 0, 0, 0, 0 };
	constexpr ColorF c1{ 0.001, 0.002, 0.003, 0.004 };
	constexpr ColorF c2{ 0.494, 0.5, 0.504, 0.506 };
	constexpr ColorF c3{ 0.9997, 0.9998, 0.9999, 1.0 };

	CHECK(Format(c0) == U"(0, 0, 0, 0)");
	CHECK(Format(c1) == U"(0.001, 0.002, 0.003, 0.004)");
	CHECK(Format(c2) == U"(0.494, 0.5, 0.504, 0.506)");
	CHECK(Format(c3) == U"(0.9997, 0.9998, 0.9999, 1)");
}
