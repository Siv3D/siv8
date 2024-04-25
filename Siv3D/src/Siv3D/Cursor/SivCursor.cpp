//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Cursor.hpp>
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Cursor
	{
		Point Pos() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().point.current;
		}

		Vec2 PosF() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().vec2.current;
		}

		Point PreviousPos() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().point.previous;
		}

		Vec2 PreviousPosF() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().vec2.previous;
		}

		Point Delta() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().point.delta;
		}

		Vec2 DeltaF() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().vec2.delta;
		}

		Point PosRaw() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().raw.current;
		}

		Point PreviousPosRaw() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().raw.previous;
		}

		Point DeltaRaw() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().raw.delta;
		}

		Point ScreenPos() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().screen.current;
		}

		Point ScreenPreviousPos() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().screen.previous;
		}

		Point ScreenDelta() noexcept
		{
			return SIV3D_ENGINE(Cursor)->getState().screen.delta;
		}

		Array<std::pair<int64, Point>> GetBuffer()
		{
			return SIV3D_ENGINE(Cursor)->getHighTemporalResolutionCursorPos();
		}
	}
}
