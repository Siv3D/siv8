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

# include "Mesh3DTestHelper.hpp"

namespace
{
	using Mesh3DTest::CheckMeshGeometry;

	[[nodiscard]]
	Array<Vec2> MakeSampledSquare()
	{
		constexpr size_t SampleCount = 16;
		Array<Vec2> result;
		result.reserve(SampleCount);
		for (size_t i = 0; i < SampleCount; ++i)
		{
			const double angle = (Math::TwoPi * i / SampleCount);
			const double x = std::cos(angle);
			const double y = std::sin(angle);
			const double scale = (1.0 / std::max(std::abs(x), std::abs(y)));
			result.emplace_back((x * scale), (y * scale));
		}

		return result;
	}

	[[nodiscard]]
	size_t CountPositiveTriangles(const Polygon& polygon)
	{
		size_t result = 0;
		for (const TriangleIndex& index : polygon.indices())
		{
			const Float2 p0 = polygon.vertices()[index.i0];
			const Float2 p1 = polygon.vertices()[index.i1];
			const Float2 p2 = polygon.vertices()[index.i2];
			const double twiceArea = (
				((static_cast<double>(p1.x) - p0.x) * (static_cast<double>(p2.y) - p0.y))
				- ((static_cast<double>(p1.y) - p0.y) * (static_cast<double>(p2.x) - p0.x)));
			REQUIRE(0.0 <= twiceArea);
			result += (0.0 < twiceArea);
		}

		return result;
	}
}

TEST_CASE("Mesh3D generators omit zero-area cap triangles")
{
	const Array<Vec2> outline = MakeSampledSquare();
	const Polygon polygon{ outline };
	REQUIRE(not polygon.isEmpty());
	const size_t positiveCapTriangleCount = CountPositiveTriangles(polygon);
	REQUIRE(0 < positiveCapTriangleCount);
	REQUIRE(positiveCapTriangleCount < polygon.indices().size());

	const Mesh3D extrude = Mesh3D::Extrude(polygon, 2.0);
	CHECK_EQ(extrude.triangleCount(),
		((positiveCapTriangleCount * 2) + (outline.size() * 2)));
	CheckMeshGeometry(extrude);

	const Mesh3D sweep = Mesh3D::Sweep(
		polygon,
		{ Vec3{ 0.0, 0.0, 0.0 }, Vec3{ 0.0, 2.0, 0.0 } },
		Arg::initialXAxis = Vec3::UnitX());
	CHECK_EQ(sweep.triangleCount(),
		((positiveCapTriangleCount * 2) + (outline.size() * 2)));
	CheckMeshGeometry(sweep);

	const Array<Array<Vec2>> sections{ outline, outline };
	const Mesh3D loft = Mesh3D::Loft(sections, std::array{ 0.0, 2.0 });
	CHECK_EQ(loft.triangleCount(),
		((positiveCapTriangleCount * 2) + (outline.size() * 2)));
	CheckMeshGeometry(loft);

}
