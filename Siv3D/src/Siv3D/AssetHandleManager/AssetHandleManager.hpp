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
# include <memory>
# include <mutex>
# include <Siv3D/Common.hpp>
# include <Siv3D/HashTable.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	template <class IDType, class Data>
	class AssetHandleManager
	{
	public:

		using container_type	= HashTable<IDType, std::unique_ptr<Data>>;
		using iterator			= typename container_type::iterator;
		using const_iterator	= typename container_type::const_iterator;

		[[nodiscard]]
		explicit AssetHandleManager(const std::string& name);

		void setNullData(std::unique_ptr<Data>&& data);

		[[nodiscard]]
		Data* operator [](const IDType id);

		[[nodiscard]]
		IDType add(std::unique_ptr<Data>&& data, const String& info = U"");

		void erase(const IDType id);

		void destroy();

		[[nodiscard]]
		iterator begin();

		[[nodiscard]]
		iterator end();

		[[nodiscard]]
		const_iterator begin() const;

		[[nodiscard]]
		const_iterator end() const;

		[[nodiscard]]
		size_t size() const noexcept;

	private:

		std::string m_assetTypeName;

		std::mutex m_mutex;

		container_type m_data;

		typename IDType::value_type m_idCount = 0;

		bool m_idFilled = false;
	};
}

# include "AssetHandleManager.ipp"
