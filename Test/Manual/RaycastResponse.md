# Raycast reflection and sliding

A point moves among stationary, axis-aligned rectangular walls. This interactive
example chooses a contact response, collects simultaneous contacts, and bounds the
work per movement update. See the [usage guide](../../docs/geometry2d/README.md)
for the distinction between a boundary query and movement response. The separate
[continuation diagnostic](RaycastContinuation.md) inspects the query APIs across
shape types and numerical scales.

## Execution

1. Paste the complete code below into a separate Siv3D application's `Main.cpp`.
   Keep this repository's platform entry points unchanged. No assets or output
   files are needed.
2. Run on macOS (Metal) or Windows (D3D11). Space selects a case; M switches
   reflection/sliding; Left/Right changes the simulated duration; R reverses wall
   order. Each change recomputes the entire displayed trajectory.
3. Green marks the start, orange the final position, and cyan the path. The circles
   are display markers: the moving object has **zero radius**.
4. Inspect the result label, unused time, contact count, and `Checks passed` label.
   The built-in checks compare expected positions, wall order, and subdivision of
   completed motion into ten updates.

## Chosen policy and scope

- Wall interiors are forbidden; their boundaries are passable. A tangent or
  departing ray is ignored for that rectangle only. This relies on its convexity
  and the straight path until the next response; do not extend that exclusion to
  a concave Polygon or a whole MultiPolygon.
- Each query includes distance zero. At a hit, rectangle coordinates identify
  every incident face, including both faces of a corner. Contacts at the nearest
  distance are merged across walls. A single returned normal is insufficient for
  this policy.
- Reflection reverses all blocked velocity components. Sliding sets those
  components to zero without renormalizing the velocity. Subtract `distance / speed`
  from the remaining time before applying the response, then query again.
- `ContactTolerance` is for this roughly 10-unit scene, whose thinnest wall is
  0.02 units. It groups nearly simultaneous contacts and snaps hit coordinates to
  the known wall planes; it does not advance the origin past a wall. Inputs must
  have finite coordinates/velocities, positive rectangle dimensions comfortably
  larger than this tolerance, and a finite nonnegative duration.
- At eight contacts, keep the last contact position, set velocity to zero, and
  discard the unused movement for this update. A turn blocked at a touching
  vertex can reach this limit without advancing time. There is no attempt to find
  an alternative escape direction or carry a backlog into the next update.
- Starting inside a wall reports `InvalidStart` and leaves the position unchanged.
  Teleport recovery, moving walls, rotated/curved walls, and finite-radius bodies
  need their own handling. The independent wall strips overlap at room corners;
  they are stored in an `Array<RectF>`.

`Move()` writes into reusable storage, uses a fixed path buffer, and performs at
most eight Raycast passes over the walls. The viewer prepares its wall list and reruns the
query only when the controls change. When calling `Move()` every frame, pass the
previous result's position and velocity unless the application intentionally
supplies a new movement command.

## Expected results

At the default duration multiplier of 1:

| Case | Response | Expected result |
|---|---|---|
| Two-wall corner | Reflect | One simultaneous contact at (10, 6), then finish at (7, 4.2) with velocity (-5, -3). Switching to Slide stops at (10, 6). |
| Slide into a wall | Slide | Finish at (10, 5) with velocity (0, 2); the remaining tangential motion is retained. |
| Start tangent to a wall | Slide | Finish at (10, 5) without a blocking contact. |
| Pass a touching vertex | Reflect | Pass through (5, 3) and finish at (4, 4) without entering either wall. This is specific to point motion. |
| Blocked turn at a touching vertex | Reflect | Stop at (5, 3) at the contact limit, with all 1 second unused; never move into either wall. |
| Thin wall at high speed | Reflect | Four contacts, finish at (0.4, 3); no crossing of the wall at x = 4. |
| Contact budget | Reflect | Stop at (0, 3) after eight contacts, with 0.625 seconds unused. |
| Invalid start inside a wall | Slide | Report `InvalidStart`; no automatic relocation. |

Reversing wall order must preserve the result. A stopped/limited update is an
explicit incomplete movement, not a promise of frame-rate-independent physics.

## Complete Main.cpp

```cpp
# include <Siv3D.hpp>
# include <array>
# include <span>

namespace RaycastResponse
{
	using namespace s3d;
	constexpr double ContactTolerance = 1e-9; // For this roughly 10-unit scene.
	constexpr size_t MaxContacts = 8;
	enum class Response { Reflect, Slide };
	enum class Stop { Finished, Blocked, ContactLimit, InvalidStart };

	struct MotionResult
	{
		Vec2 position, velocity;
		double unusedTime = 0;
		size_t contacts = 0, pointCount = 0;
		std::array<Vec2, MaxContacts + 2> path{};
		Stop stop = Stop::Finished;
	};

	bool Inside(Vec2 p, const RectF& r)
	{
		return ((r.x + ContactTolerance < p.x) && (p.x < r.x + r.w - ContactTolerance)
			&& (r.y + ContactTolerance < p.y) && (p.y < r.y + r.h - ContactTolerance));
	}

	// A point enters a convex rectangle only if it moves inward at every
	// incident face. Departures and motion along a face remain passable.
	uint32 EntryAxes(const RectF& r, Vec2 p, Vec2 velocity)
	{
		uint32 axes = 0;
		if (Abs(p.x - r.x) <= ContactTolerance)
		{
			if (velocity.x <= 0) { return 0; }
			axes |= 1;
		}
		if (Abs(p.x - (r.x + r.w)) <= ContactTolerance)
		{
			if (0 <= velocity.x) { return 0; }
			axes |= 1;
		}
		if (Abs(p.y - r.y) <= ContactTolerance)
		{
			if (velocity.y <= 0) { return 0; }
			axes |= 2;
		}
		if (Abs(p.y - (r.y + r.h)) <= ContactTolerance)
		{
			if (0 <= velocity.y) { return 0; }
			axes |= 2;
		}
		return axes;
	}

	void Move(std::span<const RectF> walls, Vec2 position, Vec2 velocity,
		double seconds, Response response, MotionResult& result)
	{
		result = {};
		result.position = position;
		result.velocity = velocity;
		result.path[result.pointCount++] = position;
		for (const auto& wall : walls)
		{
			if (Inside(position, wall))
			{
				result.stop = Stop::InvalidStart;
				result.velocity = Vec2{ 0, 0 };
				result.unusedTime = seconds;
				return;
			}
		}
		while (0 < seconds)
		{
			const double speed = result.velocity.length();
			if (speed == 0) { result.stop = Stop::Blocked; break; }
			const Ray2D ray{ result.position, result.velocity };
			const double distanceLimit = speed * seconds;
			double nearest = Math::Inf;
			uint32 axes = 0;
			Vec2 planes{ 0, 0 };
			for (const auto& wall : walls)
			{
				// Include zero: a different wall at this position may block the turn.
				const auto hit = Geometry2D::Raycast(ray, wall, distanceLimit);
				if (not hit) { continue; }
				const uint32 entry = EntryAxes(wall, hit->position, result.velocity);
				if ((entry == 0) || (nearest + ContactTolerance < hit->distance)) { continue; }
				if (hit->distance + ContactTolerance < nearest) { axes = 0; }
				nearest = Min(nearest, hit->distance);
				const Vec2 face{
					(0 < result.velocity.x) ? wall.x : wall.x + wall.w,
					(0 < result.velocity.y) ? wall.y : wall.y + wall.h };
				for (int axis = 0; axis < 2; ++axis)
				{
					const uint32 bit = (1u << axis);
					if (not (entry & bit)) { continue; }
					double& plane = (axis == 0) ? planes.x : planes.y;
					const double candidate = (axis == 0) ? face.x : face.y;
					const double component = (axis == 0) ? result.velocity.x : result.velocity.y;
					plane = (axes & bit) ? ((0 < component) ? Min(plane, candidate) : Max(plane, candidate)) : candidate;
				}
				axes |= entry;
			}
			if (axes == 0)
			{
				result.position += result.velocity * seconds;
				result.path[result.pointCount++] = result.position;
				seconds = 0;
				break;
			}
			result.position = ray.pointAt(nearest);
			// Retain exact wall coordinates so the next tangent query stays on it.
			if (axes & 1) { result.position.x = planes.x; }
			if (axes & 2) { result.position.y = planes.y; }
			result.path[result.pointCount++] = result.position;
			seconds = Max(0.0, seconds - nearest / speed);
			if (axes & 1) { result.velocity.x = (response == Response::Reflect) ? -result.velocity.x : 0.0; }
			if (axes & 2) { result.velocity.y = (response == Response::Reflect) ? -result.velocity.y : 0.0; }
			++result.contacts;
			if ((result.contacts == MaxContacts) && (0 < seconds))
			{
				result.stop = Stop::ContactLimit;
				result.velocity = Vec2{ 0, 0 };
				break;
			}
		}
		result.unusedTime = seconds;
	}

	Array<RectF> Room()
	{
		return { RectF{ -1, -1, 1, 8 }, RectF{ 10, -1, 1, 8 },
			RectF{ -1, -1, 12, 1 }, RectF{ -1, 6, 12, 1 } };
	}

	struct Sample
	{
		String name;
		Array<RectF> walls;
		Vec2 start, velocity;
		double seconds;
		Response response;
	};

	Array<Sample> Samples()
	{
		auto touching = Room();
		touching.push_back(RectF{ 3, 1, 2, 2 });
		touching.push_back(RectF{ 5, 3, 2, 2 });
		auto thin = Room();
		thin.push_back(RectF{ 4, 1, 0.02, 4 });
		return {
			{ U"Two-wall corner", Room(), { 5, 3 }, { 5, 3 }, 1.6, Response::Reflect },
			{ U"Slide into a wall", Room(), { 8, 1 }, { 4, 2 }, 2, Response::Slide },
			{ U"Start tangent to a wall", Room(), { 10, 1 }, { 0, 2 }, 2, Response::Slide },
			{ U"Pass a touching vertex", touching, { 6, 2 }, { -2, 2 }, 1, Response::Reflect },
			{ U"Blocked turn at a touching vertex", touching, { 5, 3 }, { 2, 1 }, 1, Response::Reflect },
			{ U"Thin wall at high speed", thin, { 2, 3 }, { 18, 0 }, 0.8, Response::Reflect },
			{ U"Contact budget", Room(), { 5, 3 }, { 200, 0 }, 1, Response::Reflect },
			{ U"Invalid start inside a wall", thin, { 4.01, 3 }, { 2, 0 }, 1, Response::Slide }
		};
	}

	bool Checks()
	{
		const auto samples = Samples();
		MotionResult result, reversed;
		const auto near = [](Vec2 a, Vec2 b) { return a.distanceFrom(b) < 1e-8; };
		bool ok = true;
		for (const auto& sample : samples)
		{
			Move(sample.walls, sample.start, sample.velocity, sample.seconds, sample.response, result);
			const auto walls = sample.walls.reversed();
			Move(walls, sample.start, sample.velocity, sample.seconds, sample.response, reversed);
			ok &= near(result.position, reversed.position) && near(result.velocity, reversed.velocity)
				&& (result.stop == reversed.stop) && (result.contacts == reversed.contacts);
			if (result.stop == Stop::InvalidStart) { continue; }
			for (const auto& wall : sample.walls) { ok &= not Inside(result.position, wall); }
			if (result.stop != Stop::ContactLimit)
			{
				Vec2 position = sample.start, velocity = sample.velocity;
				MotionResult part;
				for (int step = 0; step < 10; ++step)
				{
					Move(sample.walls, position, velocity, sample.seconds / 10, sample.response, part);
					position = part.position;
					velocity = part.velocity;
				}
				ok &= near(position, result.position) && near(velocity, result.velocity);
			}
		}
		Move(samples[0].walls, { 5, 3 }, { 5, 3 }, 1.6, Response::Reflect, result);
		ok &= near(result.position, { 7, 4.2 }) && near(result.velocity, { -5, -3 }) && (result.contacts == 1);
		Move(samples[0].walls, { 5, 3 }, { 5, 3 }, 1.6, Response::Slide, result);
		ok &= near(result.position, { 10, 6 }) && (result.velocity == Vec2{ 0, 0 });
		Move(samples[1].walls, { 8, 1 }, { 4, 2 }, 2, Response::Slide, result);
		ok &= near(result.position, { 10, 5 }) && near(result.velocity, { 0, 2 });
		Move(samples[2].walls, { 10, 1 }, { 0, 2 }, 2, Response::Slide, result);
		ok &= near(result.position, { 10, 5 }) && (result.contacts == 0);
		Move(samples[3].walls, { 6, 2 }, { -2, 2 }, 1, Response::Reflect, result);
		ok &= near(result.position, { 4, 4 }) && (result.contacts == 0);
		Move(samples[4].walls, { 5, 3 }, { 2, 1 }, 1, Response::Reflect, result);
		ok &= near(result.position, { 5, 3 }) && (result.stop == Stop::ContactLimit) && (result.unusedTime == 1);
		Move(samples[5].walls, { 2, 3 }, { 18, 0 }, 0.8, Response::Reflect, result);
		ok &= near(result.position, { 0.4, 3 });
		Move(samples[6].walls, { 5, 3 }, { 200, 0 }, 1, Response::Reflect, result);
		ok &= near(result.position, { 0, 3 }) && (result.stop == Stop::ContactLimit) && (Abs(result.unusedTime - 0.625) < 1e-12);
		return ok;
	}

	StringView StopName(Stop stop)
	{
		switch (stop)
		{
		case Stop::Finished: return U"Finished";
		case Stop::Blocked: return U"Blocked";
		case Stop::ContactLimit: return U"Contact limit: remaining motion stopped";
		default: return U"Invalid start: recover before moving";
		}
	}

	Vec2 Screen(Vec2 p) { return Vec2{ 130, 230 } + p * 64; }
}

void Main()
{
	using namespace RaycastResponse;
	Window::Resize(1040, 800);
	Scene::SetBackground(ColorF{ 0.10, 0.12, 0.16 });
	const Font font{ 20 };
	const auto samples = Samples();
	const bool checksPassed = Checks();
	const std::array<double, 4> timeScale{ 0.25, 0.5, 1, 2 };
	size_t index = 0, scaleIndex = 2;
	Response response = samples[index].response;
	bool reverse = false, dirty = true;
	Array<RectF> walls;
	MotionResult result;
	while (System::Update())
	{
		if (KeySpace.down()) { index = (index + 1) % samples.size(); response = samples[index].response; dirty = true; }
		if (KeyM.down()) { response = (response == Response::Reflect) ? Response::Slide : Response::Reflect; dirty = true; }
		if (KeyR.down()) { reverse = not reverse; dirty = true; }
		if (KeyLeft.down()) { scaleIndex = (scaleIndex + 3) % timeScale.size(); dirty = true; }
		if (KeyRight.down()) { scaleIndex = (scaleIndex + 1) % timeScale.size(); dirty = true; }
		const auto& sample = samples[index];
		const double seconds = sample.seconds * timeScale[scaleIndex];
		if (dirty)
		{
			walls = sample.walls;
			if (reverse) { walls.reverse(); }
			Move(walls, sample.start, sample.velocity, seconds, response, result);
			dirty = false;
		}
		font(sample.name, U" / ", (response == Response::Reflect) ? U"Reflect" : U"Slide").draw(28, 20);
		font(U"Space: case   M: response   Left/Right: duration   R: wall order").draw(28, 55);
		font(U"Point motion; circles are markers, not collision radii.").draw(28, 90, ColorF{ 0.7 });
		RectF{ Screen({ 0, 0 }), 640, 384 }.draw(ColorF{ 0.17, 0.20, 0.25 });
		for (const auto& wall : walls)
		{
			RectF{ Screen(wall.pos), wall.size * 64 }.draw(ColorF{ 0.42, 0.45, 0.52 });
		}
		for (size_t i = 1; i < result.pointCount; ++i)
		{
			Line{ Screen(result.path[i - 1]), Screen(result.path[i]) }.draw(3, ColorF{ 0.3, 0.85, 1.0 });
		}
		Circle{ Screen(sample.start), 6 }.draw(ColorF{ 0.4, 1.0, 0.5 });
		Circle{ Screen(result.position), 5 }.draw(ColorF{ 1.0, 0.65, 0.25 });
		font(StopName(result.stop)).draw(28, 704);
		font(U"Duration: ", ToFixed(seconds, 3), U"   Unused time: ", ToFixed(result.unusedTime, 3),
			U"   Contacts: ", result.contacts, U" / ", MaxContacts).draw(28, 736);
		font(checksPassed ? U"Checks passed" : U"CHECKS FAILED").draw(810, 704,
			checksPassed ? ColorF{ 0.4, 1.0, 0.5 } : ColorF{ 1.0, 0.3, 0.3 });
	}
}
```
