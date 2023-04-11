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
# include "Types.hpp"

namespace s3d
{
	class IntFormatter
	{
	public:

		[[nodiscard]]
		explicit IntFormatter(int32 value) noexcept;

		[[nodiscard]]
		explicit IntFormatter(int64 value) noexcept;

		[[nodiscard]]
		explicit IntFormatter(uint32 value) noexcept;

		[[nodiscard]]
		explicit IntFormatter(uint64 value) noexcept;

		[[nodiscard]]
		const char* data() const noexcept;

		[[nodiscard]]
		size_t size() const noexcept;

	private:

		static constexpr size_t BufferSize = 22; // (std::numeric_limits<unsigned long long>::digits10 + 3)

		char m_buffer[BufferSize];

		const char* m_str;

		char* formatUnsigned(uint64 value) noexcept;

		const char* formatSigned(int64 value) noexcept;
	};
}

# include "detail/IntFormatter.ipp"
