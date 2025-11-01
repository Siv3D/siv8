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

# include <Siv3D/SimpleGUI.hpp>
# include <Siv3D/GUIStyle.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/CursorStyle.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/SimpleGUI/SimpleDismissibleTooltip.hpp>

namespace s3d
{
	void SimpleDismissibleTooltip::drawOverlay() const
	{
		SimpleTooltip::drawOverlay();

		if (isHovered())
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	void SimpleDismissibleTooltip::onMouseDown()
	{
		hide();
	}
}
