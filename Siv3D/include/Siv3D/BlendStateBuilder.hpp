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
# include "Common.hpp"
# include "BlendFactor.hpp"
# include "BlendOperation.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BlendStateBuilder
	//
	////////////////////////////////////////////////////////////////

	class BlendStateBuilder
	{
	public:

		[[nodiscard]]
		explicit constexpr BlendStateBuilder(
			bool _enable = true,
			BlendFactor _sourceRGB = BlendFactor::One,
			BlendFactor _destinationRGB = BlendFactor::OneMinusSourceAlpha,
			BlendOperation _rgbOperation = BlendOperation::Add,
			BlendFactor _sourceAlpha = BlendFactor::Zero,
			BlendFactor _destinationAlpha = BlendFactor::One,
			BlendOperation _alphaOperation = BlendOperation::Add,
			bool _alphaToCoverageEnable = false,
			bool _writeR = true,
			bool _writeG = true,
			bool _writeB = true,
			bool _writeA = true
		) noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder enabled(bool _enabled) const noexcept;

		[[nodiscard]]
		constexpr bool enabled() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder sourceRGB(BlendFactor _sourceRGB) const noexcept;

		[[nodiscard]]
		constexpr BlendFactor sourceRGB() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder destinationRGB(BlendFactor _destinationRGB) const noexcept;

		[[nodiscard]]
		constexpr BlendFactor destinationRGB() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder rgbOperation(BlendOperation _rgbOperation) const noexcept;

		[[nodiscard]]
		constexpr BlendOperation rgbOperation() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder sourceAlpha(BlendFactor _sourceAlpha) const noexcept;

		[[nodiscard]]
		constexpr BlendFactor sourceAlpha() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder destinationAlpha(BlendFactor _destinationAlpha) const noexcept;

		[[nodiscard]]
		constexpr BlendFactor destinationAlpha() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder alphaOperation(BlendOperation _alphaOperation) const noexcept;

		[[nodiscard]]
		constexpr BlendOperation alphaOperation() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder alphaToCoverageEnabled(bool _alphaToCoverageEnabled) const noexcept;

		[[nodiscard]]
		constexpr bool alphaToCoverageEnabled() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder writeR(bool _writeR) const noexcept;

		[[nodiscard]]
		constexpr bool writeR() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder writeG(bool _writeG) const noexcept;

		[[nodiscard]]
		constexpr bool writeG() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder writeB(bool _writeB) const noexcept;

		[[nodiscard]]
		constexpr bool writeB() const noexcept;

		[[nodiscard]]
		constexpr BlendStateBuilder writeA(bool _writeA) const noexcept;

		[[nodiscard]]
		constexpr bool writeA() const noexcept;

	private:

		bool m_enabled : 1 = true;

		bool m_writeR : 1 = true;

		bool m_writeG : 1 = true;

		BlendFactor m_sourceRGB : 5 = BlendFactor::One;

		BlendFactor m_destinationRGB : 5 = BlendFactor::OneMinusSourceAlpha;

		BlendOperation m_rgbOperation : 3 = BlendOperation::Add;

		bool m_alphaToCoverageEnabled : 1 = false;

		bool m_writeB : 1 = true;

		bool m_writeA : 1 = true;

		BlendFactor m_sourceAlpha : 5 = BlendFactor::Zero;

		BlendFactor m_destinationAlpha : 5 = BlendFactor::One;

		BlendOperation m_alphaOperation : 3 = BlendOperation::Add;
	};
}

# include "detail/BlendStateBuilder.ipp"
