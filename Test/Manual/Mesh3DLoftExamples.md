# Loft: changing profiles and frames

This CPU modeling example generates a curved duct whose section shape changes
from a rounded rectangle to a circle. Each section also changes its orientation.
The assembly adds a base, outlet ring and shared fasteners, then creates a mirrored
linked copy. All colors come from materials; there are no textures.

## Execution

1. Use a separate Siv3D v0.8 application built with this revision and paste the
   complete code below into its Main.cpp. Keep this repository's test entry point intact.
2. Open `loft_examples/duct_smooth.obj` and `duct_faceted.obj` in an OBJ/MTL viewer.
   Keep each matching MTL next to its OBJ. Inspect the outlet, rear and side views.
3. Change the section count and contour sample count, then compare the results.

On macOS the automated recipe test can also export the models:

```sh
SIV3D_LOFT_EXAMPLE_DIR=/tmp/siv8-loft-review ./macOS/run-tests.sh '--test-case=*Loft*'
```

## Expected results

- Both models contain two matching duct units, one mirrored. Each has a blue housing,
  dark base, orange outlet ring and four gray fasteners.
- The smooth version interpolates contour normals; the faceted version keeps each
  contour edge hard. Both retain the same topology for equal section/sample counts.
- The outlet ring follows the final section frame through Align. The reflected unit
  has outward-facing surfaces. Changing a shared MeshID updates both units.
- This is an exterior surface example. `endCaps = None` creates neither an inner wall
  nor wall thickness. The base and rim overlap the housing intentionally.
- The recipe supports at least two sections and at least eight contour samples in
  multiples of four. Higher values improve tessellation; there is no hidden subdivision.

The recipe is compiled from `Test/Mesh3DLoftExamples.hpp`. The automated test also
replaces the shared duct mesh with a finer version and checks stable PartIDs,
materials and placements. Invalid-input behavior belongs to the generator tests;
this short recipe assumes valid parameters and uses Result::value() on success paths.

## Migrating from height-based Loft

Replace parallel contour/height arrays with LoftSection entries. A former height h
becomes `frame = Vec3{0, h, 0}`. Keep the referenced contour arrays alive throughout
the call. LoftSection is a non-owning view and can share a contour between sections.
UV arguments move into LoftOptions. Builder placement precedes the options argument.

The new API keeps the local mapping `(x, y) -> (x, 0, -y)`. Frame origin distances
supply side V; they are not necessarily distances measured on the generated surface.
Frames must be finite affine transforms with positive determinants. Between adjacent
sections, the origin displacement must have positive projection on both section
plane normals. Add intermediate sections to follow a bend.

Whole-mesh reflection preserves facing through Mesh3D transforms, placed Builder
additions, and Assembly baking. Do not apply reverseWinding() as a correction
after a reflection; use invert() when intentionally turning a surface inside out.
The per-section frame orientation requirements above still apply.

The generator connects matching vertex indices without automatic resampling,
reversal or correspondence search. It rejects local degeneracy and locally folded
triangles but does not detect every contour or surface self-intersection. Requested
caps are triangulated in 2D; cap-free generation avoids that work. See the public
Doxygen for exact normal, tangent, UV and failure contracts.

## Generation cost measured during migration

A local macOS arm64 Release comparison used height-only circular contours, both
caps, smoothingAngle = 0, and reusable destination vertex/index capacity. Each
result is the median of five trials of 100 generations, alternating old/new order.
The old generator was compiled alongside the new one for this development check;
that temporary comparison code is not part of the library or automated suite.

| Sections × contour vertices | Previous generator | Frame-based generator |
| --- | ---: | ---: |
| 8 × 16 | 11.3 µs | 13.6 µs |
| 32 × 32 | 47.5 µs | 64.7 µs |
| 64 × 64 | 119.7 µs | 185.1 µs |

The new generator includes section placement and local triangle checks. It avoids
extra tangent normalization for hard contours and skips cap triangulation when
caps are disabled. These local measurements describe this workload; they do not
predict every model's cost. Generate static models once and reuse their MeshIDs.

## Complete code

```cpp
# include <Siv3D.hpp>

namespace Mesh3DLoftExamples
{
	struct Duct
	{
		s3d::Mesh3D mesh;
		s3d::Mesh3DPlacement outlet;
	};

	// Changes both the contour shape and its plane along a curved centerline.
	inline Duct MakeDuct(const s3d::uint32 samples = 32, const s3d::uint32 sectionCount = 9,
		const double smoothingAngle = s3d::Math::Pi)
	{
		using namespace s3d;
		Array<Array<Vec2>> contours(sectionCount);
		Array<LoftSection> sections;
		sections.reserve(sectionCount);
		for (uint32 i = 0; i < sectionCount; ++i)
		{
			const double t = static_cast<double>(i) / (sectionCount - 1);
			const double exponent = (0.6 + 0.4 * t);
			const double rx = (0.65 - 0.27 * t), rz = (0.45 - 0.07 * t);
			for (uint32 j = 0; j < samples; ++j)
			{
				const double a = Math::TwoPi * j / samples;
				const double c = std::cos(a), s = std::sin(a);
				contours[i].emplace_back(rx * std::copysign(std::pow(std::abs(c), exponent), c),
					rz * std::copysign(std::pow(std::abs(s), exponent), s));
			}
			const Mesh3DPlacement frame{ Vec3{ 0, 2.4 * t, t * t }, Quaternion::RotateX(std::atan(2 * t / 2.4)) };
			sections.push_back({ contours[i], frame });
		}
		return { Mesh3D::Loft(sections, { .endCaps = Mesh3DEndCaps::None, .smoothingAngle = smoothingAngle }), sections.back().frame };
	}

	inline s3d::Mesh3DAssembly MakeAssembly(const s3d::uint32 samples = 32, const s3d::uint32 sectionCount = 9,
		const double smoothingAngle = s3d::Math::Pi)
	{
		using namespace s3d;
		Mesh3DAssembly a;
		const auto blue = a.addMaterial(Material{ .name = U"blue enamel", .baseColor = ColorF{ 0.10, 0.43, 0.70 } });
		const auto dark = a.addMaterial(Material{ .name = U"base", .baseColor = ColorF{ 0.08, 0.11, 0.14 } });
		const auto orange = a.addMaterial(Material{ .name = U"outlet ring", .baseColor = ColorF{ 0.95, 0.4, 0.06 } });
		const auto grey = a.addMaterial(Material{ .name = U"fasteners", .baseColor = ColorF{ 0.5, 0.55, 0.58 } });
		auto duct = MakeDuct(samples, sectionCount, smoothingAngle);
		const auto body = a.addMesh(std::move(duct.mesh)).value();
		const auto base = a.addMesh(Mesh3D::ChamferedBox(Vec3{ 1.55, 0.25, 1.2 }, 0.07)).value();
		const auto rim = a.addMesh(Mesh3D::Torus(0.40, 0.04, samples, 8)).value();
		const auto bolt = a.addMesh(Mesh3D::Cylinder(0.055, 0.045, 8)).value();
		const auto root = a.addPart({ .name = U"duct unit" }).value();
		(void)a.addPart({ .name = U"curved housing", .mesh = body, .material = blue, .parent = root }).value();
		(void)a.addPart({ .name = U"mounting base", .mesh = base, .material = dark, .parent = root, .placement = Vec3{ 0, -0.125, 0 } }).value();
		(void)a.addPart({ .name = U"outlet", .mesh = rim, .material = orange, .parent = root,
			.placement = Mesh3DPlacement::Align(Vec3::Zero(), duct.outlet) }).value();
		for (const double x : { -0.65, 0.65 })
		{
			for (const double z : { -0.46, 0.46 })
			{
				(void)a.addPart({ .name = U"bolt", .mesh = bolt, .material = grey, .parent = root,
					.placement = Vec3{ x, 0.0225, z } }).value();
			}
		}
		(void)a.cloneSubtree(root, Mat4x4::Scale(Float3{ -1, 1, 1 }) * Mat4x4::Translate(2.0f, 0, 0)).value();
		return a;
	}
}

void Main()
{
    const FilePath directory = U"loft_examples/";
    if (not FileSystem::CreateDirectories(directory))
    {
        throw Error{ U"Could not create output directory" };
    }
    const auto smooth = Mesh3DLoftExamples::MakeAssembly(32, 9, Math::Pi);
    const auto faceted = Mesh3DLoftExamples::MakeAssembly(32, 9, 0.0);
    if (not smooth.saveOBJ(directory + U"duct_smooth.obj")
        || not faceted.saveOBJ(directory + U"duct_faceted.obj"))
    {
        throw Error{ U"Could not save Loft examples" };
    }
    Print << U"Saved smooth and faceted Loft examples.";
    while (System::Update()) {}
}
```
