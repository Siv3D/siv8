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
# include <array>
# include <limits>
# include <type_traits>

namespace
{
	template <class Value, class Float, size_t N>
	void CheckFiniteComponents(const Value baseline, const std::array<Float Value::*, N>& components)
	{
		static_assert(noexcept(baseline.isFinite()));
		static_assert(std::is_same_v<decltype(baseline.isFinite()), bool>);
		CHECK(baseline.isFinite());

		// Large finite components must not fail because their sum or squared length overflows.
		for (const Float finite : { Float{ 0 }, -Float{ 0 }, Float{ 1 }, Float{ -2 },
			std::numeric_limits<Float>::min(), std::numeric_limits<Float>::max(),
			std::numeric_limits<Float>::lowest(), std::numeric_limits<Float>::denorm_min() })
		{
			Value value = baseline;
			for (const auto component : components)
			{
				value.*component = finite;
			}
			CHECK(value.isFinite());
		}

		// Every component, including alpha / w / bottom, must reject either infinity and NaN.
		for (size_t i = 0; i < N; ++i)
		{
			CAPTURE(i);
			for (const Float nonFinite : { std::numeric_limits<Float>::infinity(),
				-std::numeric_limits<Float>::infinity(), std::numeric_limits<Float>::quiet_NaN() })
			{
				Value value = baseline;
				value.*components[i] = nonFinite;
				CHECK_FALSE(value.isFinite());
				value.*components[i] = baseline.*components[i];
				CHECK(value.isFinite());
			}
		}
	}
}

TEST_CASE_TEMPLATE("Vector.isFinite", Float, float, double, long double)
{
	using V2 = Vector2D<Float>;
	using V3 = Vector3D<Float>;
	using V4 = Vector4D<Float>;
	CheckFiniteComponents(V2{ 1, -2 }, std::array{ &V2::x, &V2::y });
	CheckFiniteComponents(V3{ 1, -2, 3 }, std::array{ &V3::x, &V3::y, &V3::z });
	CheckFiniteComponents(V4{ 1, -2, 3, -4 }, std::array{ &V4::x, &V4::y, &V4::z, &V4::w });

	const V3 infinite{ std::numeric_limits<Float>::infinity(), 0, 0 };
	CHECK_FALSE(infinite.hasNaN());
	CHECK_FALSE(infinite.isFinite());
}

TEST_CASE("ColorF.isFinite")
{
	// Finiteness includes alpha but imposes no color range restriction.
	CheckFiniteComponents(ColorF{ -1, 2, 3, -4 }, std::array{ &ColorF::r, &ColorF::g, &ColorF::b, &ColorF::a });
}

TEST_CASE("FloatRect.isFinite")
{
	// Inverted and collapsed bounds still have finite components.
	CheckFiniteComponents(FloatRect{ 3, 2, 1, -1 },
		std::array{ &FloatRect::left, &FloatRect::top, &FloatRect::right, &FloatRect::bottom });
}
