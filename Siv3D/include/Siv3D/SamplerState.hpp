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
# include "SamplerStateBuilder.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SamplerState
	//
	////////////////////////////////////////////////////////////////

	/// @brief サンプラーステート
	struct SamplerState
	{
		static constexpr uint32 TextureSlotCount = 16;

		TextureAddressMode uAddressMode	: 4 = TextureAddressMode::Clamp;

		TextureAddressMode vAddressMode	: 4 = TextureAddressMode::Clamp;

		TextureAddressMode wAddressMode	: 4 = TextureAddressMode::Clamp;

		TextureFilter minFilter			: 1 = TextureFilter::Linear;

		TextureFilter magFilter			: 1 = TextureFilter::Linear;

		TextureFilter mipFilter			: 2 = TextureFilter::Linear;

		uint8 maxAnisotropy					= 1;

		CompareFunction compareFunction		= CompareFunction::Never;

		Float4 borderColor					= Float4{ 0.0f, 0.0f, 0.0f, 0.0f };

		float minLOD						= 0.0f;

		using storage_type = std::array<uint32, 6>;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit constexpr SamplerState(
			TextureAddressMode _uAddressMode = TextureAddressMode::Clamp,
			TextureAddressMode _vAddressMode = TextureAddressMode::Clamp,
			TextureAddressMode _wAddressMode = TextureAddressMode::Clamp,
			TextureFilter _minFilter = TextureFilter::Linear,
			TextureFilter _magFilter = TextureFilter::Linear,
			TextureFilter _mipFilter = TextureFilter::Linear,
			uint8 _maxAnisotropy = 1,
			CompareFunction _compareFunction = CompareFunction::Never,
			Float4 _borderColor = Float4{ 0.0f, 0.0f, 0.0f, 0.0f },
			float _minLOD = 0.0f
		) noexcept;

		[[nodiscard]]
		constexpr SamplerState(const SamplerStateBuilder& builder) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asValue
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr storage_type asValue() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(const SamplerState& lhs, const SamplerState& rhs) noexcept
		{
			return (lhs.asValue() == rhs.asValue());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr SamplerState Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const SamplerState& value);
	};

	static_assert(sizeof(SamplerState) == sizeof(SamplerState::storage_type));
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::SamplerState>
{
	[[nodiscard]]
	size_t operator ()(const s3d::SamplerState& value) const noexcept
	{
		return s3d::Hash(&value, sizeof(value));
	}
};

# include "detail/SamplerState.ipp"
