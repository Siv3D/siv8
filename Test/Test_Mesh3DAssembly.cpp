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
# include "Mesh3DAssemblyExamples.hpp"
# include <cstdlib>

namespace
{
	using Assembly = Mesh3DAssembly;
	using Mesh3DTest::CheckMeshDataEqual;
	using Mesh3DTest::CheckMeshGeometry;

	const auto MissingMesh = static_cast<Assembly::MeshID>(-1);
	const auto MissingMaterial = static_cast<Assembly::MaterialID>(-1);
	const auto MissingPart = static_cast<Assembly::PartID>(-1);

	void CheckSnapshot(const Assembly::BakedMesh& actual, const Assembly::BakedMesh& expected)
	{
		CheckMeshDataEqual(actual.mesh, expected.mesh);
		REQUIRE_EQ(actual.parts.size(), expected.parts.size());
		REQUIRE_EQ(actual.materials.size(), expected.materials.size());
		for (size_t i = 0; i < actual.parts.size(); ++i)
		{
			const auto& a = actual.parts[i];
			const auto& e = expected.parts[i];
			CHECK(a.id == e.id);
			CHECK_EQ(a.name, e.name);
			CHECK(a.parent == e.parent);
			CHECK(a.material == e.material);
			CHECK_EQ(a.range.vertexOffset, e.range.vertexOffset);
			CHECK_EQ(a.range.vertexCount, e.range.vertexCount);
			CHECK_EQ(a.range.triangleOffset, e.range.triangleOffset);
			CHECK_EQ(a.range.triangleCount, e.range.triangleCount);
			CHECK(a.worldTransform == e.worldTransform);
		}
		for (size_t i = 0; i < actual.materials.size(); ++i)
		{
			CHECK_EQ(actual.materials[i].name, expected.materials[i].name);
			CHECK_EQ(actual.materials[i].baseColor, expected.materials[i].baseColor);
		}
	}
}

TEST_CASE("Mesh3DAssembly::empty and transform-only parts")
{
	Assembly a;
	a.reserve(2, 2, 4);
	CHECK_EQ(a.meshCount(), size_t{ 0 });
	CHECK_EQ(a.materialCount(), size_t{ 0 });
	CHECK_EQ(a.partCount(), size_t{ 0 });
	Assembly::BakedMesh output;
	output.mesh = Mesh3D::Box();
	REQUIRE(a.bake(output, { .maxVertexCount = 0, .maxTriangleCount = 0 }));
	CHECK(output.mesh.isEmpty());
	CHECK(output.parts.isEmpty());
	const auto parent = a.addPart({ .name = U"root", .placement = Vec3{ 1, 2, 3 } }).value();
	const auto child = a.addPart({ .name = U"child", .parent = parent, .placement = Vec3{ 4, 5, 6 } }).value();
	REQUIRE(a.bake(output));
	REQUIRE_EQ(output.parts.size(), size_t{ 2 });
	CHECK(output.parts[0].range.isEmpty());
	CHECK(output.parts[1].range.isEmpty());
	CHECK_EQ(output.parts[1].worldTransform.transformPoint(Float3::Zero()), (Float3{ 5, 7, 9 }));
	CHECK_EQ(a.computeWorldTransform(child).value().transformPoint(Float3::Zero()), (Float3{ 5, 7, 9 }));
	a.clear();
	CHECK_EQ(a.partCount(), size_t{ 0 });
	CHECK(a.getPart(parent) == nullptr);
	REQUIRE(a.bake(output));
	CHECK(output.parts.isEmpty());
}

TEST_CASE("Mesh3DAssembly::mesh ownership and registration failure")
{
	const ScopedLogSilencer silence;
	Assembly a;
	Mesh3D mesh = Mesh3D::Box();
	const Vertex3D* storage = mesh.vertices.data();
	const auto id = a.addMesh(std::move(mesh)).value();
	REQUIRE(a.getMesh(id));
	CHECK_EQ(a.getMesh(id)->vertices.data(), storage);
	CHECK_EQ(a.meshCount(), size_t{ 1 });
	const auto copyID = a.addMesh(*a.getMesh(id)).value();
	CHECK_NE(a.getMesh(copyID)->vertices.data(), storage);
	CHECK_EQ(a.addMesh(Mesh3D{}).error().code, Mesh3DErrorCode::InvalidGeometry);
	Mesh3D invalid = Mesh3D::Box();
	invalid.indices[0].i0 = static_cast<uint32>(invalid.vertexCount());
	CHECK_EQ(a.addMesh(invalid).error().code, Mesh3DErrorCode::InvalidGeometry);
	CHECK_EQ(a.setMesh(id, invalid).error().code, Mesh3DErrorCode::InvalidGeometry);
	CHECK_EQ(a.setMesh(MissingMesh, Mesh3D::Box()).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.meshCount(), size_t{ 2 });
	CheckMeshDataEqual(*a.getMesh(id), Mesh3D::Box());
	CHECK(a.getMesh(MissingMesh) == nullptr);
	CHECK(a.getMaterial(MissingMaterial) == nullptr);
	CHECK(a.getPart(MissingPart) == nullptr);
}

TEST_CASE("Mesh3DAssembly::part references and failed edits")
{
	const ScopedLogSilencer silence;
	Assembly a;
	const auto mesh = a.addMesh(Mesh3D::Box()).value();
	const auto material = a.addMaterial(Material{ .name = U"steel" });
	const auto root = a.addPart({ .name = U"root", .material = material }).value();
	const auto child = a.addPart({ .name = U"child", .mesh = mesh, .parent = root }).value();
	const auto before = a.bake().value();
	for (const Assembly::Part& part : {
		Assembly::Part{ .mesh = MissingMesh },
		Assembly::Part{ .material = MissingMaterial },
		Assembly::Part{ .parent = MissingPart } })
	{
		CHECK_EQ(a.addPart(part).error().code, Mesh3DErrorCode::InvalidArgument);
		CHECK_EQ(a.setPart(child, part).error().code, Mesh3DErrorCode::InvalidArgument);
	}
	CHECK_EQ(a.setPart(root, { .parent = root }).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.setPart(root, { .parent = child }).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.setPart(child, { .parent = child }).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.setPart(MissingPart, {}).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.setPlacement(MissingPart, Vec3::Zero()).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.setMaterial(MissingMaterial, Material{}).error().code, Mesh3DErrorCode::InvalidArgument);
	CHECK_EQ(a.computeWorldTransform(MissingPart).error().code, Mesh3DErrorCode::InvalidArgument);
	CheckSnapshot(a.bake().value(), before);
	CHECK_FALSE(before.parts[1].material); // Materials do not inherit from parents.

	REQUIRE(a.setPart(child, { .name = U"renamed", .mesh = mesh, .material = material }));
	CHECK_EQ(a.getPart(child)->name, U"renamed");
	CHECK_FALSE(a.getPart(child)->parent);
	CHECK(a.getPart(child)->material == material);
	const auto otherParent = a.addPart({ .placement = Vec3{ 2, 0, 0 } }).value();
	const auto reparented = a.addPart({ .mesh = mesh, .parent = child }).value();
	REQUIRE(a.setPart(reparented, { .mesh = mesh, .parent = otherParent }));
	CHECK_EQ(a.computeWorldTransform(reparented).value().transformPoint(Float3::Zero()), (Float3{ 2, 0, 0 }));
}

TEST_CASE("Mesh3DAssembly::hierarchy transforms and mirrored geometry")
{
	Assembly a;
	const Mesh3D mesh = Mesh3D::Box();
	const auto shape = a.addMesh(mesh).value();
	const Mat4x4 rootTransform = Mat4x4::AffineTransform(Float3{ -2, 3, 1 }, Quaternion::RotateY(0.6), Float3{ 4, 5, 6 });
	const Mat4x4 localTransform = Mat4x4::AffineTransform(Float3::One(), Quaternion::RotateX(0.4), Float3{ 1, 2, 3 });
	const auto root = a.addPart({ .placement = rootTransform }).value();
	const auto child = a.addPart({ .name = U"mirrored", .mesh = shape, .parent = root, .placement = localTransform }).value();
	const auto doubleMirror = a.addPart({ .name = U"double mirror", .mesh = shape, .parent = child,
		.placement = Mat4x4::Scale(Float3{ -1, 1, 1 }) }).value();
	const auto baked = a.bake().value();
	const Mat4x4 world = (localTransform * rootTransform);
	CHECK(a.computeWorldTransform(child).value().epsilonEquals(world, 1e-5f));
	CHECK(baked.parts[1].worldTransform == world);
	CHECK(baked.parts[2].worldTransform.epsilonEquals(a.computeWorldTransform(doubleMirror).value(), 1e-5f));
	Mesh3D expected = mesh.transformed(world);
	REQUIRE(expected.append(mesh, (Mat4x4::Scale(Float3{ -1, 1, 1 }) * world)));
	CheckMeshDataEqual(baked.mesh, expected);
	CheckMeshGeometry(baked.mesh);
	CHECK_EQ(a.getMesh(shape)->vertices[0].pos, mesh.vertices[0].pos);

	// Every part's triangle indices remain confined to its own vertex range.
	for (const auto& part : baked.parts)
	{
		const auto& r = part.range;
		for (size_t i = r.triangleOffset; i < (r.triangleOffset + r.triangleCount); ++i)
		{
			const auto& t = baked.mesh.indices[i];
			for (const uint32 index : { t.i0, t.i1, t.i2 })
			{
				CHECK(r.vertexOffset <= index);
				CHECK(index < (r.vertexOffset + r.vertexCount));
			}
		}
	}
}

TEST_CASE("Mesh3DAssembly::reusable bake and size budgets")
{
	const ScopedLogSilencer silence;
	Assembly a;
	const auto shape = a.addMesh(Mesh3D::Box()).value();
	const auto material = a.addMaterial(Material{ .name = U"paint" });
	const auto part = a.addPart({ .name = U"box", .mesh = shape, .material = material }).value();
	(void)a.addMesh(Mesh3D::Sphere(1.0)).value(); // Unused shapes consume no bake budget.
	Assembly::BakedMesh output = a.bake({ .maxVertexCount = 24, .maxTriangleCount = 12 }).value();
	const auto before = output;
	const auto* vertices = output.mesh.vertices.data();
	const auto* indices = output.mesh.indices.data();
	const auto* parts = output.parts.data();
	const auto* materials = output.materials.data();
	for (const Mesh3DBakeOptions options : {
		Mesh3DBakeOptions{ .maxVertexCount = 23 },
		Mesh3DBakeOptions{ .maxTriangleCount = 11 },
		Mesh3DBakeOptions{ .maxVertexCount = 0 } })
	{
		CHECK_EQ(a.bake(output, options).error().code, Mesh3DErrorCode::SizeLimit);
		CHECK_EQ(a.bake(options).error().code, Mesh3DErrorCode::SizeLimit);
		CheckSnapshot(output, before);
	}
	REQUIRE(a.setPlacement(part, Vec3{ 2, 3, 4 }));
	REQUIRE(a.bake(output));
	CHECK_EQ(output.mesh.vertices.data(), vertices);
	CHECK_EQ(output.mesh.indices.data(), indices);
	CHECK_EQ(output.parts.data(), parts);
	CHECK_EQ(output.materials.data(), materials);
	CheckSnapshot(output, a.bake().value());
	CheckMeshDataEqual(output.mesh, Mesh3D::Box().transformed(Mat4x4::Translate(2, 3, 4)));
}

TEST_CASE("Mesh3DAssembly::stable IDs and independent snapshots")
{
	Assembly a;
	const auto shape = a.addMesh(Mesh3D::Cylinder(0.4, 0.2, 8)).value();
	const auto material = a.addMaterial(Material{ .name = U"old" });
	const auto left = a.addPart({ .name = U"left", .mesh = shape, .material = material }).value();
	const auto right = a.addPart({ .name = U"right", .mesh = shape, .material = material, .placement = Vec3{ 2, 0, 0 } }).value();
	const auto before = a.bake().value();
	Assembly copy = a;
	REQUIRE(a.setMesh(shape, Mesh3D::Cylinder(0.7, 0.2, 32)));
	REQUIRE(a.setMaterial(material, Material{ .name = U"new", .baseColor = ColorF{ 1, 0, 0 } }));
	const auto after = a.bake().value();
	CHECK(after.parts[0].id == left);
	CHECK(after.parts[1].id == right);
	CHECK(a.getPart(right)->mesh == shape);
	CHECK_EQ(after.parts[1].range.vertexOffset, a.getMesh(shape)->vertexCount());
	CHECK_NE(before.parts[1].range.vertexOffset, after.parts[1].range.vertexOffset);
	CHECK_EQ(before.materials[0].name, U"old");
	CHECK_EQ(after.materials[0].name, U"new");
	CheckSnapshot(copy.bake().value(), before);
	a.clear();
	CHECK_EQ(a.meshCount(), size_t{ 0 });
	CHECK_EQ(a.materialCount(), size_t{ 0 });
	CHECK_EQ(a.partCount(), size_t{ 0 });
	CheckMeshGeometry(after.mesh);
	CHECK_EQ(after.materials[0].baseColor, (ColorF{ 1, 0, 0 }));
}

TEST_CASE("Mesh3DAssembly::deep hierarchy without recursion")
{
	Assembly a;
	a.reserve(0, 0, 4096);
	Optional<Assembly::PartID> parent;
	for (size_t i = 0; i < 4096; ++i)
	{
		parent = a.addPart({ .parent = parent, .placement = Vec3{ 1, 0, 0 } }).value();
	}
	const auto baked = a.bake().value();
	CHECK_EQ(baked.parts.back().worldTransform.transformPoint(Float3::Zero()), (Float3{ 4096, 0, 0 }));
	CHECK_EQ(a.computeWorldTransform(*parent).value().transformPoint(Float3::Zero()), (Float3{ 4096, 0, 0 }));
}

TEST_CASE("Mesh3DAssembly::singular placement follows Mesh3D transform contract")
{
	Assembly a;
	const Mesh3D source = Mesh3D::Box();
	const auto mesh = a.addMesh(source).value();
	const auto transform = Mat4x4::Scale(Float3{ 1, 0, 1 });
	(void)a.addPart({ .mesh = mesh, .placement = transform }).value();
	CheckMeshDataEqual(a.bake().value().mesh, source.transformed(transform));
}

TEST_CASE("Mesh3DAssembly::cart wheel rotates about its own center")
{
	auto cart = Mesh3DAssemblyExamples::MakeCart();
	const auto before = cart.assembly.bake().value();
	const auto wheel = cart.wheels.front();
	const auto rotation = (Quaternion::RotateZ(Math::HalfPi) * Quaternion::RotateX(0.3));
	REQUIRE(cart.assembly.setPlacement(wheel, { Vec3{ (-cart.track / 2), 0, 0 }, rotation }));
	const auto after = cart.assembly.bake().value();
	for (size_t i = 0; i < after.parts.size(); ++i)
	{
		if (after.parts[i].id == wheel)
		{
			CHECK(after.parts[i].worldTransform != before.parts[i].worldTransform);
			CHECK_EQ(after.parts[i].worldTransform.transformPoint(Float3::Zero()),
				before.parts[i].worldTransform.transformPoint(Float3::Zero()));
		}
		else
		{
			CheckMeshDataEqual(Mesh3DAssemblyExamples::ExtractPart(after, after.parts[i]),
				Mesh3DAssemblyExamples::ExtractPart(before, before.parts[i]));
		}
	}
	CheckMeshGeometry(after.mesh);
}

TEST_CASE("Mesh3DAssembly::chest hinge and material editing")
{
	auto chest = Mesh3DAssemblyExamples::MakeChest();
	const auto closed = chest.assembly.bake().value();
	Mesh3DAssemblyExamples::OpenChest(chest, Math::HalfPi);
	const auto opened = chest.assembly.bake().value();
	CHECK_EQ(chest.assembly.meshCount(), size_t{ 4 });
	CHECK_EQ(chest.assembly.partCount(), size_t{ 6 });
	CHECK(opened.parts[1].range.isEmpty());
	CHECK(opened.parts[0].worldTransform == closed.parts[0].worldTransform);
	CHECK(opened.parts[2].worldTransform != closed.parts[2].worldTransform);
	CHECK(opened.parts[3].worldTransform != closed.parts[3].worldTransform);
	const Float3 hinge = opened.parts[1].worldTransform.transformPoint(Float3::Zero());
	CHECK_EQ(hinge, closed.parts[1].worldTransform.transformPoint(Float3::Zero()));
	const Float3 front = opened.parts[2].worldTransform.transformPoint(Float3{ 0, -0.08f, -0.7f });
	CHECK(front.epsilonEquals(Float3{ 0, 2.6f, 0.7f }, 1e-5f));
	CheckMeshGeometry(opened.mesh);
	REQUIRE(chest.assembly.setMaterial(chest.metal, Material{
		.name = U"gold", .baseColor = ColorF{ 0.85, 0.6, 0.1 }, .metallic = 1.0 }));
	const auto gold = chest.assembly.bake().value();
	CheckMeshDataEqual(gold.mesh, opened.mesh);
	CHECK_EQ(gold.materials[0].name, U"wood");
	CHECK_EQ(gold.materials[1].name, U"gold");
	Mesh3DAssemblyExamples::OpenChest(chest, 0);
	CheckMeshDataEqual(chest.assembly.bake().value().mesh, closed.mesh);
}

TEST_CASE("Mesh3DAssembly::cart shared wheels and recipe parameters")
{
	for (const uint32 axleCount : { 1u, 2u, 3u })
	{
		auto cart = Mesh3DAssemblyExamples::MakeCart(2.0, 0.5, axleCount);
		CHECK_EQ(cart.assembly.meshCount(), size_t{ 3 });
		CHECK_EQ(cart.wheels.size(), (axleCount * 2));
		CHECK_EQ(cart.assembly.partCount(), (1 + axleCount * 3));
		const auto baked = cart.assembly.bake().value();
		CheckMeshGeometry(baked.mesh);
		for (size_t i = 0; i < cart.wheels.size(); ++i)
		{
			const auto id = cart.wheels[i];
			CHECK(cart.assembly.getPart(id)->mesh == cart.wheelMesh);
			const Float3 center = baked.parts[static_cast<size_t>(id)].worldTransform.transformPoint(Float3::Zero());
			CHECK(center.x == doctest::Approx((i % 2) ? 1.0 : -1.0));
			CHECK(center.y == doctest::Approx(0.5));
		}
		REQUIRE(cart.assembly.setMesh(cart.wheelMesh, Mesh3D::Cylinder(0.5, 0.22, 32)));
		const auto refined = cart.assembly.bake().value();
		CHECK_GT(refined.mesh.vertexCount(), baked.mesh.vertexCount());
		for (const auto id : cart.wheels)
		{
			const size_t i = static_cast<size_t>(id);
			CHECK(refined.parts[i].id == id);
			CHECK(refined.parts[i].worldTransform == baked.parts[i].worldTransform);
		}
	}
}

TEST_CASE("Mesh3DAssembly::consume baked ranges and export examples")
{
	using namespace Mesh3DAssemblyExamples;
	auto chest = MakeChest();
	Array<Assembly::BakedMesh> examples{ chest.assembly.bake().value() };
	OpenChest(chest, (Math::Pi / 3));
	examples << chest.assembly.bake().value();
	chest.assembly.setMaterial(chest.metal, Material{
		.name = U"gold", .baseColor = ColorF{ 0.85, 0.6, 0.1 }, .metallic = 1.0 }).value();
	examples << chest.assembly.bake().value();
	examples << MakeCart().assembly.bake().value();
	examples << MakeCart(2.4, 0.55, 3).assembly.bake().value();
	const Array<String> names{ U"chest_closed", U"chest_open", U"chest_gold", U"cart", U"cart_wide" };
	for (size_t i = 0; i < examples.size(); ++i)
	{
		const auto& baked = examples[i];
		Mesh3D combined;
		for (const auto& part : baked.parts)
		{
			const Mesh3D mesh = ExtractPart(baked, part);
			REQUIRE(mesh.validate());
			REQUIRE(combined.append(mesh));
		}
		CheckMeshDataEqual(combined, baked.mesh);
		// Optional review artifacts; the normal test suite does not write files.
		if (const char* path = std::getenv("SIV3D_ASSEMBLY_EXAMPLE_DIR"))
		{
			REQUIRE(FileSystem::CreateDirectories(Unicode::FromUTF8(path)));
			REQUIRE(baked.saveOBJ(Unicode::FromUTF8(path) + U"/" + names[i] + U".obj"));
		}
	}
}
