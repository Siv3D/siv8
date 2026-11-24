//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>

namespace s3d::WindowMisc
{
	// Native centering only; the public Window operation applies Scene afterward.
	bool CenterWindowOnCurrentScreen();

	bool CenterWindowOnScreen(size_t monitorIndex);
}
