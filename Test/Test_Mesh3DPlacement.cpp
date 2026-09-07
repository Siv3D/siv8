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

TEST_CASE("Mesh3DPlacement rotation preserves the origin and supports inverse round trips")
{
	for (const auto rotation : { Quaternion::Identity(), Quaternion::RotateX(90_deg),
		Quaternion::RotateY(180_deg), Quaternion::RotateZ(-90_deg),
		Quaternion::RotateX(0.3) * Quaternion::RotateY(-0.7) })
	{
		const Mesh3DPlacement placement = rotation;
		const Mesh3DPlacement inverse = rotation.inverse();
		CHECK(placement.getTransform().transformPoint(Float3::Zero()).epsilonEquals(Float3::Zero(), 1e-6f));
		for (const Float3 point : { Float3{ 1, 0, 0 }, Float3{ 0, 1, 0 }, Float3{ 0, 0, 1 }, Float3{ 2, -3, 4 } })
		{
			const auto transformed = placement.getTransform().transformPoint(point);
			CHECK(transformed.epsilonEquals(rotation.rotate(point), 2e-5f));
			CHECK(inverse.getTransform().transformPoint(transformed).epsilonEquals(point, 2e-5f));
		}
	}
}

TEST_CASE("Mesh3DPlacement accepts rotation in Builder Assembly and Loft")
{
	const auto rotation = Quaternion::RotateZ(90_deg);
	Mesh3DBuilder builder;
	REQUIRE(builder.addBox(Vec3{ 2, 4, 6 }, rotation));
	CHECK(builder.getMesh().computeBoundingBox().size.epsilonEquals(Vec3{ 4, 2, 6 }, 2e-5));
	Mesh3DTest::CheckMeshGeometry(builder.getMesh());

	Mesh3DAssembly assembly;
	const auto parent = assembly.addPart({ .placement = Vec3{ 3, 4, 5 } }).value();
	const auto pivot = assembly.addPart({ .parent = parent, .placement = rotation }).value();
	const auto child = assembly.addPart({ .parent = pivot, .placement = Vec3{ 2, 0, 0 } }).value();
	CHECK(assembly.computeWorldTransform(child).value().transformPoint(Float3::Zero())
		.epsilonEquals(Float3{ 3, 6, 5 }, 2e-5f));

	const Array<Vec2> contour{ Vec2{ -1, -1 }, Vec2{ 1, -1 }, Vec2{ 1, 1 }, Vec2{ -1, 1 } };
	const auto yaw = Quaternion::RotateY(90_deg);
	const auto loft = Mesh3D::Loft({ { contour, yaw }, { contour, { Vec3{ 0, 2, 0 }, yaw } } });
	Mesh3DTest::CheckMeshGeometry(loft);
	CHECK(loft.computeBoundingBox().size.epsilonEquals(Vec3{ 2, 2, 2 }, 2e-5));
}

TEST_CASE("Mesh3DPlacement Loft frame aligns contour axes and progression")
{
	const auto rotation = Quaternion::FromUnitVectorPairs(
		{ Vec3{ 1, 0, 0 }, Vec3{ 0, 0, -1 } },
		{ Vec3{ 0, 0, -1 }, Vec3{ 0, 1, 0 } });
	CHECK(rotation.rotate(Vec3{ 1, 0, 0 }).epsilonEquals(Vec3{ 0, 0, -1 }, 2e-5));
	CHECK(rotation.rotate(Vec3{ 0, 0, -1 }).epsilonEquals(Vec3{ 0, 1, 0 }, 2e-5));
	CHECK(rotation.rotate(Vec3{ 0, 1, 0 }).epsilonEquals(Vec3{ 1, 0, 0 }, 2e-5));
	const Array<Vec2> contour{ Vec2{ -1, -2 }, Vec2{ 1, -2 }, Vec2{ 1, 2 }, Vec2{ -1, 2 } };
	REQUIRE(Geometry2D::IsClockwise(contour));
	const auto mesh = Mesh3D::Loft({ { contour, rotation }, { contour, { Vec3{ 3, 0, 0 }, rotation } } });
	Mesh3DTest::CheckMeshGeometry(mesh);
	CHECK(mesh.computeBoundingBox().center.epsilonEquals(Vec3{ 1.5, 0, 0 }, 2e-5));
	CHECK(mesh.computeBoundingBox().size.epsilonEquals(Vec3{ 3, 4, 2 }, 2e-5));
}

TEST_CASE("Mesh3DPlacement::Align frame origins and axes")
{
	const Mat4x4 identity = Mat4x4::Identity();
	const Mat4x4 shear{ 1, 0.2f, 0, 0, 0, 1, 0.3f, 0, 0, 0, 1, 0, 2, 3, 4, 1 };
	const Mat4x4 source = Mat4x4::AffineTransform(Float3{ 2, 3, 4 }, Quaternion::RotateY(0.4), Float3{ 1, -2, 3 });
	const Mat4x4 target = Mat4x4::AffineTransform(Float3{ -3, 2, 1 }, Quaternion::RotateZ(0.7), Float3{ -2, 4, 6 });
	for (const auto& [from, to] : {
		std::pair{ identity, identity }, std::pair{ source, source },
		std::pair{ identity, target }, std::pair{ source, identity },
		std::pair{ source, target }, std::pair{ shear, target }, std::pair{ target, shear } })
	{
		const Mat4x4 placement = Mesh3DPlacement::Align(from, to).getTransform();
		for (const Float3 p : { Float3::Zero(), Float3{ 1, 0, 0 }, Float3{ 0, 1, 0 }, Float3{ 0, 0, 1 }, Float3{ 2, -3, 4 } })
		{
			CHECK(placement.transformPoint(from.transformPoint(p)).epsilonEquals(to.transformPoint(p), 2e-5f));
		}
		const Mat4x4 reverse = Mesh3DPlacement::Align(to, from).getTransform();
		CHECK((placement * reverse).epsilonEquals(identity, 2e-5f));
	}
}

TEST_CASE("Mesh3DPlacement::Align keeps the mounting point under a transformed parent")
{
	for (const double height : { 0.5, 2.0, 5.0 })
	{
		const Mesh3DPlacement mount{ Vec3{ 0, -height / 2, 0 } };
		const Mesh3DPlacement target{ Vec3{ 2, 0.25, 3 }, Quaternion::RotateZ(0.4) };
		const auto placement = Mesh3DPlacement::Align(mount, target);
		Mesh3DAssembly a;
		const auto mesh = a.addMesh(Mesh3D::Box(Vec3{ 1, height, 1 })).value();
		const Mat4x4 parentTransform = Mat4x4::AffineTransform(Float3{ -2, 1, 3 }, Quaternion::RotateY(0.6), Float3{ 5, 2, -1 });
		const auto parent = a.addPart({ .placement = parentTransform }).value();
		const auto child = a.addPart({ .mesh = mesh, .parent = parent, .placement = placement }).value();
		const auto world = a.computeWorldTransform(child).value();
		const Float3 bottom{ 0, static_cast<float>(-height / 2), 0 };
		CHECK(world.transformPoint(bottom).epsilonEquals(parentTransform.transformPoint(Float3{ 2, 0.25f, 3 }), 2e-5f));
		Mesh3DTest::CheckMeshGeometry(a.bake().value().mesh);

		Mesh3DBuilder builder;
		REQUIRE(builder.addBox(Vec3{ 1, height, 1 }, placement));
		Mesh3DTest::CheckMeshGeometry(builder.getMesh());
		const auto bounds = builder.getMesh().computeBoundingBox();
		CHECK(bounds.size.y > 0);
	}
}
