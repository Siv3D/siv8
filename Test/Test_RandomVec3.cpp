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
# include <cmath>
# include <limits>
# include <random>

namespace
{
	constexpr int32 SampleCount = 16384;
	constexpr double GeometryEpsilon = 1e-12;

	template <class URBG>
	std::array<Vec3, 9> SampleAll(URBG&& urbg)
	{
		return{
			RandomUnitVec3(std::forward<URBG>(urbg)),
			RandomVec3(2.5, std::forward<URBG>(urbg)),
			RandomVec3({ -2.0, 3.0 }, { 4.0, 5.0 }, { -7.0, -1.0 }, std::forward<URBG>(urbg)),
			RandomVec3(2.0, 3.0, 4.0, std::forward<URBG>(urbg)),
			RandomVec3(Box{ Vec3{ 2.0, -3.0, 4.0 }, Vec3{ 5.0, 6.0, 7.0 } }, std::forward<URBG>(urbg)),
			RandomVec3(Sphere{ Vec3{ 2.0, -3.0, 4.0 }, 2.5 }, std::forward<URBG>(urbg)),
			RandomVec3InsideUnitSphere(std::forward<URBG>(urbg)),
			RandomVec3On(Sphere{ Vec3{ 2.0, -3.0, 4.0 }, 2.5 }, std::forward<URBG>(urbg)),
			RandomVec3InsideSphericalShell(2.0, 5.0, std::forward<URBG>(urbg))
		};
	}

	class NonCopyableRNG
	{
	public:

		using result_type = std::mt19937_64::result_type;

		NonCopyableRNG() = default;
		NonCopyableRNG(const NonCopyableRNG&) = delete;
		NonCopyableRNG& operator =(const NonCopyableRNG&) = delete;

		static constexpr result_type min() { return std::mt19937_64::min(); }
		static constexpr result_type max() { return std::mt19937_64::max(); }

		result_type operator ()()
		{
			++calls;
			return m_engine();
		}

		size_t calls = 0;

	private:

		std::mt19937_64 m_engine{ 12345 };
	};

	template <class Sampler>
	void CheckUnitDirections(Sampler sample)
	{
		Vec3 sum = Vec3::Zero();
		Vec3 squaredSum = Vec3::Zero();
		std::array<int32, 3> equatorialCounts{};

		for (int32 i = 0; i < SampleCount; ++i)
		{
			const Vec3 v = sample();
			REQUIRE(v.lengthSq() == Test::Approx(1.0).epsilon(GeometryEpsilon));
			sum += v;
			squaredSum += Vec3{ v.x * v.x, v.y * v.y, v.z * v.z };
			equatorialCounts[0] += (std::abs(v.x) < 0.5);
			equatorialCounts[1] += (std::abs(v.y) < 0.5);
			equatorialCounts[2] += (std::abs(v.z) < 0.5);
		}

		for (size_t axis = 0; axis < 3; ++axis)
		{
			CHECK(std::abs(sum.elem(axis) / SampleCount) < 0.025);
			CHECK(std::abs(squaredSum.elem(axis) / SampleCount - 1.0 / 3.0) < 0.025);
			CHECK(std::abs(static_cast<double>(equatorialCounts[axis]) / SampleCount - 0.5) < 0.025);
		}
	}

	template <class Sampler>
	void CheckUnitBall(Sampler sample)
	{
		Vec3 sum = Vec3::Zero();
		double squaredRadiusSum = 0.0;
		int32 innerCount = 0;

		for (int32 i = 0; i < SampleCount; ++i)
		{
			const Vec3 v = sample();
			const double radiusSq = v.lengthSq();
			REQUIRE(radiusSq <= (1.0 + GeometryEpsilon));
			sum += v;
			squaredRadiusSum += radiusSq;
			innerCount += (radiusSq < 0.25);
		}

		CHECK(std::abs(sum.x / SampleCount) < 0.025);
		CHECK(std::abs(sum.y / SampleCount) < 0.025);
		CHECK(std::abs(sum.z / SampleCount) < 0.025);
		CHECK(std::abs(squaredRadiusSum / SampleCount - 3.0 / 5.0) < 0.025);
		CHECK(std::abs(static_cast<double>(innerCount) / SampleCount - 1.0 / 8.0) < 0.015);
	}
}

TEST_CASE("RandomVec3.unit_and_length")
{
	DefaultRNG rng{ uint64{ 12345 } };
	CheckUnitDirections([&]() { return RandomUnitVec3(rng); });
	CheckUnitDirections([&]() { return (RandomVec3(2.5, rng) / 2.5); });
	CHECK(RandomVec3(0.0, rng) == Vec3::Zero());
}

TEST_CASE("RandomVec3.component_ranges")
{
	DefaultRNG rng{ uint64{ 23456 } };
	Vec3 sum = Vec3::Zero();
	double xySum = 0.0;
	for (int32 i = 0; i < SampleCount; ++i)
	{
		const Vec3 v = RandomVec3({ -2.0, 3.0 }, { 4.0, 6.0 }, { -7.0, -1.0 }, rng);
		REQUIRE((-2.0 <= v.x && v.x < 3.0));
		REQUIRE((4.0 <= v.y && v.y < 6.0));
		REQUIRE((-7.0 <= v.z && v.z < -1.0));
		const Vec3 u{ (v.x + 2.0) / 5.0, (v.y - 4.0) / 2.0, (v.z + 7.0) / 6.0 };
		sum += u;
		xySum += (u.x * u.y);
	}
	CHECK(std::abs(sum.x / SampleCount - 0.5) < 0.025);
	CHECK(std::abs(sum.y / SampleCount - 0.5) < 0.025);
	CHECK(std::abs(sum.z / SampleCount - 0.5) < 0.025);
	CHECK(std::abs(xySum / SampleCount - 0.25) < 0.025);

	const double next = std::nextafter(1.0, 2.0);
	for (int32 i = 0; i < 128; ++i)
	{
		// The half-open interval contains exactly one representable double.
		CHECK(RandomVec3({ 1.0, next }, { 1.0, next }, { 1.0, next }, rng) == Vec3{ 1.0, 1.0, 1.0 });
		CHECK(RandomVec3({ -2.0, -2.0 }, { 5.0, 5.0 }, { 7.0, 7.0 }, rng) == Vec3{ -2.0, 5.0, 7.0 });
		const Vec3 fixedY = RandomVec3({ -2.0, 3.0 }, { 5.0, 5.0 }, { -7.0, -1.0 }, rng);
		CHECK(fixedY.y == 5.0);
		const Vec3 maximums = RandomVec3(2.0, 0.0, 4.0, rng);
		CHECK((0.0 <= maximums.x && maximums.x < 2.0));
		CHECK(maximums.y == 0.0);
		CHECK((0.0 <= maximums.z && maximums.z < 4.0));
	}
	CHECK(RandomVec3(0.0, 0.0, 0.0, rng) == Vec3::Zero());
}

TEST_CASE("RandomVec3.box")
{
	DefaultRNG rng{ uint64{ 34567 } };
	const Vec3 center{ 2.0, -3.0, 4.0 };
	for (int32 i = 0; i < 1024; ++i)
	{
		const Vec3 v = RandomVec3(Box{ center, Vec3{ 4.0, 6.0, 8.0 } }, rng);
		CHECK((0.0 <= v.x && v.x < 4.0));
		CHECK((-6.0 <= v.y && v.y < 0.0));
		CHECK((0.0 <= v.z && v.z < 8.0));
		const Vec3 plane = RandomVec3(Box{ center, Vec3{ 4.0, 0.0, 8.0 } }, rng);
		CHECK((0.0 <= plane.x && plane.x < 4.0));
		CHECK(plane.y == center.y);
		CHECK((0.0 <= plane.z && plane.z < 8.0));
		const Vec3 line = RandomVec3(Box{ center, Vec3{ 0.0, 6.0, 0.0 } }, rng);
		CHECK(line.x == center.x);
		CHECK((-6.0 <= line.y && line.y < 0.0));
		CHECK(line.z == center.z);
	}
	CHECK(RandomVec3(Box{ center, 0.0 }, rng) == center);
}

TEST_CASE("RandomVec3.sphere")
{
	DefaultRNG rng{ uint64{ 45678 } };
	const Sphere sphere{ Vec3{ 2.0, -3.0, 4.0 }, 2.5 };
	CheckUnitBall([&]() { return RandomVec3InsideUnitSphere(rng); });
	CheckUnitBall([&]() { return ((RandomVec3(sphere, rng) - sphere.center) / sphere.r); });
	CheckUnitDirections([&]() { return ((RandomVec3On(sphere, rng) - sphere.center) / sphere.r); });
	CHECK(RandomVec3(Sphere{ sphere.center, 0.0 }, rng) == sphere.center);
	CHECK(RandomVec3On(Sphere{ sphere.center, 0.0 }, rng) == sphere.center);
}

TEST_CASE("RandomVec3.spherical_shell_distribution")
{
	DefaultRNG rng{ uint64{ 56789 } };
	CheckUnitBall([&]() { return RandomVec3InsideSphericalShell(0.0, 1.0, rng); });
	CheckUnitDirections([&]() { return (RandomVec3InsideSphericalShell(3.25, 3.25, rng) / 3.25); });
	CheckUnitDirections([&]() { return RandomVec3InsideSphericalShell(2.0, 5.0, rng).normalized(); });

	std::array<int32, 4> radialCounts{};
	double volumeSum = 0.0;
	for (int32 i = 0; i < SampleCount; ++i)
	{
		const double radius = RandomVec3InsideSphericalShell(2.0, 5.0, rng).length();
		REQUIRE((2.0 - GeometryEpsilon <= radius && radius <= 5.0 + GeometryEpsilon));
		// Volume between the inner sphere and this radius, divided by shell volume.
		const double u = (radius * radius * radius - 8.0) / (125.0 - 8.0);
		volumeSum += u;
		for (size_t bin = 0; bin < radialCounts.size(); ++bin)
		{
			radialCounts[bin] += (u < (static_cast<double>(bin) + 1.0) / 5.0);
		}
	}
	CHECK(std::abs(volumeSum / SampleCount - 0.5) < 0.025);
	for (size_t bin = 0; bin < radialCounts.size(); ++bin)
	{
		CHECK(std::abs(static_cast<double>(radialCounts[bin]) / SampleCount
			- (static_cast<double>(bin) + 1.0) / 5.0) < 0.025);
	}
}

TEST_CASE("RandomVec3.spherical_shell_boundaries")
{
	DefaultRNG rng{ uint64{ 67890 } };
	CHECK(RandomVec3InsideSphericalShell(0.0, 0.0, rng) == Vec3::Zero());
	for (int32 i = 0; i < 256; ++i)
	{
		const double radius = RandomVec3InsideSphericalShell(1.0, 1.0 + 1e-9, rng).length();
		CHECK((1.0 - GeometryEpsilon <= radius && radius <= 1.0 + 1e-9 + GeometryEpsilon));
		for (const double outer : { 1e-200, 1e200, std::numeric_limits<double>::max() })
		{
			const Vec3 v = RandomVec3InsideSphericalShell(outer * 0.5, outer, rng);
			REQUIRE((std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z)));
			// Normalize before measuring to keep the length calculation representable.
			const double normalizedRadius = (v / outer).length();
			CHECK((0.5 - GeometryEpsilon <= normalizedRadius && normalizedRadius <= 1.0 + GeometryEpsilon));
		}
	}
}

TEST_CASE("RandomVec3.explicit_engines")
{
	const DefaultRNG savedDefault = GetDefaultRNG();
	DefaultRNG first{ uint64{ 78901 } };
	DefaultRNG second{ uint64{ 78901 } };
	const DefaultRNG initial = first;
	for (int32 i = 0; i < 16; ++i)
	{
		CHECK(SampleAll(first) == SampleAll(second));
	}
	CHECK(first == second);
	CHECK_FALSE(first == initial);

	std::mt19937 standardFirst{ 12345 };
	std::mt19937 standardSecond{ 12345 };
	CHECK(SampleAll(standardFirst) == SampleAll(standardSecond));
	CHECK(standardFirst == standardSecond);

	NonCopyableRNG nonCopyable;
	const auto values = SampleAll(nonCopyable);
	CHECK(nonCopyable.calls > 0);
	CHECK(values == SampleAll(NonCopyableRNG{}));
	CHECK(GetDefaultRNG() == savedDefault);
}

TEST_CASE("RandomVec3.default_engine")
{
	const DefaultRNG savedDefault = GetDefaultRNG();
	const ScopeExit restore = [&]() { GetDefaultRNG() = savedDefault; };
	Reseed(uint64{ 89012 });
	DefaultRNG explicitRNG{ uint64{ 89012 } };

	const std::array<Vec3, 9> actual{
		RandomUnitVec3(),
		RandomVec3(2.5),
		RandomVec3({ -2.0, 3.0 }, { 4.0, 5.0 }, { -7.0, -1.0 }),
		RandomVec3(2.0, 3.0, 4.0),
		RandomVec3(Box{ Vec3{ 2.0, -3.0, 4.0 }, Vec3{ 5.0, 6.0, 7.0 } }),
		RandomVec3(Sphere{ Vec3{ 2.0, -3.0, 4.0 }, 2.5 }),
		RandomVec3InsideUnitSphere(),
		RandomVec3On(Sphere{ Vec3{ 2.0, -3.0, 4.0 }, 2.5 }),
		RandomVec3InsideSphericalShell(2.0, 5.0)
	};
	CHECK(actual == SampleAll(explicitRNG));
	CHECK(GetDefaultRNG() == explicitRNG);
}
