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
# include <array>
# include <bit>
# include "Common.hpp"
# include "TextureAddressMode.hpp"
# include "TextureFilter.hpp"
# include "CompareFunction.hpp"
# include "SampleBorderColor.hpp"
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
		TextureAddressMode uAddressMode	: 4 = TextureAddressMode::Clamp;

		TextureAddressMode vAddressMode	: 4 = TextureAddressMode::Clamp;

		TextureAddressMode wAddressMode	: 4 = TextureAddressMode::Clamp;

		TextureFilter minFilter			: 1 = TextureFilter::Linear;

		TextureFilter magFilter			: 1 = TextureFilter::Linear;

		TextureFilter mipFilter			: 2 = TextureFilter::Linear;

		uint8 maxAnisotropy					= 1;

		CompareFunction compareFunction	: 4 = CompareFunction::Never;

		SampleBorderColor borderColor	: 4	= SampleBorderColor::TransparentBlack;

		float minLOD						= 0.0f;

		using storage_type = uint64;

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
			SampleBorderColor _borderColor = SampleBorderColor::TransparentBlack,
			float _minLOD = 0.0f
		) noexcept;

		[[nodiscard]]
		constexpr SamplerState(const SamplerStateBuilder& builder) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(constant)
		//
		////////////////////////////////////////////////////////////////

		static constexpr SamplerStateBuilder RepeatNearest{
			TextureAddressMode::Repeat, TextureAddressMode::Repeat, TextureAddressMode::Repeat,
			TextureFilter::Nearest, TextureFilter::Nearest, TextureFilter::Nearest };

		static constexpr SamplerStateBuilder RepeatLinear{
			TextureAddressMode::Repeat, TextureAddressMode::Repeat, TextureAddressMode::Repeat,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear };

		static constexpr SamplerStateBuilder RepeatAnisotropic{
			TextureAddressMode::Repeat, TextureAddressMode::Repeat, TextureAddressMode::Repeat,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear, 4 };

		static constexpr SamplerStateBuilder MirrorNearest{
			TextureAddressMode::Mirror, TextureAddressMode::Mirror, TextureAddressMode::Mirror,
			TextureFilter::Nearest, TextureFilter::Nearest, TextureFilter::Nearest };

		static constexpr SamplerStateBuilder MirrorLinear{
			TextureAddressMode::Mirror, TextureAddressMode::Mirror, TextureAddressMode::Mirror,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear };

		static constexpr SamplerStateBuilder MirrorAnisotropic{
			TextureAddressMode::Mirror, TextureAddressMode::Mirror, TextureAddressMode::Mirror,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear, 4 };

		static constexpr SamplerStateBuilder ClampNearest{
			TextureAddressMode::Clamp, TextureAddressMode::Clamp, TextureAddressMode::Clamp,
			TextureFilter::Nearest, TextureFilter::Nearest, TextureFilter::Nearest };

		static constexpr SamplerStateBuilder ClampLinear{
			TextureAddressMode::Clamp, TextureAddressMode::Clamp, TextureAddressMode::Clamp,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear };

		static constexpr SamplerStateBuilder ClampAnisotropic{
			TextureAddressMode::Clamp, TextureAddressMode::Clamp, TextureAddressMode::Clamp,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear, 4 };

		static constexpr SamplerStateBuilder BorderNearest{
			TextureAddressMode::BorderColor, TextureAddressMode::BorderColor, TextureAddressMode::BorderColor,
			TextureFilter::Nearest, TextureFilter::Nearest, TextureFilter::Nearest };

		static constexpr SamplerStateBuilder BorderLinear{
			TextureAddressMode::BorderColor, TextureAddressMode::BorderColor, TextureAddressMode::BorderColor,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear };

		static constexpr SamplerStateBuilder BorderAnisotropic{
			TextureAddressMode::BorderColor, TextureAddressMode::BorderColor, TextureAddressMode::BorderColor,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear, 4 };

		static constexpr SamplerStateBuilder MirrorClampNearest{
			TextureAddressMode::MirrorClamp, TextureAddressMode::MirrorClamp, TextureAddressMode::MirrorClamp,
			TextureFilter::Nearest, TextureFilter::Nearest, TextureFilter::Nearest };

		static constexpr SamplerStateBuilder MirrorClampLinear{
			TextureAddressMode::MirrorClamp, TextureAddressMode::MirrorClamp, TextureAddressMode::MirrorClamp,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear };

		static constexpr SamplerStateBuilder MirrorClampAnisotropic{
			TextureAddressMode::MirrorClamp, TextureAddressMode::MirrorClamp, TextureAddressMode::MirrorClamp,
			TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear, 4 };

		static constexpr SamplerStateBuilder Default2D = ClampLinear;

		static constexpr SamplerStateBuilder Default3D = RepeatAnisotropic;

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
