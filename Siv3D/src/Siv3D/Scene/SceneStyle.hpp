//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Scene.hpp>

namespace s3d
{
	struct SceneStyle
	{
		ColorF letterboxColor		= Scene::DefaultLetterboxColor;

		ColorF backgroundColor		= Scene::DefaultBackgroundColor;

		TextureFilter textureFilter	= Scene::DefaultTextureFilter;
	};
}
