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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Fixed
	//
	////////////////////////////////////////////////////////////////

	constexpr SceneConfig SceneConfig::Fixed(const Size size, const SceneResolution resolution) noexcept
	{
		SceneConfig config;
		config.fixedSceneSize = size;
		config.sizeMode = SceneSizeMode::Fixed;
		config.resolution = resolution;
		return config;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Window
	//
	////////////////////////////////////////////////////////////////

	constexpr SceneConfig SceneConfig::Window(const SceneResolution resolution) noexcept
	{
		SceneConfig config;
		config.resolution = resolution;
		return config;
	}

	////////////////////////////////////////////////////////////////
	//
	//	PixelArt
	//
	////////////////////////////////////////////////////////////////

	constexpr SceneConfig SceneConfig::PixelArt(const Size size) noexcept
	{
		auto config = Fixed(size);
		config.scalingFilter = TextureFilter::Nearest;
		return config;
	}

	////////////////////////////////////////////////////////////////
	//
	//	WindowPixels
	//
	////////////////////////////////////////////////////////////////

	constexpr SceneConfig SceneConfig::WindowPixels() noexcept
	{
		SceneConfig config;
		config.sizeMode = SceneSizeMode::WindowPixels;
		config.fullscreen = FullscreenSceneMode::FollowWindow;
		return config;
	}
}
