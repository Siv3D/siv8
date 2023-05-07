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
# include <cstdio>
# include <mutex>
# include <Siv3D/LogType.hpp>
# include <Siv3D/Logger/ILogger.hpp>

namespace s3d
{
	class CLogger final : public ISiv3DLogger
	{
	public:

		CLogger();

		~CLogger() override;

		void write(std::string_view s) override;

		void write(StringView s) override;

		void write(LogType type, std::string_view s) override;

		void write(LogType type, StringView s) override;

		void setOutputLevel(LogType outputLevel) override;

		LogType getOutputLevel() const noexcept override;
	
	private:

		void write(const wchar_t* s);

		std::atomic<LogType> m_outputLevel = LogType::User;

		std::mutex m_mutex;
	};
}
