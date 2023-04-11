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
# include <string_view>
# include "Types.hpp"

namespace s3d
{
	class FloatFormatter
	{
	public:

		[[nodiscard]]
		FloatFormatter(double value, int32 decimalPlace, bool fixed);

		[[nodiscard]]
		const char* data() const noexcept;

		[[nodiscard]]
		size_t size() const noexcept;

	private:

		static constexpr size_t FormatFloatBufferSize = 384;

		char m_buffer[FormatFloatBufferSize];

		std::string_view m_view;

		[[nodiscard]]
		std::string_view formatFloat(double value, int32 decimalPlace, bool fixed);
	};
}

# include "detail/FloatFormatter.ipp"
