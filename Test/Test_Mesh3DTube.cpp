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
		static_cast<Mesh3D (*)(std::span<const Vec3>, double, const TubeOptions&)>(&Mesh3D::Tube);
		static_cast<Mesh3D (*)(std::initializer_list<Vec3>, double, const TubeOptions&)>(&Mesh3D::Tube);
		static_cast<Mesh3D (*)(std::span<const Vec3>, std::span<const double>, const TubeOptions&)>(&Mesh3D::Tube);
		static_cast<Mesh3D (*)(std::initializer_list<Vec3>, std::initializer_list<double>, const TubeOptions&)>(&Mesh3D::Tube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, double, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::initializer_list<Vec3>, double, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, std::span<const double>, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::initializer_list<Vec3>, std::initializer_list<double>, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, double, Vec3, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, double, Vec3, const Quaternion&, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, double, const Mat4x4&, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, std::span<const double>, Vec3, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, std::span<const double>, Vec3, const Quaternion&, const TubeOptions&)>(&Mesh3DBuilder::addTube);
		static_cast<Mesh3DAddResult (Mesh3DBuilder::*)(std::span<const Vec3>, std::span<const double>, const Mat4x4&, const TubeOptions&)>(&Mesh3DBuilder::addTube);
	});

}

TEST_CASE("Mesh3D::Tube initializer list")
{
	const Array<Vec3> path{
		{ 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 2.0, 1.0 }
	};
	const TubeOptions options{
		.sides = 8,
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
	};
	CheckMeshDataEqual(
		Mesh3D::Tube({
			{ 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 2.0, 1.0 }
		}, 0.5, options),
		Mesh3D::Tube(path, 0.5, options));
	CHECK(Mesh3D::Tube({}, 0.5).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addTube initializer list")
{
	const Array<Vec3> path{
		{ 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 2.0, 1.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const TubeOptions options{ .sides = 8, .uvScale = uvScale, .uvOffset = uvOffset };
	Mesh3DBuilder builder;
	REQUIRE(builder.addTube({
		{ 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 1.0, 2.0, 1.0 }
	}, 0.5, options));
	CheckMeshDataEqual(builder.getMesh(), Mesh3D::Tube(path, 0.5, options));
	CHECK_FALSE(builder.addTube({}, 0.5));
}

TEST_CASE("Mesh3DBuilder::addTube")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	constexpr double Radius = 0.25;
	constexpr uint32 Sides = 8;
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const TubeOptions options{ .sides = Sides, .uvScale = uvScale, .uvOffset = uvOffset };
	const TubeOptions defaultUVOptions{ .sides = Sides };
	const Mesh3D source = Mesh3D::Tube(path, Radius, options);

	SUBCASE("Direct append reuses reserved storage")
	{
		Mesh3DBuilder builder;
		builder.reserve((source.vertexCount() * 2), (source.triangleCount() * 2));
		REQUIRE(builder.addTube(path, Radius, options));
		const Vertex3D* const vertexData = builder.getMesh().vertices.data();
		const TriangleIndex32* const indexData = builder.getMesh().indices.data();

		const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
		REQUIRE(builder.addTube(path, Radius, transform, options));
		CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
		CHECK_EQ(builder.getMesh().indices.data(), indexData);

		Mesh3D expected = source;
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });
		const Mesh3D defaultUVSource = Mesh3D::Tube(path, Radius, defaultUVOptions);

		Mesh3DBuilder builder;
		REQUIRE(builder.addTube(path, Radius, offset, defaultUVOptions));
		REQUIRE(builder.addTube(path, Radius, offset, rotation, defaultUVOptions));
		REQUIRE(builder.addTube(path, Radius, transform, defaultUVOptions));
		REQUIRE(builder.addTube(path, Radius, offset, options));
		REQUIRE(builder.addTube(path, Radius, offset, rotation, options));
		REQUIRE(builder.addTube(path, Radius, transform, options));

		Mesh3D expected;
		REQUIRE(expected.append(defaultUVSource, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(defaultUVSource,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(defaultUVSource, transform));
		REQUIRE(expected.append(source, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(source,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Failure leaves existing content unchanged")
	{
		Mesh3DBuilder builder;
		REQUIRE(builder.addBox());
		const Mesh3D expected = builder.getMesh();

		CHECK_FALSE(builder.addTube({}, Radius, defaultUVOptions));
		CHECK_FALSE(builder.addTube(path, 0.0, defaultUVOptions));
		CHECK_FALSE(builder.addTube(path, Radius, TubeOptions{ .sides = 2 }));
		CHECK_FALSE(builder.addTube(
			path, 0.0, Mat4x4::Identity(), defaultUVOptions));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3D::Tube straight path and UV repeat")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> path{ { 0.0, -1.0, 0.0 }, { 0.0, 2.0, 0.0 } };
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mesh3D mesh = Mesh3D::Tube(path, 2.0,
		TubeOptions{ .sides = Sides, .uvScale = uvScale, .uvOffset = uvOffset });
	const size_t ringStride = (Sides + 1);
	const size_t startCapBase = (path.size() * ringStride);
	const size_t endCapBase = (startCapBase + ringStride);

	CHECK_EQ(mesh.vertexCount(), size_t{ 4 * ringStride });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Sides });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 2.0f, -1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[0].normal, Float3::UnitX());
	CHECK_EQ(mesh.vertices[0].tangent, Float4{ 0.0f, 0.0f, 1.0f, -1.0f });
	CHECK_EQ(mesh.vertices[0].bitangent(), Float3::UnitY());
	CHECK_EQ(mesh.vertices[0].tex, Float2{ 0.1f, 0.2f });
	CHECK_EQ(mesh.vertices[Sides].pos, mesh.vertices[0].pos);
	CHECK_EQ(mesh.vertices[Sides].normal, mesh.vertices[0].normal);
	CHECK_EQ(mesh.vertices[Sides].tex, Float2{ 2.1f, 0.2f });
	CHECK_EQ(mesh.vertices[ringStride].tex, Float2{ 0.1f, 0.95f });
	CHECK_EQ(mesh.vertices[startCapBase].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[endCapBase].normal, Float3::UnitY());
	CHECK_EQ(mesh.vertices[startCapBase].tex, Float2{ 1.1f, 0.325f });
	CheckMeshGeometry(Mesh3D::Tube(path, 2.0));
}

TEST_CASE("Mesh3D::Tube end-cap options")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> path{ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } };
	const Array<double> radii{ 0.5, 1.0 };
	const size_t ringStride = (Sides + 1);
	const TubeOptions common{
		.sides = Sides,
		.uvScale = Vec2{ 2.0, 0.25 },
		.uvOffset = Vec2{ 0.1, 0.2 },
	};

	const Mesh3D defaultCaps = Mesh3D::Tube(path, 0.5, common);
	CheckMeshDataEqual(Mesh3D::Tube(path, 0.5,
		TubeOptions{ .sides = Sides, .uvScale = common.uvScale,
			.uvOffset = common.uvOffset, .endCaps = Mesh3DEndCaps::Both }), defaultCaps);

	const Mesh3D noCaps = Mesh3D::Tube(path, 0.5,
		TubeOptions{ .sides = Sides, .endCaps = Mesh3DEndCaps::None });
	const Mesh3D startCap = Mesh3D::Tube(path, 0.5,
		TubeOptions{ .sides = Sides, .endCaps = Mesh3DEndCaps::Start });
	const Mesh3D endCap = Mesh3D::Tube(path, radii,
		TubeOptions{ .sides = Sides, .endCaps = Mesh3DEndCaps::End });

	CHECK_EQ(noCaps.vertexCount(), (path.size() * ringStride));
	CHECK_EQ(noCaps.triangleCount(), (2 * Sides));
	CHECK_EQ(startCap.vertexCount(), ((path.size() + 1) * ringStride));
	CHECK_EQ(startCap.triangleCount(), (3 * Sides));
	CHECK_EQ(endCap.vertexCount(), ((path.size() + 1) * ringStride));
	CHECK_EQ(endCap.triangleCount(), (3 * Sides));
	CheckMeshGeometry(noCaps);
	CheckMeshGeometry(startCap);
	CheckMeshGeometry(endCap);
	CHECK_EQ(startCap.vertices[path.size() * ringStride].normal, -Float3::UnitY());
	CHECK_EQ(endCap.vertices[path.size() * ringStride].normal, Float3::UnitY());
	CHECK_EQ(endCap.vertices[path.size() * ringStride + 1].pos, Float3{ 1.0f, 2.0f, 0.0f });

	CheckMeshDataEqual(
		Mesh3D::Tube({ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } },
			{ 0.5, 1.0 }, TubeOptions{ .sides = Sides, .endCaps = Mesh3DEndCaps::End }),
		endCap);

	const Array<Vec3> closedPath{
		{ -2.0, 0.0, -2.0 }, { 2.0, 0.0, -2.0 },
		{ 2.0, 0.0, 2.0 }, { -2.0, 0.0, 2.0 }
	};
	const TubeOptions closedOptions{ .sides = Sides, .closeRing = CloseRing::Yes };
	CheckMeshDataEqual(
		Mesh3D::Tube(closedPath, 0.5, closedOptions),
		Mesh3D::Tube(closedPath, 0.5,
			TubeOptions{ .sides = Sides, .closeRing = CloseRing::Yes,
				.endCaps = Mesh3DEndCaps::None }));
	CHECK_FALSE(Mesh3D::Tube(closedPath, 0.5,
		TubeOptions{ .sides = Sides, .closeRing = CloseRing::Yes,
			.endCaps = Mesh3DEndCaps::None }).isEmpty());
	CHECK(Mesh3D::Tube(closedPath, 0.5,
		TubeOptions{ .sides = Sides, .closeRing = CloseRing::Yes,
			.endCaps = Mesh3DEndCaps::Start }).isEmpty());
}

TEST_CASE("Mesh3DBuilder::addTube options placement overloads")
{
	const Array<Vec3> path{ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } };
	const Array<double> radii{ 0.5, 1.0 };
	const TubeOptions options{ .sides = 8, .endCaps = Mesh3DEndCaps::End };
	const Mesh3D constant = Mesh3D::Tube(path, 0.5, options);
	const Mesh3D variable = Mesh3D::Tube(path, radii, options);
	const Vec3 offset{ 3.0, 4.0, 5.0 };
	const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
	const Mat4x4 transform = Mat4x4::AffineTransform(
		Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });

	Mesh3DBuilder builder;
	builder.reserve((constant.vertexCount() * 5 + variable.vertexCount() * 5),
		(constant.triangleCount() * 5 + variable.triangleCount() * 5));
	REQUIRE(builder.addTube(path, 0.5, options));
	const Vertex3D* const vertexData = builder.getMesh().vertices.data();
	const TriangleIndex32* const indexData = builder.getMesh().indices.data();
	REQUIRE(builder.addTube({ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } }, 0.5, options));
	REQUIRE(builder.addTube(path, 0.5, offset, options));
	REQUIRE(builder.addTube(path, 0.5, offset, rotation, options));
	REQUIRE(builder.addTube(path, 0.5, transform, options));
	REQUIRE(builder.addTube(path, radii, options));
	REQUIRE(builder.addTube({ Vec3::Zero(), Vec3{ 0.0, 2.0, 0.0 } }, { 0.5, 1.0 }, options));
	REQUIRE(builder.addTube(path, radii, offset, options));
	REQUIRE(builder.addTube(path, radii, offset, rotation, options));
	REQUIRE(builder.addTube(path, radii, transform, options));
	CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
	CHECK_EQ(builder.getMesh().indices.data(), indexData);

	Mesh3D expected = constant;
	REQUIRE(expected.append(constant));
	REQUIRE(expected.append(constant, Mat4x4::Translate(Float3{ offset })));
	REQUIRE(expected.append(constant,
		Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
	REQUIRE(expected.append(constant, transform));
	REQUIRE(expected.append(variable));
	REQUIRE(expected.append(variable));
	REQUIRE(expected.append(variable, Mat4x4::Translate(Float3{ offset })));
	REQUIRE(expected.append(variable,
		Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
	REQUIRE(expected.append(variable, transform));
	CheckMeshDataEqual(builder.getMesh(), expected);
}

TEST_CASE("Mesh3D::Tube per-point radii on a straight path")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> path{ { 0.0, -1.0, 0.0 }, { 0.0, 1.0, 0.0 } };
	const Array<double> radii{ 1.0, 2.0 };
	const Mesh3D mesh = Mesh3D::Tube(path, radii, TubeOptions{ .sides = Sides });
	const size_t ringStride = (Sides + 1);
	const size_t startCapBase = (path.size() * ringStride);
	const size_t endCapBase = (startCapBase + ringStride);
	const Float3 expectedNormal = Float3{ 1.0f, -0.5f, 0.0f }.normalized();

	CHECK_EQ(mesh.vertexCount(), size_t{ 4 * ringStride });
	CHECK_EQ(mesh.triangleCount(), size_t{ 4 * Sides });
	CheckMeshGeometry(mesh);
	CHECK_EQ(mesh.vertices[0].pos, Float3{ 1.0f, -1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[ringStride].pos, Float3{ 2.0f, 1.0f, 0.0f });
	CHECK(mesh.vertices[0].normal.dot(expectedNormal)
		== doctest::Approx(1.0f).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[ringStride].normal.dot(expectedNormal)
		== doctest::Approx(1.0f).epsilon(FrameEpsilon));
	CHECK_EQ(mesh.vertices[0].tangent, Float4{ 0.0f, 0.0f, 1.0f, -1.0f });
	CHECK_EQ(mesh.vertices[startCapBase].normal, -Float3::UnitY());
	CHECK_EQ(mesh.vertices[endCapBase].normal, Float3::UnitY());
	CHECK_EQ(mesh.vertices[startCapBase + 1].pos, Float3{ 1.0f, -1.0f, 0.0f });
	CHECK_EQ(mesh.vertices[endCapBase + 1].pos, Float3{ 2.0f, 1.0f, 0.0f });
}

TEST_CASE("Mesh3D::Tube per-point equal radii preserve constant Tube data")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Array<double> radii(path.size(), 0.25);
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const TubeOptions options{ .sides = 8, .uvScale = uvScale, .uvOffset = uvOffset };

	CheckMeshDataEqual(
		Mesh3D::Tube(path, radii, options),
		Mesh3D::Tube(path, 0.25, options));
	CheckMeshDataEqual(
		Mesh3D::Tube(path, radii, TubeOptions{ .sides = 8 }),
		Mesh3D::Tube(path, 0.25, TubeOptions{ .sides = 8 }));
}

TEST_CASE("Mesh3D::Tube per-point radii on bent and closed paths")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> bentPath{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 2.0, 3.0, 1.0 }
	};
	const Array<double> bentRadii{ 0.2, 0.4, 0.3 };
	const Mesh3D bent = Mesh3D::Tube(bentPath, bentRadii, TubeOptions{ .sides = Sides });
	const size_t ringStride = (Sides + 1);
	CheckMeshGeometry(bent);
	for (size_t pathIndex = 0; pathIndex < bentPath.size(); ++pathIndex)
	{
		for (uint32 sideIndex = 0; sideIndex <= Sides; ++sideIndex)
		{
			const Vertex3D& vertex = bent.vertices[(pathIndex * ringStride) + sideIndex];
			CHECK((vertex.pos - Float3{ bentPath[pathIndex] }).length()
				== doctest::Approx(bentRadii[pathIndex]).epsilon(FrameEpsilon));
		}
	}

	const Array<Vec3> closedPath{
		{ -2.0, 0.0, -2.0 }, { 2.0, 0.0, -2.0 },
		{ 2.0, 0.0, 2.0 }, { -2.0, 0.0, 2.0 }
	};
	const Array<double> closedRadii{ 0.2, 0.3, 0.4, 0.25 };
	const Mesh3D closed = Mesh3D::Tube(
		closedPath, closedRadii,
		TubeOptions{ .sides = Sides, .closeRing = CloseRing::Yes });
	const size_t seamRingBase = (closedPath.size() * ringStride);

	CHECK_EQ(closed.vertexCount(), ((closedPath.size() + 1) * ringStride));
	CHECK_EQ(closed.triangleCount(), (2 * closedPath.size() * Sides));
	CheckMeshGeometry(closed);
	for (uint32 sideIndex = 0; sideIndex <= Sides; ++sideIndex)
	{
		const Vertex3D& first = closed.vertices[sideIndex];
		const Vertex3D& seam = closed.vertices[seamRingBase + sideIndex];
		CHECK_EQ(seam.pos, first.pos);
		CHECK_EQ(seam.normal, first.normal);
		CHECK_EQ(seam.tangent, first.tangent);
		CHECK_EQ(seam.tex.x, first.tex.x);
	}
}

TEST_CASE("Mesh3DBuilder::addTube per-point radii")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
	};
	const Array<double> radii{ 0.2, 0.4, 0.3 };
	constexpr uint32 Sides = 8;
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const TubeOptions options{ .sides = Sides, .uvScale = uvScale, .uvOffset = uvOffset };
	const TubeOptions defaultUVOptions{ .sides = Sides };
	const Mesh3D source = Mesh3D::Tube(path, radii, options);

	SUBCASE("Initializer list and reserved storage")
	{
		Mesh3DBuilder builder;
		builder.reserve((source.vertexCount() * 2), (source.triangleCount() * 2));
		REQUIRE(builder.addTube({
			{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }, { 1.0, 3.0, 1.0 }
		}, { 0.2, 0.4, 0.3 }, options));
		const Vertex3D* const vertexData = builder.getMesh().vertices.data();
		const TriangleIndex32* const indexData = builder.getMesh().indices.data();

		const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
		REQUIRE(builder.addTube(path, radii, transform, options));
		CHECK_EQ(builder.getMesh().vertices.data(), vertexData);
		CHECK_EQ(builder.getMesh().indices.data(), indexData);

		Mesh3D expected = source;
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}

	SUBCASE("Transform overloads")
	{
		const Vec3 offset{ 3.0, 4.0, 5.0 };
		const Quaternion rotation = Quaternion::RotateY(Math::QuarterPiF);
		const Mat4x4 transform = Mat4x4::AffineTransform(
			Float3{ -2.0f, 3.0f, 4.0f }, rotation, Float3{ offset });
		const Mesh3D defaultUVSource = Mesh3D::Tube(path, radii, defaultUVOptions);

		Mesh3DBuilder builder;
		REQUIRE(builder.addTube(path, radii, offset, defaultUVOptions));
		REQUIRE(builder.addTube(path, radii, offset, rotation, defaultUVOptions));
		REQUIRE(builder.addTube(path, radii, transform, defaultUVOptions));
		REQUIRE(builder.addTube(path, radii, offset, options));
		REQUIRE(builder.addTube(path, radii, offset, rotation, options));
		REQUIRE(builder.addTube(path, radii, transform, options));

		Mesh3D expected;
		REQUIRE(expected.append(defaultUVSource, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(defaultUVSource,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(defaultUVSource, transform));
		REQUIRE(expected.append(source, Mat4x4::Translate(Float3{ offset })));
		REQUIRE(expected.append(source,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset })));
		REQUIRE(expected.append(source, transform));
		CheckMeshDataEqual(builder.getMesh(), expected);
	}
}

TEST_CASE("Mesh3D::Tube bent non-planar path")
{
	constexpr uint32 Sides = 12;
	constexpr double Radius = 0.2;
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 2.0, 0.0 },
		{ 1.0, 3.0, 1.0 },
		{ 2.0, 3.0, 2.0 }
	};
	const Mesh3D mesh = Mesh3D::Tube(path, Radius, TubeOptions{ .sides = Sides });
	const size_t ringStride = (Sides + 1);

	CHECK_EQ(mesh.vertexCount(), ((path.size() + 2) * ringStride));
	CHECK_EQ(mesh.triangleCount(), (2 * path.size() * Sides));
	CheckMeshGeometry(mesh);

	for (size_t pathIndex = 0; pathIndex < path.size(); ++pathIndex)
	{
		const Float3 center = path[pathIndex];
		for (uint32 sideIndex = 0; sideIndex <= Sides; ++sideIndex)
		{
			const Vertex3D& vertex = mesh.vertices[(pathIndex * ringStride) + sideIndex];
			CHECK((vertex.pos - center).length() == doctest::Approx(Radius).epsilon(FrameEpsilon));
		}
	}

	const double expectedLength = (2.0 + std::sqrt(3.0) + std::sqrt(2.0));
	CHECK(mesh.vertices[(path.size() - 1) * ringStride].tex.y
		== doctest::Approx(expectedLength).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[0].normal.dot(mesh.vertices[ringStride].normal) > 0.0f);
}

TEST_CASE("Mesh3D::Tube closed non-planar path")
{
	constexpr uint32 Sides = 10;
	constexpr double Radius = 0.15;
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 2.0, 0.0, 0.0 },
		{ 2.0, 1.0, 2.0 },
		{ 0.0, 2.0, 2.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const TubeOptions options{
		.sides = Sides,
		.uvScale = uvScale,
		.uvOffset = uvOffset,
		.closeRing = CloseRing::Yes,
	};
	const Mesh3D mesh = Mesh3D::Tube(
		path, Radius, options);
	const Mesh3D defaultSides = Mesh3D::Tube(
		path, Radius, TubeOptions{ .closeRing = CloseRing::Yes });
	const size_t ringStride = (Sides + 1);
	const size_t stationCount = (path.size() + 1);

	CHECK_EQ(mesh.vertexCount(), (stationCount * ringStride));
	CHECK_EQ(mesh.triangleCount(), (2 * path.size() * Sides));
	CHECK_EQ(defaultSides.triangleCount(), (2 * path.size() * 12));
	CheckMeshGeometry(mesh);

	const size_t seamRingBase = (path.size() * ringStride);
	for (uint32 sideIndex = 0; sideIndex <= Sides; ++sideIndex)
	{
		const Vertex3D& first = mesh.vertices[sideIndex];
		const Vertex3D& seam = mesh.vertices[seamRingBase + sideIndex];
		CHECK_EQ(seam.pos, first.pos);
		CHECK_EQ(seam.normal, first.normal);
		CHECK_EQ(seam.tangent, first.tangent);
		CHECK_EQ(seam.tex.x, first.tex.x);
	}

	const double totalLength = (2.0 + std::sqrt(5.0) + std::sqrt(5.0) + std::sqrt(8.0));
	CHECK(mesh.vertices[seamRingBase].tex.y
		== doctest::Approx(uvOffset.y + uvScale.y * totalLength).epsilon(FrameEpsilon));
	CHECK(mesh.vertices[(path.size() - 1) * ringStride].normal.dot(
		mesh.vertices[seamRingBase].normal) > 0.0f);

	CheckMeshDataEqual(
		Mesh3D::Tube({
			{ 0.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 },
			{ 2.0, 1.0, 2.0 }, { 0.0, 2.0, 2.0 }
		}, Radius, options),
		mesh);
}

TEST_CASE("Mesh3DBuilder::addTube closed path")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 },
		{ 2.0, 1.0, 2.0 }, { 0.0, 2.0, 2.0 }
	};
	const Vec2 uvScale{ 2.0, 0.25 };
	const Vec2 uvOffset{ 0.1, 0.2 };
	const Mat4x4 transform = Mat4x4::Translate(Float3{ 3.0f, 4.0f, 5.0f });
	const TubeOptions options{
		.sides = 10,
		.uvScale = uvScale,
		.uvOffset = uvOffset,
		.closeRing = CloseRing::Yes,
	};
	const Mesh3D source = Mesh3D::Tube(
		path, 0.15, options);

	Mesh3DBuilder builder;
	REQUIRE(builder.addTube(path, 0.15, transform, options));
	Mesh3D expected;
	REQUIRE(expected.append(source, transform));
	CheckMeshDataEqual(builder.getMesh(), expected);

	Mesh3DBuilder initializerListBuilder;
	REQUIRE(initializerListBuilder.addTube({
		{ 0.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 },
		{ 2.0, 1.0, 2.0 }, { 0.0, 2.0, 2.0 }
	}, 0.15, options));
	CheckMeshDataEqual(initializerListBuilder.getMesh(), source);
}

TEST_CASE("Mesh3D::Tube minimum sides and nearly straight path")
{
	const Array<Vec3> path{
		{ 0.0, 0.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 2.0, 1e-8, 0.0 }
	};
	const Mesh3D mesh = Mesh3D::Tube(path, 0.5, TubeOptions{ .sides = 3 });

	CHECK_EQ(mesh.vertexCount(), size_t{ 5 * 4 });
	CHECK_EQ(mesh.triangleCount(), size_t{ 2 * 3 * 3 });
	CheckMeshGeometry(mesh);
}

TEST_CASE("Mesh3D::Tube invalid arguments")
{
	constexpr uint32 Sides = 8;
	const Array<Vec3> valid{ { 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 } };
	const Array<double> validRadii{ 0.5, 1.0 };
	const TubeOptions options{ .sides = Sides };
	const TubeOptions closedOptions{ .sides = Sides, .closeRing = CloseRing::Yes };

	CHECK(Mesh3D::Tube(std::span<const Vec3>{}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{ { 0.0, 0.0, 0.0 } }, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, 0.0, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, -1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, std::numeric_limits<double>::infinity(), options).isEmpty());
	CHECK(Mesh3D::Tube(valid, std::numeric_limits<double>::max(), options).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0, TubeOptions{ .sides = 2 }).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0,
		TubeOptions{ .sides = std::numeric_limits<uint32>::max() }).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
	}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 1.0, 0.0, 0.0 }, { (1.0 + 1e-10), 0.0, 0.0 }
	}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
	}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }
	}, 1.0, closedOptions).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 },
		{ 1.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0 }
	}, 1.0, closedOptions).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 2.0, 0.0, 0.0 }
	}, 1.0, closedOptions).isEmpty());
	CHECK_FALSE(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }
	}, 0.1, TubeOptions{ .sides = 3, .closeRing = CloseRing::Yes }).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0 }
	}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { std::numeric_limits<double>::infinity(), 0.0, 0.0 }
	}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { std::numeric_limits<double>::max(), 0.0, 0.0 }
	}, 1.0, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0,
		TubeOptions{ .sides = Sides,
			.uvScale = Vec2{ std::numeric_limits<double>::quiet_NaN(), 1.0 } }).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0,
		TubeOptions{ .sides = Sides,
			.uvScale = Vec2{ 1.0, std::numeric_limits<float>::max() } }).isEmpty());
	CHECK(Mesh3D::Tube(valid, 1.0,
		TubeOptions{ .sides = Sides, .uvScale = Vec2{ 1.0, 1.0 },
			.uvOffset = Vec2{ 0.0, std::numeric_limits<double>::infinity() } }).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{ 0.5 }, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{ 0.0, 1.0 }, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{ -0.5, 1.0 }, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{
		std::numeric_limits<double>::denorm_min(), 1.0
	}, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{
		std::numeric_limits<double>::quiet_NaN(), 1.0
	}, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{
		std::numeric_limits<double>::infinity(), 1.0
	}, options).isEmpty());
	CHECK(Mesh3D::Tube(valid, Array<double>{
		std::numeric_limits<double>::max(), 1.0
	}, options).isEmpty());
	CHECK(Mesh3D::Tube(Array<Vec3>{
		{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }
	}, validRadii, options).isEmpty());
	CHECK(Mesh3D::Tube({
		{ 0.0, 0.0, 0.0 }, { 0.0, 2.0, 0.0 }
	}, { 0.5 }, options).isEmpty());
}
