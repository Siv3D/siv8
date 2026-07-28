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

namespace
{
	template <class Type>
	concept HasAddition = requires(Type value)
	{
		value + value;
	};

	template <class Type>
	concept HasSubtraction = requires(Type value)
	{
		value - value;
	};
}

TEST_CASE("HSV.type")
{
	static_assert(sizeof(HSV) == (sizeof(double) * 4));
	static_assert(std::is_trivially_copyable_v<HSV>);
	static_assert(std::is_standard_layout_v<HSV>);
	static_assert(not HasAddition<HSV>);
	static_assert(not HasSubtraction<HSV>);
}

TEST_CASE("HSV.constructor")
{
	static_assert(HSV{ 120.0 } == HSV{ 120.0, 1.0, 1.0, 1.0 });
	static_assert(HSV{ 120, 0.25 } == HSV{ 120.0, 1.0, 1.0, 0.25 });
	static_assert(HSV{ 120, 0.5, 0.75 } == HSV{ 120.0, 0.5, 0.75, 1.0 });
	static_assert(HSV{ HSV{ 120.0, 0.5, 0.75, 0.25 }, 0.5 } == HSV{ 120.0, 0.5, 0.75, 0.5 });
	static_assert(HSV{ Vec3{ 120.0, 0.5, 0.75 }, 0.25 } == HSV{ 120.0, 0.5, 0.75, 0.25 });
	static_assert(HSV{ Vec4{ 120.0, 0.5, 0.75, 0.25 } } == HSV{ 120.0, 0.5, 0.75, 0.25 });

	CHECK(HSV{ Color{ 255, 0, 0, 128 } }.h == doctest::Approx(0.0));
	CHECK(HSV{ Color{ 255, 0, 0, 128 } }.s == doctest::Approx(1.0));
	CHECK(HSV{ Color{ 255, 0, 0, 128 } }.v == doctest::Approx(1.0));
	CHECK(HSV{ Color{ 255, 0, 0, 128 } }.a == doctest::Approx(128.0 / 255.0));

	const HSV gray{ ColorF{ 0.5, 0.5, 0.5, 0.25 } };
	CHECK(gray.h == doctest::Approx(0.0));
	CHECK(gray.s == doctest::Approx(0.0));
	CHECK(gray.v == doctest::Approx(0.5));
	CHECK(gray.a == doctest::Approx(0.25));

	const HSV tinyRed{ ColorF{ 1e-30, 0.0, 0.0, 0.25 } };
	CHECK(tinyRed.h == doctest::Approx(0.0));
	CHECK(tinyRed.s == doctest::Approx(1.0));
	CHECK(tinyRed.v == doctest::Approx(1e-30));
	CHECK(tinyRed.a == doctest::Approx(0.25));
}

TEST_CASE("HSV.component operations")
{
	constexpr HSV color{ 120.0, 0.5, 0.75, 0.25 };

	static_assert(color.elem(0) == 120.0);
	static_assert(color.elem(1) == 0.5);
	static_assert(color.elem(2) == 0.75);
	static_assert(color.elem(3) == 0.25);
	static_assert(color.elem(4) == 0.0);
	static_assert(color.withH(240.0) == HSV{ 240.0, 0.5, 0.75, 0.25 });
	static_assert(color.withS(0.25) == HSV{ 120.0, 0.25, 0.75, 0.25 });
	static_assert(color.withV(0.25) == HSV{ 120.0, 0.5, 0.25, 0.25 });
	static_assert(color.withA(0.5) == HSV{ 120.0, 0.5, 0.75, 0.5 });
	static_assert(color.withAlpha(0.5) == HSV{ 120.0, 0.5, 0.75, 0.5 });
	static_assert(color.hueShifted(360.0) == HSV{ 480.0, 0.5, 0.75, 0.25 });
	static_assert(color.complemented() == HSV{ 300.0, 0.5, 0.75, 0.25 });
	static_assert(HSV{ 300.0, 0.5, 0.75, 0.25 }.complemented()
		== HSV{ 480.0, 0.5, 0.75, 0.25 });
	static_assert(color.hsv() == Vec3{ 120.0, 0.5, 0.75 });
	static_assert(color.sva() == Vec3{ 0.5, 0.75, 0.25 });
	static_assert(color.hsva() == Vec4{ 120.0, 0.5, 0.75, 0.25 });

	constexpr HSV modified = []
	{
		HSV result{ 0.0, 0.0, 0.0, 0.0 };
		result.setH(60.0).setS(0.25).setV(0.5).setA(0.75);
		result.setHSV(120.0, 0.5, 0.75);
		return result;
	}();

	static_assert(modified == HSV{ 120.0, 0.5, 0.75, 0.75 });

	HSV pointerAccess{ color };
	double* const p = pointerAccess.getPointer();
	p[0] = 240.0;
	p[3] = 0.5;
	CHECK(pointerAccess == HSV{ 240.0, 0.5, 0.75, 0.5 });
}

TEST_CASE("HSV.lerp")
{
	constexpr HSV from{ 350.0, 0.2, 0.4, 0.6 };
	constexpr HSV to{ 10.0, 0.8, 1.0, 0.2 };

	static_assert(from.lerp(to, 0.0) == from);
	static_assert(from.lerp(to, 1.0) == to);

	const HSV linear = from.lerp(to, 0.5);
	CHECK(linear.h == doctest::Approx(180.0));
	CHECK(linear.s == doctest::Approx(0.5));
	CHECK(linear.v == doctest::Approx(0.7));
	CHECK(linear.a == doctest::Approx(0.4));

	const HSV shortest = from.lerpShortestHue(to, 0.5);
	CHECK(shortest.h == doctest::Approx(360.0));
	CHECK(shortest.s == doctest::Approx(0.5));
	CHECK(shortest.v == doctest::Approx(0.7));
	CHECK(shortest.a == doctest::Approx(0.4));

	CHECK(to.lerpShortestHue(from, 0.5).h == doctest::Approx(0.0));
	CHECK(from.lerpShortestHue(to, 2.0).h == doctest::Approx(390.0));
	CHECK(HSV{ 0.0 }.lerpShortestHue(HSV{ 180.0 }, 0.5).h == doctest::Approx(-90.0));
	CHECK(HSV{ 720.0 }.lerpShortestHue(HSV{ -350.0 }, 0.5).h == doctest::Approx(725.0));
}

TEST_CASE("HSV.toColor")
{
	CHECK(HSV{ 0.0 }.toColor() == Color{ 255, 0, 0 });
	CHECK(HSV{ 60.0 }.toColor() == Color{ 255, 255, 0 });
	CHECK(HSV{ 120.0 }.toColor() == Color{ 0, 255, 0 });
	CHECK(HSV{ 180.0 }.toColor() == Color{ 0, 255, 255 });
	CHECK(HSV{ 240.0 }.toColor() == Color{ 0, 0, 255 });
	CHECK(HSV{ 300.0 }.toColor() == Color{ 255, 0, 255 });
	CHECK(HSV{ 360.0 }.toColor() == Color{ 255, 0, 0 });
	CHECK(HSV{ -60.0 }.toColor() == Color{ 255, 0, 255 });
	CHECK(HSV{ 720.0 }.toColor() == Color{ 255, 0, 0 });

	CHECK(HSV{ 120.0, 0.5, 0.75, 0.25 }.toColor() == Color{ 96, 191, 96, 64 });
	CHECK(HSV{ 120.0, 0.5, 0.75, 0.25 }.toColor(128) == Color{ 96, 191, 96, 128 });
	CHECK(HueToColor(-60.0) == Color{ 255, 0, 255 });

	for (const double hue : { -721.0, -60.5, 0.0, 59.5, 360.0, 721.0 })
	{
		CHECK(HueToColor(hue) == HSV{ hue }.toColor());
	}

	constexpr uint8 values[] = { 0, 1, 2, 63, 127, 128, 191, 254, 255 };

	for (const uint8 r : values)
	{
		for (const uint8 g : values)
		{
			for (const uint8 b : values)
			{
				const Color color{ r, g, b, 123 };
				CHECK(HSV{ color }.toColor() == color);
			}
		}
	}
}

TEST_CASE("HSV.toColorF")
{
	const ColorF red = HSV{ 0.0 }.toColorF();
	CHECK(red.r == doctest::Approx(1.0));
	CHECK(red.g == doctest::Approx(0.0));
	CHECK(red.b == doctest::Approx(0.0));
	CHECK(red.a == doctest::Approx(1.0));

	const ColorF color = HSV{ 120.0, 0.5, 0.75, 0.25 }.toColorF();
	CHECK(color.r == doctest::Approx(0.375));
	CHECK(color.g == doctest::Approx(0.75));
	CHECK(color.b == doctest::Approx(0.375));
	CHECK(color.a == doctest::Approx(0.25));
	CHECK(HSV{ 120.0, 0.5, 0.75, 0.25 }.toColorF(0.5).a == doctest::Approx(0.5));

	const ColorF hue = HueToColorF(-60.0);
	CHECK(hue.r == doctest::Approx(1.0));
	CHECK(hue.g == doctest::Approx(0.0));
	CHECK(hue.b == doctest::Approx(1.0));
	CHECK(hue.a == doctest::Approx(1.0));

	for (const double hueValue : { -721.0, -60.5, 0.0, 59.5, 360.0, 721.0 })
	{
		const ColorF actual = HueToColorF(hueValue);
		const ColorF expected = HSV{ hueValue }.toColorF();
		CHECK(actual.r == doctest::Approx(expected.r));
		CHECK(actual.g == doctest::Approx(expected.g));
		CHECK(actual.b == doctest::Approx(expected.b));
		CHECK(actual.a == doctest::Approx(expected.a));
	}

	const ColorF source{ 0.125, 0.25, 0.75, 0.5 };
	const ColorF roundTrip = HSV{ source }.toColorF();
	CHECK(roundTrip.r == doctest::Approx(source.r));
	CHECK(roundTrip.g == doctest::Approx(source.g));
	CHECK(roundTrip.b == doctest::Approx(source.b));
	CHECK(roundTrip.a == doctest::Approx(source.a));
}

TEST_CASE("HSV.color space conversion")
{
	const ColorF linear = HSV{ 0.0, 1.0, 0.5, 0.25 }.srgbToLinear();
	CHECK(linear.r == doctest::Approx(0.21404114048223255));
	CHECK(linear.g == doctest::Approx(0.0));
	CHECK(linear.b == doctest::Approx(0.0));
	CHECK(linear.a == doctest::Approx(0.25));

	const ColorF srgb = HSV{ 0.0, 1.0, 0.5, 0.25 }.linearToSRGB();
	CHECK(srgb.r == doctest::Approx(0.7353569830524495));
	CHECK(srgb.g == doctest::Approx(0.0));
	CHECK(srgb.b == doctest::Approx(0.0));
	CHECK(srgb.a == doctest::Approx(0.25));
}

TEST_CASE("HSV.hash")
{
	constexpr HSV positiveZero{ 0.0, 0.0, 0.0, 0.0 };
	constexpr HSV negativeZero{ -0.0, -0.0, -0.0, -0.0 };

	CHECK(positiveZero == negativeZero);
	CHECK(positiveZero.hash() == negativeZero.hash());
	CHECK(std::hash<HSV>{}(positiveZero) == std::hash<HSV>{}(negativeZero));
}

TEST_CASE("HSV.stream")
{
	const HSV color{ 120.0, 0.5, 0.75, 0.25 };

	std::ostringstream output;
	output << color;
	CHECK(output.str() == "(120, 0.5, 0.75, 0.25)");

	HSV inputColor;
	std::istringstream input{ "(240, 0.25, 0.5, 0.75)" };
	input >> inputColor;
	CHECK(inputColor == HSV{ 240.0, 0.25, 0.5, 0.75 });
}

TEST_CASE("HSV.fmt")
{
	constexpr HSV color{ 120.0, 0.5, 0.75, 0.25 };

	CHECK(U"{}"_fmt(color) == U"(120, 0.5, 0.75, 0.25)");
	CHECK(U"{:.2f}"_fmt(color) == U"(120.00, 0.50, 0.75, 0.25)");
}

TEST_CASE("HSV.Format")
{
	CHECK(Format(HSV{ 120.0, 0.5, 0.75, 0.25 }) == U"(120, 0.5, 0.75, 0.25)");
	CHECK(Format(HSV::Zero()) == U"(0, 0, 0, 0)");
}
