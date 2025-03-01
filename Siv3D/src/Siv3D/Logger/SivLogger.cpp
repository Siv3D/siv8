//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Logger.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/LogType.hpp>
# include <Siv3D/Logger/ILogger.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	writeln
		//
		////////////////////////////////////////////////////////////////

		void Logger_impl::writeln(const char* s) const
		{
			writeln(std::string_view{ s });
		}

		void Logger_impl::writeln(const std::string_view s) const
		{
			std::string output;
			output.reserve(s.size() + 1);
			output.append(s);

			if (const auto pLogger = SIV3D_ENGINE(Logger))
			{
				pLogger->writeln(LogType::App, output);
			}
		}

		void Logger_impl::writeln(const std::string& s) const
		{
			writeln(std::string_view{ s });
		}

		void Logger_impl::writeln(const char32* s) const
		{
			writeln(StringView{ s });
		}

		void Logger_impl::writeln(const StringView s) const
		{
			if (const auto pLogger = SIV3D_ENGINE(Logger))
			{
				pLogger->writeln(LogType::App, s);
			}
		}

		void Logger_impl::writeln(const String& s) const
		{
			writeln(StringView{ s });
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		void Logger_impl::operator()(const char32* s) const
		{
			writeln(StringView{ s });
		}

		void Logger_impl::operator()(const StringView s) const
		{
			writeln(s);
		}

		void Logger_impl::operator()(const String& s) const
		{
			writeln(s);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		LoggerBuffer Logger_impl::operator <<(const char32* s) const
		{
			return LoggerBuffer{ s };
		}

		LoggerBuffer Logger_impl::operator <<(const StringView s) const
		{
			return LoggerBuffer{ s };
		}

		LoggerBuffer Logger_impl::operator <<(const String& s) const
		{
			return LoggerBuffer{ s };
		}

		LoggerBuffer Logger_impl::operator <<(String&& s) const
		{
			return LoggerBuffer{ std::move(s) };
		}

		////////////////////////////////////////////////////////////////
		//
		//	setOutputLevel
		//
		////////////////////////////////////////////////////////////////

		void Logger_impl::setOutputLevel(const LogType logType) const noexcept
		{
			SIV3D_ENGINE(Logger)->setOutputLevel(logType);
		}

		////////////////////////////////////////////////////////////////
		//
		//	getOutputLevel
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LogType Logger_impl::getOutputLevel() const noexcept
		{
			return SIV3D_ENGINE(Logger)->getOutputLevel();
		}
	}
}
