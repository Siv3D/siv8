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
	constexpr BlendStateBuilder::BlendStateBuilder(
		const bool _enable,
		const BlendFactor _sourceRGB,
		const BlendFactor _destinationRGB,
		const BlendOperation _rgbOperation,
		const BlendFactor _sourceAlpha,
		const BlendFactor _destinationAlpha,
		const BlendOperation _alphaOperation,
		const bool _alphaToCoverageEnable,
		const bool _writeR,
		const bool _writeG,
		const bool _writeB,
		const bool _writeA
	) noexcept
		: m_enabled{ _enable }
		, m_writeR{ _writeR }
		, m_writeG{ _writeG }
		, m_sourceRGB{ _sourceRGB }
		, m_destinationRGB{ _destinationRGB }
		, m_rgbOperation{ _rgbOperation }
		, m_alphaToCoverageEnabled{ _alphaToCoverageEnable }
		, m_writeB{ _writeB }
		, m_writeA{ _writeA }
		, m_sourceAlpha{ _sourceAlpha }
		, m_destinationAlpha{ _destinationAlpha }
		, m_alphaOperation{ _alphaOperation } {}

	constexpr BlendStateBuilder BlendStateBuilder::enabled(const bool _enabled) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_enabled = _enabled;
		return *this;
	}

	constexpr bool BlendStateBuilder::enabled() const noexcept
	{
		return m_enabled;
	}

	constexpr BlendStateBuilder BlendStateBuilder::sourceRGB(const BlendFactor _sourceRGB) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_sourceRGB = _sourceRGB;
		return *this;
	}

	constexpr BlendFactor BlendStateBuilder::sourceRGB() const noexcept
	{
		return m_sourceRGB;
	}

	constexpr BlendStateBuilder BlendStateBuilder::destinationRGB(const BlendFactor _destinationRGB) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_destinationRGB = _destinationRGB;
		return *this;
	}

	constexpr BlendFactor BlendStateBuilder::destinationRGB() const noexcept
	{
		return m_destinationRGB;
	}

	constexpr BlendStateBuilder BlendStateBuilder::rgbOperation(const BlendOperation _rgbOperation) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_rgbOperation = _rgbOperation;
		return *this;
	}

	constexpr BlendOperation BlendStateBuilder::rgbOperation() const noexcept
	{
		return m_rgbOperation;
	}

	constexpr BlendStateBuilder BlendStateBuilder::sourceAlpha(const BlendFactor _sourceAlpha) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_sourceAlpha = _sourceAlpha;
		return *this;
	}

	constexpr BlendFactor BlendStateBuilder::sourceAlpha() const noexcept
	{
		return m_sourceAlpha;
	}

	constexpr BlendStateBuilder BlendStateBuilder::destinationAlpha(const BlendFactor _destinationAlpha) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_destinationAlpha = _destinationAlpha;
		return *this;
	}

	constexpr BlendFactor BlendStateBuilder::destinationAlpha() const noexcept
	{
		return m_destinationAlpha;
	}

	constexpr BlendStateBuilder BlendStateBuilder::alphaOperation(const BlendOperation _alphaOperation) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_alphaOperation = _alphaOperation;
		return *this;
	}

	constexpr BlendOperation BlendStateBuilder::alphaOperation() const noexcept
	{
		return m_alphaOperation;
	}

	constexpr BlendStateBuilder BlendStateBuilder::alphaToCoverageEnabled(const bool _alphaToCoverageEnabled) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_alphaToCoverageEnabled = _alphaToCoverageEnabled;
		return *this;
	}

	constexpr bool BlendStateBuilder::alphaToCoverageEnabled() const noexcept
	{
		return m_alphaToCoverageEnabled;
	}

	constexpr BlendStateBuilder BlendStateBuilder::writeR(const bool _writeR) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeR = _writeR;
		return *this;
	}

	constexpr bool BlendStateBuilder::writeR() const noexcept
	{
		return m_writeR;
	}

	constexpr BlendStateBuilder BlendStateBuilder::writeG(const bool _writeG) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeG = _writeG;
		return *this;
	}

	constexpr bool BlendStateBuilder::writeG() const noexcept
	{
		return m_writeG;
	}

	constexpr BlendStateBuilder BlendStateBuilder::writeB(const bool _writeB) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeB = _writeB;
		return *this;
	}

	constexpr bool BlendStateBuilder::writeB() const noexcept
	{
		return m_writeB;
	}

	constexpr BlendStateBuilder BlendStateBuilder::writeA(const bool _writeA) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeA = _writeA;
		return *this;
	}

	constexpr bool BlendStateBuilder::writeA() const noexcept
	{
		return m_writeA;
	}
}
