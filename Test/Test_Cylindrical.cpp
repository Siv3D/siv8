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
	constexpr double CylindricalEpsilon = 1e-10;

	void CheckCylindricalPosition(const Cylindrical& value, const Vec3 expected)
	{
		CHECK(value.toVec3().epsilonEquals(expected, CylindricalEpsilon));
		CHECK(value.toPosition().epsilonEquals(expected, CylindricalEpsilon));
		CHECK(static_cast<Vec3>(value).epsilonEquals(expected, CylindricalEpsilon));
		CHECK(value.toFloat3().epsilonEquals(Float3{ expected }, 1e-5f));
		CHECK(value.fastToVec3().epsilonEquals(expected, 1e-4));
		CHECK(value.fastToFloat3().epsilonEquals(Float3{ expected }, 1e-4f));
	}
}

TEST_CASE("Cylindrical::constructors")
{
	static_assert(sizeof(Cylindrical) == (sizeof(double) * 3));
	static_assert(sizeof(CylindricalF) == (sizeof(float) * 3));
	static_assert(std::is_trivially_copyable_v<Cylindrical>);

	constexpr Cylindrical value{ 2, 0.5, -3 };
	static_assert(value.r == 2.0);
	static_assert(value.phi == 0.5);
	static_assert(value.y == -3.0);

	constexpr Cylindrical named0{ Arg::r = 2.0, Arg::phi = 0.5, Arg::y = -3.0 };
	constexpr Cylindrical named1{ Arg::r = 2.0, Arg::y = -3.0, Arg::phi = 0.5 };
	constexpr Cylindrical named2{ Arg::phi = 0.5, Arg::r = 2.0, Arg::y = -3.0 };
	constexpr Cylindrical named3{ Arg::phi = 0.5, Arg::y = -3.0, Arg::r = 2.0 };
	constexpr Cylindrical named4{ Arg::y = -3.0, Arg::r = 2.0, Arg::phi = 0.5 };
	constexpr Cylindrical named5{ Arg::y = -3.0, Arg::phi = 0.5, Arg::r = 2.0 };
	static_assert(named0.r == value.r && named0.phi == value.phi && named0.y == value.y);
	static_assert(named1.r == value.r && named1.phi == value.phi && named1.y == value.y);
	static_assert(named2.r == value.r && named2.phi == value.phi && named2.y == value.y);
	static_assert(named3.r == value.r && named3.phi == value.phi && named3.y == value.y);
	static_assert(named4.r == value.r && named4.phi == value.phi && named4.y == value.y);
	static_assert(named5.r == value.r && named5.phi == value.phi && named5.y == value.y);

	const Cylindrical fromPosition{ Vec3{ 1.25, -2.5, 3.75 } };
	CHECK(fromPosition.toVec3().epsilonEquals(Vec3{ 1.25, -2.5, 3.75 }, CylindricalEpsilon));

	const Cylindrical onAxis{ Vec3{ 0.0, 4.0, 0.0 } };
	CHECK_EQ(onAxis.r, 0.0);
	CHECK_EQ(onAxis.phi, 0.0);
	CHECK_EQ(onAxis.y, 4.0);
}

TEST_CASE("Cylindrical::coordinate_convention")
{
	CheckCylindricalPosition(Cylindrical{ 2.0, 0.0, 3.0 }, Vec3{ 2.0, 3.0, 0.0 });
	CheckCylindricalPosition(Cylindrical{ 2.0, Math::HalfPi, 3.0 }, Vec3{ 0.0, 3.0, -2.0 });
	CheckCylindricalPosition(Cylindrical{ 2.0, Math::Pi, 3.0 }, Vec3{ -2.0, 3.0, 0.0 });
	CheckCylindricalPosition(Cylindrical{ 2.0, -Math::HalfPi, 3.0 }, Vec3{ 0.0, 3.0, 2.0 });

	for (const float angle : { -2.0f, -0.5f, 0.0f, 0.75f, 2.5f })
	{
		const Float3 expected = Quaternion::RotateY(angle).rotate(Float3{ 2.0f, 0.0f, 0.0f }) + Float3{ 0.0f, 3.0f, 0.0f };
		CHECK(CylindricalF{ 2.0f, angle, 3.0f }.toFloat3().epsilonEquals(expected, 1e-5f));
	}
}

TEST_CASE("Cylindrical::operations")
{
	constexpr Cylindrical value{ 2.0, 0.5, 3.0 };
	static_assert((+value).r == 2.0);
	static_assert(value.withR(4.0).r == 4.0);
	static_assert(value.withPhi(1.5).phi == 1.5);
	static_assert(value.withY(5.0).y == 5.0);
	static_assert(value.withOffsetR(4.0).r == 6.0);
	static_assert(value.withOffsetPhi(1.5).phi == 2.0);
	static_assert(value.withOffsetY(5.0).y == 8.0);
	static_assert(value.rotated(1.0).phi == 1.5);

	Cylindrical mutableValue = value;
	CHECK_EQ(&mutableValue.rotate(1.0), &mutableValue);
	CHECK_EQ(mutableValue.phi, 1.5);

	CHECK((-value).toVec3().epsilonEquals(-value.toVec3(), CylindricalEpsilon));
	CHECK((value + Vec3{ 1.0, 2.0, 3.0 }).epsilonEquals(value.toVec3() + Vec3{ 1.0, 2.0, 3.0 }, CylindricalEpsilon));
	CHECK((value - Vec3{ 1.0, 2.0, 3.0 }).epsilonEquals(value.toVec3() - Vec3{ 1.0, 2.0, 3.0 }, CylindricalEpsilon));

	const Cylindrical interpolated = Cylindrical{ 2.0, (Math::Pi - 0.1), 3.0 }
		.lerp(Cylindrical{ 4.0, (-Math::Pi + 0.1), 7.0 }, 0.5);
	CHECK_EQ(interpolated.r, 3.0);
	CHECK_EQ(interpolated.y, 5.0);
	CHECK(std::abs(std::abs(interpolated.phi) - Math::Pi) < CylindricalEpsilon);

	const CylindricalF interpolatedF = CylindricalF{ 2.0f, 0.25f, 3.0f }.lerp(CylindricalF{ 4.0f, 0.75f, 7.0f }, 0.5);
	CHECK_EQ(interpolatedF.r, 3.0f);
	CHECK_EQ(interpolatedF.phi, 0.5f);
	CHECK_EQ(interpolatedF.y, 5.0f);
}

TEST_CASE("Cylindrical::format_hash_and_json")
{
	const Cylindrical value{ 1.5, -2.25, 3.75 };
	CHECK_EQ(Format(value), U"(1.5, -2.25, 3.75)");
	CHECK_EQ(U"{:.1f}"_fmt(value), U"(1.5, -2.2, 3.8)");
	CHECK_EQ(std::hash<Cylindrical>{}(value), value.hash());

	std::stringstream stream;
	stream << value;
	Cylindrical parsed;
	stream >> parsed;
	CHECK_EQ(parsed.r, value.r);
	CHECK_EQ(parsed.phi, value.phi);
	CHECK_EQ(parsed.y, value.y);

	const JSON json = value;
	CHECK_EQ(json[U"r"].get<double>(), value.r);
	CHECK_EQ(json[U"phi"].get<double>(), value.phi);
	CHECK_EQ(json[U"y"].get<double>(), value.y);
	const Cylindrical restored = json.get<Cylindrical>();
	CHECK_EQ(restored.r, value.r);
	CHECK_EQ(restored.phi, value.phi);
	CHECK_EQ(restored.y, value.y);
}
