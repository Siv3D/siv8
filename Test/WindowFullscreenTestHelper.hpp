//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "WindowSceneTestHelper.hpp"
# include <Siv3D/Scene/SceneUtility.hpp>

namespace s3d::Test::Fullscreen
{
	struct RestoreWindowAndScene : Test::WindowSceneSnapshot
	{
		const bool toggleEnabled = Window::IsToggleFullscreenEnabled();

		~RestoreWindowAndScene()
		{
			Test::SetFullscreenWithPause(false);
			Test::RestorePlacement(window, Test::WindowSizeUnit::Virtual, Test::PlacementOrder::PositionThenSize);
			if (window.maximized) { Window::Maximize(); }
			Scene::Configure(config);
			Window::SetToggleFullscreenEnabled(toggleEnabled);
		}
	};

	inline void CheckSelectedConfig(const SceneConfig& config, const Size expected)
	{
		CHECK(Scene::GetConfig() == config);
		CHECK(Scene::Size() == expected);
		SceneMisc::RenderState reference;
		reference.config = config;
		reference.sceneSize = expected;
		reference.windowFrameBufferSize = Window::GetState().frameBufferSize;
		CHECK(Scene::RenderResolution() == reference.renderResolution());
		const auto [scale, area] = reference.composition();
		CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
		const Mat3x2 all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
		CHECK(Cursor::PosF().epsilonEquals(all.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
		CHECK(Scene::ClientToScene(reference.windowFrameBufferSize * 0.5).epsilonEquals(expected * 0.5, 1e-6));
	}

	inline void CheckPresentation(const SceneSizeMode mode, const bool highResolution, const TextureFilter filter,
		const Size fixedSize = Size{ 400, 300 })
	{
		CHECK(Scene::GetConfig().sizeMode == mode);
		CHECK((Scene::GetConfig().resolution == SceneResolution::Display) == highResolution);
		CHECK(Scene::GetConfig().scalingFilter == filter);
		const auto& window = Window::GetState();
		const Size expected = (mode == SceneSizeMode::Fixed) ? fixedSize
			: ((mode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
		CHECK(Scene::Size() == expected);
		SceneMisc::RenderState reference;
		reference.sceneSize = expected;
		reference.windowFrameBufferSize = window.frameBufferSize;
		reference.config.resolution = (highResolution ? SceneResolution::Display : SceneResolution::SceneSize);
		CHECK(Scene::RenderResolution() == reference.renderResolution());
		const auto [scale, area] = reference.composition();
		CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
	}
}
