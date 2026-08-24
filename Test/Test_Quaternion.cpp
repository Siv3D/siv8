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
# include <type_traits>

namespace
{
	void CheckComponents(const Quaternion& actual, const Float4 expected)
	{
		CHECK_EQ(actual.toFloat4(), expected);
	}
}

TEST_CASE("Quaternion::constructors")
{
	static_assert(sizeof(Quaternion) == 16);
	static_assert(alignof(Quaternion) == 16);
	static_assert(std::is_trivially_copyable_v<Quaternion>);
	static_assert(std::is_trivially_copy_assignable_v<Quaternion>);
	static_assert(not std::is_trivially_default_constructible_v<Quaternion>);

	Quaternion identity;
	CheckComponents(identity, Float4{ 0.0f, 0.0f, 0.0f, 1.0f });
	CHECK(identity.isIdentity());

	const Quaternion copied = identity;
	CHECK(copied == identity);
	Quaternion assigned;
	CHECK_EQ(&(assigned = Quaternion{ 1.0f, 2.0f, 3.0f, 4.0f }), &assigned);
	CheckComponents(assigned, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });

	CheckComponents(Quaternion{ 1.0f, 2.0f, 3.0f, 4.0f }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(Quaternion{ 1, 2.0, 3u, 4LL }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(Quaternion{ Float4{ 1.0f, 2.0f, 3.0f, 4.0f } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CheckComponents(Quaternion{ SimdFloat4{ 1.0f, 2.0f, 3.0f, 4.0f } }, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
	CHECK(Quaternion::Identity() == identity);
}

TEST_CASE("Quaternion::comparison_and_access")
{
	const Quaternion q{ 1.0f, 2.0f, 3.0f, 4.0f };

	CHECK(q == Quaternion{ 1.0f, 2.0f, 3.0f, 4.0f });
	CHECK_FALSE(q == Quaternion{ -1.0f, -2.0f, -3.0f, -4.0f });
	CHECK(q.epsilonEquals(Quaternion{ 1.01f, 1.99f, 3.01f, 3.99f }, 0.011f));
	CHECK_FALSE(q.epsilonEquals(Quaternion{ 1.01f, 1.99f, 3.01f, 3.99f }, 0.009f));

	CHECK_EQ(q.xyz(), (Float3{ 1.0f, 2.0f, 3.0f }));
	CHECK_EQ(q.getX(), 1.0f);
	CHECK_EQ(q.getY(), 2.0f);
	CHECK_EQ(q.getZ(), 3.0f);
	CHECK_EQ(q.getW(), 4.0f);
}

TEST_CASE("Quaternion::classification")
{
	CHECK(Quaternion{}.isFinite());
	CHECK(Quaternion{}.isIdentity());
	CHECK(Quaternion{}.isNormalized());
	CHECK_FALSE(Quaternion{ 0.0f, 0.0f, 0.0f, 2.0f }.isNormalized());
	CHECK(Quaternion{ 0.0f, 0.0f, 0.0f, 1.000004f }.isNormalized());
	CHECK_FALSE(Quaternion{ 0.0f, 0.0f, 0.0f, 1.000006f }.isNormalized());

	const float nan = std::numeric_limits<float>::quiet_NaN();
	const float inf = std::numeric_limits<float>::infinity();
	CHECK(Quaternion{ nan, 0.0f, 0.0f, 1.0f }.hasNaN());
	CHECK_FALSE(Quaternion{ nan, 0.0f, 0.0f, 1.0f }.isFinite());
	CHECK(Quaternion{ 0.0f, inf, 0.0f, 1.0f }.hasInf());
	CHECK_FALSE(Quaternion{ 0.0f, inf, 0.0f, 1.0f }.isFinite());
}

TEST_CASE("Quaternion::math")
{
	const Quaternion q{ 1.0f, 2.0f, 2.0f, 4.0f };
	CHECK_EQ(q.dot(q), 25.0f);
	CHECK_EQ(q.lengthSq(), 25.0f);
	CHECK_EQ(q.length(), 5.0f);

	const Quaternion normalized = q.normalized();
	CHECK(normalized.epsilonEquals(Quaternion{ 0.2f, 0.4f, 0.4f, 0.8f }, 0.000001f));
	CHECK(normalized.isNormalized());

	Quaternion mutableValue = q;
	CHECK_EQ(&mutableValue.normalize(), &mutableValue);
	CHECK(mutableValue.epsilonEquals(normalized, 0.000001f));

	CheckComponents(q.conjugated(), Float4{ -1.0f, -2.0f, -2.0f, 4.0f });
	CHECK(q.inverse().epsilonEquals(Quaternion{ -0.04f, -0.08f, -0.08f, 0.16f }, 0.000001f));

	const Quaternion zero{ 0.0f, 0.0f, 0.0f, 0.0f };
	CheckComponents(zero.normalized(), Float4::Zero());
	CheckComponents(zero.inverse(), Float4::Zero());
}

TEST_CASE("Quaternion::format_and_stream")
{
	const Quaternion q{ 1.25f, 2.5f, 3.75f, 4.0f };
	CHECK_EQ(Format(q), U"(1.25, 2.5, 3.75, 4)");
	CHECK_EQ(fmt::format("{}", q), "(1.25, 2.5, 3.75, 4)");
	CHECK_EQ(fmt::format("{:.1f}", q), "(1.2, 2.5, 3.8, 4.0)");
	CHECK_EQ(U"{}"_fmt(q), U"(1.25, 2.5, 3.75, 4)");
	CHECK_EQ(U"{:.1f}"_fmt(q), U"(1.2, 2.5, 3.8, 4.0)");

	std::stringstream output;
	output << q;
	CHECK_EQ(output.str(), "(1.25, 2.5, 3.75, 4)");

	std::stringstream input{ "(1, 2, 3, 4)" };
	Quaternion parsed;
	input >> parsed;
	CHECK(input);
	CheckComponents(parsed, Float4{ 1.0f, 2.0f, 3.0f, 4.0f });
}
