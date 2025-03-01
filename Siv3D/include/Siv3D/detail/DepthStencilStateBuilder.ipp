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
	constexpr DepthStencilStateBuilder::DepthStencilStateBuilder(
		const bool _depthTestEnabled,
		const bool _depthWriteEnabled,
		const CompareFunction _depthCompareFunction
	) noexcept
		: m_depthTestEnabled{ _depthTestEnabled }
		, m_depthWriteEnabled{ _depthWriteEnabled }
		, m_depthCompareFunction{ _depthCompareFunction } {}

	constexpr DepthStencilStateBuilder DepthStencilStateBuilder::depthTestEnabled(const bool enabled) const noexcept
	{
		return DepthStencilStateBuilder{ enabled, m_depthWriteEnabled, m_depthCompareFunction };
	}

	constexpr bool DepthStencilStateBuilder::depthTestEnabled() const noexcept
	{
		return m_depthTestEnabled;
	}

	constexpr DepthStencilStateBuilder DepthStencilStateBuilder::depthWriteEnabled(const bool enabled) const noexcept
	{
		return DepthStencilStateBuilder{ m_depthTestEnabled, enabled, m_depthCompareFunction };
	}

	constexpr bool DepthStencilStateBuilder::depthWriteEnabled() const noexcept
	{
		return m_depthWriteEnabled;
	}

	constexpr DepthStencilStateBuilder DepthStencilStateBuilder::depthCompareFunction(const CompareFunction function) const noexcept
	{
		return DepthStencilStateBuilder{ m_depthTestEnabled, m_depthWriteEnabled, function };
	}

	constexpr CompareFunction DepthStencilStateBuilder::depthCompareFunction() const noexcept
	{
		return m_depthCompareFunction;
	}
}
