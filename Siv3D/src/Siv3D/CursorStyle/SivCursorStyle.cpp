//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
		//	Register
		//
		////////////////////////////////////////////////////////////////

		bool Register(const StringView name, const Image& image, const Point hotSpot, const AlphaPremultiplied alphaPremultiplied)
		{
			return SIV3D_ENGINE(CursorStyle)->registerCustomStyle(name, image, hotSpot, alphaPremultiplied);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Unregister
		//
		////////////////////////////////////////////////////////////////

		void Unregister(const StringView name)
		{
			SIV3D_ENGINE(CursorStyle)->unregisterCustomStyle(name);
		}

		////////////////////////////////////////////////////////////////
		//
		//	UnregisterAll
		//
		////////////////////////////////////////////////////////////////

		void UnregisterAll()
		{
			SIV3D_ENGINE(CursorStyle)->clearCustomStyles();
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
