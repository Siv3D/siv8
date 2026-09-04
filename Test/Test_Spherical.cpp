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
	constexpr double SphericalEpsilon = 1e-10;

	void CheckSphericalPosition(const Spherical& value, const Vec3 expected)
	{
		CHECK(value.toVec3().epsilonEquals(expected, SphericalEpsilon));
		CHECK(value.toPosition().epsilonEquals(expected, SphericalEpsilon));
		CHECK(static_cast<Vec3>(value).epsilonEquals(expected, SphericalEpsilon));
		CHECK(value.toFloat3().epsilonEquals(Float3{ expected }, 1e-5f));
		CHECK(value.fastToVec3().epsilonEquals(expected, 1e-4));
		CHECK(value.fastToFloat3().epsilonEquals(Float3{ expected }, 1e-4f));
	}
}

TEST_CASE("Spherical::constructors")
{
	static_assert(sizeof(Spherical) == (sizeof(double) * 3));
	static_assert(sizeof(SphericalF) == (sizeof(float) * 3));
	static_assert(std::is_trivially_copyable_v<Spherical>);

	constexpr Spherical value{ 2, 0.5, -3 };
	static_assert(value.r == 2.0);
	static_assert(value.theta == 0.5);
	static_assert(value.phi == -3.0);

	constexpr Spherical named0{ Arg::r = 2.0, Arg::theta = 0.5, Arg::phi = -3.0 };
	constexpr Spherical named1{ Arg::r = 2.0, Arg::phi = -3.0, Arg::theta = 0.5 };
	constexpr Spherical named2{ Arg::theta = 0.5, Arg::r = 2.0, Arg::phi = -3.0 };
	constexpr Spherical named3{ Arg::theta = 0.5, Arg::phi = -3.0, Arg::r = 2.0 };
	constexpr Spherical named4{ Arg::phi = -3.0, Arg::r = 2.0, Arg::theta = 0.5 };
	constexpr Spherical named5{ Arg::phi = -3.0, Arg::theta = 0.5, Arg::r = 2.0 };
	static_assert(named0.r == value.r && named0.theta == value.theta && named0.phi == value.phi);
	static_assert(named1.r == value.r && named1.theta == value.theta && named1.phi == value.phi);
	static_assert(named2.r == value.r && named2.theta == value.theta && named2.phi == value.phi);
	static_assert(named3.r == value.r && named3.theta == value.theta && named3.phi == value.phi);
	static_assert(named4.r == value.r && named4.theta == value.theta && named4.phi == value.phi);
	static_assert(named5.r == value.r && named5.theta == value.theta && named5.phi == value.phi);

	const Spherical fromPosition{ Vec3{ 1.25, -2.5, 3.75 } };
	CHECK(fromPosition.toVec3().epsilonEquals(Vec3{ 1.25, -2.5, 3.75 }, SphericalEpsilon));

	const Spherical origin{ Vec3::Zero() };
	CHECK_EQ(origin.r, 0.0);
	CHECK_EQ(origin.theta, 0.0);
	CHECK_EQ(origin.phi, 0.0);

	const Spherical positiveY{ Vec3{ 0.0, 3.0, 0.0 } };
	CHECK_EQ(positiveY.r, 3.0);
	CHECK_EQ(positiveY.theta, 0.0);
	CHECK_EQ(positiveY.phi, 0.0);

	const Spherical negativeY{ Vec3{ 0.0, -3.0, 0.0 } };
	CHECK_EQ(negativeY.r, 3.0);
	CHECK(negativeY.theta == doctest::Approx(Math::Pi));
	CHECK_EQ(negativeY.phi, 0.0);
}

TEST_CASE("Spherical::coordinate_convention")
{
	CheckSphericalPosition(Spherical{ 2.0, 0.0, 1.0 }, Vec3{ 0.0, 2.0, 0.0 });
	CheckSphericalPosition(Spherical{ 2.0, Math::Pi, 1.0 }, Vec3{ 0.0, -2.0, 0.0 });
	CheckSphericalPosition(Spherical{ 2.0, Math::HalfPi, 0.0 }, Vec3{ 2.0, 0.0, 0.0 });
	CheckSphericalPosition(Spherical{ 2.0, Math::HalfPi, Math::HalfPi }, Vec3{ 0.0, 0.0, -2.0 });
	CheckSphericalPosition(Spherical{ 2.0, Math::HalfPi, -Math::HalfPi }, Vec3{ 0.0, 0.0, 2.0 });

	constexpr float r = 2.0f;
	constexpr float theta = 0.75f;
	const Float3 base{ (r * std::sin(theta)), (r * std::cos(theta)), 0.0f };
	for (const float phi : { -2.0f, -0.5f, 0.0f, 0.75f, 2.5f })
	{
		const Float3 expected = Quaternion::RotateY(phi).rotate(base);
		CHECK(SphericalF{ r, theta, phi }.toFloat3().epsilonEquals(expected, 1e-5f));
	}
}

TEST_CASE("Spherical::operations")
{
	constexpr Spherical value{ 2.0, 0.5, 1.0 };
	static_assert((+value).r == 2.0);
	static_assert(value.withR(4.0).r == 4.0);
	static_assert(value.withTheta(1.5).theta == 1.5);
	static_assert(value.withPhi(2.0).phi == 2.0);
	static_assert(value.withOffsetR(4.0).r == 6.0);
	static_assert(value.withOffsetTheta(1.5).theta == 2.0);
	static_assert(value.withOffsetPhi(2.0).phi == 3.0);
	static_assert(value.rotated(1.0).phi == 2.0);

	Spherical mutableValue = value;
	CHECK_EQ(&mutableValue.rotate(1.0), &mutableValue);
	CHECK_EQ(mutableValue.phi, 2.0);

	CHECK((-value).toVec3().epsilonEquals(-value.toVec3(), SphericalEpsilon));
	CHECK((value + Vec3{ 1.0, 2.0, 3.0 }).epsilonEquals(value.toVec3() + Vec3{ 1.0, 2.0, 3.0 }, SphericalEpsilon));
	CHECK((value - Vec3{ 1.0, 2.0, 3.0 }).epsilonEquals(value.toVec3() - Vec3{ 1.0, 2.0, 3.0 }, SphericalEpsilon));

	const Spherical interpolated = Spherical{ 2.0, 0.25, (Math::Pi - 0.1) }
		.lerp(Spherical{ 4.0, 0.75, (-Math::Pi + 0.1) }, 0.5);
	CHECK_EQ(interpolated.r, 3.0);
	CHECK_EQ(interpolated.theta, 0.5);
	CHECK(std::abs(std::abs(interpolated.phi) - Math::Pi) < SphericalEpsilon);

	const SphericalF interpolatedF = SphericalF{ 2.0f, 0.25f, 0.5f }.lerp(SphericalF{ 4.0f, 0.75f, 1.0f }, 0.5);
	CHECK_EQ(interpolatedF.r, 3.0f);
	CHECK_EQ(interpolatedF.theta, 0.5f);
	CHECK_EQ(interpolatedF.phi, 0.75f);
}

TEST_CASE("Spherical::format_hash_and_json")
{
	const Spherical value{ 1.5, -2.25, 3.75 };
	CHECK_EQ(Format(value), U"(1.5, -2.25, 3.75)");
	CHECK_EQ(U"{:.1f}"_fmt(value), U"(1.5, -2.2, 3.8)");
	CHECK_EQ(std::hash<Spherical>{}(value), value.hash());

	std::stringstream stream;
	stream << value;
	Spherical parsed;
	stream >> parsed;
	CHECK_EQ(parsed.r, value.r);
	CHECK_EQ(parsed.theta, value.theta);
	CHECK_EQ(parsed.phi, value.phi);

	const JSON json = value;
	CHECK_EQ(json[U"r"].get<double>(), value.r);
	CHECK_EQ(json[U"theta"].get<double>(), value.theta);
	CHECK_EQ(json[U"phi"].get<double>(), value.phi);
	const Spherical restored = json.get<Spherical>();
	CHECK_EQ(restored.r, value.r);
	CHECK_EQ(restored.theta, value.theta);
	CHECK_EQ(restored.phi, value.phi);
}
