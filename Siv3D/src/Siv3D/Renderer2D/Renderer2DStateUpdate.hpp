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

	// A pending shader change also retains the shader when its ID is unchanged.
	template <class CommandType, class Shader, class State, class Buffer, class Reserved>
	inline void UpdateRenderer2DCustomShaderState(BatchStateTracker<CommandType>& tracker, const CommandType command,
		const Shader& shader, State& current, const Buffer& buffer, Reserved& reserved)
	{
		const auto id = shader.id();
		if (not tracker.has(command))
		{
			if (id != current)
			{
				current = id;
				tracker.set(command);
				reserved.try_emplace(id, shader);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				tracker.clear(command);
			}

			current = id;
			reserved.try_emplace(id, shader);
		}
	}

	// Equal texture IDs need neither a recorded-value read nor a retention lookup.
	template <class CommandType, class Texture, class State, class Buffer, class Reserved>
	inline void UpdateRenderer2DTextureState(BatchStateTracker<CommandType>& tracker, const CommandType command,
		const Texture& texture, State& current, const Buffer& buffer, Reserved& reserved)
	{
		const auto id = texture.id();
		if (id == current)
		{
			return;
		}

		reserved.try_emplace(id, texture);

		if (not tracker.has(command))
		{
			tracker.set(command);
		}
		else if (id == buffer.back())
		{
			tracker.clear(command);
		}

		current = id;
	}
}
