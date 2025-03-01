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

# include "SceneUtility.hpp"

namespace s3d
{
	namespace SceneMisc
	{
		std::pair<double, RectF> CalculateLetterboxComposition(const SizeF& backBufferSize, const SizeF& sceneSize)
		{
			const double sx = (backBufferSize.x / sceneSize.x);
			const double sy = (backBufferSize.y / sceneSize.y);
			const double s = Min(sx, sy);

			if (sx <= sy)
			{
				const double offsetY = ((backBufferSize.y - sceneSize.y * s) * 0.5);
				const double width = backBufferSize.x;
				const double height = (backBufferSize.y - offsetY * 2.0);
				return{ s, RectF{ 0.0, offsetY, width, height } };
			}
			else
			{
				const double offsetX = ((backBufferSize.x - sceneSize.x * s) * 0.5);
				const double width = (backBufferSize.x - offsetX * 2.0);
				const double height = backBufferSize.y;
				return{ s, RectF{ offsetX, 0.0, width, height } };
			}
		}
	}
}
