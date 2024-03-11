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

# pragma once
# include <Siv3D/Common.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DSystem
	{
	public:

		[[nodiscard]]
		static ISiv3DSystem* Create();

		virtual ~ISiv3DSystem() = default;

		virtual void init() = 0;

		virtual bool update() = 0;
	};
}
