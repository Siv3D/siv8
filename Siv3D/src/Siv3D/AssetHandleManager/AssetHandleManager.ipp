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

namespace s3d
{
	template <class IDType, class Data>
	AssetHandleManager<IDType, Data>::AssetHandleManager(const std::string& name)
		: m_assetTypeName{ name } {}

	template <class IDType, class Data>
	void AssetHandleManager<IDType, Data>::setNullData(std::unique_ptr<Data>&& data)
	{
		m_data.emplace(IDType::Null(), std::move(data));

		LOG_DEBUG(fmt::format("💠 Created {0}[0(null)]", m_assetTypeName));
	}

	template <class IDType, class Data>
	Data* AssetHandleManager<IDType, Data>::operator [](const IDType id)
	{
		std::lock_guard lock{ m_mutex };

		return m_data[id].get();
	}

	template <class IDType, class Data>
	IDType AssetHandleManager<IDType, Data>::add(std::unique_ptr<Data>&& data, [[maybe_unused]] const String& info)
	{
		std::lock_guard lock{ m_mutex };

		if (++m_idCount == IDType::InvalidID)
		{
			m_idFilled = true;
			m_idCount = 0;
		}

		if (not m_idFilled)
		{
			m_data.emplace(m_idCount, std::move(data));

			LOG_DEBUG(fmt::format("💠 Created {0}[{1}] {2}", m_assetTypeName, m_idCount, info));

			return IDType(m_idCount);
		}
		else
		{
			if (m_data.size() == (IDType::InvalidID - 1))
			{
				LOG_FAIL(fmt::format("❌ No more {0}s can be created", m_assetTypeName));

				return IDType::Null();
			}

			while (++m_idCount < IDType::InvalidID)
			{
				if (m_data.find(IDType(++m_idCount)) == m_data.end())
				{
					m_data.emplace(m_idCount, std::move(data));

					LOG_DEBUG(fmt::format("💠 Created {0}[{1}] {2}", m_assetTypeName, m_idCount, info));

					return IDType(m_idCount);
				}
			}

			return IDType::Null();
		}
	}

	template <class IDType, class Data>
	void AssetHandleManager<IDType, Data>::erase(const IDType id)
	{
		if (id.isNull())
		{
			return;
		}

		std::lock_guard lock{ m_mutex };

		const auto it = m_data.find(id);

		assert(it != m_data.end());

		LOG_DEBUG(fmt::format("♻️ Released {0}[{1}]", m_assetTypeName, id.value()));

		m_data.erase(it);

		//SIV3D_ENGINE(AssetMonitor)->released();
	}

	template <class IDType, class Data>
	void AssetHandleManager<IDType, Data>::destroy()
	{
		std::lock_guard lock{ m_mutex };

		for (const auto& data : m_data)
		{
			if (const auto id = data.first; (not id.isNull()))
			{
				LOG_DEBUG(fmt::format("♻️ Released {0}[{1}]", m_assetTypeName, id.value()));
			}
			else
			{
				LOG_DEBUG(fmt::format("♻️ Released {0}[0(null)]", m_assetTypeName));
			}
		}

		m_data.clear();
	}

	template <class IDType, class Data>
	typename AssetHandleManager<IDType, Data>::iterator AssetHandleManager<IDType, Data>::begin()
	{
		return m_data.begin();
	}

	template <class IDType, class Data>
	typename AssetHandleManager<IDType, Data>::iterator AssetHandleManager<IDType, Data>::end()
	{
		return m_data.end();
	}

	template <class IDType, class Data>
	typename AssetHandleManager<IDType, Data>::const_iterator AssetHandleManager<IDType, Data>::begin() const
	{
		return m_data.cbegin();
	}

	template <class IDType, class Data>
	typename AssetHandleManager<IDType, Data>::const_iterator AssetHandleManager<IDType, Data>::end() const
	{
		return m_data.cend();
	}

	template <class IDType, class Data>
	size_t AssetHandleManager<IDType, Data>::size() const noexcept
	{
		return m_data.size();
	}
}
