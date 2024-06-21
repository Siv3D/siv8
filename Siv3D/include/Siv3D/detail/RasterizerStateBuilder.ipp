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
	constexpr RasterizerStateBuilder::RasterizerStateBuilder(
		const TriangleFillMode _triangleFillMode,
		const CullMode _cullMode,
		const bool _antialiasedLine3D,
		const int32 _depthBias
	) noexcept
		: m_triangleFillMode{ _triangleFillMode }
		, m_cullMode{ _cullMode }
		, m_antialiasedLine3D{ _antialiasedLine3D }
		, m_depthBias{ _depthBias } {}

	constexpr RasterizerStateBuilder RasterizerStateBuilder::triangleFillMode(const TriangleFillMode _triangleFillMode) const noexcept
	{
		RasterizerStateBuilder result{ *this };
		result.m_triangleFillMode = _triangleFillMode;
		return result;
	}

	constexpr TriangleFillMode RasterizerStateBuilder::triangleFillMode() const noexcept
	{
		return m_triangleFillMode;
	}

	constexpr RasterizerStateBuilder RasterizerStateBuilder::cullMode(const CullMode _cullMode) const noexcept
	{
		RasterizerStateBuilder result{ *this };
		result.m_cullMode = _cullMode;
		return result;
	}

	constexpr CullMode RasterizerStateBuilder::cullMode() const noexcept
	{
		return m_cullMode;
	}

	constexpr RasterizerStateBuilder RasterizerStateBuilder::antialiasedLine3D(const bool _antialiasedLine3D) const noexcept
	{
		RasterizerStateBuilder result{ *this };
		result.m_antialiasedLine3D = _antialiasedLine3D;
		return result;
	}

	constexpr bool RasterizerStateBuilder::antialiasedLine3D() const noexcept
	{
		return m_antialiasedLine3D;
	}

	constexpr RasterizerStateBuilder RasterizerStateBuilder::depthBias(const int32 _depthBias) const noexcept
	{
		RasterizerStateBuilder result{ *this };
		result.m_depthBias = _depthBias;
		return result;
	}

	constexpr int32 RasterizerStateBuilder::depthBias() const noexcept
	{
		return m_depthBias;
	}
}
