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
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Scene.hpp>

namespace s3d
{
	struct WindowState;

	namespace SceneMisc
	{
		[[nodiscard]]
		std::pair<double, RectF> CalculateLetterboxComposition(const SizeF& backBufferSize, const SizeF& sceneSize);

		// Configuration and the last usable window geometry. GPU resources are owned by the backend.
		struct RenderState
		{
			Size size = Scene::DefaultSceneSize;
			Size virtualSize = Scene::DefaultSceneSize;
			Size frameBufferSize = Scene::DefaultSceneSize;
			ResizeMode resizeMode = Scene::DefaultResizeMode;
			bool highResolutionEnabled = false;

			friend bool operator ==(const RenderState&, const RenderState&) = default;

			// force also applies a newly selected resize mode or an explicit window resize.
			void updateWindow(const WindowState& window, bool force = false);

			[[nodiscard]]
			Size renderSize() const noexcept;

			[[nodiscard]]
			std::pair<double, RectF> composition() const noexcept;
		};

		void ValidateSceneSize(Size size);
	}
}
