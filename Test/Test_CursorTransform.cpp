//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Cursor/CursorState.hpp"

TEST_CASE("CursorTransform.composition_and_round_trip")
{
	CursorTransform transform;
	CHECK(transform.all == Mat3x2::Identity());
	CHECK(transform.allInv == Mat3x2::Identity());

	// (3, -2) -> (10, -11) -> (21, 30) -> (72, 100).
	const Mat3x2 local = Mat3x2::Scale(2, 3).translated(4, -5);
	const Mat3x2 camera{ 0, 1, -1, 0, 10, 20 };
	REQUIRE(transform.setLocal(local));
	REQUIRE(transform.setCamera(camera));
	REQUIRE(transform.setBaseWindow({ 2.0, RectF{ 30, 40, 800, 600 } }));
	CHECK(transform.all.transformPoint(Vec2{ 3, -2 }) == Vec2{ 72, 100 });
	CHECK(transform.allInv.transformPoint(Vec2{ 72, 100 }).epsilonEquals(Vec2{ 3, -2 }, 1e-5));

	for (const Vec2 p : { Vec2{ 0, 0 }, Vec2{ -12.5, 20.25 }, Vec2{ 400, -300 } })
	{
		CHECK(transform.allInv.transformPoint(transform.all.transformPoint(p)).epsilonEquals(p, 1e-4));
	}

	CursorTransform reverseSetup;
	reverseSetup.setBaseWindow({ 2.0, RectF{ 30, 40, 800, 600 } });
	reverseSetup.setCamera(camera);
	reverseSetup.setLocal(local);
	CHECK(reverseSetup.all == transform.all);
	CHECK(reverseSetup.allInv == transform.allInv);

	CHECK_FALSE(transform.setLocal(local));
	CHECK_FALSE(transform.setCamera(camera));
	// Only the scale and origin affect the mapping.
	CHECK_FALSE(transform.setBaseWindow({ 2.0, RectF{ 30, 40, 1600, 1200 } }));
	CHECK(transform.all == reverseSetup.all);
	CHECK(transform.allInv == reverseSetup.allInv);
}

TEST_CASE("CursorTransform.integer_delta_uses_integer_endpoints")
{
	CursorTransform transform;
	transform.setLocal(Mat3x2::Scale(4));
	CursorState state;
	state.advanceRaw(Point{ 103, 197 }, Point{ 3, -3 });
	state.refreshTransformed(transform);
	state.advanceRaw(Point{ 105, 195 }, Point{ 5, -5 });
	state.refreshTransformed(transform);

	CHECK(state.vec2.previous == Vec2{ 0.75, -0.75 });
	CHECK(state.vec2.current == Vec2{ 1.25, -1.25 });
	CHECK(state.vec2.delta == Vec2{ 0.5, -0.5 });
	CHECK(state.point.previous == Point{ 0, 0 });
	CHECK(state.point.current == Point{ 1, -1 });
	CHECK(state.point.delta == Point{ 1, -1 });
	CHECK(state.point.delta == (state.point.current - state.point.previous));
	CHECK(state.raw.delta == Point{ 2, -2 });
	CHECK(state.screen.delta == Point{ 2, -2 });
}

TEST_CASE("CursorTransform.window_change_reprojects_both_samples")
{
	CursorTransform transform;
	CursorState state;
	state.advanceRaw(Point{ 400, 300 }, Point{ 100, 80 });
	state.refreshTransformed(transform);
	state.advanceRaw(Point{ 400, 300 }, Point{ 100, 80 });
	state.refreshTransformed(transform);

	// A frame update after resizing must not report a stationary mouse as moving.
	transform.setBaseWindow({ 2.0, RectF{ 20, 10, 800, 600 } });
	state.advanceRaw(Point{ 400, 300 }, Point{ 100, 80 });
	state.refreshTransformed(transform);
	CHECK(state.vec2.current == Vec2{ 40, 35 });
	CHECK(state.vec2.previous == Vec2{ 40, 35 });
	CHECK(state.vec2.delta == Vec2{ 0, 0 });
	CHECK(state.point.delta == Point{ 0, 0 });

	// Reproject a moving pair using the new scale and letterbox origin.
	transform.setBaseWindow({ 0.5, RectF{ -10, -20, 800, 600 } });
	state.advanceRaw(Point{ 403, 296 }, Point{ 103, 76 });
	state.refreshTransformed(transform);
	CHECK(state.vec2.previous == Vec2{ 220, 200 });
	CHECK(state.vec2.current == Vec2{ 226, 192 });
	CHECK(state.vec2.delta == Vec2{ 6, -8 });
	CHECK(state.point.delta == Point{ 6, -8 });
	CHECK(state.raw.delta == Point{ 3, -4 });
	CHECK(state.screen.delta == Point{ 3, -4 });
}

TEST_CASE("CursorTransform.reprojection_preserves_raw_history")
{
	CursorTransform transform;
	transform.setBaseWindow({ 2.0, RectF{} });
	CursorState state;
	state.advanceRaw(Point{ 110, 220 }, Point{ 10, 20 });
	state.advanceRaw(Point{ 114, 218 }, Point{ 14, 18 });
	state.refreshTransformed(transform);
	const CursorState original = state;

	for (int32 i = 0; i < 3; ++i)
	{
		transform.setLocal(Mat3x2::Translate(2, -3));
		state.refreshTransformed(transform);
		CHECK(state.vec2.previous == Vec2{ 3, 13 });
		CHECK(state.vec2.current == Vec2{ 5, 12 });
		transform.setCamera(Mat3x2::Scale(2));
		state.refreshTransformed(transform);
		CHECK(state.vec2.previous == Vec2{ 0.5, 8 });
		CHECK(state.vec2.current == Vec2{ 1.5, 7.5 });
		CHECK(state.point.delta == Point{ 1, -1 });

		transform.setLocal(Mat3x2::Identity());
		state.refreshTransformed(transform);
		transform.setCamera(Mat3x2::Identity());
		state.refreshTransformed(transform);
		state.refreshTransformed(transform);
		CHECK(state.vec2.previous == original.vec2.previous);
		CHECK(state.vec2.current == original.vec2.current);
		CHECK(state.vec2.delta == original.vec2.delta);
		CHECK(state.point.previous == original.point.previous);
		CHECK(state.point.current == original.point.current);
		CHECK(state.point.delta == original.point.delta);
		CHECK(state.raw.previous == original.raw.previous);
		CHECK(state.raw.current == original.raw.current);
		CHECK(state.raw.delta == original.raw.delta);
		CHECK(state.screen.previous == original.screen.previous);
		CHECK(state.screen.current == original.screen.current);
		CHECK(state.screen.delta == original.screen.delta);
	}
}

TEST_CASE("CursorTransform.point_conversion_keeps_sub_float_precision")
{
	CursorTransform transform;
	transform.setLocal(Mat3x2::Translate(16777216, -16777216));
	CursorState state;
	state.advanceRaw(Point{ 0, 0 }, Point{ 16777216, -16777216 });
	state.advanceRaw(Point{ 1, -1 }, Point{ 16777217, -16777217 });
	state.refreshTransformed(transform);
	CHECK(state.vec2.previous == Vec2{ 0, 0 });
	CHECK(state.vec2.current == Vec2{ 1, -1 });
	CHECK(state.vec2.delta == Vec2{ 1, -1 });
	CHECK(state.point.delta == Point{ 1, -1 });
}

TEST_CASE("CursorTransform.initial_window_scale")
{
	CursorTransform transform;
	// Before renderer initialization, only the window scale is available.
	transform.setBaseWindow({ 2.0, RectF{} });
	CursorState state;
	state.advanceRaw(Point{ 800, 500 }, Point{ 640, 360 });
	state.refreshTransformed(transform);
	CHECK(state.vec2.current == Vec2{ 320, 180 });
	CHECK(transform.all.transformPoint(state.vec2.current) == Vec2{ 640, 360 });

	transform.setLocal(Mat3x2::Translate(20, 10));
	state.refreshTransformed(transform);
	CHECK(state.vec2.current == Vec2{ 300, 170 });
	CHECK(state.raw.current == Point{ 640, 360 });
}
