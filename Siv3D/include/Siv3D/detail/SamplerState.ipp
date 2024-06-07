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

	constexpr SamplerState::SamplerState(
		const TextureAddressMode _uAddressMode,
		const TextureAddressMode _vAddressMode,
		const TextureAddressMode _wAddressMode,
		const TextureFilter _minFilter,
		const TextureFilter _magFilter,
		const TextureFilter _mipFilter,
		const uint8 _maxAnisotropy,
		const CompareFunction _compareFunction,
		const Float4 _borderColor,
		const float _minLOD
	) noexcept
		: uAddressMode{ _uAddressMode }
		, vAddressMode{ _vAddressMode }
		, wAddressMode{ _wAddressMode }
		, minFilter{ _minFilter }
		, magFilter{ _magFilter }
		, mipFilter{ _mipFilter }
		, maxAnisotropy{ _maxAnisotropy }
		, compareFunction{ _compareFunction }
		, borderColor{ _borderColor }
		, minLOD{ _minLOD } {}

	constexpr SamplerState::SamplerState(const SamplerStateBuilder& builder) noexcept
		: uAddressMode{ builder.uAddressMode() }
		, vAddressMode{ builder.vAddressMode() }
		, wAddressMode{ builder.wAddressMode() }
		, minFilter{ builder.minFilter() }
		, magFilter{ builder.magFilter() }
		, mipFilter{ builder.mipFilter() }
		, maxAnisotropy{ builder.maxAnisotropy() }
		, compareFunction{ builder.compareFunction() }
		, borderColor{ builder.borderColor() }
		, minLOD{ builder.minLOD() } {}

	////////////////////////////////////////////////////////////////
	//
	//	asValue
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerState::storage_type SamplerState::asValue() const noexcept
	{
		return std::bit_cast<storage_type>(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Invalid
	//
	////////////////////////////////////////////////////////////////

	constexpr SamplerState SamplerState::Invalid()
	{
		return SamplerState{
			TextureAddressMode{ 0 },
			TextureAddressMode{ 0 },
			TextureAddressMode{ 0 },
			TextureFilter{ 0 },
			TextureFilter{ 0 },
			TextureFilter{ 0 },
			0,
			CompareFunction{ 0 },
			Float4{ 0.0f, 0.0f, 0.0f, 0.0f },
			0.0f
		};
	}
}
