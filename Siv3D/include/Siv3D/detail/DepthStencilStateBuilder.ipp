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
	constexpr DepthStencilStateBuilder::DepthStencilStateBuilder(
		const bool _depthEnabled,
		const bool _depthWriteEnabled,
		const CompareFunction _depthCompareFunction
	) noexcept
		: m_depthEnabled{ _depthEnabled }
		, m_depthWriteEnabled{ _depthWriteEnabled }
		, m_depthCompareFunction{ _depthCompareFunction } {}

	constexpr DepthStencilStateBuilder DepthStencilStateBuilder::depthEnabled(const bool enabled) const noexcept
	{
		return DepthStencilStateBuilder{ enabled, m_depthWriteEnabled, m_depthCompareFunction };
	}

	constexpr bool DepthStencilStateBuilder::depthEnabled() const noexcept
	{
		return m_depthEnabled;
	}

	constexpr DepthStencilStateBuilder DepthStencilStateBuilder::depthWriteEnabled(const bool enabled) const noexcept
	{
		return DepthStencilStateBuilder{ m_depthEnabled, enabled, m_depthCompareFunction };
	}

	constexpr bool DepthStencilStateBuilder::depthWriteEnabled() const noexcept
	{
		return m_depthWriteEnabled;
	}

	constexpr DepthStencilStateBuilder DepthStencilStateBuilder::depthCompareFunction(const CompareFunction function) const noexcept
	{
		return DepthStencilStateBuilder{ m_depthEnabled, m_depthWriteEnabled, function };
	}

	constexpr CompareFunction DepthStencilStateBuilder::depthCompareFunction() const noexcept
	{
		return m_depthCompareFunction;
	}
}
