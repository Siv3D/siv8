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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr DepthStencilState::DepthStencilState(
		const bool _depthTestEnabled,
		const bool _depthWriteEnabled,
		const CompareFunction _depthCompareFunction
	) noexcept
		: depthTestEnabled{ _depthTestEnabled }
		, depthWriteEnabled{ _depthWriteEnabled }
		, depthCompareFunction{ _depthCompareFunction } {}

	constexpr DepthStencilState::DepthStencilState(const DepthStencilStateBuilder& builder) noexcept
		: depthTestEnabled{ builder.depthTestEnabled() }
		, depthWriteEnabled{ builder.depthWriteEnabled() }
		, depthCompareFunction{ builder.depthCompareFunction() } {}

	////////////////////////////////////////////////////////////////
	//
	//	asValue
	//
	////////////////////////////////////////////////////////////////

	constexpr DepthStencilState::storage_type DepthStencilState::asValue() const noexcept
	{
		return std::bit_cast<storage_type>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Invalid
	//
	////////////////////////////////////////////////////////////////

	constexpr DepthStencilState DepthStencilState::Invalid()
	{
		return DepthStencilState{
			false,
			false,
			CompareFunction{ 0 }
		};
	}
}
