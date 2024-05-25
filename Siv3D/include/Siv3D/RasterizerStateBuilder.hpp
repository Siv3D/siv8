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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RasterizerStateBuilder
	//
	////////////////////////////////////////////////////////////////

	class RasterizerStateBuilder
	{
	public:

		[[nodiscard]]
		explicit constexpr RasterizerStateBuilder(
			TriangleFillMode _triangleFillMode = TriangleFillMode::Solid,
			CullMode _cullMode = CullMode::Back,
			bool _scissorEnabled = false,
			bool _antialiasedLine3D = false,
			int32 _depthBias = 0
		) noexcept;

		[[nodiscard]]
		constexpr RasterizerStateBuilder triangleFillMode(TriangleFillMode _triangleFillMode) const noexcept;

		[[nodiscard]]
		constexpr TriangleFillMode triangleFillMode() const noexcept;

		[[nodiscard]]
		constexpr RasterizerStateBuilder cullMode(CullMode _cullMode) const noexcept;

		[[nodiscard]]
		constexpr CullMode cullMode() const noexcept;

		[[nodiscard]]
		constexpr RasterizerStateBuilder scissorEnabled(bool _scissorEnabled) const noexcept;

		[[nodiscard]]
		constexpr bool scissorEnabled() const noexcept;

		[[nodiscard]]
		constexpr RasterizerStateBuilder antialiasedLine3D(bool _antialiasedLine3D) const noexcept;

		[[nodiscard]]
		constexpr bool antialiasedLine3D() const noexcept;

		[[nodiscard]]
		constexpr RasterizerStateBuilder depthBias(int32 _depthBias) const noexcept;

		[[nodiscard]]
		constexpr int32 depthBias() const noexcept;

	private:

		TriangleFillMode m_triangleFillMode : 2 = TriangleFillMode::Solid;

		CullMode m_cullMode : 2 = CullMode::Back;

		bool m_scissorEnabled : 2 = false;

		bool m_antialiasedLine3D : 2 = false;

		int32 m_depthBias = 0;
	};
}

# include "detail/RasterizerStateBuilder.ipp"
