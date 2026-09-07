# Mesh3D modeling studies: attachment frames and open grid surfaces

This self-contained CPU example tests two modeling techniques without adding
public anchor or Surface APIs. It exports three column/beam variants, three
pipe-joint variants, a cloth patch, and a terrain skirt. All use existing
Mesh3D/Assembly APIs, including the new scale-aware Mesh3DPlacement constructor.

## Execution

1. Build a separate Siv3D v0.8 application with this revision. Paste the complete
   code below into its Main.cpp. Keep this repository's test entry point intact.
2. Run it once. Any failed assertion or export throws an Error. Eight OBJ/MTL
   pairs should appear in `modeling_studies/` in the working directory.
3. Inspect `support_0` through `support_2`: increasing column height and beam
   depth must keep the beam's bottom seat on the column's head.
4. Inspect `pipe_0` through `pipe_2`: both the inlet pose and the outlet connection
   remain aligned as bend angle and sampling count change.
5. Inspect `cloth.obj` from above and `skirt.obj` from +X. They are open, one-sided
   patches. The skirt's top follows the supplied samples exactly; there is no
   implicit bottom, thickness, seam welding, or Boolean union.

## Checks and fixture limits

- Four probe points verify each attachment frame's origin and axes under changed
  dimensions and transformed parents.
- Pipe fixtures are planar XY sweeps with a specified +Z section axis. Both cap
  vertex sets must coincide after placement. The matching convex contours have
  13 sides. This checks the polygonal join, not a watertight Boolean union.
- Open grids must have the expected vertex/triangle counts, boundary incidence,
  opposite shared-edge directions, UV endpoints, and valid normal/tangent frames.
- The 2x2 plane faces +Y. The 1x2 grid must be rejected. Skirt upper vertices
  exactly match the supplied samples and face +X.
- Fixture positions are finite and triangles are nondegenerate. These checks do
  not cover arbitrary self-intersections, closed seams, poles, or spatial-curve frames.

For reusable techniques, see the [modeling guide](../../docs/mesh3d/modeling.md).
The prototype's unresolved public contract belongs to the
[open-grid design study](../../docs/mesh3d/proposals/open-grid-surface.md).

## Complete sample

```cpp
# include <Siv3D.hpp>
# include <map>

namespace Study
{
	void Check(bool condition, StringView message)
	{
		if (not condition)
		{
			throw Error{ String{ message } };
		}
	}

	void CheckGeometry(const Mesh3D& mesh)
	{
		Check(not mesh.isEmpty() && mesh.validate(), U"Invalid mesh");
		for (const auto& vertex : mesh.vertices)
		{
			Check(std::isfinite(vertex.pos.x) && std::isfinite(vertex.pos.y)
				&& std::isfinite(vertex.pos.z), U"Non-finite position");
			Check(Math::Abs(vertex.normal.length() - 1.0f) < 1e-4f, U"Invalid normal");
			Check(Math::Abs(vertex.tangent.xyz().length() - 1.0f) < 1e-4f
				&& Math::Abs(vertex.normal.dot(vertex.tangent.xyz())) < 1e-4f, U"Invalid tangent");
		}
		for (const auto triangle : mesh.indices)
		{
			const auto& a = mesh.vertices[triangle.i0];
			const auto& b = mesh.vertices[triangle.i1];
			const auto& c = mesh.vertices[triangle.i2];
			const auto normal = (b.pos - a.pos).cross(c.pos - a.pos);
			Check(normal.lengthSq() > 1e-12f, U"Degenerate triangle");
			Check(normal.dot(a.normal + b.normal + c.normal) > 0, U"Inconsistent winding");
		}
	}

	void CheckAligned(const Mat4x4& source, const Mat4x4& target)
	{
		for (const Float3 point : { Float3::Zero(), Float3{ 1, 0, 0 },
			Float3{ 0, 1, 0 }, Float3{ 0, 0, 1 } })
		{
			Check(source.transformPoint(point).epsilonEquals(target.transformPoint(point), 2e-5f),
				U"Attachment origin or axis mismatch");
		}
	}

	// These frames belong to this recipe; they are not a generic anchor registry.
	struct Column
	{
		Mesh3D mesh;
		Mesh3DPlacement foot;
		Mesh3DPlacement head;
	};

	Column MakeColumn(double height, double width)
	{
		return { Mesh3D::Box(Vec3{ width, height, width }),
			Vec3{ 0, -height * 0.5, 0 }, Vec3{ 0, height * 0.5, 0 } };
	}

	struct Beam
	{
		Mesh3D mesh;
		Mesh3DPlacement seat;
	};

	Beam MakeBeam(double span, double depth)
	{
		return { Mesh3D::Box(Vec3{ span, depth, 0.8 }), Vec3{ 0, -depth * 0.5, 0 } };
	}

	void ExportSupport(double height, double depth, int32 variant)
	{
		Mesh3DAssembly assembly;
		const auto stone = assembly.addMaterial(Material{ .name = U"stone", .baseColor = ColorF{ 0.7, 0.72, 0.75 } });
		const auto wood = assembly.addMaterial(Material{ .name = U"wood", .baseColor = ColorF{ 0.55, 0.27, 0.10 } });
		auto column = MakeColumn(height, 0.6);
		auto beam = MakeBeam(3.0, depth);
		const auto columnMesh = assembly.addMesh(std::move(column.mesh)).value();
		const auto beamMesh = assembly.addMesh(std::move(beam.mesh)).value();
		const Mesh3DPlacement rootPlacement{ Vec3{ 1, 0, -2 }, Quaternion::RotateY(0.4), Vec3{ 1.2, 1, 0.8 } };
		const auto root = assembly.addPart({ .name = U"support", .placement = rootPlacement }).value();
		const auto post = assembly.addPart({ .name = U"column", .mesh = columnMesh, .material = stone,
			.parent = root, .placement = Mesh3DPlacement::Align(column.foot, Vec3::Zero()) }).value();
		const auto lintel = assembly.addPart({ .name = U"beam", .mesh = beamMesh, .material = wood,
			.parent = post, .placement = Mesh3DPlacement::Align(beam.seat, column.head) }).value();
		const auto postWorld = assembly.computeWorldTransform(post).value();
		CheckAligned(column.foot.getTransform() * postWorld, rootPlacement.getTransform());
		CheckAligned(beam.seat.getTransform() * assembly.computeWorldTransform(lintel).value(),
			column.head.getTransform() * postWorld);
		const auto baked = assembly.bake().value();
		CheckGeometry(baked.mesh);
		Check(baked.saveOBJ(U"modeling_studies/support_{}.obj"_fmt(variant)), U"Support export failed");
	}

	struct Pipe
	{
		Mesh3D mesh;
		Mesh3DPlacement inlet;
		Mesh3DPlacement outlet;
	};

	Pipe MakeBend(double bendRadius, double angle, int32 segments, double tubeRadius)
	{
		Array<Vec3> path;
		path.reserve(segments + 1);
		for (int32 i = 0; i <= segments; ++i)
		{
			const double t = angle * i / segments;
			path << Vec3{ bendRadius * (1 - std::cos(t)), bendRadius * std::sin(t), 0 };
		}
		// For this planar XY path, a +Z section axis remains +Z during transport.
		// Specify it explicitly so the polygon's rotational phase also matches at the joint.
		const auto frameAt = [](Vec3 tangent)
		{
			return Quaternion::FromUnitVectorPairs(
				{ Vec3{ 1, 0, 0 }, Vec3{ 0, 1, 0 } },
				{ Vec3{ 0, 0, 1 }, tangent.normalized() });
		};
		const auto inletRotation = frameAt(path[1] - path[0]);
		const auto outletRotation = frameAt(path.back() - path[path.size() - 2]);
		return { Mesh3D::Sweep(Circle{ tubeRadius }.asPolygon(PointsPerCircle{ 13 }), path,
			SweepOptions{ .initialXAxis = Vec3{ 0, 0, 1 } }),
			{ path.front(), inletRotation }, { path.back(), outletRotation } };
	}

	Array<Float3> CapPoints(const Mesh3D& mesh, const Mat4x4& world, Vec3 capOrigin, Vec3 capDirection)
	{
		Array<Float3> points;
		for (const auto& vertex : mesh.vertices)
		{
			if (vertex.normal.dot(Float3{ capDirection }) > 0.999f
				&& Math::Abs((vertex.pos - Float3{ capOrigin }).dot(Float3{ capDirection })) < 1e-5f)
			{
				points << world.transformPoint(vertex.pos);
			}
		}
		Check(points.size() >= 3, U"Missing cap vertices");
		return points;
	}

	void CheckCapMatch(const Array<Float3>& a, const Array<Float3>& b)
	{
		for (const auto& points : { std::pair{ &a, &b }, std::pair{ &b, &a } })
		{
			for (const auto point : *points.first)
			{
				bool found = false;
				for (const auto other : *points.second)
				{
					found |= point.epsilonEquals(other, 2e-5f);
				}
				Check(found, U"Cap polygons do not coincide");
			}
		}
	}

	void ExportPipe(double angle, int32 segments, int32 variant)
	{
		Mesh3DAssembly assembly;
		const auto red = assembly.addMaterial(Material{ .name = U"bend", .baseColor = ColorF{ 0.7, 0.12, 0.08 } });
		const auto blue = assembly.addMaterial(Material{ .name = U"extension", .baseColor = ColorF{ 0.12, 0.3, 0.7 } });
		auto bend = MakeBend(1.2, angle, segments, 0.10);
		const auto bendMesh = assembly.addMesh(std::move(bend.mesh)).value();
		const auto extensionMesh = assembly.addMesh(Mesh3D::Sweep(Circle{ 0.10 }.asPolygon(PointsPerCircle{ 13 }),
			{ Vec3::Zero(), Vec3{ 0, 0.8, 0 } }, SweepOptions{ .initialXAxis = Vec3{ 1, 0, 0 } })).value();
		const Mesh3DPlacement target{ Vec3{ 2, 1, -1 }, Quaternion::RotateZ(-0.3) * Quaternion::RotateY(0.5) };
		const auto elbow = assembly.addPart({ .name = U"elbow", .mesh = bendMesh, .material = red,
			.placement = Mesh3DPlacement::Align(bend.inlet, target) }).value();
		const auto extension = assembly.addPart({ .name = U"extension", .mesh = extensionMesh, .material = blue,
			.parent = elbow, .placement = Mesh3DPlacement::Align(Vec3::Zero(), bend.outlet) }).value();
		const auto elbowWorld = assembly.computeWorldTransform(elbow).value();
		CheckAligned(bend.inlet.getTransform() * elbowWorld, target.getTransform());
		CheckAligned(assembly.computeWorldTransform(extension).value(), bend.outlet.getTransform() * elbowWorld);
		const auto& outlet = bend.outlet.getTransform();
		const Vec3 endDirection{ outlet.transformPoint(Float3{ 0, 1, 0 }) - outlet.transformPoint(Float3::Zero()) };
		CheckCapMatch(CapPoints(*assembly.getMesh(bendMesh), elbowWorld, Vec3{ outlet.transformPoint(Float3::Zero()) }, endDirection),
			CapPoints(*assembly.getMesh(extensionMesh), assembly.computeWorldTransform(extension).value(), Vec3::Zero(), Vec3{ 0, -1, 0 }));
		const auto baked = assembly.bake().value();
		CheckGeometry(baked.mesh);
		Check(baked.saveOBJ(U"modeling_studies/pipe_{}.obj"_fmt(variant)), U"Pipe export failed");
	}

	// Prototype, not a proposed public signature. Input is a small, regular open grid.
	// Column is u, row is v. Front side follows cross(dP/du, dP/dv).
	Mesh3D MakeOpenSurface(const Grid<Vec3>& positions)
	{
		const int32 width = positions.width();
		const int32 height = positions.height();
		Check(width >= 2 && height >= 2, U"Surface needs at least a 2 by 2 grid");
		Mesh3D mesh(static_cast<size_t>(width) * height,
			static_cast<size_t>(width - 1) * (height - 1) * 2);
		for (int32 y = 0; y < height; ++y)
		{
			for (int32 x = 0; x < width; ++x)
			{
				auto& vertex = mesh.vertices[y * width + x];
				vertex.pos = Float3{ positions[y][x] };
				vertex.tex = Float2{ static_cast<float>(x) / (width - 1), static_cast<float>(y) / (height - 1) };
			}
		}
		size_t triangle = 0;
		for (int32 y = 0; y < height - 1; ++y)
		{
			for (int32 x = 0; x < width - 1; ++x)
			{
				const uint32 a = y * width + x;
				const uint32 b = a + 1, c = a + width, d = c + 1;
				mesh.indices[triangle++] = { a, b, c };
				mesh.indices[triangle++] = { b, d, c };
			}
		}
		mesh.computeNormals().computeTangents();
		return mesh;
	}

	void CheckOpenGrid(const Mesh3D& mesh, int32 width, int32 height)
	{
		CheckGeometry(mesh);
		Check(mesh.vertexCount() == static_cast<size_t>(width) * height, U"Unexpected vertex count");
		Check(mesh.triangleCount() == static_cast<size_t>(width - 1) * (height - 1) * 2, U"Unexpected triangle count");
		std::map<std::pair<uint32, uint32>, std::pair<int32, int32>> edges;
		for (const auto t : mesh.indices)
		{
			for (const auto [a, b] : { std::pair{ t.i0, t.i1 }, std::pair{ t.i1, t.i2 }, std::pair{ t.i2, t.i0 } })
			{
				auto& [count, direction] = edges[{ Min(a, b), Max(a, b) }];
				++count;
				direction += (a < b ? 1 : -1);
			}
		}
		int32 boundary = 0;
		for (const auto& [edge, incidence] : edges)
		{
			const auto [count, direction] = incidence;
			Check(count == 1 || (count == 2 && direction == 0), U"Invalid grid edge incidence");
			boundary += (count == 1);
		}
		Check(boundary == 2 * (width - 1 + height - 1), U"Unexpected open boundary");
		Check(mesh.vertices.front().tex == Float2{ 0, 0 } && mesh.vertices.back().tex == Float2{ 1, 1 }, U"UV endpoint mismatch");
	}

	void ExportSurfaces()
	{
		Grid<Vec3> plane(2, 2);
		plane[0][0] = Vec3{ 0, 0, 0 }; plane[0][1] = Vec3{ 1, 0, 0 };
		plane[1][0] = Vec3{ 0, 0, -1 }; plane[1][1] = Vec3{ 1, 0, -1 };
		const auto flat = MakeOpenSurface(plane);
		CheckOpenGrid(flat, 2, 2);
		for (const auto& vertex : flat.vertices)
		{
			Check(vertex.normal.epsilonEquals(Float3{ 0, 1, 0 }, 1e-5f), U"Plane front should be +Y");
		}
		bool rejected = false;
		try { (void)MakeOpenSurface(Grid<Vec3>(1, 2)); }
		catch (const Error&) { rejected = true; }
		Check(rejected, U"One-column surface should fail");

		Grid<Vec3> cloth(25, 17);
		for (int32 y = 0; y < cloth.height(); ++y)
		{
			for (int32 x = 0; x < cloth.width(); ++x)
			{
				const double u = x / 24.0, v = y / 16.0;
				cloth[y][x] = Vec3{ 4 * u - 2, 0.4 * std::sin(Math::TwoPi * u) * std::sin(Math::Pi * v), 1.5 - 3 * v };
			}
		}
		const auto fabric = MakeOpenSurface(cloth);
		CheckOpenGrid(fabric, 25, 17);
		Check(fabric.saveOBJ(U"modeling_studies/cloth.obj", Material{ .name = U"cloth", .baseColor = ColorF{ 0.9, 0.75, 0.3 } }), U"Cloth export failed");

		Grid<Vec3> skirt(33, 2);
		for (int32 x = 0; x < skirt.width(); ++x)
		{
			const double z = -2.0 + x / 8.0;
			skirt[0][x] = Vec3{ 0, 1.5 + 0.3 * std::sin(2 * z), z };
			skirt[1][x] = Vec3{ 0, 0, z };
		}
		const auto wall = MakeOpenSurface(skirt);
		CheckOpenGrid(wall, 33, 2);
		for (size_t x = 0; x < 33; ++x)
		{
			Check(wall.vertices[x].pos == Float3{ skirt[0][x] }, U"Skirt top moved");
			Check(wall.vertices[x].normal.epsilonEquals(Float3{ 1, 0, 0 }, 1e-5f), U"Wall front should be +X");
		}
		Check(wall.saveOBJ(U"modeling_studies/skirt.obj", Material{ .name = U"earth", .baseColor = ColorF{ 0.4, 0.27, 0.16 } }), U"Skirt export failed");
	}
}

void Main()
{
	Study::ExportSupport(1.0, 0.2, 0);
	Study::ExportSupport(2.5, 0.5, 1);
	Study::ExportSupport(4.0, 0.8, 2);
	Study::ExportPipe(45_deg, 4, 0);
	Study::ExportPipe(100_deg, 12, 1);
	Study::ExportPipe(150_deg, 20, 2);
	Study::ExportSurfaces();
}
```
