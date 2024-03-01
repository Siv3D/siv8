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
# include <memory>
# include "FormatData.hpp"
# include "Formatter.hpp"

namespace s3d
{
	namespace detail
	{
		struct LoggerBuffer
		{
			std::unique_ptr<FormatData> formatData;

			////////////////////////////////////////////////////////////////
			//
			//	(constructor)
			//
			////////////////////////////////////////////////////////////////

			LoggerBuffer();

			LoggerBuffer(LoggerBuffer&& other) noexcept;

			explicit LoggerBuffer(const char32* s);

			explicit LoggerBuffer(StringView s);

			explicit LoggerBuffer(const String& s);

			explicit LoggerBuffer(String&& s);

			////////////////////////////////////////////////////////////////
			//
			//	(destructor)
			//
			////////////////////////////////////////////////////////////////

			~LoggerBuffer();

			////////////////////////////////////////////////////////////////
			//
			//	operator <<
			//
			////////////////////////////////////////////////////////////////

			LoggerBuffer& operator <<(const char32* s);

			LoggerBuffer& operator <<(StringView s);

			LoggerBuffer& operator <<(const String& s);

			LoggerBuffer& operator <<(const Concept::Formattable auto& value);

			template <class Type>
			LoggerBuffer& operator <<(const Type& value) = delete;
		};
	}
}

# include "detail/LoggerBuffer.ipp"

