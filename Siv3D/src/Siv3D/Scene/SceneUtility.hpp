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
			Size sceneSize = Scene::DefaultSceneSize;
			Size windowVirtualSize = Scene::DefaultSceneSize;
			Size windowFrameBufferSize = Scene::DefaultSceneSize;
			SceneConfig config;
			bool windowFullscreen = false;

			friend bool operator ==(const RenderState&, const RenderState&) = default;

			void configure(const SceneConfig& settings, const WindowState& window);

			void updateWindow(const WindowState& window);

			[[nodiscard]]
			Size renderResolution() const noexcept;

			[[nodiscard]]
			std::pair<double, RectF> composition() const noexcept;

		private:

			bool updateWindowGeometry(const WindowState& window);

			void updateSceneSize();
		};

		void ValidateSceneSize(Size size);
	}
}
