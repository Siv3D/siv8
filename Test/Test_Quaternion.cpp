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
	constexpr float QuaternionEpsilon = 1e-5f;

	void CheckComponents(const Quaternion& actual, const Float4 expected)
	{
		CHECK_EQ(actual.toFloat4(), expected);
	}

	void CheckVector(const Float3 actual, const Float3 expected, const float epsilon = QuaternionEpsilon)
	{
		CHECK(actual.epsilonEquals(expected, epsilon));
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

	const Quaternion rotation = Quaternion::RollPitchYaw(0.25f, -0.5f, 0.75f);
	const Float4 components = rotation.toFloat4();
	const Quaternion negative{ -components.x, -components.y, -components.z, -components.w };
	CHECK(rotation.rotationEquals(negative));
	CHECK_FALSE(rotation.epsilonEquals(negative, QuaternionEpsilon));
	CHECK(rotation.angleTo(negative) == doctest::Approx(0.0f).epsilon(QuaternionEpsilon));
	CHECK(Quaternion{}.angleTo(Quaternion::RotateZ(Math::HalfPiF))
		== doctest::Approx(Math::HalfPiF).epsilon(QuaternionEpsilon));
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
	CHECK(q.fastNormalized().isNormalized(0.02f));

	CheckComponents(q.conjugated(), Float4{ -1.0f, -2.0f, -2.0f, 4.0f });
	mutableValue = q;
	CHECK_EQ(&mutableValue.conjugate(), &mutableValue);
	CheckComponents(mutableValue, Float4{ -1.0f, -2.0f, -2.0f, 4.0f });
	CHECK(q.inverse().epsilonEquals(Quaternion{ -0.04f, -0.08f, -0.08f, 0.16f }, 0.000001f));

	const Quaternion zero{ 0.0f, 0.0f, 0.0f, 0.0f };
	CheckComponents(zero.normalized(), Float4::Zero());
	CHECK(zero.fastNormalized().hasNaN());
	CheckComponents(zero.inverse(), Float4::Zero());
}

TEST_CASE("Quaternion::canonicalization")
{
	const Quaternion q{ 1.0f, -2.0f, 3.0f, -4.0f };
	CheckComponents(q.canonicalized(), Float4{ -1.0f, 2.0f, -3.0f, 4.0f });
	CheckComponents(q, Float4{ 1.0f, -2.0f, 3.0f, -4.0f });

	Quaternion mutableValue{ -1.0f, 2.0f, -3.0f, 0.0f };
	CHECK_EQ(&mutableValue.canonicalize(), &mutableValue);
	CheckComponents(mutableValue, Float4{ 1.0f, -2.0f, 3.0f, 0.0f });

	CheckComponents(Quaternion{ 0.0f, -2.0f, 3.0f, 0.0f }.canonicalized(),
		Float4{ 0.0f, 2.0f, -3.0f, 0.0f });
	CheckComponents(Quaternion{ 0.0f, 0.0f, -3.0f, 0.0f }.canonicalized(),
		Float4{ 0.0f, 0.0f, 3.0f, 0.0f });
}

TEST_CASE("Quaternion::composition_and_rotation")
{
	const Quaternion first = Quaternion::RotateX(0.4f);
	const Quaternion second = Quaternion::RotateZ(-0.7f);
	const Float3 vector{ 0.25f, -0.5f, 1.0f };
	const Quaternion composed = (first * second);
	CheckVector(composed.rotate(vector), second.rotate(first.rotate(vector)));

	Quaternion mutableValue = first;
	CHECK_EQ(&(mutableValue *= second), &mutableValue);
	CHECK(mutableValue.rotationEquals(composed));
	CheckVector(composed.inverseRotate(composed.rotate(vector)), vector);
}

TEST_CASE("Quaternion::interpolation_and_transcendentals")
{
	const Quaternion from;
	const Quaternion to = Quaternion::RotateZ(Math::HalfPiF);
	const Quaternion expectedHalf = Quaternion::RotateZ(Math::QuarterPiF);

	CHECK(from.slerp(to, 0.0f).rotationEquals(from));
	CHECK(from.slerp(to, 1.0f).rotationEquals(to));
	CHECK(from.slerp(to, 0.5f).rotationEquals(expectedHalf));
	CHECK(from.nlerp(to, 0.0f).rotationEquals(from));
	CHECK(from.nlerp(to, 1.0f).rotationEquals(to));

	const Float4 toComponents = to.toFloat4();
	const Quaternion negativeTo{ -toComponents.x, -toComponents.y, -toComponents.z, -toComponents.w };
	CHECK(from.nlerp(negativeTo, 0.5f).rotationEquals(from.nlerp(to, 0.5f)));

	const Quaternion rotation = Quaternion::RollPitchYaw(0.3f, -0.4f, 0.5f);
	CHECK(rotation.log().exp().rotationEquals(rotation, 0.00001f));

	const auto [axis, angle] = Quaternion::RotationNormal(Float3::UnitZ(), 1.25f).toAxisAngle();
	CheckVector(axis, Float3::UnitZ());
	CHECK(angle == doctest::Approx(1.25f).epsilon(QuaternionEpsilon));

	const auto [identityAxis, identityAngle] = Quaternion{}.toAxisAngle();
	CheckVector(identityAxis, Float3::Zero());
	CHECK(identityAngle == doctest::Approx(0.0f).epsilon(QuaternionEpsilon));
}

TEST_CASE("Quaternion::factories")
{
	CHECK(Quaternion::RollPitchYaw(Float3{ 0.3f, -0.4f, 0.5f })
		.rotationEquals(Quaternion::RollPitchYaw(0.3f, -0.4f, 0.5f)));
	CHECK(Quaternion::RotationNormal(Float3::UnitX(), 0.75f)
		.rotationEquals(Quaternion::RotateX(0.75f)));
	CHECK(Quaternion::RotationAxis(Float3{ 2.0f, 0.0f, 0.0f }, 0.75f)
		.rotationEquals(Quaternion::RotateX(0.75f)));
	CHECK(Quaternion::RotationNormal(Float3::UnitY(), -0.5f)
		.rotationEquals(Quaternion::RotateY(-0.5f)));
	CHECK(Quaternion::RotationNormal(Float3::UnitZ(), 1.0f)
		.rotationEquals(Quaternion::RotateZ(1.0f)));

	const Vec3 from = Vec3::UnitX();
	const Vec3 to = Vec3::UnitY();
	CheckVector(Quaternion::FromUnitVectors(from, to).rotate(Float3{ from }), Float3{ to });
	CheckVector(Quaternion::FromUnitVectors(from, -from).rotate(Float3{ from }), Float3{ -from });
	CheckVector(Quaternion::FromVectors((from * 2.0), (to * 7.0)).rotate(Float3{ from }), Float3{ to });

	const Quaternion expected = Quaternion::RollPitchYaw(0.35f, -0.6f, 0.2f);
	const std::pair<Vec3, Vec3> sourcePair{ Vec3::UnitX(), Vec3::UnitY() };
	const std::pair<Vec3, Vec3> targetPair{
		Vec3{ expected.rotate(Float3::UnitX()) },
		Vec3{ expected.rotate(Float3::UnitY()) }
	};
	const Quaternion pairRotation = Quaternion::FromUnitVectorPairs(sourcePair, targetPair);
	CheckVector(pairRotation.rotate(Float3::UnitX()), Float3{ targetPair.first });
	CheckVector(pairRotation.rotate(Float3::UnitY()), Float3{ targetPair.second });
}

TEST_CASE("Quaternion::axis rotation directions")
{
	CheckVector(
		Quaternion::RotateX(Math::HalfPiF).rotate(Float3::UnitY()),
		Float3::UnitZ());
	CheckVector(
		Quaternion::RotateY(Math::HalfPiF).rotate(Float3::UnitX()),
		-Float3::UnitZ());
	CheckVector(
		Quaternion::RotateZ(Math::HalfPiF).rotate(Float3::UnitX()),
		Float3::UnitY());
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
