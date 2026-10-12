//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <mutex>
# include <Siv3D/WindowState.hpp>

namespace s3d
{
	// One window-thread writer and one engine-thread reader. Nested native
	// notifications commit only when the enclosing state operation finishes.
	class WindowStateSync
	{
	public:

		void beginUpdate() noexcept
		{
			++m_depth;
		}

		bool endUpdate(const WindowState& state)
		{
			if (--m_depth == 0)
			{
				std::lock_guard lock{ m_mutex };
				m_committed = state;
				return true;
			}
			return false;
		}

		bool isUpdating() const noexcept
		{
			return (m_depth != 0);
		}

		// Engine thread only, except before the initialization promise is fulfilled.
		void synchronize()
		{
			std::lock_guard lock{ m_mutex };
			m_published = m_committed;
		}

		const WindowState& get() const noexcept
		{
			return m_published;
		}

	private:

		uint32 m_depth = 0; // Window thread only; never held as a lock across Win32.
		std::mutex m_mutex;
		WindowState m_committed;
		WindowState m_published; // Stable address, engine thread only.
	};
}
