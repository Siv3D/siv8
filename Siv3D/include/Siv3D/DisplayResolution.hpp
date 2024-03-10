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

# pragma once
# include "Common.hpp"
# include "PointVector.hpp"

namespace s3d
{
	namespace DisplayResolution
	{
		/// @brief 640x480
		inline constexpr Size VGA_640x480{ 640, 480 };

		/// @brief 800x600
		inline constexpr Size SVGA_800x600{ 800, 600 };

		/// @brief 1024x768
		inline constexpr Size XGA_1024x768{ 1024, 768 };

		/// @brief 1280x720
		inline constexpr Size HD_1280x720{ 1280, 720 };

		/// @brief 1366x768
		inline constexpr Size HD_1366x768{ 1366, 768 };

		/// @brief 1920x1080
		inline constexpr Size FHD_1920x1080{ 1920, 1080 };

		/// @brief 2560x1440
		inline constexpr Size WQHD_2560x1440{ 2560, 1440 };

		/// @brief 3840x2160
		inline constexpr Size UHD_3840x2160{ 3840, 2160 };

		/// @brief 7680x4320
		inline constexpr Size UHD_7680x4320{ 7680, 4320 };
	}
}
