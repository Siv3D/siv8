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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DepthStencilStateBuilder
	//
	////////////////////////////////////////////////////////////////

	class DepthStencilStateBuilder
	{
	public:

		[[nodiscard]]
		explicit constexpr DepthStencilStateBuilder(
			bool _depthEnabled = false,
			bool _depthWriteEnabled = false,
			CompareFunction _depthCompareFunction = CompareFunction::Always
		) noexcept;
		
		[[nodiscard]]
		constexpr DepthStencilStateBuilder depthEnabled(bool enabled) const noexcept;

		[[nodiscard]]
		constexpr bool depthEnabled() const noexcept;

		[[nodiscard]]
		constexpr DepthStencilStateBuilder depthWriteEnabled(bool enabled) const noexcept;

		[[nodiscard]]
		constexpr bool depthWriteEnabled() const noexcept;

		[[nodiscard]]
		constexpr DepthStencilStateBuilder depthCompareFunction(CompareFunction function) const noexcept;

		[[nodiscard]]
		constexpr CompareFunction depthCompareFunction() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const DepthStencilStateBuilder& value);

	private:

		bool m_depthEnabled : 1 = false;

		bool m_depthWriteEnabled : 1 = false;

		CompareFunction m_depthCompareFunction : 4 = CompareFunction::Always;
	};
}

# include "detail/DepthStencilStateBuilder.ipp"
