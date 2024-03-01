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

# pragma once
# include <mutex>
# include <Siv3D/Logger/ILogger.hpp>
# include <Siv3D/LogType.hpp>

namespace s3d
{
	class CLogger final : public ISiv3DLogger
	{
	public:

		CLogger();

		void write(std::string_view s) override;

		void write(StringView s) override;

		void write(LogType type, std::string_view s) override;

		void write(LogType type, StringView s) override;

		void setOutputLevel(LogType logType) noexcept override;

		LogType getOutputLevel() const noexcept override;

	private:

		std::atomic<LogType> m_outputLevel = LogType::Info;

		std::mutex m_mutex;

		void writeImpl(const std::string& s);
	};
}
