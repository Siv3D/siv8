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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class AssetType>
	AssetHandle<AssetType>::AssetHandle()
		: m_handle{ std::make_shared<AssetIDWrapperType>() } {}

	template <class AssetType>
	AssetHandle<AssetType>::AssetHandle(std::shared_ptr<AssetIDWrapperType>&& id)
		: m_handle{ std::move(id) } {}

	////////////////////////////////////////////////////////////////
	//
	//	id
	//
	////////////////////////////////////////////////////////////////

	template <class AssetType>
	typename AssetHandle<AssetType>::IDType AssetHandle<AssetType>::id() const noexcept
	{
		return m_handle->id();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	template <class AssetType>
	bool AssetHandle<AssetType>::isEmpty() const noexcept
	{
		return m_handle->id().isNull();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	template <class AssetType>
	AssetHandle<AssetType>::operator bool() const noexcept
	{
		return (not m_handle->id().isNull());
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	template <class AssetType>
	void AssetHandle<AssetType>::release()
	{
		m_handle = std::make_shared<AssetIDWrapperType>();
	}
}
