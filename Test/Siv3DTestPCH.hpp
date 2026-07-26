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

// pcg_extras.hpp intentionally uses __DATE__ and __TIME__ for a compile-time seed.
# if defined(__clang__)
#	pragma clang diagnostic ignored "-Wpch-date-time"
# endif

# include <Siv3D.hpp>
