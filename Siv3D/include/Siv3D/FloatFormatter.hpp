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

# pragma once
# include <string_view>
# include "Types.hpp"

namespace s3d
{
	class String;

	////////////////////////////////////////////////////////////////
	//
	//	FloatFormatter
	//
	////////////////////////////////////////////////////////////////

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

		static constexpr size_t FormatFloatBufferSize = 328;

		char m_buffer[FormatFloatBufferSize];

		std::string_view m_view;

		[[nodiscard]]
		std::string_view formatFloat(double value, int32 decimalPlace, bool fixed);
	};

	namespace detail
	{
		void AppendFloat(String& dst, float value);

		void AppendFloat(String& dst, double value);
	}
}

# include "detail/FloatFormatter.ipp"
