﻿//-----------------------------------------------
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
# include "Common.hpp"
# include "AssetID.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	AssetIDWrapper
	//
	////////////////////////////////////////////////////////////////

	template <class AssetTag>
	class AssetIDWrapper
	{
	public:

		using IDType = AssetID<AssetTag>;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		AssetIDWrapper();

		[[nodiscard]]
		explicit AssetIDWrapper(IDType id) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~AssetIDWrapper();

		////////////////////////////////////////////////////////////////
		//
		//	id
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr IDType id() const noexcept;
	
	private:

		AssetID<AssetTag> m_id;
	};
}

# include "detail/AssetIDWrapper.ipp"
