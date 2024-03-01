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
		struct ConsoleBuffer
		{
			std::unique_ptr<FormatData> formatData;

			////////////////////////////////////////////////////////////////
			//
			//	(constructor)
			//
			////////////////////////////////////////////////////////////////

			ConsoleBuffer();

			ConsoleBuffer(ConsoleBuffer&& other) noexcept;

			explicit ConsoleBuffer(const char32* s);

			explicit ConsoleBuffer(StringView s);

			explicit ConsoleBuffer(const String& s);

			explicit ConsoleBuffer(String&& s);

			////////////////////////////////////////////////////////////////
			//
			//	(destructor)
			//
			////////////////////////////////////////////////////////////////

			~ConsoleBuffer();

			////////////////////////////////////////////////////////////////
			//
			//	operator <<
			//
			////////////////////////////////////////////////////////////////

			ConsoleBuffer& operator <<(const char32* s);

			ConsoleBuffer& operator <<(StringView s);

			ConsoleBuffer& operator <<(const String& s);

			ConsoleBuffer& operator <<(const Concept::Formattable auto& value);

			template <class Type>
			ConsoleBuffer& operator <<(const Type& value) = delete;
		};
	}
}

# include "detail/ConsoleBuffer.ipp"
