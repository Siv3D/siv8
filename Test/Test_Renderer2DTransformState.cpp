//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cmath>

# if SIV3D_PLATFORM(WINDOWS)
# include "../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp"
# elif SIV3D_PLATFORM(MACOS)
# include "../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.hpp"
# endif

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

namespace
{
# if SIV3D_PLATFORM(WINDOWS)
	using CommandManager = D3D11Renderer2DCommandManager;
	using CommandType = D3D11Renderer2DCommandType;
# else
	using CommandManager = MetalRenderer2DCommandManager;
	using CommandType = MetalRenderer2DCommandType;
# endif
}

TEST_CASE("Renderer2D.transform_defaults")
{
	CommandManager manager;
	CHECK(manager.getCurrentLocalTransform() == Mat3x2::Identity());
	CHECK(manager.getCurrentCameraTransform() == Mat3x2::Identity());
	CHECK(manager.getCurrentQuadWarpTransform() == Mat3x3::Identity());
	CHECK(manager.getCurrentCombinedTransform() == Mat3x3::Identity());
	CHECK(manager.getSceneScale() == Float2{ 1, 1 });
	CHECK(manager.getCurrentRMSScaling() == 1.0f);
	CHECK(manager.getCurrentRasterScaling() == 1.0f);
}

TEST_CASE("Renderer2D.transform_composition_and_raster_scale")
{
	CommandManager manager;
	const Mat3x2 local{ 2, 1, 0, 3, 5, 7 };
	const Mat3x2 camera{ 0, 1, -1, 0, 11, 13 };
	const Mat3x3 warp{ 1, 0, 0.0625f, 0, 1, -0.03125f, 3, -2, 1 };
	const Mat3x3 affine{ -1, 2, 0, -3, 0, 0, 4, 18, 1 };
	const Mat3x3 combined{ -1, 2, -0.125f, -3, 0, -0.1875f, 7, 16, 0.6875f };
	manager.pushLocalTransform(local);
	manager.pushCameraTransform(camera);
	CHECK(manager.getCurrentCombinedTransform() == affine);
	CHECK(std::abs(manager.getCurrentRMSScaling() - std::sqrt(7.0f)) < 1e-5f);
	CHECK(manager.getCurrentRasterScaling() == manager.getCurrentRMSScaling());

	manager.setSceneScale({ 2, 3 });
	CHECK(manager.getCurrentCombinedTransform() == affine);
	CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(38.0f)) < 1e-5f);
	manager.pushQuadWarpTransform(warp);
	CHECK(manager.getCurrentCombinedTransform() == combined);
	CHECK(std::abs(manager.getCurrentRMSScaling() - std::sqrt(7.0f)) < 1e-5f);
	CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(38.0f)) < 1e-5f);
	manager.flush();
	REQUIRE(manager.getCommands().back().type == CommandType::Transform);
	CHECK(manager.getCombinedTransform(manager.getCommands().back().index) == combined);

	const size_t count = manager.getCommands().size();
	manager.pushLocalTransform(local);
	manager.pushCameraTransform(camera);
	manager.pushQuadWarpTransform(warp);
	manager.setSceneScale({ 2, 3 });
	manager.flush();
	CHECK(manager.getCommands().size() == count);

	manager.setSceneScale({ 1, 1 });
	CHECK(manager.getCurrentRasterScaling() == manager.getCurrentRMSScaling());
	manager.flush();
	CHECK(manager.getCommands().size() == count);
}

TEST_CASE("Renderer2D.transform_scale_cancellation_and_boundaries")
{
	for (const bool sceneFirst : { false, true })
	{
		CAPTURE(sceneFirst);
		CommandManager manager;
		if (sceneFirst)
		{
			manager.setSceneScale({ 2, 0.5f });
		}
		manager.pushLocalTransform(Mat3x2::Scale(4, 1));
		manager.pushCameraTransform(Mat3x2::Scale(0.25f, 1));
		if (not sceneFirst)
		{
			manager.setSceneScale({ 2, 0.5f });
		}
		CHECK(manager.getCurrentCombinedTransform() == Mat3x3::Identity());
		CHECK(manager.getCurrentRMSScaling() == 1.0f);
		CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(2.125f)) < 1e-5f);

		manager.pushCameraTransform(Mat3x2::Identity());
		manager.pushLocalTransform(Mat3x2::Scale(-2, 3));
		CHECK(std::abs(manager.getCurrentRMSScaling() - std::sqrt(6.5f)) < 1e-5f);
		CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(9.125f)) < 1e-5f);

		manager.pushLocalTransform(Mat3x2::Scale(0));
		CHECK(manager.getCurrentRMSScaling() == 0.0f);
		CHECK(manager.getCurrentRasterScaling() == 0.0f);
		manager.pushLocalTransform(Mat3x2::Translate(20, -10));
		CHECK(manager.getCurrentRMSScaling() == 1.0f);
		CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(2.125f)) < 1e-5f);
	}
}

TEST_CASE("Renderer2D.transform_equivalent_matrices_preserve_batch")
{
	CommandManager manager;
	const size_t first = manager.getCommands().size();
	manager.pushDraw(3);
	manager.pushLocalTransform(Mat3x2::Scale(2));
	manager.pushCameraTransform(Mat3x2::Scale(0.5f));
	manager.pushDraw(6);
	manager.pushQuadWarpTransform(Mat3x3{ Mat3x2::Scale(2) });
	manager.pushLocalTransform(Mat3x2::Identity());
	manager.setSceneScale({ 2, 4 });
	CHECK(manager.getCurrentCombinedTransform() == Mat3x3::Identity());
	CHECK(manager.getCurrentRMSScaling() == 0.5f);
	CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(2.5f)) < 1e-5f);
	manager.pushDraw(12);
	manager.flush();

	const auto& commands = manager.getCommands();
	REQUIRE(commands.size() == (first + 1));
	REQUIRE(commands.back().type == CommandType::Draw);
	CHECK(manager.getDraw(commands.back().index).indexCount == 21);

	manager.pushCameraTransform(Mat3x2::Identity());
	manager.pushDraw(6);
	manager.flush();
	REQUIRE(commands.size() == (first + 3));
	REQUIRE(commands[first + 1].type == CommandType::Transform);
	CHECK(manager.getCombinedTransform(commands[first + 1].index) == Mat3x3{ Mat3x2::Scale(2) });
	REQUIRE(commands.back().type == CommandType::Draw);
	CHECK(manager.getDraw(commands.back().index).indexCount == 6);
}

TEST_CASE("Renderer2D.transform_reset_preserves_cached_state")
{
	CommandManager manager;
	const Mat3x2 local = Mat3x2::Scale(2, 4);
	const Mat3x2 camera = Mat3x2::Translate(3, 5);
	const Mat3x3 warp{ Mat3x2::Translate(-1, 2) };
	const Mat3x3 combined{ Mat3x2{ 2, 0, 0, 4, 2, 7 } };
	manager.pushLocalTransform(local);
	manager.pushCameraTransform(camera);
	manager.pushQuadWarpTransform(warp);
	manager.setSceneScale({ 3, 2 });
	manager.pushDraw(6);
	manager.flush();
	manager.reset();
	CHECK(manager.getCurrentLocalTransform() == local);
	CHECK(manager.getCurrentCameraTransform() == camera);
	CHECK(manager.getCurrentQuadWarpTransform() == warp);
	CHECK(manager.getCurrentCombinedTransform() == combined);
	CHECK(manager.getCombinedTransform(0) == combined);
	CHECK(manager.getSceneScale() == Float2{ 3, 2 });
	CHECK(std::abs(manager.getCurrentRMSScaling() - std::sqrt(10.0f)) < 1e-5f);
	CHECK(std::abs(manager.getCurrentRasterScaling() - std::sqrt(50.0f)) < 1e-5f);

	const size_t first = manager.getCommands().size();
	manager.pushLocalTransform(local);
	manager.pushCameraTransform(camera);
	manager.pushQuadWarpTransform(warp);
	manager.setSceneScale({ 3, 2 });
	manager.pushDraw(3);
	manager.flush();
	REQUIRE(manager.getCommands().size() == (first + 1));
	CHECK(manager.getCommands().back().type == CommandType::Draw);
}

# endif
