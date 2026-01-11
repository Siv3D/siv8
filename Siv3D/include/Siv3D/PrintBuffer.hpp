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
# include <memory>
# include "FormatData.hpp"
# include "Formatter.hpp"

namespace s3d
{
	namespace detail
	{
		struct PrintBuffer
		{
			std::unique_ptr<FormatData> formatData;

			////////////////////////////////////////////////////////////////
			//
			//	(constructor)
			//
			////////////////////////////////////////////////////////////////

			PrintBuffer();

			PrintBuffer(PrintBuffer&& other) noexcept;

			explicit PrintBuffer(const char32* s);

			explicit PrintBuffer(StringView s);

			explicit PrintBuffer(const String& s);

			explicit PrintBuffer(String&& s);

			////////////////////////////////////////////////////////////////
			//
			//	(destructor)
			//
			////////////////////////////////////////////////////////////////

			~PrintBuffer();

			////////////////////////////////////////////////////////////////
			//
			//	operator <<
			//
			////////////////////////////////////////////////////////////////

			PrintBuffer& operator <<(const char32* s);

			PrintBuffer& operator <<(StringView s);

			PrintBuffer& operator <<(const String& s);

			PrintBuffer& operator <<(const Concept::Formattable auto& value);

			template <class Type>
			PrintBuffer& operator <<(const Type& value) = delete;
		};
	}
}

# include "detail/PrintBuffer.ipp"
