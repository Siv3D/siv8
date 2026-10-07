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
# include <Siv3D/Graphics.hpp>
# include <Siv3D/ShaderStage.hpp>
# include <array>
# include <bit>
# include <cstddef>
# include <cstring>

namespace s3d
{
	struct ConstantBuffer2DCommand
	{
		ShaderStage stage;
		uint32 slot;
		uint32 snapshot;
		size_t size;
	};

	// 設定値をコマンド・現在の設定・スコープで共有する。GPU リソースの寿命とは独立。
	// 参照がなくなった保存領域はサイズ別に再利用する。操作は描画スレッドに限定する。
	class ConstantBuffer2DCommands
	{
	public:

		// 値はここでコピーするが、コマンドは次の描画または flush まで確定しない。
		void push(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
		{
			const uint32 snapshot = (size ? create(data, size) : 0);
			apply(stage, slot, snapshot);
			release(snapshot);
		}

		// 戻り値は restore() または release() で解放する。0 は未設定。
		uint32 save(const ShaderStage stage, const uint32 slot) noexcept
		{
			const uint32 snapshot = m_current[FromEnum(stage)][slot];
			retain(snapshot);
			return snapshot;
		}

		void restore(const ShaderStage stage, const uint32 slot, const uint32 snapshot) noexcept
		{
			apply(stage, slot, snapshot);
			release(snapshot);
		}

		bool hasStateChange() const noexcept
		{
			return (m_changes != 0);
		}

		void flush()
		{
			// 変更したスロットだけ確定する。復元後に再設定された中間状態は記録しない。
			while (m_changes)
			{
				const uint32 bit = static_cast<uint32>(std::countr_zero(m_changes));
				const uint32 stage = (bit / Graphics::ConstantBufferSlotCount);
				const uint32 slot = (bit % Graphics::ConstantBufferSlotCount);
				const uint32 snapshot = m_current[stage][slot];
				record(static_cast<ShaderStage>(stage), slot, snapshot);
				m_recorded[stage][slot] = snapshot;
				m_changes &= ~(1u << bit);
			}
		}

		void release(const uint32 snapshot) noexcept
		{
			if (snapshot && (--m_snapshots[snapshot].references == 0))
			{
				auto& value = m_snapshots[snapshot];
				value.nextFree = m_free[value.bucket];
				m_free[value.bucket] = snapshot;
			}
		}

		const ConstantBuffer2DCommand& get(const uint32 index) const noexcept
		{
			return m_commands[index];
		}

		const void* data(const ConstantBuffer2DCommand& command) const noexcept
		{
			return (command.snapshot ? m_snapshots[command.snapshot].bytes.data() : nullptr);
		}

		size_t size() const noexcept
		{
			return m_commands.size();
		}

		// 実行済みコマンドだけ破棄し、次の実行に現在値を再記録する。値のコピーは不要。
		void reset()
		{
			for (const auto& command : m_commands)
			{
				release(command.snapshot);
			}
			m_commands.clear();
			m_recorded = m_current;
			m_changes = 0;
			for (uint32 stage = 0; stage < m_current.size(); ++stage)
			{
				for (uint32 slot = 2; slot < Graphics::ConstantBufferSlotCount; ++slot)
				{
					if (const uint32 snapshot = m_current[stage][slot])
					{
						record(static_cast<ShaderStage>(stage), slot, snapshot);
					}
				}
			}
		}

	private:

		struct Snapshot
		{
			Array<std::byte> bytes;
			uint32 references = 0;
			uint32 nextFree = 0;
			uint32 bucket = 0;
		};

		Array<Snapshot> m_snapshots = Array<Snapshot>(1);
		std::array<uint32, 13> m_free{}; // 16, 32, ... 65536 bytes
		std::array<std::array<uint32, Graphics::ConstantBufferSlotCount>, 2> m_current{};
		// m_commands 内の確定済み状態。外部レンダラーと共有する GPU 状態のキャッシュではない。
		// 値は m_commands が保持する。比較は ID だけで行い、バイト列を走査しない。
		std::array<std::array<uint32, Graphics::ConstantBufferSlotCount>, 2> m_recorded{};
		uint32 m_changes = 0;
		static_assert((2 * Graphics::ConstantBufferSlotCount) <= 32);
		Array<ConstantBuffer2DCommand> m_commands;

		void retain(const uint32 snapshot) noexcept
		{
			if (snapshot)
			{
				++m_snapshots[snapshot].references;
			}
		}

		uint32 create(const void* data, const size_t size)
		{
			const uint32 bucket = static_cast<uint32>(std::bit_width(size - 1) - 4);
			uint32 snapshot = m_free[bucket];
			if (not snapshot)
			{
				Snapshot value;
				value.bytes.reserve(std::bit_ceil(size));
				value.bucket = bucket;
				snapshot = static_cast<uint32>(m_snapshots.size());
				m_snapshots.push_back(std::move(value));
			}
			else
			{
				m_free[bucket] = m_snapshots[snapshot].nextFree;
			}
			auto& value = m_snapshots[snapshot];
			value.bytes.resize(size);
			std::memcpy(value.bytes.data(), data, size);
			value.references = 1;
			return snapshot;
		}

		void record(const ShaderStage stage, const uint32 slot, const uint32 snapshot)
		{
			m_commands.push_back({ stage, slot, snapshot, m_snapshots[snapshot].bytes.size() });
			retain(snapshot);
		}

		void apply(const ShaderStage stage, const uint32 slot, const uint32 snapshot) noexcept
		{
			auto& current = m_current[FromEnum(stage)][slot];
			retain(snapshot);
			release(current);
			current = snapshot;

			const uint32 bit = (1u << (FromEnum(stage) * Graphics::ConstantBufferSlotCount + slot));
			if (snapshot == m_recorded[FromEnum(stage)][slot])
			{
				m_changes &= ~bit;
			}
			else
			{
				m_changes |= bit;
			}
		}
	};
}
