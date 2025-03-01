﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <ThirdParty/absl/functional/function_ref.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FunctionRef
	//
	////////////////////////////////////////////////////////////////

	template <class T>
	using FunctionRef = absl::FunctionRef<T>;
}

