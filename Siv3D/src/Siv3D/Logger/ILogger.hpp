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
# include <Siv3D/Common.hpp>
# include <Siv3D/StringView.hpp>

namespace s3d
{
	enum class LogLevel : uint8;

	class SIV3D_NOVTABLE ISiv3DLogger
	{
	public:

		[[nodiscard]]
		static ISiv3DLogger* Create();

		virtual ~ISiv3DLogger() = default;

		virtual void writeln(std::string_view s) = 0;

		virtual void writeln(StringView s) = 0;

		virtual void writeln(LogLevel logLevel, std::string_view s) = 0;

		virtual void writeln(LogLevel logLevel, StringView s) = 0;

		virtual void setOutputLevel(LogLevel logLevel) noexcept = 0;

		virtual LogLevel getOutputLevel() const noexcept = 0;
	};
}
