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

# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/CursorStyle/ICursorStyle.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Cursor
	{
		////////////////////////////////////////////////////////////////
		//
		//	SetDefaultStyle
		//
		////////////////////////////////////////////////////////////////

		void SetDefaultStyle(const CursorStyle style)
		{
			return SIV3D_ENGINE(CursorStyle)->setDefaultStyle(style);
		}

		////////////////////////////////////////////////////////////////
		//
		//	RegisterCustomCursorStyle
		//
		////////////////////////////////////////////////////////////////

		bool RegisterCustomCursorStyle(const StringView name, const Image& image, const Point hotSpot)
		{
			return SIV3D_ENGINE(CursorStyle)->registerCursor(name, image, hotSpot);
		}

		////////////////////////////////////////////////////////////////
		//
		//	RequestStyle
		//
		////////////////////////////////////////////////////////////////

		void RequestStyle(const CursorStyle style)
		{
			return SIV3D_ENGINE(CursorStyle)->requestStyle(style);
		}

		void RequestStyle(const StringView name)
		{
			return SIV3D_ENGINE(CursorStyle)->requestStyle(name);
		}
	}
}
