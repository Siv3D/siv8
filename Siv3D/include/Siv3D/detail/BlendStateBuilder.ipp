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

	constexpr BlendStateBuilder::BlendStateBuilder(
		const bool _enable,
		const BlendFactor _sourceRGB,
		const BlendFactor _destinationRGB,
		const BlendOperation _rgbOperation,
		const BlendFactor _sourceAlpha,
		const BlendFactor _destinationAlpha,
		const BlendOperation _alphaOperation,
		const bool _alphaToCoverageEnable,
		const ColorWriteMask colorWriteMask
	) noexcept
		: m_enabled{ _enable }
		, m_writeR{ static_cast<bool>(colorWriteMask & ColorWriteMask::Red) }
		, m_writeG{ static_cast<bool>(colorWriteMask & ColorWriteMask::Green) }
		, m_sourceRGB{ _sourceRGB }
		, m_destinationRGB{ _destinationRGB }
		, m_rgbOperation{ _rgbOperation }
		, m_alphaToCoverageEnabled{ _alphaToCoverageEnable }
		, m_writeB{ static_cast<bool>(colorWriteMask & ColorWriteMask::Blue) }
		, m_writeA{ static_cast<bool>(colorWriteMask & ColorWriteMask::Alpha) }
		, m_sourceAlpha{ _sourceAlpha }
		, m_destinationAlpha{ _destinationAlpha }
		, m_alphaOperation{ _alphaOperation } {}

	////////////////////////////////////////////////////////////////
	//
	//	enabled
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::enabled(const bool _enabled) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_enabled = _enabled;
		return result;
	}

	constexpr bool BlendStateBuilder::enabled() const noexcept
	{
		return m_enabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sourceRGB
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::sourceRGB(const BlendFactor _sourceRGB) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_sourceRGB = _sourceRGB;
		return result;
	}

	constexpr BlendFactor BlendStateBuilder::sourceRGB() const noexcept
	{
		return m_sourceRGB;
	}

	////////////////////////////////////////////////////////////////
	//
	//	destinationRGB
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::destinationRGB(const BlendFactor _destinationRGB) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_destinationRGB = _destinationRGB;
		return result;
	}

	constexpr BlendFactor BlendStateBuilder::destinationRGB() const noexcept
	{
		return m_destinationRGB;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rgbOperation
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::rgbOperation(const BlendOperation _rgbOperation) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_rgbOperation = _rgbOperation;
		return result;
	}

	constexpr BlendOperation BlendStateBuilder::rgbOperation() const noexcept
	{
		return m_rgbOperation;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sourceAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::sourceAlpha(const BlendFactor _sourceAlpha) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_sourceAlpha = _sourceAlpha;
		return result;
	}

	constexpr BlendFactor BlendStateBuilder::sourceAlpha() const noexcept
	{
		return m_sourceAlpha;
	}

	////////////////////////////////////////////////////////////////
	//
	//	destinationAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::destinationAlpha(const BlendFactor _destinationAlpha) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_destinationAlpha = _destinationAlpha;
		return result;
	}

	constexpr BlendFactor BlendStateBuilder::destinationAlpha() const noexcept
	{
		return m_destinationAlpha;
	}

	////////////////////////////////////////////////////////////////
	//
	//	alphaOperation
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::alphaOperation(const BlendOperation _alphaOperation) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_alphaOperation = _alphaOperation;
		return result;
	}

	constexpr BlendOperation BlendStateBuilder::alphaOperation() const noexcept
	{
		return m_alphaOperation;
	}

	////////////////////////////////////////////////////////////////
	//
	//	alphaToCoverageEnabled
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::alphaToCoverageEnabled(const bool _alphaToCoverageEnabled) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_alphaToCoverageEnabled = _alphaToCoverageEnabled;
		return result;
	}

	constexpr bool BlendStateBuilder::alphaToCoverageEnabled() const noexcept
	{
		return m_alphaToCoverageEnabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeR
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::writeR(const bool _writeR) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeR = _writeR;
		return result;
	}

	constexpr bool BlendStateBuilder::writeR() const noexcept
	{
		return m_writeR;
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeG
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::writeG(const bool _writeG) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeG = _writeG;
		return result;
	}

	constexpr bool BlendStateBuilder::writeG() const noexcept
	{
		return m_writeG;
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeB
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::writeB(const bool _writeB) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeB = _writeB;
		return result;
	}

	constexpr bool BlendStateBuilder::writeB() const noexcept
	{
		return m_writeB;
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeA
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::writeA(const bool _writeA) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeA = _writeA;
		return result;
	}

	constexpr bool BlendStateBuilder::writeA() const noexcept
	{
		return m_writeA;
	}

	////////////////////////////////////////////////////////////////
	//
	//	colorWriteMask
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendStateBuilder BlendStateBuilder::colorWriteMask(const ColorWriteMask colorWriteMask) const noexcept
	{
		BlendStateBuilder result{ *this };
		result.m_writeR = static_cast<bool>(colorWriteMask & ColorWriteMask::Red);
		result.m_writeG = static_cast<bool>(colorWriteMask & ColorWriteMask::Green);
		result.m_writeB = static_cast<bool>(colorWriteMask & ColorWriteMask::Blue);
		result.m_writeA = static_cast<bool>(colorWriteMask & ColorWriteMask::Alpha);
		return result;
	}

	constexpr ColorWriteMask BlendStateBuilder::colorWriteMask() const noexcept
	{
		ColorWriteMask mask{};

		if (m_writeR)
		{
			mask |= ColorWriteMask::Red;
		}

		if (m_writeG)
		{
			mask |= ColorWriteMask::Green;
		}

		if (m_writeB)
		{
			mask |= ColorWriteMask::Blue;
		}

		if (m_writeA)
		{
			mask |= ColorWriteMask::Alpha;
		}

		return mask;
	}
}
