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
# include <limits>
# include <Siv3D/Types.hpp>

namespace s3d
{
	namespace detail
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
			const char* data() const noexcept
			{
				return m_str;
			}

			[[nodiscard]]
			size_t size() const noexcept
			{
				return static_cast<size_t>(m_buffer - m_str + BufferSize - 1);
			}

		private:

			static constexpr size_t BufferSize = (std::numeric_limits<unsigned long long>::digits10 + 3);

			char m_buffer[BufferSize];

			const char* m_str;

			char* formatUnsigned(uint64 value) noexcept;

			const char* formatSigned(int64 value) noexcept;
		};
	}
}
