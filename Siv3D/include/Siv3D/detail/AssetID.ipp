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
	template <class AssetTag>
	constexpr AssetID<AssetTag>::AssetID(const value_type id) noexcept
		: m_value{ id } {}

	template <class AssetTag>
	constexpr typename AssetID<AssetTag>::value_type AssetID<AssetTag>::value() const noexcept
	{
		return m_value;
	}

	template <class AssetTag>
	constexpr bool AssetID<AssetTag>::isNull() const noexcept
	{
		return (m_value == NullID);
	}

	template <class AssetTag>
	constexpr bool AssetID<AssetTag>::isInvalid() const noexcept
	{
		return (m_value == InvalidID);
	}

	template <class AssetTag>
	constexpr AssetID<AssetTag> AssetID<AssetTag>::Null() noexcept
	{
		return AssetID{ NullID };
	}

	template <class AssetTag>
	constexpr AssetID<AssetTag> AssetID<AssetTag>::Invalid() noexcept
	{
		return AssetID{ InvalidID };
	}
}
