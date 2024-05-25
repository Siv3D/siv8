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
# include "CompareFunction.hpp"
# include "DepthStencilStateBuilder.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DepthStencilState
	//
	////////////////////////////////////////////////////////////////

	/// @brief デプス・ステンシルステート
	struct DepthStencilState
	{
		bool depthEnabled						: 1	= false;

		bool depthWriteEnabled					: 1	= false;

		CompareFunction depthCompareFunction	: 4 = CompareFunction::Always;

		uint8 _unused							: 2 = 0;

		using storage_type = uint8;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit constexpr DepthStencilState(
			bool _depthEnabled = false,
			bool _depthWriteEnabled = false,
			CompareFunction _depthCompareFunction = CompareFunction::Always
		) noexcept;

		[[nodiscard]]
		constexpr DepthStencilState(const DepthStencilStateBuilder& builder) noexcept;

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
		friend constexpr bool operator ==(const DepthStencilState& lhs, const DepthStencilState& rhs) noexcept
		{
			return (lhs.asValue() == rhs.asValue());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr DepthStencilState Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const DepthStencilState& value);
	};

	static_assert(sizeof(DepthStencilState) == sizeof(DepthStencilState::storage_type));
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::DepthStencilState>
{
	[[nodiscard]]
	size_t operator ()(const s3d::DepthStencilState& value) const noexcept
	{
		return hash<s3d::DepthStencilState::storage_type>()(value.asValue());
	}
};

# include "detail/DepthStencilState.ipp"
