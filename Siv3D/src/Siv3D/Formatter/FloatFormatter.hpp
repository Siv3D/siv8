//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <string_view>
# include <ThirdParty/double-conversion/double-to-string.h>
# include <Siv3D/Types.hpp>

namespace s3d
{
	namespace detail
	{
		class FloatFormatter
		{
		public:

			[[nodiscard]]
			FloatFormatter(double value, int32 decimalPlace, bool fixed);

			[[nodiscard]]
			const char* data() const noexcept
			{
				return m_view.data();
			}

			[[nodiscard]]
			size_t size() const noexcept
			{
				return m_view.size();
			}

		private:

			static constexpr size_t FormatFloatBufferSize = 384;

			static constexpr int FormatFlags = (double_conversion::DoubleToStringConverter::UNIQUE_ZERO |
				double_conversion::DoubleToStringConverter::EMIT_POSITIVE_EXPONENT_SIGN);

			char m_buffer[FormatFloatBufferSize];

			std::string_view m_view;

			[[nodiscard]]
			std::string_view formatFloat(double value, int32 decimalPlace, bool fixed);
		};
	}
}
