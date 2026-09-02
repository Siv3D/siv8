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
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(
			std::span<const std::span<const Vec2>>,
			std::span<const double>, Vec2, Vec2)>(&Mesh3D::Loft);
		static_cast<Mesh3D (*)(
			const Array<Array<Vec2>>&,
			std::span<const double>, Vec2, Vec2)>(&Mesh3D::Loft);
	});

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

}

TEST_CASE("Mesh3D::Loft runtime sections")
{
	const std::array fixedSections{
		std::array{
			Vec2{ -2.0, -1.0 }, Vec2{ 2.0, -1.0 },
			Vec2{ 2.0, 1.0 }, Vec2{ -2.0, 1.0 }
		},
		std::array{
			Vec2{ -1.5, -0.75 }, Vec2{ 1.5, -0.75 },
			Vec2{ 1.5, 0.75 }, Vec2{ -1.5, 0.75 }
		},
		std::array{
			Vec2{ -1.0, -0.5 }, Vec2{ 1.0, -0.5 },
			Vec2{ 1.0, 0.5 }, Vec2{ -1.0, 0.5 }
		}
	};
	const std::array fixedHeights{ -1.0, 1.0, 4.0 };
	const Array<Array<Vec2>> sections{
		Array<Vec2>(fixedSections[0].begin(), fixedSections[0].end()),
		Array<Vec2>(fixedSections[1].begin(), fixedSections[1].end()),
		Array<Vec2>(fixedSections[2].begin(), fixedSections[2].end()),
	};
	const Array<double> heights(fixedHeights.begin(), fixedHeights.end());
	const Vec2 uvScale{ 2.0, 0.5 };
	const Vec2 uvOffset{ 0.25, -0.5 };
	const Mesh3D expected = Mesh3D::Loft(fixedSections, fixedHeights, uvScale, uvOffset);

	const Mesh3D arrayResult = Mesh3D::Loft(sections, heights, uvScale, uvOffset);
	CheckMeshDataEqual(arrayResult, expected);

	Array<std::span<const Vec2>> sectionViews(sections.size());
	for (size_t i = 0; i < sections.size(); ++i)
	{
		sectionViews[i] = sections[i];
	}
	const Mesh3D spanResult = Mesh3D::Loft(sectionViews, heights, uvScale, uvOffset);
	CheckMeshDataEqual(spanResult, expected);
}

TEST_CASE("Mesh3D::Loft runtime invalid dimensions")
{
	const Array<double> twoHeights{ 0.0, 1.0 };
	CHECK(Mesh3D::Loft(Array<Array<Vec2>>{}, Array<double>{}).isEmpty());
	CHECK(Mesh3D::Loft(Array<Array<Vec2>>{
		{ { 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 1.0 } }
	}, Array<double>{ 0.0 }).isEmpty());
	CHECK(Mesh3D::Loft(Array<Array<Vec2>>{
		{ { 0.0, 0.0 }, { 1.0, 0.0 } },
		{ { 0.0, 0.0 }, { 1.0, 0.0 } }
	}, twoHeights).isEmpty());
	CHECK(Mesh3D::Loft(Array<Array<Vec2>>{
		{ { 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 1.0 } },
		{ { 0.0, 0.0 }, { 1.0, 0.0 }, { 1.0, 1.0 }, { 0.0, 1.0 } }
	}, twoHeights).isEmpty());
	CHECK(Mesh3D::Loft(Array<Array<Vec2>>{
		{ { 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 1.0 } },
		{ { 0.0, 0.0 }, { 0.5, 0.0 }, { 0.0, 0.5 } }
	}, Array<double>{ 0.0 }).isEmpty());
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
