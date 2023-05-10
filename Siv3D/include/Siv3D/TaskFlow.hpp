//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4456)
# include <ThirdParty/taskflow/taskflow.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	using Executor = tf::Executor;
	using TaskFlow = tf::Taskflow;
}
