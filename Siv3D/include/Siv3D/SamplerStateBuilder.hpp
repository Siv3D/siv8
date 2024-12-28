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
# include "TextureAddressMode.hpp"
# include "TextureFilter.hpp"
# include "CompareFunction.hpp"
# include "PointVector.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SamplerStateBuilder
	//
	////////////////////////////////////////////////////////////////

	class SamplerStateBuilder
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit constexpr SamplerStateBuilder(
			TextureAddressMode _uAddressMode = TextureAddressMode::Clamp,
			TextureAddressMode _vAddressMode = TextureAddressMode::Clamp,
			TextureAddressMode _wAddressMode = TextureAddressMode::Clamp,
			TextureFilter _minFilter = TextureFilter::Linear,
			TextureFilter _magFilter = TextureFilter::Linear,
			TextureFilter _mipFilter = TextureFilter::Linear,
			uint8 _maxAnisotropy = 1,
			CompareFunction _compareFunction = CompareFunction::Never,
			SampleBorderColor _borderColor = SampleBorderColor::TransparentBlack,
			float _minLOD = 0.0f
		) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	uAddressMode
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder uAddressMode(TextureAddressMode _uAddressMode) const noexcept;

		[[nodiscard]]
		constexpr TextureAddressMode uAddressMode() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	vAddressMode
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder vAddressMode(TextureAddressMode _vAddressMode) const noexcept;

		[[nodiscard]]
		constexpr TextureAddressMode vAddressMode() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	wAddressMode
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder wAddressMode(TextureAddressMode _wAddressMode) const noexcept;

		[[nodiscard]]
		constexpr TextureAddressMode wAddressMode() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minFilter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder minFilter(TextureFilter _minFilter) const noexcept;

		[[nodiscard]]
		constexpr TextureFilter minFilter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	magFilter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder magFilter(TextureFilter _magFilter) const noexcept;

		[[nodiscard]]
		constexpr TextureFilter magFilter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	mipFilter
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder mipFilter(TextureFilter _mipFilter) const noexcept;

		[[nodiscard]]
		constexpr TextureFilter mipFilter() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	maxAnisotropy
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder maxAnisotropy(uint8 _maxAnisotropy) const noexcept;

		[[nodiscard]]
		constexpr uint8 maxAnisotropy() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	compareFunction
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder compareFunction(CompareFunction _compareFunction) const noexcept;

		[[nodiscard]]
		constexpr CompareFunction compareFunction() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	borderColor
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder borderColor(SampleBorderColor _borderColor) const noexcept;

		[[nodiscard]]
		constexpr SampleBorderColor borderColor() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	minLOD
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr SamplerStateBuilder minLOD(float _minLOD) const noexcept;

		[[nodiscard]]
		constexpr float minLOD() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const SamplerStateBuilder& value);

	private:

		TextureAddressMode m_uAddressMode : 4 = TextureAddressMode::Clamp;

		TextureAddressMode m_vAddressMode : 4 = TextureAddressMode::Clamp;

		TextureAddressMode m_wAddressMode : 4 = TextureAddressMode::Clamp;

		TextureFilter m_minFilter : 1 = TextureFilter::Linear;

		TextureFilter m_magFilter : 1 = TextureFilter::Linear;

		TextureFilter m_mipFilter : 2 = TextureFilter::Linear;

		uint8 m_maxAnisotropy = 1;

		CompareFunction m_compareFunction	: 4 = CompareFunction::Never;

		SampleBorderColor m_borderColor		: 4 = SampleBorderColor::TransparentBlack;

		float m_minLOD = 0.0f;
	};
}

# include "detail/SamplerStateBuilder.ipp"
