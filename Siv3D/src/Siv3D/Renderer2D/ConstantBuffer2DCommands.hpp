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
# include <Siv3D/Array.hpp>
# include <Siv3D/ShaderStage.hpp>
# include <cstddef>

namespace s3d
{
	struct ConstantBuffer2DCommand
	{
		ShaderStage stage;
		uint32 slot;
		size_t offset;
		size_t size;
	};

	// 設定時の値を所有する。clear() 後も記録領域の容量を再利用する。
	class ConstantBuffer2DCommands
	{
	public:

		uint32 push(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
		{
			const size_t offset = m_data.size();
			const auto* bytes = static_cast<const std::byte*>(data);
			m_data.insert(m_data.end(), bytes, (bytes + size));
			const uint32 index = static_cast<uint32>(m_commands.size());
			m_commands.push_back({ stage, slot, offset, size });
			return index;
		}

		const ConstantBuffer2DCommand& get(const uint32 index) const noexcept
		{
			return m_commands[index];
		}

		const void* data(const ConstantBuffer2DCommand& command) const noexcept
		{
			return (m_data.data() + command.offset);
		}

		void clear() noexcept
		{
			m_commands.clear();
			m_data.clear();
		}

	private:

		Array<ConstantBuffer2DCommand> m_commands;

		Array<std::byte> m_data;
	};
}
