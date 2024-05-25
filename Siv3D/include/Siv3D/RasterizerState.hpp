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
# include "TriangleFillMode.hpp"
# include "CullMode.hpp"
# include "RasterizerStateBuilder.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RasterizerState
	//
	////////////////////////////////////////////////////////////////

	/// @brief ラスタライザーステート
	struct RasterizerState
	{
		TriangleFillMode triangleFillMode	: 2 = TriangleFillMode::Solid;

		CullMode cullMode					: 2 = CullMode::Back;

		bool scissorEnabled					: 2 = false;

		bool antialiasedLine3D				: 2 = false;

		int32 depthBias							= 0;

		using storage_type = uint64;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit constexpr RasterizerState(
			TriangleFillMode _triangleFillMode = TriangleFillMode::Solid,
			CullMode _cullMode = CullMode::Back,
			bool _scissorEnabled = false,
			bool _antialiasedLine3D = false,
			int32 _depthBias = 0
		) noexcept;

		[[nodiscard]]
		constexpr RasterizerState(const RasterizerStateBuilder& builder) noexcept;

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
		friend constexpr bool operator ==(const RasterizerState& lhs, const RasterizerState& rhs) noexcept
		{
			return (lhs.asValue() == rhs.asValue());
		}

		static constexpr RasterizerStateBuilder SolidCullBack{ TriangleFillMode::Solid, CullMode::Back };

		static constexpr RasterizerStateBuilder SolidCullFront{ TriangleFillMode::Solid, CullMode::Front };

		static constexpr RasterizerStateBuilder SolidCullNone{ TriangleFillMode::Solid, CullMode::None };

		static constexpr RasterizerStateBuilder WireframeCullBack{ TriangleFillMode::Wireframe, CullMode::Back };

		static constexpr RasterizerStateBuilder WireframeCullFront{ TriangleFillMode::Wireframe, CullMode::Front };

		static constexpr RasterizerStateBuilder WireframeCullNone{ TriangleFillMode::Wireframe, CullMode::None };

		static constexpr RasterizerStateBuilder AntialiasedLine3D{ TriangleFillMode::Solid, CullMode::None, false, true };

		static constexpr RasterizerStateBuilder Default2D = SolidCullNone;

		static constexpr RasterizerStateBuilder Default3D = SolidCullBack;

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static constexpr RasterizerState Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const RasterizerState& value);
	};

	static_assert(sizeof(RasterizerState) == sizeof(RasterizerState::storage_type));
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::RasterizerState>
{
	[[nodiscard]]
	size_t operator ()(const s3d::RasterizerState& value) const noexcept
	{
		return hash<s3d::RasterizerState::storage_type>()(value.asValue());
	}
};

# include "detail/RasterizerState.ipp"
