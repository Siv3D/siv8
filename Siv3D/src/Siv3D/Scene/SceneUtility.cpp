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

# include "SceneUtility.hpp"
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Error.hpp>
# include <cmath>

namespace s3d
{
	namespace SceneMisc
	{
		void RenderState::updateWindow(const WindowState& window, const bool force)
		{
			if (window.minimized || (window.frameBufferSize.x <= 0) || (window.frameBufferSize.y <= 0)
				|| (window.virtualSize.x <= 0) || (window.virtualSize.y <= 0))
			{
				return;
			}
			if (not force && (virtualSize == window.virtualSize) && (frameBufferSize == window.frameBufferSize))
			{
				return;
			}
			virtualSize = window.virtualSize;
			frameBufferSize = window.frameBufferSize;
			if (resizeMode == ResizeMode::Virtual)
			{
				size = virtualSize;
			}
			else if (resizeMode == ResizeMode::Actual)
			{
				size = frameBufferSize;
			}
		}

		Size RenderState::renderSize() const noexcept
		{
			if (not highResolutionEnabled)
			{
				return size;
			}
			const double scale = Max(1.0, Min((double(frameBufferSize.x) / size.x), (double(frameBufferSize.y) / size.y)));
			return{ static_cast<int32>(std::round(size.x * scale)), static_cast<int32>(std::round(size.y * scale)) };
		}

		std::pair<double, RectF> RenderState::composition() const noexcept
		{
			return CalculateLetterboxComposition(frameBufferSize, size);
		}

		void ValidateSceneSize(const Size size)
		{
			if ((size.x < 1) || (16384 < size.x) || (size.y < 1) || (16384 < size.y))
			{
				throw Error{ "Scene size and render size must be in the range [1, 16384]" };
			}
		}

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
