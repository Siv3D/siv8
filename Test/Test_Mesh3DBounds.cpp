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
# include <type_traits>

namespace
{
	constexpr double BoundsEpsilon = 1e-5;

	[[nodiscard]]
	static Vertex3D MakeVertex(const Float3 position)
	{
		return Vertex3D{ .pos = position };
	}
}

TEST_CASE("Box and Sphere value types")
{
	static_assert(sizeof(Box) == (sizeof(double) * 6));
	static_assert(sizeof(Sphere) == (sizeof(double) * 4));
	static_assert(std::is_trivially_copyable_v<Box>);
	static_assert(std::is_trivially_copyable_v<Sphere>);

	constexpr Box box0{ 2.0 };
	static_assert(box0.center == Vec3{ 0.0, 0.0, 0.0 });
	static_assert(box0.size == Vec3{ 2.0, 2.0, 2.0 });

	constexpr Box box1{ Vec3{ 1.0, 2.0, 3.0 }, Vec3{ 4.0, 5.0, 6.0 } };
	static_assert(box1.center == Vec3{ 1.0, 2.0, 3.0 });
	static_assert(box1.size == Vec3{ 4.0, 5.0, 6.0 });

	constexpr Sphere sphere0{ 2.0 };
	static_assert(sphere0.center == Vec3{ 0.0, 0.0, 0.0 });
	static_assert(sphere0.r == 2.0);

	constexpr Sphere sphere1{ Vec3{ 1.0, 2.0, 3.0 }, 4.0 };
	static_assert(sphere1.center == Vec3{ 1.0, 2.0, 3.0 });
	static_assert(sphere1.r == 4.0);
}

TEST_CASE("Mesh3D::computeBoundingBox")
{
	SECTION("No vertices")
	{
		const Box box = Mesh3D{}.computeBoundingBox();
		CHECK((box) == (Box{ 0.0 }));
	}

	SECTION("One unreferenced vertex")
	{
		const Mesh3D mesh{
			{ MakeVertex(Float3{ 1.0f, -2.0f, 3.0f }) },
			{}
		};
		const Box box = mesh.computeBoundingBox();
		CHECK((box.center) == (Vec3{ 1.0, -2.0, 3.0 }));
		CHECK((box.size) == (Vec3::Zero()));
	}

	SECTION("All stored vertices")
	{
		const Mesh3D mesh{
			{
				MakeVertex(Float3{ -2.0f, -4.0f, -6.0f }),
				MakeVertex(Float3{ 4.0f, 8.0f, 10.0f }),
				MakeVertex(Float3{ 20.0f, 1.0f, 2.0f }),
			},
			{ TriangleIndex32{ 0, 1, 1 } }
		};
		const Box box = mesh.computeBoundingBox();
		CHECK(box.center.epsilonEquals(Vec3{ 9.0, 2.0, 2.0 }, BoundsEpsilon));
		CHECK(box.size.epsilonEquals(Vec3{ 22.0, 12.0, 16.0 }, BoundsEpsilon));
	}
}

TEST_CASE("Mesh3D::computeBoundingSphere")
{
	SECTION("No vertices")
	{
		const Sphere sphere = Mesh3D{}.computeBoundingSphere();
		CHECK((sphere) == (Sphere{ 0.0 }));
	}

	SECTION("One unreferenced vertex")
	{
		const Mesh3D mesh{
			{ MakeVertex(Float3{ 1.0f, -2.0f, 3.0f }) },
			{}
		};
		const Sphere sphere = mesh.computeBoundingSphere();
		CHECK((sphere.center) == (Vec3{ 1.0, -2.0, 3.0 }));
		CHECK((sphere.r) == (0.0));
	}

	SECTION("Two vertices")
	{
		const Mesh3D mesh{
			{
				MakeVertex(Float3{ -2.0f, 1.0f, 3.0f }),
				MakeVertex(Float3{ 6.0f, 1.0f, 3.0f }),
			},
			{}
		};
		const Sphere sphere = mesh.computeBoundingSphere();
		CHECK(sphere.center.epsilonEquals(Vec3{ 2.0, 1.0, 3.0 }, BoundsEpsilon));
		CHECK(sphere.r == Test::Approx(4.0).epsilon(BoundsEpsilon));
	}

	SECTION("Contains all stored vertices")
	{
		const Array<Vertex3D> vertices{
			MakeVertex(Float3{ -3.0f, 0.0f, 1.0f }),
			MakeVertex(Float3{ 2.0f, 4.0f, -1.0f }),
			MakeVertex(Float3{ 5.0f, -2.0f, 3.0f }),
			MakeVertex(Float3{ 0.0f, 1.0f, -6.0f }),
		};
		const Mesh3D mesh{ vertices, {} };
		const Sphere sphere = mesh.computeBoundingSphere();

		for (const auto& vertex : vertices)
		{
			CHECK(Vec3{ vertex.pos }.distanceFrom(sphere.center) <= (sphere.r + BoundsEpsilon));
		}
	}
}
