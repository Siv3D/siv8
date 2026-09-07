# Mesh3DAssembly: texture-free color, a hinged chest, and shared wheels

This CPU modeling exercise exports a complete model as one OBJ and one MTL.
Each part has a group and a material assignment. The service robot uses eight
base-color materials and no textures: orange or blue enamel, ivory housings,
dark rubber, gray joints, a dark visor, cyan eyes, yellow markings, and red signals.
The material palette is applied to actual mesh parts; no image assets are required.

## Execution

1. Use a separate Siv3D v0.8 application built with this revision. Paste the complete
   code below into its `Main.cpp` and run it. Keep this repository's test entry point intact.
2. Open any exported `.obj` under `assembly_examples/` in the application's working
   directory with a viewer that supports OBJ groups and MTL materials. Keep the matching
   `.mtl` beside it. One OBJ now loads the complete model.
3. Compare the two robot palettes from the front and rear, then inspect the chest and cart variants.

The recipes are compiled in `Test/Mesh3DAssemblyExamples.hpp`. The Assembly tests
verify editing and baked ranges; `Test/Test_Mesh3DAssemblyOBJ.cpp` verifies output
coordinates, grouping, and the diffuse color assigned to each robot face.
The code below is self-contained. On macOS, the tests can also export review models:

```sh
SIV3D_ASSEMBLY_EXAMPLE_DIR=/tmp/siv8-assembly-color-review ./macOS/run-tests.sh '--test-case=*Mesh3D*'
```

## Expected results

- Each model produces one OBJ and one MTL. Its groups retain part IDs, including
  duplicate names such as eyes, vents, and fingers. Geometry-free frames have no output group.
- The robot has contrasting housings, a visor with cyan eyes, three colored status
  lights, chest vents, a striped rear battery, two arms with claws, and feet with soles.
  Its left arm is a linked copy of the right arm hierarchy with a mirrored placement;
  both should have outward-facing surfaces. The orange and blue variants differ only in the enamel material.
- All robot materials use metallic = 0: baseColor.rgb maps directly to MTL Kd.
  Inspecting the MTL should reveal eight `newmtl` entries and no texture references.
- The chest lid, straps, and lock rotate together about the rear upper edge; the body stays fixed.
  Its gold variant changes only the fittings. Unlike the robot palette, the chest fittings
  use metallic materials; their appearance depends on the viewer's specular/environment lighting.
- The four cart wheels share one registered mesh. Replacing that mesh with the 32-sided
  version changes all four wheels without changing their IDs or placements. The wider cart
  has six larger wheels, with centers raised to keep their bottoms at ground level.
- Parts intentionally overlap at joints. These are assembled models, not Boolean unions
  or a claim that the complete assembly is a single manifold surface.

## Output and editing contracts

Use `assembly.saveOBJ(path)` for a one-off export. For repeated baking, keep a
`Mesh3DAssembly::BakedMesh`, call `assembly.bake(destination)`, and save it with
`destination.saveOBJ(path)`. Its `encodeOBJ(objWriter, mtlWriter, mtlFileName)`
provides the same data to two separate Writers without intermediate part meshes.

OBJ groups are `part_<ID>_<name>` and material identifiers are `material_<ID>_<name>`.
Names use reversible UTF-8 percent encoding outside ASCII letters, digits, `_`, `-`, and `.`;
empty names omit the final underscore and name. This is the exporter naming convention,
not an OBJ decoding feature. The ID prefix remains directly readable in external viewers.
The MTL filename is escaped by the same rule to avoid whitespace in `mtllib`.
Unassigned parts explicitly use the default material; unused registered materials are omitted.
OBJ stores baked geometry and materials; editing hierarchy and shared meshes remain in Assembly.

Parents precede children; transforms compose as `local * parentWorld`. Names may repeat;
IDs identify parts. Numeric placement preconditions are documented in Mesh3DAssembly.
The sample uses `Result::value()` to stop on a modeling error; applications needing recovery
should inspect the Result and its Mesh3DError. Save failures are returned as bool and logged.

## Frame alignment and linked hierarchy copies

`Mesh3DPlacement::Align(sourceFrame, targetFrame)` computes a placement that maps
an attachment frame in the part's local coordinates to a frame in the parent's
coordinates. It includes position, orientation, scale, and shear. Both frames
must be finite affine transforms, the source must be invertible, and its inverse
and the result must be finite and representable as float values. It is a one-time calculation; changing recipe dimensions
requires rebuilding the frames. In this recipe, the foot housing's bottom is
derived from `footSize.y`, keeping its mounting height fixed when the size changes.
The mounting plane intentionally lies slightly inside the sole.

`cloneSubtree(root, placement, parent)` copies the root and its descendants and
replaces the root's placement and parent. Omitting the parent places the new root
directly in Assembly coordinates. Mesh and material IDs stay shared; names and
local transforms are copied. The returned `ClonedSubtree::find(originalPartID)`
finds the new ID for editing a particular copied joint. Shared resource edits
propagate, while placement and later hierarchy edits remain independent.
The robot builds one arm hierarchy and uses this operation for the other arm.

The shared-resource behavior follows the familiar distinction described in
Blender's [Duplicate Linked](https://docs.blender.org/manual/en/4.1/scene_layout/object/editing/duplicate_linked.html).
Frame alignment is related to [Align to Transform Orientation](https://docs.blender.org/manual/en/5.2/scene_layout/object/editing/transform/align_transform_orientation.html),
but this API maps complete affine frames, including their origins, rather than
replicating Blender's rotation-only operator.

## Complete code

```cpp
# include <Siv3D.hpp>

namespace Mesh3DAssemblyExamples
{
	using Assembly = s3d::Mesh3DAssembly;

	struct Chest
	{
		Assembly assembly;
		Assembly::PartID hinge;
		Assembly::PartID lid;
		Assembly::MaterialID metal;
	};

	inline Chest MakeChest()
	{
		Chest result;
		auto& a = result.assembly;
		const auto wood = a.addMaterial(s3d::Material{
			.name = U"wood", .baseColor = s3d::ColorF{ 0.40, 0.18, 0.06 }, .roughness = 0.85 });
		result.metal = a.addMaterial(s3d::Material{
			.name = U"iron", .baseColor = s3d::ColorF{ 0.25, 0.28, 0.32 }, .metallic = 0.9, .roughness = 0.3 });
		const auto shell = a.addMesh(s3d::Mesh3D::BoxShell(s3d::Vec3{ 2.4, 1.2, 1.4 }, 0.12, s3d::BoxFace::PositiveY)).value();
		const auto lid = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ 2.4, 0.16, 1.4 })).value();
		const auto strap = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ 0.12, 0.035, 1.3 })).value();
		const auto lock = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ 0.25, 0.3, 0.08 })).value();
		(void)a.addPart({ .name = U"body", .mesh = shell, .material = wood,
			.placement = s3d::Vec3{ 0, 0.6, 0 } }).value();

		// The rear top edge is the hinge. The lid's front extends in local -Z.
		result.hinge = a.addPart({ .name = U"hinge", .placement = s3d::Vec3{ 0, 1.2, 0.7 } }).value();
		result.lid = a.addPart({ .name = U"lid", .mesh = lid, .material = wood, .parent = result.hinge,
			.placement = s3d::Vec3{ 0, 0.08, -0.7 } }).value();
		for (const double x : { -0.8, 0.8 })
		{
			(void)a.addPart({ .name = U"lid strap", .mesh = strap, .material = result.metal, .parent = result.lid,
				.placement = s3d::Vec3{ x, 0.085, 0 } }).value();
		}
		(void)a.addPart({ .name = U"lock", .mesh = lock, .material = result.metal, .parent = result.lid,
			.placement = s3d::Vec3{ 0, -0.04, -0.72 } }).value();
		return result;
	}

	inline void OpenChest(Chest& chest, const double angle)
	{
		chest.assembly.setPlacement(chest.hinge,
			{ s3d::Vec3{ 0, 1.2, 0.7 }, s3d::Quaternion::RotateX(angle) }).value();
	}

	struct Cart
	{
		Assembly assembly;
		Assembly::MeshID wheelMesh;
		s3d::Array<Assembly::PartID> axles;
		s3d::Array<Assembly::PartID> wheels;
		double track = 1.8;
		double radius = 0.4;
	};

	inline Cart MakeCart(const double track = 1.8, const double radius = 0.4, const s3d::uint32 axleCount = 2)
	{
		Cart result;
		result.track = track;
		result.radius = radius;
		auto& a = result.assembly;
		const auto paint = a.addMaterial(s3d::Material{
			.name = U"paint", .baseColor = s3d::ColorF{ 0.1, 0.45, 0.65 }, .roughness = 0.5 });
		const auto rubber = a.addMaterial(s3d::Material{
			.name = U"rubber", .baseColor = s3d::ColorF{ 0.06 }, .roughness = 0.95 });
		const auto metal = a.addMaterial(s3d::Material{
			.name = U"steel", .baseColor = s3d::ColorF{ 0.5 }, .metallic = 0.9, .roughness = 0.3 });
		result.wheelMesh = a.addMesh(s3d::Mesh3D::Cylinder(radius, 0.22, 16)).value();
		const auto axleMesh = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ track, 0.12, 0.12 })).value();
		const auto deckMesh = a.addMesh(s3d::Mesh3D::Box(s3d::Vec3{ (track - 0.32), 0.18, 2.8 })).value();
		(void)a.addPart({ .name = U"deck", .mesh = deckMesh, .material = paint,
			.placement = s3d::Vec3{ 0, (radius + 0.14), 0 } }).value();
		for (s3d::uint32 i = 0; i < axleCount; ++i)
		{
			const double z = ((axleCount == 1) ? 0.0 : (-0.9 + (1.8 * i / (axleCount - 1))));
			const auto axle = a.addPart({ .name = U"axle", .mesh = axleMesh, .material = metal,
				.placement = s3d::Vec3{ 0, radius, z } }).value();
			result.axles << axle;
			for (const double side : { -1.0, 1.0 })
			{
				result.wheels << a.addPart({ .name = ((side < 0) ? U"left wheel" : U"right wheel"),
					.mesh = result.wheelMesh, .material = rubber, .parent = axle,
					.placement = s3d::Mesh3DPlacement{
						s3d::Vec3{ (side * track / 2), 0, 0 }, s3d::Quaternion::RotateZ(s3d::Math::HalfPi) } }).value();
			}
		}
		return result;
	}

	inline s3d::Mesh3D ExtractPart(const Assembly::BakedMesh& baked, const Assembly::BakedPart& part)
	{
		const auto& r = part.range;
		if (r.isEmpty())
		{
			return{};
		}
		s3d::Mesh3D result{
			std::span<const s3d::Vertex3D>{ (baked.mesh.vertices.data() + r.vertexOffset), r.vertexCount },
			std::span<const s3d::TriangleIndex32>{ (baked.mesh.indices.data() + r.triangleOffset), r.triangleCount } };
		const auto base = static_cast<s3d::uint32>(r.vertexOffset);
		for (auto& triangle : result.indices)
		{
			triangle.i0 -= base;
			triangle.i1 -= base;
			triangle.i2 -= base;
		}
		return result;
	}

	// A texture-free stylized service robot. Every visible accent is geometry
	// with a base-color material; repeated fittings reuse registered meshes.
	inline Assembly MakeRobot(const s3d::ColorF bodyColor = s3d::ColorF{ 0.95, 0.38, 0.08 })
	{
		using namespace s3d;
		Assembly a;
		const auto paint = a.addMaterial(Material{ .name = U"enamel", .baseColor = bodyColor, .roughness = 0.5 });
		const auto ivory = a.addMaterial(Material{ .name = U"ivory", .baseColor = ColorF{ 0.88, 0.90, 0.82 } });
		const auto dark = a.addMaterial(Material{ .name = U"rubber", .baseColor = ColorF{ 0.055, 0.075, 0.10 } });
		const auto grey = a.addMaterial(Material{ .name = U"joints", .baseColor = ColorF{ 0.24, 0.31, 0.35 } });
		const auto glass = a.addMaterial(Material{ .name = U"visor", .baseColor = ColorF{ 0.02, 0.13, 0.20 }, .roughness = 0.25 });
		const auto cyan = a.addMaterial(Material{ .name = U"status cyan", .baseColor = ColorF{ 0.16, 0.86, 0.96 } });
		const auto yellow = a.addMaterial(Material{ .name = U"warning yellow", .baseColor = ColorF{ 1.0, 0.76, 0.10 } });
		const auto red = a.addMaterial(Material{ .name = U"signal red", .baseColor = ColorF{ 0.86, 0.12, 0.09 } });
		const auto body = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 1.1, 1.05, 0.7 }, 0.10)).value();
		const auto head = a.addMesh(Mesh3D::RoundedBox(Vec3{ 1.16, 0.68, 0.76 }, 0.14, 4)).value();
		const auto visor = a.addMesh(Mesh3D::RoundedBox(Vec3{ 0.98, 0.39, 0.12 }, 0.055, 3)).value();
		const auto eye = a.addMesh(Mesh3D::RoundedBox(Vec3{ 0.12, 0.19, 0.045 }, 0.02, 3)).value();
		const auto joint = a.addMesh(Mesh3D::Cylinder(0.20, 0.18, 16)).value();
		const auto upperArm = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.28, 0.38, 0.30 }, 0.045)).value();
		const auto forearm = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.34, 0.42, 0.36 }, 0.05)).value();
		const auto finger = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.095, 0.21, 0.18 }, 0.02)).value();
		const auto shin = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.32, 0.38, 0.36 }, 0.04)).value();
		const Vec3 footSize{ 0.48, 0.24, 0.72 };
		const auto foot = a.addMesh(Mesh3D::RoundedBox(footSize, 0.07, 3)).value();
		const auto sole = a.addMesh(Mesh3D::Box(Vec3{ 0.49, 0.07, 0.73 })).value();
		const auto vent = a.addMesh(Mesh3D::Box(Vec3{ 0.48, 0.055, 0.045 })).value();
		const auto panel = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.78, 0.40, 0.10 }, 0.04)).value();
		const auto button = a.addMesh(Mesh3D::Cylinder(0.042, 0.045, 12)).value();
		const auto backpack = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.74, 0.75, 0.36 }, 0.08)).value();
		const auto rim = a.addMesh(Mesh3D::Box(Vec3{ 0.62, 0.065, 0.025 })).value();
		const auto belt = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 0.86, 0.18, 0.59 }, 0.045)).value();
		const auto antenna = a.addMesh(Mesh3D::Cylinder(0.028, 0.32, 10)).value();
		const auto tip = a.addMesh(Mesh3D::Sphere(0.075, 12, 8)).value();

		const auto add = [&](String name, Assembly::MeshID mesh, Assembly::MaterialID material,
			Vec3 offset, Optional<Assembly::PartID> parent = none, Quaternion rotation = Quaternion{})
		{
			return a.addPart({ .name = std::move(name), .mesh = mesh, .material = material, .parent = parent,
				.placement = Mesh3DPlacement{ offset, rotation } }).value();
		};
		const auto torso = add(U"torso", body, paint, Vec3{ 0, 1.25, 0 });
		add(U"waist", belt, grey, Vec3{ 0, 0.69, 0 });
		const auto face = add(U"head", head, ivory, Vec3{ 0, 2.15, 0 });
		// The neck connects the head to the torso; cylinders start along +Y.
		add(U"neck", joint, grey, Vec3{ 0, 1.80, 0 });
		add(U"visor", visor, glass, Vec3{ 0, 0.025, -0.385 }, face);
		for (const double side : { -1.0, 1.0 })
		{
			add(U"eye", eye, cyan, Vec3{ (side * 0.24), 0.025, -0.455 }, face);
		}
		add(U"antenna", antenna, grey, Vec3{ 0.35, 0.47, 0.12 }, face);
		add(U"antenna tip", tip, red, Vec3{ 0.35, 0.66, 0.12 }, face);
		const auto chest = add(U"chest panel", panel, dark, Vec3{ 0, 0.21, -0.35 }, torso);
		for (uint32 i = 0; i < 3; ++i)
		{
			add(U"status light", button, (i == 0 ? red : (i == 1 ? yellow : cyan)),
				Vec3{ (-0.23 + i * 0.23), 0.02, -0.055 }, chest, Quaternion::RotateX(Math::HalfPi));
			add(U"vent", vent, grey, Vec3{ 0, (-0.10 - i * 0.12), -0.35 }, torso);
		}
		const auto pack = add(U"battery pack", backpack, grey, Vec3{ 0, 0.03, 0.47 }, torso);
		for (uint32 i = 0; i < 3; ++i)
		{
			add(U"battery stripe", rim, yellow, Vec3{ 0, (0.20 - i * 0.20), 0.18 }, pack);
		}
		// Build one arm, then copy its hierarchy with shared mesh/material IDs.
		const auto arm = a.addPart({ .name = U"arm frame",
			.placement = Mesh3DPlacement{ Vec3{ 0.62, 1.62, 0 }, Quaternion::RotateZ(0.10) } }).value();
		add(U"shoulder", joint, grey, Vec3::Zero(), arm, Quaternion::RotateZ(Math::HalfPi));
		add(U"upper arm", upperArm, paint, Vec3{ 0.12, -0.23, 0 }, arm);
		add(U"elbow", joint, dark, Vec3{ 0.12, -0.45, 0 }, arm, Quaternion::RotateZ(Math::HalfPi));
		add(U"forearm", forearm, ivory, Vec3{ 0.12, -0.68, 0 }, arm);
		add(U"claw", finger, grey, Vec3{ 0.02, -0.94, -0.04 }, arm);
		add(U"claw", finger, grey, Vec3{ 0.22, -0.94, -0.04 }, arm);
		(void)a.cloneSubtree(arm, Mat4x4::AffineTransform(Float3{ -1, 1, 1 },
			Quaternion::RotateZ(-0.10), Float3{ -0.62f, 1.62f, 0 })).value();
		for (const double side : { -1.0, 1.0 })
		{
			const auto leg = a.addPart({ .name = U"leg frame", .placement = Vec3{ (side * 0.27), 0, 0 } }).value();
			add(U"knee", joint, dark, Vec3{ 0, 0.56, 0 }, leg, Quaternion::RotateZ(Math::HalfPi));
			add(U"shin", shin, paint, Vec3{ 0, 0.40, 0 }, leg);
			// Align the housing's bottom to a mounting plane inside the sole.
			(void)a.addPart({ .name = U"foot", .mesh = foot, .material = ivory, .parent = leg,
				.placement = Mesh3DPlacement::Align(Vec3{ 0, (-footSize.y * 0.5), 0 }, Vec3{ 0, 0.05, -0.13 }) }).value();
			add(U"sole", sole, dark, Vec3{ 0, 0.035, -0.13 }, leg);
		}
		return a;
	}
}

void Main()
{
    using namespace Mesh3DAssemblyExamples;
    const FilePath directory = U"assembly_examples/";
    if (not FileSystem::CreateDirectories(directory))
    {
        return;
    }
    auto chest = MakeChest();
    bool success = chest.assembly.saveOBJ(directory + U"chest_closed.obj");
    OpenChest(chest, 60_deg);
    success &= chest.assembly.saveOBJ(directory + U"chest_open.obj");
    chest.assembly.setMaterial(chest.metal, Material{
        .name = U"gold", .baseColor = ColorF{ 0.85, 0.6, 0.1 }, .metallic = 1.0 }).value();
    success &= chest.assembly.saveOBJ(directory + U"chest_gold.obj");
    auto cart = MakeCart();
    success &= cart.assembly.saveOBJ(directory + U"cart.obj");
    cart.assembly.setMesh(cart.wheelMesh, Mesh3D::Cylinder(cart.radius, 0.22, 32)).value();
    success &= cart.assembly.saveOBJ(directory + U"cart_refined.obj");
    success &= MakeCart(2.4, 0.55, 3).assembly.saveOBJ(directory + U"cart_wide.obj");
    success &= MakeRobot().saveOBJ(directory + U"robot_orange.obj");
    success &= MakeRobot(ColorF{ 0.16, 0.48, 0.72 }).saveOBJ(directory + U"robot_blue.obj");
    Print << (success ? U"Exported to assembly_examples/" : U"Export failed; inspect the engine log.");
    while (System::Update()) {}
}
```
