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
# include <Siv3D/StringView.hpp>

namespace s3d
{
	enum class LogType : uint8;

	class SIV3D_NOVTABLE ISiv3DLogger
	{
	public:

		static ISiv3DLogger* Create();

		virtual ~ISiv3DLogger() = default;

		virtual void write(std::string_view s) = 0;

		virtual void write(StringView s) = 0;

		virtual void write(LogType type, std::string_view s) = 0;

		virtual void write(LogType type, StringView s) = 0;

		virtual void setOutputLevel(LogType logType) noexcept = 0;

		virtual LogType getOutputLevel() const noexcept = 0;
	};
}
