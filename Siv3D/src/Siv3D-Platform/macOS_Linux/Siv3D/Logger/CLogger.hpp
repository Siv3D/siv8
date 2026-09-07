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
# include <atomic>
# include <mutex>
# include <Siv3D/Logger/ILogger.hpp>
# include <Siv3D/LogLevel.hpp>

namespace s3d
{
	class CLogger final : public ISiv3DLogger
	{
	public:

		CLogger();

		void writeln(std::string_view s) override;

		void writeln(StringView s) override;

		void writeln(LogLevel logLevel, std::string_view s) override;

		void writeln(LogLevel logLevel, StringView s) override;

		void setOutputLevel(LogLevel logLevel) noexcept override;

		LogLevel getOutputLevel() const noexcept override;

	private:

		std::atomic<LogLevel> m_outputLevel = LogLevel::Info;

		std::mutex m_mutex;

		void writeImpl(const std::string& s);
	};
}
