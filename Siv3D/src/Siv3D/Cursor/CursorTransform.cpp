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

# include "CursorTransform.hpp"

namespace s3d
{
	void CursorTransform::setLocal(const Mat3x2& matrix) noexcept
	{
		local = matrix;
		updateAll();
	}

	void CursorTransform::setCamera(const Mat3x2& matrix) noexcept
	{
		camera = matrix;
		updateAll();
	}

	void CursorTransform::setBaseWindow(const std::pair<double, RectF>& letterboxComposition) noexcept
	{
		baseWindow = Mat3x2::Scale(letterboxComposition.first).translated(letterboxComposition.second.pos);
		updateAll();
	}

	void CursorTransform::updateAll() noexcept
	{
		all		= (local * camera * baseWindow);
		allInv	= all.inverse();
	}
}
