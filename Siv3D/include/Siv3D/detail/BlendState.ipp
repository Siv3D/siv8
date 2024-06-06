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

	constexpr BlendState::BlendState(
		const bool _enabled,
		const BlendFactor _sourceRGB,
		const BlendFactor _destinationRGB,
		const BlendOperation _rgbOperation,
		const BlendFactor _sourceAlpha,
		const BlendFactor _destinationAlpha,
		const BlendOperation _alphaOperation,
		const bool _alphaToCoverageEnabled,
		const bool _writeR,
		const bool _writeG,
		const bool _writeB,
		const bool _writeA
	) noexcept
		: enabled{ _enabled }
		, writeR{ _writeR }
		, writeG{ _writeG }
		, sourceRGB{ _sourceRGB }
		, destinationRGB{ _destinationRGB }
		, rgbOperation{ _rgbOperation }
		, alphaToCoverageEnabled{ _alphaToCoverageEnabled }
		, writeB{ _writeB }
		, writeA{ _writeA }
		, sourceAlpha{ _sourceAlpha }
		, destinationAlpha{ _destinationAlpha }
		, alphaOperation{ _alphaOperation } {}

	constexpr BlendState::BlendState(const BlendStateBuilder& builder) noexcept
		: enabled{ builder.enabled() }
		, writeR{ builder.writeR() }
		, writeG{ builder.writeG() }
		, sourceRGB{ builder.sourceRGB() }
		, destinationRGB{ builder.destinationRGB() }
		, rgbOperation{ builder.rgbOperation() }
		, alphaToCoverageEnabled{ builder.alphaToCoverageEnabled() }
		, writeB{ builder.writeB() }
		, writeA{ builder.writeA() }
		, sourceAlpha{ builder.sourceAlpha() }
		, destinationAlpha{ builder.destinationAlpha() }
		, alphaOperation{ builder.alphaOperation() } {}

	////////////////////////////////////////////////////////////////
	//
	//	setColorWriteMask
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendState& BlendState::setColorWriteMask(const bool r, const bool g, const bool b, const bool a) noexcept
	{
		writeR = r;
		writeG = g;
		writeB = b;
		writeA = a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	asValue
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendState::storage_type BlendState::asValue() const noexcept
	{
		return std::bit_cast<storage_type>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Invalid
	//
	////////////////////////////////////////////////////////////////

	constexpr BlendState BlendState::Invalid()
	{
		return BlendState{
			false,
			BlendFactor{ 0 },
			BlendFactor{ 0 },
			BlendOperation{ 0 },
			BlendFactor{ 0 },
			BlendFactor{ 0 },
			BlendOperation{ 0 },
			false,
			false,
			false,
			false,
			false
		};
	}
}
