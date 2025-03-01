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

# pragma once
# include "Platform.hpp"
# if SIV3D_PLATFORM(WINDOWS)
#	include <execution>
# endif
# include <future>
# include "Threading.hpp"

# include "detail/ArrayParallel.ipp"
