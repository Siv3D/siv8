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
	using Assembly = Mesh3DAssembly;
	constexpr Assembly::PartID Missing{ static_cast<size_t>(-1) };
}

TEST_CASE("Mesh3DAssembly::cloneSubtree preserves interleaved hierarchy and resource sharing")
{
	Assembly a;
	const auto mesh = a.addMesh(Mesh3D::Box()).value();
	const auto material = a.addMaterial(Material{ .name = U"paint" });
	const auto parent = a.addPart({ .placement = Vec3{ 10, 0, 0 } }).value();
	const auto root = a.addPart({ .name = U"arm", .parent = parent, .placement = Vec3{ 2, 0, 0 } }).value();
	const auto unrelated = a.addPart({ .name = U"unrelated", .mesh = mesh }).value();
	const auto child = a.addPart({ .name = U"joint", .mesh = mesh, .material = material, .parent = root,
		.placement = Vec3{ 0, 2, 0 } }).value();
	const auto unrelatedChild = a.addPart({ .parent = unrelated }).value();
	const auto grandchild = a.addPart({ .name = U"joint", .mesh = mesh, .parent = child,
		.placement = Vec3{ 0, 0, 3 } }).value();
	const size_t oldCount = a.partCount();
	const auto* vertices = a.getMesh(mesh)->vertices.data();
	const auto copy = a.cloneSubtree(root, Vec3{ -2, 0, 0 }, parent).value();
	REQUIRE_EQ(copy.parts.size(), size_t{ 3 });
	CHECK_EQ(a.partCount(), oldCount + 3);
	CHECK_EQ(copy.root, Assembly::PartID{ oldCount });
	CHECK_EQ(copy.find(root).value(), copy.root);
	CHECK_FALSE(copy.find(parent));
	CHECK_FALSE(copy.find(unrelated));
	CHECK_FALSE(copy.find(unrelatedChild));
	CHECK_FALSE(copy.find(Missing));
	for (size_t i = 0; i < copy.parts.size(); ++i)
	{
		const auto& [source, destination] = copy.parts[i];
		CHECK_EQ(destination, Assembly::PartID{ oldCount + i });
		const auto& original = *a.getPart(source);
		const auto& cloned = *a.getPart(destination);
		CHECK_EQ(cloned.name, original.name);
		CHECK(cloned.mesh == original.mesh);
		CHECK(cloned.material == original.material);
		if (source != root)
		{
			CHECK(cloned.parent == copy.find(*original.parent));
			CHECK(cloned.placement.getTransform() == original.placement.getTransform());
		}
	}
	CHECK_EQ(a.meshCount(), size_t{ 1 });
	CHECK_EQ(a.materialCount(), size_t{ 1 });
	CHECK_EQ(a.getMesh(mesh)->vertices.data(), vertices);
	CHECK_EQ(a.computeWorldTransform(copy.find(grandchild).value()).value().transformPoint(Float3::Zero()), (Float3{ 8, 2, 3 }));
	CHECK_EQ(a.computeWorldTransform(grandchild).value().transformPoint(Float3::Zero()), (Float3{ 12, 2, 3 }));
	REQUIRE(a.setPlacement(copy.find(child).value(), Vec3{ 0, 4, 0 }));
	CHECK_EQ(a.computeWorldTransform(grandchild).value().transformPoint(Float3::Zero()), (Float3{ 12, 2, 3 }));
	CHECK_EQ(a.computeWorldTransform(copy.find(grandchild).value()).value().transformPoint(Float3::Zero()), (Float3{ 8, 4, 3 }));
	REQUIRE(a.setMesh(mesh, Mesh3D::Sphere(0.5, 8, 4)));
	REQUIRE(a.setMaterial(material, Material{ .baseColor = ColorF{ 1, 0, 0 } }));
	const auto baked = a.bake().value();
	CHECK_EQ(baked.parts[static_cast<size_t>(child)].range.vertexCount,
		baked.parts[static_cast<size_t>(copy.find(child).value())].range.vertexCount);
	CHECK_EQ(baked.materials[0].baseColor, (ColorF{ 1, 0, 0 }));
	Mesh3DTest::CheckMeshGeometry(baked.mesh);
}

TEST_CASE("Mesh3DAssembly::cloneSubtree supports leaf roots and original descendants as parents")
{
	Assembly a;
	const auto root = a.addPart({ .placement = Vec3{ 1, 0, 0 } }).value();
	const auto child = a.addPart({ .parent = root, .placement = Vec3{ 0, 2, 0 } }).value();
	const auto leaf = a.cloneSubtree(child, Vec3{ 0, 0, 3 }).value();
	REQUIRE_EQ(leaf.parts.size(), size_t{ 1 });
	CHECK_FALSE(a.getPart(leaf.root)->parent);
	CHECK_EQ(a.computeWorldTransform(leaf.root).value().transformPoint(Float3::Zero()), (Float3{ 0, 0, 3 }));
	const auto copy = a.cloneSubtree(root, Vec3{ 0, 0, 4 }, child).value();
	REQUIRE_EQ(copy.parts.size(), size_t{ 2 }); // Newly inserted descendants are not recursively cloned.
	CHECK(a.getPart(copy.root)->parent == child);
	CHECK_EQ(a.computeWorldTransform(copy.find(child).value()).value().transformPoint(Float3::Zero()), (Float3{ 1, 4, 4 }));
	const auto second = a.cloneSubtree(copy.root, Vec3{ 5, 0, 0 }).value();
	CHECK_EQ(second.parts.size(), size_t{ 2 });
	CHECK_FALSE(a.getPart(second.root)->parent);
	CHECK_EQ(a.computeWorldTransform(second.find(copy.find(child).value()).value()).value().transformPoint(Float3::Zero()), (Float3{ 5, 2, 0 }));
}

TEST_CASE("Mesh3DAssembly::cloneSubtree snapshots aliased placement and mirrors geometry")
{
	Assembly a;
	const auto mesh = a.addMesh(Mesh3D::Box()).value();
	const auto root = a.addPart({ .placement = Mat4x4::Scale(Float3{ -1, 1, 1 }) }).value();
	(void)a.addPart({ .mesh = mesh, .parent = root, .placement = Vec3{ 2, 0, 0 } }).value();
	for (size_t i = 0; i < 12; ++i)
	{
		const auto copy = a.cloneSubtree(root, a.getPart(root)->placement).value();
		CHECK_EQ(copy.parts.size(), size_t{ 2 });
		CHECK(a.getPart(copy.root)->placement.getTransform() == Mat4x4::Scale(Float3{ -1, 1, 1 }));
	}
	CHECK_EQ(a.partCount(), size_t{ 26 });
	CHECK_EQ(a.meshCount(), size_t{ 1 });
	Mesh3DTest::CheckMeshGeometry(a.bake().value().mesh);
}

TEST_CASE("Mesh3DAssembly::cloneSubtree invalid IDs leave the assembly unchanged")
{
	const ScopedLogSilencer silence;
	Assembly a;
	CHECK_EQ(a.cloneSubtree(Assembly::PartID{ 0 }, Vec3::Zero()).error().code, Mesh3DErrorCode::InvalidArgument);
	const auto mesh = a.addMesh(Mesh3D::Box()).value();
	const auto root = a.addPart({ .name = U"original", .mesh = mesh }).value();
	const auto before = a.bake().value();
	const auto* pointer = a.getPart(root);
	CHECK_EQ(a.cloneSubtree(Missing, Vec3::Zero()).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.cloneSubtree(root, Vec3::Zero(), Missing).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.getPart(root), pointer);
	CHECK_EQ(a.partCount(), size_t{ 1 });
	CHECK_EQ(a.meshCount(), size_t{ 1 });
	CHECK_EQ(a.getPart(root)->name, U"original");
	Mesh3DTest::CheckMeshDataEqual(a.bake().value().mesh, before.mesh);
	CHECK_FALSE(Assembly::ClonedSubtree{}.find(root));
}
