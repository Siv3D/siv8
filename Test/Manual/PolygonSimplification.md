# Polygon simplification with holes

Compare the original and simplified polygons while changing the distance
threshold. This checks small holes, a hole close to an outer protrusion, and
neighboring holes. No external assets or output files are required.

## Execution

1. Paste the complete code below into a separate application's `Main.cpp` built
   with this Siv3D revision. Keep the repository test entry points intact.
2. Run on Windows (D3D11) or macOS (Metal).
3. Press Space to select a shape. Use Left and Right to change the threshold.
4. Compare the blue original on the left with the orange result on the right.
   The threshold is measured in the original coordinates, before display scaling.

## Expected results

- Every input and result has the same number of holes. The original and result
  remain nonempty, including at thresholds larger than the shape.
- The validity label stays `valid`. Hole interiors stay unfilled; no stray
  triangles cover holes or extend across the outer boundary.
- Small holes may retain their original outlines while larger outlines lose
  vertices. A larger threshold does not guarantee fewer vertices: a conflicting
  simplification is rejected for that ring.
- For `Neighboring holes` at distance 16, the outer boundary loses its collinear
  vertices while the two holes retain their outlines.
- A result may equal the input when no valid simplification can be accepted.

The automated regression cases are in [Test_Polygon.cpp](../Test_Polygon.cpp).

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>

namespace
{
	struct Sample
	{
		String name;
		Polygon polygon;
		double displayScale;
	};

	Array<Sample> MakeSamples()
	{
		const Array<Vec2> largeOuter{
			{ 0, 0 }, { 50, 0 }, { 100, 0 }, { 100, 50 },
			{ 100, 100 }, { 50, 100 }, { 0, 100 }, { 0, 50 }
		};
		const Array<Vec2> outer{
			{ 0, 0 }, { 20, 0 }, { 40, 0 }, { 40, 20 },
			{ 40, 40 }, { 20, 40 }, { 0, 40 }, { 0, 20 }
		};
		const Array<Vec2> cShape{
			{ 4, 4 }, { 28, 4 }, { 28, 8 }, { 8, 8 },
			{ 8, 24 }, { 28, 24 }, { 28, 28 }, { 4, 28 }
		};
		return {
			{ U"Small holes", Polygon{ largeOuter, Array<Array<Vec2>>{
				{ { 10, 10 }, { 10, 11 }, { 11, 11 }, { 11, 10 } },
				{ { 30, 30 }, { 30, 50 }, { 50, 50 }, { 50, 30 } }
			} }, 4.0 },
			{ U"Hole near the outer boundary", Polygon{
				Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 6 }, { 26, 6 },
					{ 26, 14 }, { 20, 14 }, { 20, 20 }, { 0, 20 } },
				Array<Array<Vec2>>{ { { 21, 8 }, { 21, 12 }, { 24, 12 }, { 24, 8 } } }
			}, 14.0 },
			{ U"Neighboring holes", Polygon{ outer, Array<Array<Vec2>>{
				cShape.reversed(), { { 16, 12 }, { 16, 18 }, { 20, 18 }, { 20, 12 } }
			} }, 10.0 }
		};
	}

	size_t VertexCount(const Polygon& polygon)
	{
		size_t count = polygon.outer().size();
		for (const auto& hole : polygon.inners())
		{
			count += hole.size();
		}
		return count;
	}
}

void Main()
{
	Window::Resize(1200, 720);
	Scene::SetBackground(ColorF{ 0.12, 0.14, 0.18 });
	const Font font{ 20 };
	const auto samples = MakeSamples();
	const Array<double> distances{ 0, 0.5, 2, 4, 8, 16, 64, 100 };
	size_t sampleIndex = 0, distanceIndex = 0;
	bool dirty = true, valid = false;
	Polygon simplified, left, right;

	while (System::Update())
	{
		if (KeySpace.down())
		{
			sampleIndex = ((sampleIndex + 1) % samples.size());
			dirty = true;
		}
		if (KeyRight.down())
		{
			distanceIndex = ((distanceIndex + 1) % distances.size());
			dirty = true;
		}
		if (KeyLeft.down())
		{
			distanceIndex = ((distanceIndex + distances.size() - 1) % distances.size());
			dirty = true;
		}
		const auto& sample = samples[sampleIndex];
		if (dirty)
		{
			simplified = sample.polygon.simplified(distances[distanceIndex]);
			valid = simplified && (simplified.holeCount() == sample.polygon.holeCount())
				&& (Polygon::Validate(simplified.outer(), simplified.inners()) == PolygonFailureType::Ok);
			left = sample.polygon.scaledFromOrigin(sample.displayScale).movedBy(70, 200);
			right = simplified.scaledFromOrigin(sample.displayScale).movedBy(670, 200);
			dirty = false;
		}
		font(sample.name).draw(30, 20);
		font(U"Space: shape   Left / Right: threshold").draw(30, 55);
		font(Format(U"maxDistance: ", distances[distanceIndex],
			U"   holes: ", sample.polygon.holeCount(), U" -> ", simplified.holeCount(),
			U"   vertices: ", VertexCount(sample.polygon), U" -> ", VertexCount(simplified))).draw(30, 90);
		font(valid ? U"valid" : U"INVALID").draw(30, 125, valid ? Palette::Limegreen : Palette::Red);
		left.draw(ColorF{ 0.25, 0.55, 0.85 }).drawFrame(1.0, Palette::White);
		right.draw(ColorF{ 0.95, 0.65, 0.25 }).drawFrame(1.0, Palette::White);
		font(U"Original").draw(70, 650);
		font(U"Simplified").draw(670, 650);
	}
}
```
