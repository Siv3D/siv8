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
	constexpr float FrameEpsilon = 1e-5f;
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;

	static_assert(requires
	{
		static_cast<Mesh3D (*)(const Polygon&, std::span<const Vec3>, const SweepOptions&)>(&Mesh3D::Sweep);
		static_cast<Mesh3D (*)(const Polygon&, std::initializer_list<Vec3>, const SweepOptions&)>(&Mesh3D::Sweep);
		static_cast<Mesh3D (*)(const Polygon&, std::span<const Vec3>, std::span<const SweepSectionTransform>, const SweepOptions&)>(&Mesh3D::Sweep);
		static_cast<Mesh3D (*)(const Polygon&, std::initializer_list<Vec3>, std::initializer_list<SweepSectionTransform>, const SweepOptions&)>(&Mesh3D::Sweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::initializer_list<Vec3>, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, Vec3, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, Vec3, const Quaternion&, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, const Mat4x4&, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, std::span<const SweepSectionTransform>, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::initializer_list<Vec3>, std::initializer_list<SweepSectionTransform>, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, std::span<const SweepSectionTransform>, Vec3, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, std::span<const SweepSectionTransform>, Vec3, const Quaternion&, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(const Polygon&, std::span<const Vec3>, std::span<const SweepSectionTransform>, const Mat4x4&, const SweepOptions&)>(&Mesh3DBuilder::addSweep);
	});

}

TEST_CASE("Mesh3D::Sweep initializer list")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const SweepOptions options{ .uvScale = uvScale, .uvOffset = uvOffset };
	const SweepOptions orientedOptions{
		.initialXAxis = Vec3::UnitX(), .uvScale = uvScale, .uvOffset = uvOffset
	};

	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, {
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
		}, options),
		Mesh3D::Sweep(crossSection, path, options));
	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, {
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
		}, orientedOptions),
		Mesh3D::Sweep(crossSection, path, orientedOptions));
	CHECK(Mesh3D::Sweep(crossSection, {}).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addSweep initializer list")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const SweepOptions options{ .uvScale = uvScale, .uvOffset = uvOffset };
	const SweepOptions orientedOptions{
		.initialXAxis = Vec3::UnitX(), .uvScale = uvScale, .uvOffset = uvOffset
	};
	Mesh3DBuilder builder;
	REQUIRE(builder.addSweep(crossSection, {
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	}, options));
	REQUIRE(builder.addSweep(crossSection, {
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	}, orientedOptions));

	Mesh3D expected = Mesh3D::Sweep(crossSection, path, options);
	REQUIRE(expected.append(Mesh3D::Sweep(crossSection, path, orientedOptions)));
	CheckMeshDataEqual(builder.getMesh(), expected);
	CHECK_FALSE(builder.addSweep(crossSection, {}));
}

TEST_CASE("Mesh3DBuilder::addSweep")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const SweepOptions uvOptions{ .uvScale = uvScale, .uvOffset = uvOffset };
	const SweepOptions orientedOptions{ .initialXAxis = Vec3::UnitX() };
	const SweepOptions orientedUVOptions{
		.initialXAxis = Vec3::UnitX(), .uvScale = uvScale, .uvOffset = uvOffset
	};
	const Mesh3D automatic = Mesh3D::Sweep(crossSection, path);
	const Mesh3D automaticUV = Mesh3D::Sweep(crossSection, path, uvOptions);
	const Mesh3D oriented = Mesh3D::Sweep(crossSection, path, orientedOptions);
	const Mesh3D orientedUV = Mesh3D::Sweep(crossSection, path, orientedUVOptions);

	SUBCASE("Direct append reuses reserved storage")
	{
		Mesh3DBuilder builder;
		builder.reserve((automaticUV.vertexCount() * 2), (automaticUV.triangleCount() * 2));
		REQUIRE(builder.addSweep(crossSection, path, uvOptions));
		const Vertex3D* const vertexData = builder.getMesh().vertices.data();
		const TriangleIndex32* const indexData = builder.getMesh().indices.data();

		const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
		REQUIRE(builder.addSweep(crossSection, path, transform, uvOptions));
		CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
		CHECK_EQ(builder.getMesh().indices.data(), indexData);

		Mesh3D expected = automaticUV;
		REQUIRE(expected.append(automaticUV, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Automatic-frame transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });

		Mesh3DBuilder builder;
		REQUIRE(builder.addSweep(crossSection, path, offset));
		REQUIRE(builder.addSweep(crossSection, path, offset, rotation));
		REQUIRE(builder.addSweep(crossSection, path, transform));
		REQUIRE(builder.addSweep(crossSection, path, offset, uvOptions));
		REQUIRE(builder.addSweep(crossSection, path, offset, rotation, uvOptions));
		REQUIRE(builder.addSweep(crossSection, path, transform, uvOptions));

		Mesh3D expected;
		REQUIRE(expected.append(automatic, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(automatic,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(automatic, transform));
		REQUIRE(expected.append(automaticUV, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(automaticUV,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(automaticUV, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Named initial-axis transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });

		Mesh3DBuilder builder;
		REQUIRE(builder.addSweep(crossSection, path, orientedOptions));
		REQUIRE(builder.addSweep(crossSection, path, offset, orientedOptions));
		REQUIRE(builder.addSweep(crossSection, path, offset, rotation, orientedOptions));
		REQUIRE(builder.addSweep(crossSection, path, transform, orientedOptions));
		REQUIRE(builder.addSweep(crossSection, path, offset, orientedUVOptions));
		REQUIRE(builder.addSweep(crossSection, path, offset, rotation, orientedUVOptions));
		REQUIRE(builder.addSweep(crossSection, path, transform, orientedUVOptions));

		Mesh3D expected = oriented;
		REQUIRE(expected.append(oriented, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(oriented,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(oriented, transform));
		REQUIRE(expected.append(orientedUV, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(orientedUV,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(orientedUV, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addSweep(Polygon{}, path));
		CHECK_FALSE(builder.addSweep(
			crossSection, {}, orientedOptions));
		CHECK_FALSE(builder.addSweep(
			crossSection, path, SweepOptions{ .initialXAxis = Vec3::UnitY() }));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3D::Sweep rectangle and UV repeat")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { -2.0, 1.0 }
	} };
	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 0.0, 3.0, 0.0 } };
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mesh3D mesh = Mesh3D::Sweep(
		crossSection, path,
		SweepOptions{ .initialXAxis = Vec3::UnitX(),
			.uvScale = uvScale, .uvOffset = uvOffset });
	const size_t capVertexCount = (crossSection.vertices().size() * 2);

	CHECK_EQ(mesh.vertexCount(), size_t{ 24 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 12 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.1f, 0.2f });
	CHECK_EQ(mesh.vertices[0].tangent, Float4{ 1.0f, 0.0f, 0.0f, 1.0f });
	CHECK_EQ(mesh.vertices[0].bitangent(), Float3::UnitZ());

	CHECK_EQ(mesh.vertices[capVertexCount + 0].pos, Float3{ -2.0f, 0.0f, -1.0f });
	CHECK_EQ(mesh.vertices[capVertexCount + 0].normal, -Float3::UnitZ());
	CHECK_EQ(mesh.vertices[capVertexCount + 0].tex, Float2{ 0.1f, 0.2f });
	CHECK(mesh.vertices[capVertexCount + 1].tex.x
		== doctest::Approx(0.1 + (2.0 / 3.0)).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[capVertexCount + 2].tex, Float2{ 0.1f, 0.95f });
	CHECK_EQ(mesh.vertices[capVertexCount + 0].bitangent(), Float3::UnitY());

	const Mesh3D automatic = Mesh3D::Sweep(
		crossSection, path, SweepOptions{ .uvScale = uvScale, .uvOffset = uvOffset });
	CheckMeshGeometry(automatic);
	CHECK_EQ(automatic.vertices[0].pos, mesh.vertices[0].pos);
}

TEST_CASE("Mesh3D::Sweep end-cap options")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -1.0, -0.5 }, { 1.0, -0.5 }, { 1.0, 0.5 }, { -1.0, 0.5 }
	} };
	const Array<Vec3> path{ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } };
	const Array<SweepSectionTransform> transforms{
		{}, { .scale = Vec2{ 1.5, 0.75 }, .twist = 0.1 }
	};
	const SweepOptions common{
		.initialXAxis = Vec3::UnitX(),
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
	};
	const size_t capVertexCount = crossSection.vertices().size();
	const size_t capTriangleCount = crossSection.indices().size();
	const size_t sideVertexCount = (crossSection.outer().size() * path.size() * 2);
	const size_t sideTriangleCount = (crossSection.outer().size() * 2);

	const Mesh3D defaultCaps = Mesh3D::Sweep(crossSection, path, common);
	CheckMeshDataEqual(Mesh3D::Sweep(crossSection, path,
		SweepOptions{ .initialXAxis = Vec3::UnitX(), .uvScale = common.uvScale,
			.uvOffset = common.uvOffset, .endCaps = Mesh3DEndCaps::Both }), defaultCaps);

	const Mesh3D noCaps = Mesh3D::Sweep(crossSection, path,
		SweepOptions{ .initialXAxis = Vec3::UnitX(), .endCaps = Mesh3DEndCaps::None });
	const Mesh3D startCap = Mesh3D::Sweep(crossSection, path,
		SweepOptions{ .initialXAxis = Vec3::UnitX(), .endCaps = Mesh3DEndCaps::Start });
	const Mesh3D endCap = Mesh3D::Sweep(crossSection, path, transforms,
		SweepOptions{ .initialXAxis = Vec3::UnitX(), .endCaps = Mesh3DEndCaps::End });

	CHECK_EQ(noCaps.vertexCount(), sideVertexCount);
	CHECK_EQ(noCaps.triangleCount(), sideTriangleCount);
	CHECK_EQ(startCap.vertexCount(), (sideVertexCount + capVertexCount));
	CHECK_EQ(startCap.triangleCount(), (sideTriangleCount + capTriangleCount));
	CHECK_EQ(endCap.vertexCount(), (sideVertexCount + capVertexCount));
	CHECK_EQ(endCap.triangleCount(), (sideTriangleCount + capTriangleCount));
	CheckMeshGeometry(noCaps);
	CheckMeshGeometry(startCap);
	CheckMeshGeometry(endCap);
	CHECK_EQ(startCap.vertices[0].normal, -Float3::UnitY());
	CHECK_EQ(endCap.vertices[0].normal, Float3::UnitY());

	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection,
			{ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } }, common),
		defaultCaps);

	const Array<Vec3> closedPath{
		{ -2.0, 0.0, -2.0 }, { 2.0, 0.0, -2.0 },
		{ 2.0, 0.0, 2.0 }, { -2.0, 0.0, 2.0 }
	};
	const SweepOptions closedOptions{
		.initialXAxis = Vec3::UnitY(), .closeRing = CloseRing::Yes
	};
	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, closedPath, closedOptions),
		Mesh3D::Sweep(crossSection, closedPath,
			SweepOptions{ .initialXAxis = Vec3::UnitY(),
				.closeRing = CloseRing::Yes, .endCaps = Mesh3DEndCaps::None }));
	CHECK_FALSE(Mesh3D::Sweep(crossSection, closedPath,
		SweepOptions{ .initialXAxis = Vec3::UnitY(), .closeRing = CloseRing::Yes,
			.endCaps = Mesh3DEndCaps::None }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, closedPath,
		SweepOptions{ .initialXAxis = Vec3::UnitY(), .closeRing = CloseRing::Yes,
			.endCaps = Mesh3DEndCaps::End }).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addSweep options placement overloads")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const SweepOptions options{
		.initialXAxis = Vec3::UnitX(),
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
		.endCaps = Mesh3DEndCaps::Start,
	};
	const Mesh3D source = Mesh3D::Sweep(crossSection, path, options);
	const Vec3 offset{ 3.0, 4.0, 5.0 };
	const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
	const Mat4x4 transform = Mat4x4::AffineTransform(
		Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });

	Mesh3DBuilder builder;
	builder.reserve((source.vertexCount() * 5), (source.triangleCount() * 5));
	REQUIRE(builder.addSweep(crossSection, path, options));
	const Vertex3D* const vertexData = builder.getMesh().vertices.data();
	const TriangleIndex32* const indexData = builder.getMesh().indices.data();
	REQUIRE(builder.addSweep(crossSection,
		{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 } }, options));
	REQUIRE(builder.addSweep(crossSection, path, offset, options));
	REQUIRE(builder.addSweep(crossSection, path, offset, rotation, options));
	REQUIRE(builder.addSweep(crossSection, path, transform, options));
	CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
	CHECK_EQ(builder.getMesh().indices.data(), indexData);

	Mesh3D expected = source;
	REQUIRE(expected.append(source));
	REQUIRE(expected.append(source, Mat4x4::Translate(Float3{ offset })));
	REQUIRE(expected.append(source,
		Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
	REQUIRE(expected.append(source, transform));
	CheckMeshDataEqual(builder.getMesh(), expected);
}

TEST_CASE("Mesh3D::Sweep per-point identity transforms preserve constant Sweep data")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Array<SweepSectionTransform> transforms(path.size());
	const SweepOptions options{
		.initialXAxis = Vec3::UnitX(),
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
	};

	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, path, transforms, options),
		Mesh3D::Sweep(crossSection, path, options));
	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, {
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
		}, { {}, {}, {} }, options),
		Mesh3D::Sweep(crossSection, path, transforms, options));
}

TEST_CASE("Mesh3D::Sweep per-point positive twist direction")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { -2.0, 1.0 }
	} };
	const Array<Vec3> path{ Vec3::Zero(), Vec3{ 0.0, 3.0, 0.0 } };
	const Array<SweepSectionTransform> transforms{
		{ .twist = Math::HalfPi }, { .twist = Math::HalfPi }
	};
	const SweepOptions options{ .initialXAxis = Vec3::UnitX() };
	const Mesh3D mesh = Mesh3D::Sweep(crossSection, path, transforms, options);
	const size_t sideVertexBase = (crossSection.vertices().size() * 2);

	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ -1.0f, 0.0f, 2.0f });
	CHECK(mesh.vertices[0].tangent.xyz().dot(-Float3::UnitZ())
		== doctest::Approx(1.0f).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[0].tangent.w, 1.0f);
	CHECK_EQ(mesh.vertices[sideVertexBase].pos, Float3{ -1.0f, 0.0f, 2.0f });
	CHECK(mesh.vertices[sideVertexBase].normal.dot(-Float3::UnitX())
		== doctest::Approx(1.0f).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[sideVertexBase].tangent.xyz().dot(-Float3::UnitZ())
		== doctest::Approx(1.0f).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[sideVertexBase].tangent.w, -1.0f);
}

TEST_CASE("Mesh3D::Sweep per-point varying scale and twist")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -1.0, -1.0 }, { 1.0, -1.0 }, { 1.0, 1.0 }, { -1.0, 1.0 }
	} };
	const Array<Vec3> path{
		Vec3::Zero(), Vec3{ 0.0, 4.0, 0.0 }, Vec3{ 0.0, 8.0, 0.0 }
	};
	const Array<SweepSectionTransform> transforms{
		{ .scale = Vec2{ 1.0, 1.0 }, .twist = 0.0 },
		{ .scale = Vec2{ 2.0, 0.5 }, .twist = 0.1 },
		{ .scale = Vec2{ 1.5, 1.25 }, .twist = 0.2 },
	};
	const SweepOptions options{
		.initialXAxis = Vec3::UnitX(),
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
	};
	const Mesh3D mesh = Mesh3D::Sweep(crossSection, path, transforms, options);
	const size_t capVertexCount = (crossSection.vertices().size() * 2);
	const size_t verticesPerEdge = (path.size() * 2);

	CHECK_EQ(mesh.vertexCount(), size_t{ 32 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 20 });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[capVertexCount].pos, Float3{ -1.0f, 0.0f, -1.0f });
	CHECK_NE(mesh.vertices[capVertexCount + 2].normal,
		mesh.vertices[capVertexCount].normal);
	CHECK_NE(mesh.vertices[capVertexCount + verticesPerEdge].tangent,
		mesh.vertices[capVertexCount].tangent);
	CHECK_EQ(mesh.vertices[capVertexCount].tex, Float2{ 0.1f, 0.2f });
	CHECK(mesh.vertices[capVertexCount + 4].tex.y
		== doctest::Approx(2.2f).epsilon(FrameEpsilon));
}

TEST_CASE("Mesh3D::Sweep per-point closed seam")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ -2.0, 0.0, -2.0 }, { 2.0, 0.0, -2.0 },
		{ 2.0, 0.0, 2.0 }, { -2.0, 0.0, 2.0 }
	};
	const Array<SweepSectionTransform> transforms{
		{ .scale = Vec2{ 1.0, 1.0 }, .twist = 0.0 },
		{ .scale = Vec2{ 1.1, 0.9 }, .twist = 0.1 },
		{ .scale = Vec2{ 1.2, 1.0 }, .twist = 0.2 },
		{ .scale = Vec2{ 0.9, 1.1 }, .twist = 0.1 },
	};
	const SweepOptions options{
		.initialXAxis = Vec3::UnitY(),
		.closeRing = CloseRing::Yes,
	};
	const Mesh3D mesh = Mesh3D::Sweep(crossSection, path, transforms, options);
	const size_t stationCount = (path.size() + 1);
	const size_t verticesPerEdge = (stationCount * 2);

	CHECK_EQ(mesh.vertexCount(), (crossSection.outer().size() * verticesPerEdge));
	CHECK_EQ(mesh.triangleCount(), (2 * crossSection.outer().size() * path.size()));
	CheckMeshGeometry(mesh);
	for (size_t edgeIndex = 0; edgeIndex < crossSection.outer().size(); ++edgeIndex)
	{
		const size_t edgeBase = (edgeIndex * verticesPerEdge);
		const size_t seamBase = (edgeBase + path.size() * 2);
		for (size_t endpoint = 0; endpoint < 2; ++endpoint)
		{
			const Vertex3D& first = mesh.vertices[edgeBase + endpoint];
			const Vertex3D& seam = mesh.vertices[seamBase + endpoint];
			CHECK_EQ(seam.pos, first.pos);
			CHECK_EQ(seam.normal, first.normal);
			CHECK_EQ(seam.tangent, first.tangent);
			CHECK_EQ(seam.tex.x, first.tex.x);
		}
	}
}

TEST_CASE("Mesh3DBuilder::addSweep per-point transforms")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Array<SweepSectionTransform> transforms{
		{}, { .scale = Vec2{ 1.2, 0.8 }, .twist = 0.1 },
		{ .scale = Vec2{ 0.9, 1.1 }, .twist = 0.2 }
	};
	const SweepOptions options{
		.initialXAxis = Vec3::UnitX(),
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
	};
	const Mesh3D source = Mesh3D::Sweep(crossSection, path, transforms, options);
	const Vec3 offset{ 3.0, 4.0, 5.0 };
	const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
	const Mat4x4 transform = Mat4x4::AffineTransform(
		Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });

	Mesh3DBuilder builder;
	builder.reserve((source.vertexCount() * 5), (source.triangleCount() * 5));
	REQUIRE(builder.addSweep(crossSection, path, transforms, options));
	const Vertex3D* const vertexData = builder.getMesh().vertices.data();
	const TriangleIndex32* const indexData = builder.getMesh().indices.data();
	REQUIRE(builder.addSweep(crossSection, {
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	}, { {}, { Vec2{ 1.2, 0.8 }, 0.1 }, { Vec2{ 0.9, 1.1 }, 0.2 } }, options));
	REQUIRE(builder.addSweep(crossSection, path, transforms, offset, options));
	REQUIRE(builder.addSweep(crossSection, path, transforms, offset, rotation, options));
	REQUIRE(builder.addSweep(crossSection, path, transforms, transform, options));
	CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
	CHECK_EQ(builder.getMesh().indices.data(), indexData);

	Mesh3D expected = source;
	REQUIRE(expected.append(source));
	REQUIRE(expected.append(source, Mat4x4::Translate(Float3{ offset })));
	REQUIRE(expected.append(source,
		Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
	REQUIRE(expected.append(source, transform));
	CheckMeshDataEqual(builder.getMesh(), expected);
}

TEST_CASE("Mesh3D::Sweep bent non-planar path")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 2.0, 0.0 },
		{ 1.0, 3.0, 1.0 },
		{ 2.0, 3.0, 2.0 }
	};
	const Mesh3D mesh = Mesh3D::Sweep(
		crossSection, path, SweepOptions{ .initialXAxis = Vec3::UnitX() });

	CHECK_EQ(mesh.vertexCount(), size_t{ 40 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 28 });
	CheckMeshGeometry(mesh);
	const size_t sideVertexBase = (crossSection.vertices().size() * 2);
	const size_t verticesPerEdge = (path.size() * 2);
	CHECK(mesh.vertices[sideVertexBase].normal.dot(
		mesh.vertices[sideVertexBase + 2].normal) > 0.0f);
	CHECK_NE(mesh.vertices[sideVertexBase].normal,
		mesh.vertices[sideVertexBase + verticesPerEdge].normal);
}

TEST_CASE("Mesh3D::Sweep closed non-planar path")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 2.0, 0.0, 0.0 },
		{ 2.0, 1.0, 2.0 },
		{ 0.0, 2.0, 2.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const SweepOptions options{
		.initialXAxis = Vec3::UnitY(),
		.uvScale = uvScale,
		.uvOffset = uvOffset,
		.closeRing = CloseRing::Yes,
	};
	const Mesh3D mesh = Mesh3D::Sweep(
		crossSection, path, options);
	const size_t edgeCount = crossSection.outer().size();
	const size_t stationCount = (path.size() + 1);
	const size_t verticesPerEdge = (stationCount * 2);

	CHECK_EQ(mesh.vertexCount(), (edgeCount * verticesPerEdge));
	CHECK_EQ(mesh.triangleCount(), (2 * edgeCount * path.size()));
	CheckMeshGeometry(mesh);

	for (size_t edgeIndex = 0; edgeIndex < edgeCount; ++edgeIndex)
	{
		const size_t edgeBase = (edgeIndex * verticesPerEdge);
		const size_t seamBase = (edgeBase + path.size() * 2);
		for (size_t endpoint = 0; endpoint < 2; ++endpoint)
		{
			const Vertex3D& first = mesh.vertices[edgeBase + endpoint];
			const Vertex3D& seam = mesh.vertices[seamBase + endpoint];
			CHECK_EQ(seam.pos, first.pos);
			CHECK_EQ(seam.normal, first.normal);
			CHECK_EQ(seam.tangent, first.tangent);
			CHECK_EQ(seam.tex.x, first.tex.x);
		}
	}

	const double totalLength = (2.0 + std::sqrt(5.0) + std::sqrt(5.0) + std::sqrt(8.0));
	CHECK(mesh.vertices[path.size() * 2].tex.y
		== doctest::Approx(uvOffset.y + uvScale.y * totalLength).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[(path.size() - 1) * 2].normal.dot(
		mesh.vertices[path.size() * 2].normal) > 0.0f);

	CheckMeshDataEqual(
		Mesh3D::Sweep(crossSection, {
			{ 0.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 },
			{ 2.0, 1.0, 2.0 }, { 0.0, 2.0, 2.0 }
		}, options),
		mesh);
}

TEST_CASE("Mesh3DBuilder::addSweep closed path")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -0.5, -0.25 }, { 0.5, -0.25 }, { 0.5, 0.25 }, { -0.5, 0.25 }
	} };
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 },
		{ 2.0, 1.0, 2.0 }, { 0.0, 2.0, 2.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
	const SweepOptions options{
		.initialXAxis = Vec3::UnitY(),
		.uvScale = uvScale,
		.uvOffset = uvOffset,
		.closeRing = CloseRing::Yes,
	};
	const Mesh3D source = Mesh3D::Sweep(
		crossSection, path, options);

	Mesh3DBuilder builder;
	REQUIRE(builder.addSweep(crossSection, path, transform, options));
	Mesh3D expected;
	REQUIRE(expected.append(source, transform));
	CheckMeshDataEqual(builder.getMesh(), expected);

	Mesh3DBuilder initializerListBuilder;
	REQUIRE(initializerListBuilder.addSweep(crossSection, {
		{ 0.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 },
		{ 2.0, 1.0, 2.0 }, { 0.0, 2.0, 2.0 }
	}, options));
	CheckMeshDataEqual(initializerListBuilder.getMesh(), source);
}

TEST_CASE("Mesh3D::Sweep polygon with a hole")
{
	const Polygon crossSection{
		Array<Vec2>{
			{ -3.0, -3.0 }, { 3.0, -3.0 }, { 3.0, 3.0 }, { -3.0, 3.0 }
		},
		Array<Array<Vec2>>{ Array<Vec2>{
			{ -1.0, -1.0 }, { -1.0, 1.0 }, { 1.0, 1.0 }, { 1.0, -1.0 }
		} }
	};
	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 4.0, 0.0, 0.0 } };
	const Mesh3D mesh = Mesh3D::Sweep(
		crossSection, path, SweepOptions{ .initialXAxis = Vec3::UnitY() });
	const size_t edgeCount = (crossSection.outer().size() + crossSection.inners()[0].size());

	CHECK_EQ(mesh.vertexCount(), ((crossSection.vertices().size() * 2) + (edgeCount * 4)));
	CHECK_EQ(mesh.triangleCount(), ((crossSection.indices().size() * 2) + (edgeCount * 2)));
	CheckMeshGeometry(mesh);

	const size_t innerSideBase = ((crossSection.vertices().size() * 2)
		+ (crossSection.outer().size() * 4));
	CHECK_EQ(mesh.vertices[innerSideBase].normal, Float3::UnitY());

	const Array<SweepSectionTransform> transforms{
		{ .scale = Vec2{ 1.0, 1.0 }, .twist = 0.1 },
		{ .scale = Vec2{ 1.2, 0.8 }, .twist = 0.2 }
	};
	const Mesh3D transformed = Mesh3D::Sweep(
		crossSection, path, transforms,
		SweepOptions{ .initialXAxis = Vec3::UnitY() });
	CHECK_EQ(transformed.vertexCount(), mesh.vertexCount());
	CHECK_EQ(transformed.triangleCount(), mesh.triangleCount());
	CheckMeshGeometry(transformed);
}

TEST_CASE("Mesh3D::Sweep initial orientation")
{
	const Polygon crossSection{ Array<Vec2>{
		{ -2.0, -1.0 }, { 2.0, -1.0 }, { 2.0, 1.0 }, { -2.0, 1.0 }
	} };
	const Array<Vec3> path{ { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 3.0 } };
	const Mesh3D xOriented = Mesh3D::Sweep(
		crossSection, path, SweepOptions{ .initialXAxis = Vec3::UnitX() });
	const Mesh3D yOriented = Mesh3D::Sweep(
		crossSection, path, SweepOptions{ .initialXAxis = Vec3::UnitY() });

	CheckMeshGeometry(xOriented);
	CheckMeshGeometry(yOriented);
	CHECK_EQ(xOriented.vertices[0].pos, Float3{ -2.0f, 1.0f, 0.0f });
	CHECK_EQ(yOriented.vertices[0].pos, Float3{ -1.0f, -2.0f, 0.0f });
}

TEST_CASE("Mesh3D::Sweep invalid arguments")
{
	const Array<Vec2> outer{
		{ 0.0, 0.0 }, { 2.0, 0.0 }, { 2.0, 2.0 }, { 0.0, 2.0 }
	};
	const Polygon crossSection{ outer };
	const Array<Vec3> validPath{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 } };
	const Array<SweepSectionTransform> validTransforms{
		{}, { .scale = Vec2{ 1.5, 0.5 }, .twist = 0.1 }
	};
	const SweepOptions closedOptions{ .closeRing = CloseRing::Yes };

	CHECK(Mesh3D::Sweep(Polygon{}, validPath).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, std::span<const Vec3>{}).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 0.0, 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 } },
		closedOptions).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 },
			{ 2.0, 2.0, 0.0 }, { 0.0, 0.0, 0.0 }
		}, closedOptions).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{
			{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 }
		}, closedOptions).isEmpty());
	CHECK_FALSE(Mesh3D::Sweep(crossSection,
		Array<Vec3>{
			{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }
		}, closedOptions).isEmpty());
	CHECK(Mesh3D::Sweep(
		crossSection, validPath, SweepOptions{ .initialXAxis = Vec3::Zero() }).isEmpty());
	CHECK(Mesh3D::Sweep(
		crossSection, validPath, SweepOptions{ .initialXAxis = Vec3::UnitY() }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		SweepOptions{ .initialXAxis = Vec3{
			std::numeric_limits<double>::infinity(), 0.0, 0.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		SweepOptions{ .uvScale = Vec2{
			std::numeric_limits<double>::quiet_NaN(), 1.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		SweepOptions{ .uvScale = Vec2{
			1.0, std::numeric_limits<float>::max() } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Array<SweepSectionTransform>{ {} }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Array<SweepSectionTransform>{ {}, { .scale = Vec2{ 0.0, 1.0 } } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Array<SweepSectionTransform>{ {}, { .scale = Vec2{ -1.0, 1.0 } } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Array<SweepSectionTransform>{ {}, {
			.scale = Vec2{ std::numeric_limits<double>::denorm_min(), 1.0 }
		} }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Array<SweepSectionTransform>{ {}, {
			.scale = Vec2{ std::numeric_limits<double>::infinity(), 1.0 }
		} }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath,
		Array<SweepSectionTransform>{ {}, {
			.twist = std::numeric_limits<double>::quiet_NaN()
		} }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath, validTransforms,
		SweepOptions{ .initialXAxis = Vec3::UnitY() }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection, validPath, validTransforms,
		SweepOptions{ .uvScale = Vec2{ std::numeric_limits<double>::infinity(), 1.0 } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ Vec3::Zero(), Vec3{ 0.0, 0.01, 0.0 } },
		Array<SweepSectionTransform>{ {}, { .twist = Math::QuarterPi } }).isEmpty());
	CHECK(Mesh3D::Sweep(crossSection,
		Array<Vec3>{ Vec3::Zero(), Vec3::Zero() }, validTransforms).isEmpty());

	const Polygon invalidIndices{
		outer,
		Array<TriangleIndex>{ TriangleIndex{ 0, 1, 9 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Sweep(invalidIndices, validPath).isEmpty());

	const Array<Vec2> reversedOuter{
		{ 0.0, 0.0 }, { 0.0, 2.0 }, { 2.0, 2.0 }, { 2.0, 0.0 }
	};
	const Polygon invalidOuter{
		reversedOuter,
		Array<TriangleIndex>{ TriangleIndex{ 0, 2, 1 }, TriangleIndex{ 0, 3, 2 } },
		RectF{ 0.0, 0.0, 2.0, 2.0 },
		SkipValidation::Yes
	};
	CHECK(Mesh3D::Sweep(invalidOuter, validPath).isEmpty());
}
