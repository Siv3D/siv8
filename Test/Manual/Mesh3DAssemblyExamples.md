# Mesh3DAssembly: a hinged chest and a cart

This is a CPU modeling and export exercise. It demonstrates reusable geometry,
part IDs, local coordinate systems, hierarchical pivots, material edits, and
baking a complete model without losing the relationship between parts and mesh ranges.

## Execution

1. Use a separate Siv3D v0.8 application built with this revision. Paste the complete
   code below into its `Main.cpp` and run it. Keep this repository's test entry point intact.
2. Open an exported subdirectory under `assembly_examples/` in the application's working
   directory. Import **all** of its `part_*.obj` files together into an OBJ viewer that
   supports multiple objects and MTL base colors. Each OBJ has a matching MTL; keep them together.
3. Compare `chest_closed`, `chest_open`, and `chest_gold` from the same viewpoint.
4. Compare `cart`, `cart_refined`, and `cart_wide` from the same viewpoint.

The recipes and the range extraction used here are compiled in
`Test/Mesh3DAssemblyExamples.hpp` and exercised by `Test/Test_Mesh3DAssembly.cpp`.
The full code below is self-contained; it does not require those test files.
On macOS, the automated tests can additionally export five review variants:

```sh
SIV3D_ASSEMBLY_EXAMPLE_DIR=/tmp/siv8-assembly-review ./macOS/run-tests.sh '--test-case=Mesh3DAssembly::*'
```

Use an empty export directory for each run if you change the number of parts;
this sample only writes current parts and does not delete files from previous runs.

## Expected results

- The closed chest has an open-top shell covered by its lid. The lid's two metal
  straps and front lock are children of the lid. At 60 degrees, the lid and its
  fittings rotate together around the rear upper edge. The body stays fixed.
- The gold variant changes only the metal material. Geometry and wood are unchanged.
- The default cart has four wheels. All four reference one registered wheel mesh.
  The deck touches the axles and the axles enter the wheels; the wheel faces stay clear of the deck.
- `cart_refined` replaces that one wheel mesh with a 32-sided version. All wheels
  become smoother without changing their IDs, placement, or material assignment.
- The wider cart has six larger wheels, a wider deck, and longer axles.
  Wheel centers rise with their radius, keeping the bottoms at ground level.
- Parts may intentionally overlap at connections. These examples do not perform
  Boolean unions and do not claim a single manifold surface for the whole assembly.

## API choices illustrated

A part without a mesh provides a coordinate frame, such as the chest hinge.
A part with a mesh may also have children, such as the lid and its fittings.
Parents must be registered before children. The local-to-world composition is
`local * parentWorld`; local origins are pivots. Names may repeat; IDs identify parts.

The assembly owns shared geometry; copying an assembly makes an independent copy.
`setMesh()` keeps its mesh ID, so existing instances reference the replacement.
`bake(destination)` reuses output arrays, while `bake()` returns a new snapshot.
Baked ranges refer to each part's own geometry, excluding children. An unassigned
material stays unassigned and is not inherited from a parent. This sample gives
unassigned parts a default material during export.

The sample calls `Result::value()` to stop immediately on a modeling error.
Applications that need recovery should inspect each Result and its Mesh3DError.
Placement has the same finite-affine numeric preconditions documented on
`Mesh3DAssembly`; this prototype does not add per-edit numeric validation.

Exporting individual parts here uses the existing Mesh3D OBJ/MTL API.
It preserves their baked positions and per-part materials, but does not export
hierarchy or shared geometry. The original Assembly retains that information.

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

	// Export each part at its baked world position. Import all OBJ files in the
	// directory together to inspect the complete model with per-part materials.
	inline bool SaveParts(const Assembly::BakedMesh& baked, const s3d::FilePath& directory)
	{
		if (not s3d::FileSystem::CreateDirectories(directory))
		{
			return false;
		}
		for (const auto& part : baked.parts)
		{
			if (part.range.isEmpty())
			{
				continue;
			}
			const s3d::Mesh3D mesh = ExtractPart(baked, part);
			const s3d::Material material = (part.material
				? baked.materials[static_cast<size_t>(*part.material)] : s3d::Material{});
			// IDs make filenames independent of duplicate or filesystem-unsafe names.
			const auto path = (directory + U"/part_" + s3d::Format(static_cast<size_t>(part.id)) + U".obj");
			if (not mesh.saveOBJ(path, material))
			{
				return false;
			}
		}
		return true;
	}
}

void Main()
{
    using namespace Mesh3DAssemblyExamples;
    const FilePath directory = U"assembly_examples";
    auto chest = MakeChest();
    auto cart = MakeCart();
    bool success = SaveParts(chest.assembly.bake().value(), directory + U"/chest_closed");
    OpenChest(chest, 60_deg);
    success &= SaveParts(chest.assembly.bake().value(), directory + U"/chest_open");
    chest.assembly.setMaterial(chest.metal, Material{
        .name = U"gold", .baseColor = ColorF{ 0.85, 0.6, 0.1 }, .metallic = 1.0 }).value();
    success &= SaveParts(chest.assembly.bake().value(), directory + U"/chest_gold");
    success &= SaveParts(cart.assembly.bake().value(), directory + U"/cart");
    cart.assembly.setMesh(cart.wheelMesh, Mesh3D::Cylinder(cart.radius, 0.22, 32)).value();
    success &= SaveParts(cart.assembly.bake().value(), directory + U"/cart_refined");
    auto wide = MakeCart(2.4, 0.55, 3);
    success &= SaveParts(wide.assembly.bake().value(), directory + U"/cart_wide");
    Print << (success ? U"Exported to assembly_examples/" : U"Export failed; inspect the engine log.");
    while (System::Update()) {}
}
```
