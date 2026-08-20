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
# include <limits>
# include <sstream>

namespace
{
	void CheckComponents(const SimdFloat4& actual, const Float4 expected)
	{
		CHECK_EQ(actual.toFloat4(), expected);
	}
}

TEST_CASE("SimdFloat4.constructors")
{
	CheckComponents(SimdFloat4{ 2.5f }, Float4{ 2.5f, 2.5f, 2.5f, 2.5f });
	CheckComponents(SimdFloat4{ 1.0f, 2.0f, 3.0f, 4.0f }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ 1, 2.0, 3u, 4LL }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });

	CheckComponents(SimdFloat4{ Float2{ 1.0f, 2.0f }, 3, 4.0 }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ 1, Vec2{ 2.0, 3.0 }, 4u }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ 1.0, 2, Float2{ 3.0f, 4.0f } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ Float2{ 1.0f, 2.0f }, Vec2{ 3.0, 4.0 } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ Float3{ 1.0f, 2.0f, 3.0f }, 4 }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ 1, Vec3{ 2.0, 3.0, 4.0 } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });

	CheckComponents(SimdFloat4{ Float4{ 1.0f, 2.0f, 3.0f, 4.0f } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ Vec4{ 1.0, 2.0, 3.0, 4.0 } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(SimdFloat4{ DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 4.0f) }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
}

TEST_CASE("SimdFloat4.arithmetic")
{
	const SimdFloat4 a{ 1.0f, 2.0f, 3.0f, 4.0f };
	const SimdFloat4 b{ 2.0f, 4.0f, 6.0f, 8.0f };

	CheckComponents(+a, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(-a, Float4{ -1.0f, -2.0f, -3.0f, -4.0f });
	CheckComponents((a + b), Float4{ 3.0f, 6.0f, 9.0f, 12.0f });
	CheckComponents((b - a), Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents((a * 2.0f), Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CheckComponents((2.0f * a), Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CheckComponents((a * b), Float4{ 2.0f, 8.0f, 18.0f, 32.0f });
	CheckComponents((b / 2.0f), Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents((b / a), Float4{ 2.0f, 2.0f, 2.0f, 2.0f });

	SimdFloat4 v = a;
	CHECK_EQ(&(v += b), &v);
	CheckComponents(v, Float4{ 3.0f, 6.0f, 9.0f, 12.0f });
	CHECK_EQ(&(v -= a), &v);
	CheckComponents(v, Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CHECK_EQ(&(v *= 2.0f), &v);
	CheckComponents(v, Float4{ 4.0f, 8.0f, 12.0f, 16.0f });
	CHECK_EQ(&(v *= SimdFloat4{ 0.5f }), &v);
	CheckComponents(v, Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CHECK_EQ(&(v /= 2.0f), &v);
	CheckComponents(v, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CHECK_EQ(&(v /= a), &v);
	CheckComponents(v, Float4::One());

	CHECK((a == SimdFloat4{ 1.0f, 2.0f, 3.0f, 4.0f }));
	CHECK_FALSE(a == b);
	CHECK(a.epsilonEquals(SimdFloat4{ 1.01f, 1.99f, 3.01f, 3.99f }, 0.011f));
	CHECK_FALSE(a.epsilonEquals(SimdFloat4{ 1.01f, 1.99f, 3.01f, 3.99f }, 0.009f));
}

TEST_CASE("SimdFloat4.access_and_mutation")
{
	SimdFloat4 v{ 1.0f, 2.0f, 3.0f, 4.0f };

	CHECK_EQ(v.toFloat3(), (Float3{ 1.0f, 2.0f, 3.0f }));
	CHECK_EQ(v.toFloat4(), (Float4{ 1.0f, 2.0f, 3.0f, 4.0f }));
	CHECK_EQ(v.elem(0), 1.0f);
	CHECK_EQ(v.elem(1), 2.0f);
	CHECK_EQ(v.elem(2), 3.0f);
	CHECK_EQ(v.elem(3), 4.0f);
	CHECK_EQ(v.elem(4), 0.0f);
	CHECK_EQ(v.getX(), 1.0f);
	CHECK_EQ(v.getY(), 2.0f);
	CHECK_EQ(v.getZ(), 3.0f);
	CHECK_EQ(v.getW(), 4.0f);
	const aligned_float4 native = v;
	CheckComponents(SimdFloat4{ native }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });

	CheckComponents(v.withX(10.0f), Float4{ 10.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(v.withY(20.0f), Float4{ 1.0f, 20.0f, 3.0f, 4.0f });
	CheckComponents(v.withZ(30.0f), Float4{ 1.0f, 2.0f, 30.0f, 4.0f });
	CheckComponents(v.withW(40.0f), Float4{ 1.0f, 2.0f, 3.0f, 40.0f });

	v.setX(10.0f);
	v.setY(20.0f);
	v.setZ(30.0f);
	v.setW(40.0f);
	CheckComponents(v, Float4{ 10.0f, 20.0f, 30.0f, 40.0f });
	CHECK_EQ(&v.set(1.0f, 2.0f, 3.0f, 4.0f), &v);
	CHECK_EQ(&v.set(Float4{ 5.0f, 6.0f, 7.0f, 8.0f }), &v);
	CHECK_EQ(&v.set(SimdFloat4{ 1.0f, 2.0f, 3.0f, 4.0f }), &v);
	CheckComponents(v, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });

	CheckComponents(v.movedBy(1.0f, 2.0f, 3.0f, 4.0f), Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CheckComponents(v.movedBy(SimdFloat4{ 1.0f }), Float4{ 2.0f, 3.0f, 4.0f, 5.0f });
	CheckComponents(v.withOffset(1.0f, 2.0f, 3.0f, 4.0f), Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CheckComponents(v.withOffset(SimdFloat4{ 1.0f }), Float4{ 2.0f, 3.0f, 4.0f, 5.0f });
	CheckComponents(v.withOffsetX(10.0f), Float4{ 11.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(v.withOffsetY(10.0f), Float4{ 1.0f, 12.0f, 3.0f, 4.0f });
	CheckComponents(v.withOffsetZ(10.0f), Float4{ 1.0f, 2.0f, 13.0f, 4.0f });
	CheckComponents(v.withOffsetW(10.0f), Float4{ 1.0f, 2.0f, 3.0f, 14.0f });

	CHECK_EQ(&v.moveBy(1.0f, 2.0f, 3.0f, 4.0f), &v);
	CheckComponents(v, Float4{ 2.0f, 4.0f, 6.0f, 8.0f });
	CHECK_EQ(&v.moveBy(SimdFloat4{ 1.0f }), &v);
	CheckComponents(v, Float4{ 3.0f, 5.0f, 7.0f, 9.0f });

	v.setZero();
	CHECK(v.isZero());
	CheckComponents(v, Float4::Zero());
}

TEST_CASE("SimdFloat4.math")
{
	const SimdFloat4 v{ 1.0f, 2.0f, 2.0f, 4.0f };
	CHECK_EQ(v.lengthSq(), 25.0f);
	CHECK_EQ(v.length(), 5.0f);

	CheckComponents(SimdFloat4{ 1.0f, 2.0f, 4.0f, 8.0f }.reciprocal(), Float4{ 1.0f, 0.5f, 0.25f, 0.125f });
	CHECK((SimdFloat4{ 1.0f, 2.0f, 4.0f, 8.0f }.fastReciprocal().epsilonEquals(
		SimdFloat4{ 1.0f, 0.5f, 0.25f, 0.125f }, 0.01f)));
	CheckComponents(SimdFloat4{ 1.0f, 4.0f, 9.0f, 16.0f }.sqrt(), Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CHECK((SimdFloat4{ 1.0f, 4.0f, 9.0f, 16.0f }.fastSqrt().epsilonEquals(
		SimdFloat4{ 1.0f, 2.0f, 3.0f, 4.0f }, 0.01f)));
	CheckComponents(SimdFloat4{ 1.0f, 4.0f, 16.0f, 64.0f }.rsqrt(), Float4{ 1.0f, 0.5f, 0.25f, 0.125f });
	CHECK((SimdFloat4{ 1.0f, 4.0f, 16.0f, 64.0f }.fastRsqrt().epsilonEquals(
		SimdFloat4{ 1.0f, 0.5f, 0.25f, 0.125f }, 0.01f)));

	const SimdFloat4 normalized = v.normalized();
	CHECK(normalized.epsilonEquals(SimdFloat4{ 0.2f, 0.4f, 0.4f, 0.8f }, 0.000001f));
	SimdFloat4 mutableValue = v;
	CHECK_EQ(&mutableValue.normalize(), &mutableValue);
	CHECK(mutableValue.epsilonEquals(normalized, 0.000001f));
	CheckComponents(SimdFloat4::Zero().normalized(), Float4::Zero());

	CheckComponents(SimdFloat4::Zero().lerp(SimdFloat4{ 2.0f, 4.0f, 6.0f, 8.0f }, 0.25f),
		Float4{ 0.5f, 1.0f, 1.5f, 2.0f });
}

TEST_CASE("SimdFloat4.classification_swizzles_and_constants")
{
	CHECK(SimdFloat4::Zero().isZero());
	CHECK(SimdFloat4{ -0.0f }.isZero());
	CHECK_FALSE(SimdFloat4::One().isZero());
	CHECK(SimdFloat4::NaN().hasNaN());
	CHECK_FALSE(SimdFloat4::NaN().isFinite());
	CHECK((SimdFloat4{ 0.0f, std::numeric_limits<float>::infinity(), 0.0f, 0.0f }.hasInf()));
	CHECK_FALSE((SimdFloat4{ 0.0f, std::numeric_limits<float>::infinity(), 0.0f, 0.0f }.isFinite()));
	CHECK(SimdFloat4::One().isFinite());

	const SimdFloat4 v{ 1.0f, 2.0f, 3.0f, 4.0f };
	CHECK_EQ(v.xy(), (Float2{ 1.0f, 2.0f }));
	CHECK_EQ(v.yz(), (Float2{ 2.0f, 3.0f }));
	CHECK_EQ(v.zw(), (Float2{ 3.0f, 4.0f }));
	CHECK_EQ(v.xyz(), (Float3{ 1.0f, 2.0f, 3.0f }));
	CHECK_EQ(v.yzw(), (Float3{ 2.0f, 3.0f, 4.0f }));
	CheckComponents(v.xyz0(), Float4{ 1.0f, 2.0f, 3.0f, 0.0f });
	CheckComponents(v.xyz1(), Float4{ 1.0f, 2.0f, 3.0f, 1.0f });

	CheckComponents(SimdFloat4::Zero(), Float4{ 0.0f, 0.0f, 0.0f, 0.0f });
	CheckComponents(SimdFloat4::One(), Float4{ 1.0f, 1.0f, 1.0f, 1.0f });
	CheckComponents(SimdFloat4::All(2.0f), Float4{ 2.0f, 2.0f, 2.0f, 2.0f });
	CheckComponents(SimdFloat4::UnitX(), Float4{ 1.0f, 0.0f, 0.0f, 0.0f });
	CheckComponents(SimdFloat4::UnitY(), Float4{ 0.0f, 1.0f, 0.0f, 0.0f });
	CheckComponents(SimdFloat4::UnitZ(), Float4{ 0.0f, 0.0f, 1.0f, 0.0f });
	CheckComponents(SimdFloat4::UnitW(), Float4{ 0.0f, 0.0f, 0.0f, 1.0f });
}

TEST_CASE("SimdFloat4.format_and_stream")
{
	const SimdFloat4 v{ 1.25f, 2.5f, 3.75f, 4.0f };
	CHECK_EQ(Format(v), U"(1.25, 2.5, 3.75, 4)");
	CHECK_EQ(fmt::format("{}", v), "(1.25, 2.5, 3.75, 4)");
	CHECK_EQ(fmt::format("{:.1f}", v), "(1.2, 2.5, 3.8, 4.0)");
	CHECK_EQ(U"{}"_fmt(v), U"(1.25, 2.5, 3.75, 4)");
	CHECK_EQ(U"{:.1f}"_fmt(v), U"(1.2, 2.5, 3.8, 4.0)");

	std::stringstream output;
	output << v;
	CHECK_EQ(output.str(), "(1.25, 2.5, 3.75, 4)");

	std::stringstream input{ "(1, 2, 3, 4)" };
	SimdFloat4 parsed = SimdFloat4::Zero();
	input >> parsed;
	CHECK(input);
	CheckComponents(parsed, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
}
