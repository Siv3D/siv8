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

# include "CRenderer_Metal.hpp"
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CRenderer_Metal::~CRenderer_Metal()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::~CRenderer_Metal()");
	}

	void CRenderer_Metal::init()
	{
		LOG_SCOPED_DEBUG("CRenderer_Metal::init()");



	}

	void CRenderer_Metal::clear()
	{

	}

	void CRenderer_Metal::flush()
	{

	}

	bool CRenderer_Metal::present()
	{
		return(true);
	}

	SceneStyle& CRenderer_Metal::getSceneStyle() noexcept
	{

	}

	void CRenderer_Metal::setSceneResizeMode(const ResizeMode resizeMode)
	{

	}

	ResizeMode CRenderer_Metal::getSceneResizeMode() const noexcept
	{

	}

	void CRenderer_Metal::resizeSceneBuffer(const Size size)
	{

	}

	const Size& CRenderer_Metal::getSceneBufferSize() const noexcept
	{

	}
}
 