﻿//-----------------------------------------------
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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class AssetTag>
	AssetIDWrapper<AssetTag>::AssetIDWrapper(const IDType id) noexcept
		: m_id{ id } {}

	////////////////////////////////////////////////////////////////
	//
	//	id
	//
	////////////////////////////////////////////////////////////////

	template <class AssetTag>
	constexpr typename AssetIDWrapper<AssetTag>::IDType AssetIDWrapper<AssetTag>::id() const noexcept
	{
		return m_id;
	}
}
