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
# include <Siv3D/Common.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/2DShapes.hpp>

namespace s3d
{
	struct CursorTransform
	{
		Mat3x2 local		= Mat3x2::Identity();
			
		Mat3x2 camera		= Mat3x2::Identity();
			
		Mat3x2 baseWindow	= Mat3x2::Identity();
			
		Mat3x2 all			= Mat3x2::Identity();
			
		Mat3x2 allInv		= Mat3x2::Identity();

		void setLocal(const Mat3x2& matrix) noexcept;

		void setCamera(const Mat3x2& matrix) noexcept;

		void setBaseWindow(const std::pair<double, RectF>& letterboxComposition) noexcept;

		void updateAll() noexcept;
	};
}
