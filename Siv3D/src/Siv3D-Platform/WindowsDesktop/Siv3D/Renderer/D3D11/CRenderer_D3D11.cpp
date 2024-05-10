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

# include "CRenderer_D3D11.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CRenderer_D3D11::~CRenderer_D3D11()
	{
		LOG_SCOPED_DEBUG("CRenderer_D3D11::~CRenderer_D3D11()");
	}

	void CRenderer_D3D11::init()
	{
		LOG_SCOPED_DEBUG("CRenderer_D3D11::init()");

		m_device.init();
	}

	void CRenderer_D3D11::clear()
	{

	}

	void CRenderer_D3D11::flush()
	{

	}

	bool CRenderer_D3D11::present()
	{
		return(true);
	}
}
