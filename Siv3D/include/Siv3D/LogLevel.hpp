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
# include "Types.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	LogLevel
	//
	////////////////////////////////////////////////////////////////

	/// @brief 出力されるログの詳細度
	enum class LogLevel : uint8
	{
		////////////////////////////////////////////////////////////////
		//
		//	LogLevel::Default
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルト | Default
		Default,

		////////////////////////////////////////////////////////////////
		//
		//	LogLevel::Trace
		//
		////////////////////////////////////////////////////////////////

		/// @brief トレース | Trace
		Trace,

		////////////////////////////////////////////////////////////////
		//
		//	LogLevel::Verbose
		//
		////////////////////////////////////////////////////////////////

		/// @brief 詳細 | Verbose
		Verbose,
	};
}