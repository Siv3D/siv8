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

namespace
{
	constexpr float FrameEpsilon = 1e-5f;
	constexpr float TriangleAreaEpsilon = 1e-10f;

	template <size_t SectionCount, size_t HeightCount, size_t VertexCount>
	concept LoftCallable = requires(
		const std::array<std::array<Vec2, VertexCount>, SectionCount>& sections,
		const std::array<double, HeightCount>& heights)
	{
		Mesh3D::Loft(sections, heights);
	};

	static_assert(LoftCallable<2, 2, 3>);
	static_assert(LoftCallable<4, 4, 8>);
	static_assert(not LoftCallable<1, 1, 3>);
	static_assert(not LoftCallable<2, 2, 2>);
	static_assert(not LoftCallable<2, 3, 3>);

	static void CheckVertexFrame(const Vertex3D& vertex)
	{
		CHECK(std::isfinite(vertex.pos.x));
		CHECK(std::isfinite(vertex.pos.y));
		CHECK(std::isfinite(vertex.pos.z));
		CHECK(std::isfinite(vertex.tex.x));
		CHECK(std::isfinite(vertex.tex.y));
		CHECK(vertex.normal.length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(vertex.tangent.xyz().length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
		CHECK(std::abs(vertex.normal.dot(vertex.tangent.xyz())) < FrameEpsilon);
		CHECK(std::abs(std::abs(vertex.tangent.w) - 1.0f) < FrameEpsilon);
		CHECK(vertex.bitangent().length() == doctest::Approx(1.0f).epsilon(FrameEpsilon));
	}

	static void CheckMeshGeometry(const Mesh3D& mesh)
	{
		REQUIRE_FALSE(mesh.isEmpty());
		REQUIRE(mesh.validate());

		for (const Vertex3D& vertex : mesh.vertices)
		{
			CheckVertexFrame(vertex);
		}

		for (const TriangleIndex32& triangle : mesh.indices)
		{
			const Vertex3D& v0 = mesh.vertices[triangle.i0];
			const Vertex3D& v1 = mesh.vertices[triangle.i1];
			const Vertex3D& v2 = mesh.vertices[triangle.i2];
			const Float3 faceNormal = (v1.pos - v0.pos).cross(v2.pos - v0.pos);
			const Float3 vertexNormal = (v0.normal + v1.normal + v2.normal);

			CHECK(faceNormal.lengthSq() > TriangleAreaEpsilon);
			CHECK(faceNormal.dot(vertexNormal) > 0.0f);
		}
	}
}

TEST_CASE("Mesh3D::Loft rectangular sections")
{
	const std::array sections{
		std::array{
			Vec2{ -2.0, -2.0 }, Vec2{ 2.0, -2.0 },
			Vec2{ 2.0, 2.0 }, Vec2{ -2.0, 2.0 }
		},
		std::array{
			Vec2{ -1.0, -1.0 }, Vec2{ 1.0, -1.0 },
			Vec2{ 1.0, 1.0 }, Vec2{ -1.0, 1.0 }
		}
	};
	const std::array heights{ -1.0, 3.0 };
	const Mesh3D mesh = Mesh3D::Loft(
		sections, heights, Vec2{ 2.0, 0.5 }, Vec2{ 0.25, -0.5 });

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, -1.0f, 2.0f });
	CHECK_EQ(mesh.vertices[4].pos, Float3{ -1.0f, 3.0f, 1.0f });
	CHECK_EQ(mesh.vertices[8].tex, Float2{ 0.25f, -0.5f });
	CHECK_EQ(mesh.vertices[9].tex, Float2{ 0.75f, -0.5f });
	CHECK_EQ(mesh.vertices[10].tex, Float2{ 0.25f, 1.5f });
	CHECK_EQ(mesh.vertices[23].tex, Float2{ 2.25f, 1.5f });
}

TEST_CASE("Mesh3D::Loft multiple offset sections")
{
	const std::array sections{
		std::array{
			Vec2{ -2.0, -1.0 }, Vec2{ 2.0, -1.0 },
			Vec2{ 2.0, 1.0 }, Vec2{ -2.0, 1.0 }
		},
		std::array{
			Vec2{ -1.5, -0.75 }, Vec2{ 2.0, -0.5 },
			Vec2{ 1.75, 1.0 }, Vec2{ -1.25, 0.75 }
		},
		std::array{
			Vec2{ -0.5, -0.5 }, Vec2{ 1.5, -0.5 },
			Vec2{ 1.5, 0.5 }, Vec2{ -0.5, 0.5 }
		}
	};
	const std::array heights{ 0.0, 1.0, 4.0 };
	const Mesh3D mesh = Mesh3D::Loft(sections, heights);

	CHECK_EQ(mesh.vertexCount(), size_t{ 32 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 20 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[10].pos, Float3{ -1.5f, 1.0f, 0.75f });
	CHECK_EQ(mesh.vertices[10].tex.y, 1.0f);
	CHECK_EQ(mesh.vertices[12].tex.y, 4.0f);
}

TEST_CASE("Mesh3D::Loft concave sections")
{
	const std::array sections{
		std::array{
			Vec2{ -2.0, -1.0 }, Vec2{ 0.0, -1.0 }, Vec2{ 0.0, 0.0 },
			Vec2{ 2.0, 0.0 }, Vec2{ 2.0, 1.0 }, Vec2{ -2.0, 1.0 }
		},
		std::array{
			Vec2{ -1.5, -0.75 }, Vec2{ 0.0, -0.75 }, Vec2{ 0.0, 0.0 },
			Vec2{ 1.5, 0.0 }, Vec2{ 1.5, 0.75 }, Vec2{ -1.5, 0.75 }
		}
	};
	const std::array heights{ -2.0, 2.0 };
	const Mesh3D mesh = Mesh3D::Loft(sections, heights);

	CHECK_EQ(mesh.vertexCount(), size_t{ 36 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 20 });
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Loft minimum dimensions")
{
	const std::array sections{
		std::array{ Vec2{ 0.0, -1.0 }, Vec2{ 1.0, 1.0 }, Vec2{ -1.0, 1.0 } },
		std::array{ Vec2{ 0.0, -0.5 }, Vec2{ 0.5, 0.5 }, Vec2{ -0.5, 0.5 } }
	};
	const std::array heights{ 0.0, 1.0 };
	const Mesh3D mesh = Mesh3D::Loft(sections, heights);

	CHECK_EQ(mesh.vertexCount(), size_t{ 18 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 8 });
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Loft invalid geometry")
{
	std::array sections{
		std::array{
			Vec2{ -1.0, -1.0 }, Vec2{ 1.0, -1.0 },
			Vec2{ 1.0, 1.0 }, Vec2{ -1.0, 1.0 }
		},
		std::array{
			Vec2{ -0.5, -0.5 }, Vec2{ 0.5, -0.5 },
			Vec2{ 0.5, 0.5 }, Vec2{ -0.5, 0.5 }
		}
	};

	CHECK(Mesh3D::Loft(sections, std::array{ 1.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::Loft(sections, std::array{ 2.0, 1.0 }).isEmpty());
	CHECK(Mesh3D::Loft(sections,
		std::array{ 0.0, std::numeric_limits<double>::infinity() }).isEmpty());
	CHECK(Mesh3D::Loft(sections,
		std::array{ 0.0, std::numeric_limits<double>::denorm_min() }).isEmpty());

	auto invalidSections = sections;
	invalidSections[0][0].x = std::numeric_limits<double>::quiet_NaN();
	CHECK(Mesh3D::Loft(invalidSections, std::array{ 0.0, 1.0 }).isEmpty());

	invalidSections = sections;
	invalidSections[0][1] = invalidSections[0][0];
	CHECK(Mesh3D::Loft(invalidSections, std::array{ 0.0, 1.0 }).isEmpty());

	invalidSections = sections;
	std::reverse(invalidSections[0].begin(), invalidSections[0].end());
	CHECK(Mesh3D::Loft(invalidSections, std::array{ 0.0, 1.0 }).isEmpty());

	invalidSections = sections;
	invalidSections[0] = std::array{
		Vec2{ -1.0, -1.0 }, Vec2{ 1.0, 1.0 },
		Vec2{ 1.0, -1.0 }, Vec2{ -1.0, 1.0 }
	};
	CHECK(Mesh3D::Loft(invalidSections, std::array{ 0.0, 1.0 }).isEmpty());
}

TEST_CASE("Mesh3D::Loft invalid UV")
{
	const std::array sections{
		std::array{
			Vec2{ -1.0, -1.0 }, Vec2{ 1.0, -1.0 },
			Vec2{ 1.0, 1.0 }, Vec2{ -1.0, 1.0 }
		},
		std::array{
			Vec2{ -0.5, -0.5 }, Vec2{ 0.5, -0.5 },
			Vec2{ 0.5, 0.5 }, Vec2{ -0.5, 0.5 }
		}
	};
	const std::array heights{ 0.0, 2.0 };

	CHECK(Mesh3D::Loft(sections, heights,
		Vec2{ std::numeric_limits<double>::quiet_NaN(), 1.0 }).isEmpty());
	CHECK(Mesh3D::Loft(sections, heights,
		Vec2{ 1.0, std::numeric_limits<float>::max() }).isEmpty());
	CHECK(Mesh3D::Loft(sections, heights,
		Vec2{ std::numeric_limits<float>::max(), 1.0 },
		Vec2{ std::numeric_limits<float>::max(), 0.0 }).isEmpty());
}
