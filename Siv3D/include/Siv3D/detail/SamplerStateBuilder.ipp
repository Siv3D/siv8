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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder::SamplerStateBuilder(
		const TextureAddressMode _uAddressMode,
		const TextureAddressMode _vAddressMode,
		const TextureAddressMode _wAddressMode,
		const TextureFilter _minFilter,
		const TextureFilter _magFilter,
		const TextureFilter _mipFilter,
		const uint8 _maxAnisotropy,
		const CompareFunction _compareFunction,
		const SampleBorderColor _borderColor,
		const float _minLOD
	) noexcept
		: m_uAddressMode{ _uAddressMode }
		, m_vAddressMode{ _vAddressMode }
		, m_wAddressMode{ _wAddressMode }
		, m_minFilter{ _minFilter }
		, m_magFilter{ _magFilter }
		, m_mipFilter{ _mipFilter }
		, m_maxAnisotropy{ _maxAnisotropy }
		, m_compareFunction{ _compareFunction }
		, m_borderColor{ _borderColor }
		, m_minLOD{ _minLOD } {}

	////////////////////////////////////////////////////////////////
	//
	//	uAddressMode
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::uAddressMode(const TextureAddressMode _uAddressMode) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_uAddressMode = _uAddressMode;
		return result;
	}

	constexpr TextureAddressMode SamplerStateBuilder::uAddressMode() const noexcept
	{
		return m_uAddressMode;
	}

	////////////////////////////////////////////////////////////////
	//
	//	vAddressMode
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::vAddressMode(const TextureAddressMode _vAddressMode) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_vAddressMode = _vAddressMode;
		return result;
	}

	constexpr TextureAddressMode SamplerStateBuilder::vAddressMode() const noexcept
	{
		return m_vAddressMode;
	}

	////////////////////////////////////////////////////////////////
	//
	//	wAddressMode
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::wAddressMode(const TextureAddressMode _wAddressMode) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_wAddressMode = _wAddressMode;
		return result;
	}

	constexpr TextureAddressMode SamplerStateBuilder::wAddressMode() const noexcept
	{
		return m_wAddressMode;
	}

	////////////////////////////////////////////////////////////////
	//
	//	minFilter
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::minFilter(const TextureFilter _minFilter) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_minFilter = _minFilter;
		return result;
	}

	constexpr TextureFilter SamplerStateBuilder::minFilter() const noexcept
	{
		return m_minFilter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	magFilter
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::magFilter(const TextureFilter _magFilter) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_magFilter = _magFilter;
		return result;
	}

	constexpr TextureFilter SamplerStateBuilder::magFilter() const noexcept
	{
		return m_magFilter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	mipFilter
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::mipFilter(const TextureFilter _mipFilter) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_mipFilter = _mipFilter;
		return result;
	}

	constexpr TextureFilter SamplerStateBuilder::mipFilter() const noexcept
	{
		return m_mipFilter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	maxAnisotropy
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::maxAnisotropy(const uint8 _maxAnisotropy) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_maxAnisotropy = _maxAnisotropy;
		return result;
	}

	constexpr uint8 SamplerStateBuilder::maxAnisotropy() const noexcept
	{
		return m_maxAnisotropy;
	}

	////////////////////////////////////////////////////////////////
	//
	//	compareFunction
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::compareFunction(const CompareFunction _compareFunction) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_compareFunction = _compareFunction;
		return result;
	}

	constexpr CompareFunction SamplerStateBuilder::compareFunction() const noexcept
	{
		return m_compareFunction;
	}

	////////////////////////////////////////////////////////////////
	//
	//	borderColor
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::borderColor(const SampleBorderColor _borderColor) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_borderColor = _borderColor;
		return result;
	}

	constexpr SampleBorderColor SamplerStateBuilder::borderColor() const noexcept
	{
		return m_borderColor;
	}

	////////////////////////////////////////////////////////////////
	//
	//	minLOD
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerStateBuilder SamplerStateBuilder::minLOD(const float _minLOD) const noexcept
	{
		SamplerStateBuilder result{ *this };
		result.m_minLOD = _minLOD;
		return result;
	}

	constexpr float SamplerStateBuilder::minLOD() const noexcept
	{
		return m_minLOD;
	}
}
