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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr RasterizerState::RasterizerState(
		const TriangleFillMode _triangleFillMode,
		const CullMode _cullMode,
		const bool _antialiasedLine3D,
		const int32 _depthBias
	) noexcept
		: triangleFillMode{ _triangleFillMode }
		, cullMode{ _cullMode }
		, antialiasedLine3D{ _antialiasedLine3D }
		, depthBias{ _depthBias } {}

	constexpr RasterizerState::RasterizerState(const RasterizerStateBuilder& builder) noexcept
		: triangleFillMode{ builder.triangleFillMode() }
		, cullMode{ builder.cullMode() }
		, antialiasedLine3D{ builder.antialiasedLine3D() }
		, depthBias{ builder.depthBias() } {}

	////////////////////////////////////////////////////////////////
	//
	//	asValue
	//
	////////////////////////////////////////////////////////////////

	constexpr RasterizerState::storage_type RasterizerState::asValue() const noexcept
	{
		return std::bit_cast<storage_type>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Invalid
	//
	////////////////////////////////////////////////////////////////

	constexpr RasterizerState RasterizerState::Invalid()
	{
		return RasterizerState{
			TriangleFillMode{ 0 },
			CullMode{ 0 },
			false,
			0
		};
	}
}
