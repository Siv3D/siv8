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
