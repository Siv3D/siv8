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
# include "BatchStateTracker.hpp"

namespace s3d
{
	// Read the last recorded value only when a change is already pending.
	template <class CommandType, class State, class Buffer>
	inline void UpdateRenderer2DState(BatchStateTracker<CommandType>& tracker, const CommandType command,
		const State& value, State& current, const Buffer& buffer)
	{
		if (not tracker.has(command))
		{
			if (value != current)
			{
				current = value;
				tracker.set(command);
			}
		}
		else
		{
			if (value == buffer.back())
			{
				tracker.clear(command);
			}

			current = value;
		}
	}
}
