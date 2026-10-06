//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <type_traits>

namespace
{
	void CheckCursorCoordinates(const Mat3x2& local, const Mat3x2& camera)
	{
		CHECK(Cursor::GetLocalTransform() == local);
		CHECK(Cursor::GetCameraTransform() == camera);
		const Mat3x2 baseInverse = Cursor::GetBaseWindowTransform().inverse();
		const auto toLocal = [&](const Point raw)
		{
			return local.inverse().transformPoint(camera.inverse().transformPoint(baseInverse.transformPoint(Vec2{ raw })));
		};
		CHECK(Cursor::PosF().epsilonEquals(toLocal(Cursor::PosRaw()), 1e-3));
		CHECK(Cursor::PreviousPosF().epsilonEquals(toLocal(Cursor::PreviousPosRaw()), 1e-3));
		CHECK(Cursor::DeltaF() == (Cursor::PosF() - Cursor::PreviousPosF()));
		CHECK(Cursor::Pos() == Cursor::PosF().asPoint());
		CHECK(Cursor::PreviousPos() == Cursor::PreviousPosF().asPoint());
		CHECK(Cursor::Delta() == (Cursor::Pos() - Cursor::PreviousPos()));
	}
}

TEST_CASE("Transformer2D.cursor_push_set_and_restore")
{
	const Mat3x2 oldLocal = Cursor::GetLocalTransform();
	const Mat3x2 oldCamera = Cursor::GetCameraTransform();
	const Mat3x2 oldGraphicsLocal = Graphics2D::GetLocalTransform();
	const Mat3x2 oldGraphicsCamera = Graphics2D::GetCameraTransform();
	const Point rawCurrent = Cursor::PosRaw();
	const Point rawPrevious = Cursor::PreviousPosRaw();
	const Point screenCurrent = Cursor::ScreenPos();
	const Point screenPrevious = Cursor::ScreenPreviousPos();
	{
		const Transformer2D resetLocal{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D resetCamera{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		const Mat3x2 local = Mat3x2::Scale(2, 4);
		const Mat3x2 camera = Mat3x2::Translate(10, -20);
		const Mat3x2 shift = Mat3x2::Translate(3, 5);
		const Mat3x2 rotation{ 0, 1, -1, 0, 40, 60 };
		{
			const Transformer2D cameraScope{ camera, TransformCursor::Yes, Transformer2D::Target::PushCamera };
			const Transformer2D localScope{ local, TransformCursor::Yes };
			CheckCursorCoordinates(local, camera);
			{
				const Transformer2D nested{ shift, TransformCursor::Yes };
				CheckCursorCoordinates(shift * local, camera);
				CHECK(Graphics2D::GetLocalTransform() == shift * local);
				{
					const Transformer2D replace{ rotation, TransformCursor::Yes, Transformer2D::Target::SetCamera };
					CheckCursorCoordinates(shift * local, rotation);
					CHECK(Graphics2D::GetCameraTransform() == rotation);
				}
				CheckCursorCoordinates(shift * local, camera);
			}
			CheckCursorCoordinates(local, camera);
		}
		CheckCursorCoordinates(Mat3x2::Identity(), Mat3x2::Identity());
		{
			// Creating Local before Camera gives the same coordinate system.
			const Transformer2D localScope{ local, TransformCursor::Yes };
			const Transformer2D cameraScope{ camera, TransformCursor::Yes, Transformer2D::Target::PushCamera };
			CheckCursorCoordinates(local, camera);
			const Transformer2D nestedCamera{ rotation, TransformCursor::Yes, Transformer2D::Target::PushCamera };
			CheckCursorCoordinates(local, rotation * camera);
			{
				const Transformer2D replaceLocal{ shift, TransformCursor::Yes, Transformer2D::Target::SetLocal };
				CheckCursorCoordinates(shift, rotation * camera);
			}
			CheckCursorCoordinates(local, rotation * camera);
		}
	}
	CheckCursorCoordinates(oldLocal, oldCamera);
	CHECK(Graphics2D::GetLocalTransform() == oldGraphicsLocal);
	CHECK(Graphics2D::GetCameraTransform() == oldGraphicsCamera);
	CHECK(Cursor::PosRaw() == rawCurrent);
	CHECK(Cursor::PreviousPosRaw() == rawPrevious);
	CHECK(Cursor::ScreenPos() == screenCurrent);
	CHECK(Cursor::ScreenPreviousPos() == screenPrevious);
}

TEST_CASE("Transformer2D.independent_graphics_and_cursor_transforms")
{
	const Transformer2D resetLocal{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
	const Transformer2D resetCamera{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };
	const Mat3x2 graphics = Mat3x2::Scale(4);
	const Mat3x2 cursor = Mat3x2::Translate(30, -10);
	const Mat3x2 shift = Mat3x2::Translate(5, 7);
	{
		const Transformer2D separate{ graphics, cursor };
		CheckCursorCoordinates(cursor, Mat3x2::Identity());
		CHECK(Graphics2D::GetLocalTransform() == graphics);
		{
			const Transformer2D graphicsOnly{ shift };
			CheckCursorCoordinates(cursor, Mat3x2::Identity());
			CHECK(Graphics2D::GetLocalTransform() == shift * graphics);
		}
		{
			const Transformer2D graphicsOnly{ shift, Transformer2D::Target::SetLocal };
			CheckCursorCoordinates(cursor, Mat3x2::Identity());
			CHECK(Graphics2D::GetLocalTransform() == shift);
		}
		{
			const Transformer2D nested{ shift, graphics };
			CheckCursorCoordinates(graphics * cursor, Mat3x2::Identity());
			CHECK(Graphics2D::GetLocalTransform() == shift * graphics);
		}
		{
			const Transformer2D replace{ shift, graphics, Transformer2D::Target::SetLocal };
			CheckCursorCoordinates(graphics, Mat3x2::Identity());
			CHECK(Graphics2D::GetLocalTransform() == shift);
		}
		CheckCursorCoordinates(cursor, Mat3x2::Identity());
	}
	{
		const Transformer2D separate{ graphics, cursor, Transformer2D::Target::SetCamera };
		CheckCursorCoordinates(Mat3x2::Identity(), cursor);
		{
			const Transformer2D nested{ shift, graphics, Transformer2D::Target::PushCamera };
			CheckCursorCoordinates(Mat3x2::Identity(), graphics * cursor);
			CHECK(Graphics2D::GetCameraTransform() == shift * graphics);
		}
		{
			const Transformer2D graphicsOnly{ shift, TransformCursor::No, Transformer2D::Target::SetCamera };
			CheckCursorCoordinates(Mat3x2::Identity(), cursor);
			CHECK(Graphics2D::GetCameraTransform() == shift);
		}
		CheckCursorCoordinates(Mat3x2::Identity(), cursor);
	}
	CheckCursorCoordinates(Mat3x2::Identity(), Mat3x2::Identity());
}

TEST_CASE("Transformer2D.move_transfers_restoration")
{
	static_assert(not std::is_copy_constructible_v<Transformer2D>);
	static_assert(std::is_nothrow_move_constructible_v<Transformer2D>);
	const Transformer2D resetLocal{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
	const Transformer2D resetCamera{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };
	const Mat3x2 local = Mat3x2::Translate(10, 20);
	Optional<Transformer2D> owner;
	{
		Transformer2D original{ local, TransformCursor::Yes };
		owner.emplace(std::move(original));
		CheckCursorCoordinates(local, Mat3x2::Identity());
	}
	CheckCursorCoordinates(local, Mat3x2::Identity());
	CHECK(Graphics2D::GetLocalTransform() == local);
	owner.reset();
	CheckCursorCoordinates(Mat3x2::Identity(), Mat3x2::Identity());
	CHECK(Graphics2D::GetLocalTransform() == Mat3x2::Identity());
	{
		Transformer2D inactive;
		const Transformer2D movedInactive{ std::move(inactive) };
	}
	CheckCursorCoordinates(Mat3x2::Identity(), Mat3x2::Identity());
}
