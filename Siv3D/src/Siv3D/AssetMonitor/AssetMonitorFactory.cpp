﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CAssetMonitor.hpp"

namespace s3d
{
	ISiv3DAssetMonitor* ISiv3DAssetMonitor::Create()
	{
		return new CAssetMonitor;
	}
}
