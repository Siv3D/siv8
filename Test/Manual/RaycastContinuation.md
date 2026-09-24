# Raycast continuation diagnostic

This observes reflection and sliding with the current API. It is not a collision
response implementation or a regression specification. See the
[continuation guide](../../docs/geometry2d/README.md#continuing-a-ray-after-contact)
for choosing a search range and interpreting the limitations.
For a complete movement policy in a restricted scene, use the
[reflection and sliding example](RaycastResponse.md).

## Execution

1. Paste the complete code below into a separate Siv3D application's `Main.cpp`
   built with the revision under review. Keep the repository platform entry points
   unchanged. No external assets are required.
2. On Windows, read the console opened by the program. On macOS, launch the app's
   executable from Terminal (or use Xcode's debug console) to see standard output.
   Redirect stdout to a caller-chosen file if a retained report is useful.
3. The program prints focused cases, then a CSV matrix. Close the application
   window when finished. There are no automatic output files or timing claims.

## Expected observations

| Case | Current behavior to inspect |
|---|---|
| Outgoing, inward, and slide at the rectangle boundary | Distance zero. |
| Hole reflection | Zero before displacement; after a small rightward step, the opposite wall is at x = 7. The concave example reaches the same x. |
| Hole travel, strategy 0 / 2 | Unchanged position and approximately 8 units remaining when the 8-call cap is reached, including the one-representable-step strategy. |
| Hole travel, strategy 3 | Completes the same path without moving the origin between queries, using the range overload. |
| Hole travel, strategy 1 | Completes the 10-unit path near (3, 5). The displaced step is charged to the budget; this is not safe for arbitrary geometry. |
| Thin member | A step of 1e-6 jumps over the member at x in [1e-7, 2e-7]; a step of 1e-8 retains its entry hit. |
| Container normal offset | Outward offset re-hits the nearby entry; inward offset reaches the far side near x = -5. |
| Circle tangent | A direction offset of 1e-8 still returns zero; a normal offset of that size leaves the circle. |
| Corner and point contact | A single-normal reflection can leave the rectangle interior or enter the other member. A returned unit normal is not a complete collision response. |
| Returned-position restart versus exact-axis start | Do not require identical results. A numerical hit can lie slightly off the boundary; inspect zero, tiny positive, distant, and absent hits separately. |

The matrix covers all supported shape types, with several Polygon/MultiPolygon
layouts and ten SuperEllipse exponents. Every geometry is tested at three sizes
and three positions. Rect uses its integer unit-scale case. Polygon rings are
constructed before transformation to avoid conflating construction validity with
query behavior.

- Mode 0: reflect an incoming ray from outside; mode 1: reflect a ray starting
  near the bounding-box center (which can be outside a concave or holed target);
  mode 2: continue along a tangent to the returned normal.
- Strategy 0: unchanged hit position; 1: advance 1e-6 along the new direction;
  2: advance `scale * 1e-8`; 3: advance each moving coordinate one representable
  step; 4: advance `1e-12 * max(1, scale, abs(x), abs(y))`; 5: retain the
  origin and search from `scale * 1e-8` using the range overload.
- Each mode/strategy uses 64 oblique directions and three transverse offsets.
  Counts describe a second query after each successful first hit. `near` means a
  positive distance no greater than `scale * 1e-7`; this is a diagnostic bucket,
  not an API tolerance. Rows must satisfy `initial_hits = none + zero + near + far`.
- `none` can mean a correct departure or a missed obstacle; `far` can mean the
  desired next boundary or a skipped thin region. Counts alone are not error rates.
  Tiny shapes at large translations intentionally stress coordinate precision.
- The matrix compares independent queries, not a fixed travel budget. Only the
  hole travel loop demonstrates budget accounting. Function wrappers, shape
  copies, and printing make this unsuitable as a performance benchmark.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <algorithm>
# include <array>
# include <cmath>
# include <cstdio>
# include <functional>
# include <string>
# include <vector>

namespace RaycastContinuation
{
	using namespace s3d;

	struct Target
	{
		std::string name;
		std::function<Optional<RaycastHit2D>(const Ray2D&, double)> cast;
		std::function<Optional<RaycastHit2D>(const Ray2D&, double, double)> castRange;
	};

	template <class Shape>
	Target Make(std::string name, const Shape& shape)
	{
		return { std::move(name), [shape](const Ray2D& ray, double limit)
			{ return Geometry2D::Raycast(ray, shape, limit); },
			[shape](const Ray2D& ray, double minimum, double limit)
			{ return Geometry2D::Raycast(ray, shape, minimum, limit); } };
	}

	std::vector<Target> Shapes(double scale, Vec2 center)
	{
		const auto point = [&](double x, double y) { return center + Vec2{ x, y } * scale; };
		const auto rect = [&](double x, double y, double w, double h)
			{ return RectF{ point(x, y), w * scale, h * scale }; };
		// Construct valid rings before scaling; construction is outside the queries.
		const auto polygon = [&](double x, double y, double w, double h)
			{ return RectF{ x, y, w, h }.asPolygon().scaledFromOrigin(scale).movedBy(center); };
		std::vector<Target> shapes;
		if ((scale == 1) && (center == Vec2{ 0, 0 }))
		{
			shapes.push_back(Make("Rect", Rect{ -5, -3, 10, 6 }));
		}
		shapes.push_back(Make("RectF", rect(-5, -3, 10, 6)));
		shapes.push_back(Make("Circle", Circle{ center, 5 * scale }));
		shapes.push_back(Make("Ellipse", Ellipse{ center, 5 * scale, 3 * scale }));
		for (double n : { 0.25, 0.5, 0.9, 1.0, 1.1, 1.5, 2.0, 4.0, 16.0, 64.0 })
		{
			shapes.push_back(Make("SuperEllipse-" + std::to_string(n),
				SuperEllipse{ center, SizeF{ 5 * scale, 3 * scale }, n }));
		}
		shapes.push_back(Make("Triangle", Triangle{ point(-5, -3), point(5, -3), point(0, 6) }));
		shapes.push_back(Make("Quad", Quad{ point(-5, -3), point(4, -3), point(5, 3), point(-4, 3) }));
		shapes.push_back(Make("RoundRect", RoundRect{ rect(-5, -3, 10, 6), 2 * scale }));
		shapes.push_back(Make("Polygon", polygon(-5, -3, 10, 6)));
		const Polygon concave{ Array<Vec2>{
			{ -5, -3 }, { 5, -3 }, { 5, 3 }, { 2, 3 }, { 2, -1 }, { -2, -1 }, { -2, 3 }, { -5, 3 } } };
		shapes.push_back(Make("Concave", concave.scaledFromOrigin(scale).movedBy(center)));
		const Polygon hole{ Array<Vec2>{ { -5, -3 }, { 5, -3 }, { 5, 3 }, { -5, 3 } },
			Array<Array<Vec2>>{ { { -2, -1 }, { -2, 1 }, { 2, 1 }, { 2, -1 } } } };
		shapes.push_back(Make("Hole", hole.scaledFromOrigin(scale).movedBy(center)));
		shapes.push_back(Make("Multi", MultiPolygon{ polygon(-5, -3, 4, 6), polygon(1, -3, 4, 6) }));
		shapes.push_back(Make("PointTouch", MultiPolygon{ polygon(-5, -3, 5, 3), polygon(0, 0, 5, 3) }));
		return shapes;
	}

	Vec2 NextRepresentable(Vec2 point, Vec2 direction)
	{
		const auto next = [](double p, double d)
			{ return (d == 0) ? p : std::nextafter(p, std::copysign(Math::Inf, d)); };
		return { next(point.x, direction.x), next(point.y, direction.y) };
	}

	void Record(const char* label, const Target& target, const Ray2D& ray, double limit = 100)
	{
		const auto hit = target.cast(ray, limit);
		if (hit)
		{
			std::printf("%s: distance=%.17g position=(%.17g,%.17g) normal=(%.17g,%.17g) inside=%d\n",
				label, hit->distance, hit->position.x, hit->position.y,
				hit->normal.x, hit->normal.y, hit->startsInside);
		}
		else
		{
			std::printf("%s: none\n", label);
		}
	}

	void Scenarios()
	{
		const auto rect = Make("rect", RectF{ 0, 0, 10, 10 });
		Record("outgoing", rect, { { 0, 5 }, { -1, 0 } });
		Record("inward", rect, { { 0, 5 }, { 1, 0 } });
		Record("slide", rect, { { 0, 5 }, { 0, 1 } });
		Record("corner from inside", rect, { { 5, 5 }, { 1, 1 } });
		Record("corner single-normal reflection", rect, { { 10, 10 }, { -1, 1 } });
		Record("corner after step", rect, { { 10 - 1e-6, 10 + 1e-6 }, { -1, 1 } });

		const auto hole = Make("hole", Polygon{
			Array<Vec2>{ { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } },
			Array<Array<Vec2>>{ { { 3, 3 }, { 3, 7 }, { 7, 7 }, { 7, 3 } } } });
		Record("hole first", hole, { { 5, 5 }, { -1, 0 } });
		Record("hole reflection", hole, { { 3, 5 }, { 1, 0 } });
		Record("hole next wall after step", hole, { { 3 + 1e-6, 5 }, { 1, 0 } });
		const auto concave = Make("concave", Polygon{ Array<Vec2>{
			{ 0, 0 }, { 10, 0 }, { 10, 10 }, { 7, 10 }, { 7, 3 }, { 3, 3 }, { 3, 10 }, { 0, 10 } } });
		Record("concave next wall after step", concave, { { 3 + 1e-6, 5 }, { 1, 0 } });

		const auto thin = Make("thin", MultiPolygon{
			RectF{ -10, -1, 10, 2 }.asPolygon(), RectF{ 1e-7, -1, 1e-7, 2 }.asPolygon() });
		Record("thin from boundary", thin, { { 0, 0 }, { 1, 0 } });
		Record("thin skipped by step", thin, { { 1e-6, 0 }, { 1, 0 } });
		Record("thin retained by smaller step", thin, { { 1e-8, 0 }, { 1, 0 } });
		const auto touch = Make("touch", MultiPolygon{
			RectF{ 0, 0, 2, 2 }.asPolygon(), RectF{ 2, 2, 2, 2 }.asPolygon() });
		Record("point contact incoming", touch, { { 3, 1 }, { -1, 1 } });
		Record("point contact reflected", touch, { { 2, 2 }, { 1, 1 } });
		Record("point contact after step", touch, { { 2 + 1e-6, 2 + 1e-6 }, { 1, 1 } });

		const auto circle = Make("circle", Circle{ Vec2{ 0, 0 }, 5 });
		Record("container first", circle, { { 0, 0 }, { 1, 0 } });
		Record("container outward offset", circle, { { 5 + 1e-6, 0 }, { -1, 0 } });
		Record("container inward offset", circle, { { 5 - 1e-6, 0 }, { -1, 0 } });
		Record("tangent direction offset", circle, { { 1e-8, -5 }, { 1, 0 } });
		Record("tangent normal offset", circle, { { 0, -5 - 1e-8 }, { 1, 0 } });

		for (int strategy = 0; strategy < 4; ++strategy)
		{
			Vec2 position{ 5, 5 }, direction{ -1, 0 };
			double remaining = 10;
			int calls = 0;
			while ((calls < 8) && (0 < remaining))
			{
				++calls;
				const auto hit = (strategy == 3)
					? hole.castRange({ position, direction }, (calls == 1) ? 0.0 : 0.001, remaining)
					: hole.cast({ position, direction }, remaining);
				if (not hit)
				{
					position += direction * remaining;
					remaining = 0;
					break;
				}
				remaining -= hit->distance;
				position = hit->position;
				direction -= 2 * direction.dot(hit->normal) * hit->normal;
				if (strategy == 1)
				{
					const double step = Min(1e-6, remaining);
					position += direction * step;
					remaining -= step;
				}
				if (strategy == 2)
				{
					const Vec2 next = NextRepresentable(position, direction);
					const double step = next.distanceFrom(position);
					if (step <= remaining) { position = next; remaining -= step; }
				}
			}
			std::printf("hole travel strategy=%d calls=%d remaining=%.17g position=(%.17g,%.17g)\n",
				strategy, calls, remaining, position.x, position.y);
		}

		for (const auto& target : Shapes(1, { 0, 0 }))
		{
			const auto hit = target.cast({ { -20, 0 }, { 1, 0 } }, 100);
			if (not hit) { continue; }
			for (Vec2 direction : { hit->normal, -hit->normal, Vec2{ -hit->normal.y, hit->normal.x } })
			{
				Record((target.name + " returned-position restart").c_str(), target, { hit->position, direction });
			}
		}
		for (double n : { 0.25, 0.5, 0.9, 1.0, 1.1, 1.5, 2.0, 4.0, 16.0, 64.0 })
		{
			const auto target = Make("SuperEllipse", SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, n });
			Record("exact-axis outward", target, { { -5, 0 }, { -1, 0 } });
			Record("exact-axis tangent", target, { { -5, 0 }, { 0, 1 } });
		}
	}

	void Matrix()
	{
		std::puts("shape,scale,translation,mode,strategy,initial_hits,none,zero,near,far");
		for (double scale : { 1e-6, 1.0, 1e6 })
		for (Vec2 center : { Vec2{ 0, 0 }, Vec2{ 1e4, -2e4 }, Vec2{ 1e8, -2e8 } })
		for (const auto& target : Shapes(scale, center))
		for (int mode = 0; mode < 3; ++mode)
		for (int strategy = 0; strategy < 6; ++strategy)
		{
			std::array<int, 5> counts{};
			for (int i = 0; i < 64; ++i)
			for (double shift : { -2.0, 0.0, 2.0 })
			{
				const double angle = Math::TwoPi * (i + 0.137) / 64;
				const Vec2 u{ std::cos(angle), std::sin(angle) }, v{ -u.y, u.x };
				const Ray2D ray{ (mode == 1) ? center + v * (shift * 0.1 * scale)
					: center + u * (20 * scale) + v * (shift * scale), (mode == 1) ? u : -u };
				const auto hit = target.cast(ray, 50 * scale);
				if (not hit) { continue; }
				++counts[0];
				const Vec2 direction = (mode == 2) ? Vec2{ -hit->normal.y, hit->normal.x }
					: ray.direction - 2 * ray.direction.dot(hit->normal) * hit->normal;
				Vec2 origin = hit->position;
				if (strategy == 1) { origin += direction * 1e-6; }
				if (strategy == 2) { origin += direction * (scale * 1e-8); }
				if (strategy == 3) { origin = NextRepresentable(origin, direction); }
				if (strategy == 4)
				{
					origin += direction * (std::max({ 1.0, scale, Abs(origin.x), Abs(origin.y) }) * 1e-12);
				}
				const auto next = (strategy == 5)
					? target.castRange({ origin, direction }, scale * 1e-8, 50 * scale)
					: target.cast({ origin, direction }, 50 * scale);
				if (not next) { ++counts[1]; }
				else if (next->distance == 0) { ++counts[2]; }
				else if (next->distance <= scale * 1e-7) { ++counts[3]; }
				else { ++counts[4]; }
			}
			std::printf("%s,%.0e,%.0e,%d,%d,%d,%d,%d,%d,%d\n", target.name.c_str(),
				scale, center.x, mode, strategy, counts[0], counts[1], counts[2], counts[3], counts[4]);
		}
	}
}

void Main()
{
	Console.open();
	RaycastContinuation::Scenarios();
	RaycastContinuation::Matrix();
	std::fflush(stdout);
	Print << U"Raycast diagnostic finished. See standard output; close this window to exit.";
	while (System::Update()) {}
}
```
