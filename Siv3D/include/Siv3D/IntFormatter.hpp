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
# include "Types.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	IntFormatter
	//
	////////////////////////////////////////////////////////////////

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

	namespace detail
	{
		inline constexpr size_t Uint32Width = 10;

		inline constexpr size_t Int32Width = 11;

		void AppendUint32(char32** p, uint32 value);

		void AppendInt32(char32** p, int32 value);
	}
}

# include "detail/IntFormatter.ipp"
