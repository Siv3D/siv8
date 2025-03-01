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
# include <Siv3D/Common.hpp>

namespace s3d
{
	template <class CommandType>
	class BatchStateTracker
	{
	public:

		[[nodiscard]]
		constexpr bool has(const CommandType command) const noexcept
		{
			return ((m_states & (0x1ull << FromEnum(command))) != 0);
		}

		[[nodiscard]]
		constexpr bool hasStateChange() const noexcept
		{
			return (1 < m_states);
		}

		constexpr void set(const CommandType command) noexcept
		{
			m_states |= (0x1ull << FromEnum(command));
		}

		constexpr void clear(const CommandType command) noexcept
		{
			m_states &= ~(0x1ull << FromEnum(command));
		}

		constexpr void clear() noexcept
		{
			m_states = 0;
		}
	
	private:

		uint64 m_states = 0;
	};
}
