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

# include "CursorTransform.hpp"

namespace s3d
{
	bool CursorTransform::setLocal(const Mat3x2& matrix) noexcept
	{
		if (local == matrix)
		{
			return false;
		}

		local = matrix;
		updateAll();
		return true;
	}

	bool CursorTransform::setCamera(const Mat3x2& matrix) noexcept
	{
		if (camera == matrix)
		{
			return false;
		}

		camera = matrix;
		updateAll();
		return true;
	}

	bool CursorTransform::setBaseWindow(const std::pair<double, RectF>& letterboxComposition) noexcept
	{
		const Mat3x2 matrix = Mat3x2::Scale(letterboxComposition.first).translated(letterboxComposition.second.pos);
		if (baseWindow == matrix)
		{
			return false;
		}

		baseWindow = matrix;
		updateAll();
		return true;
	}

	void CursorTransform::updateAll() noexcept
	{
		all		= (local * camera * baseWindow);
		allInv	= all.inverse();
	}
}
