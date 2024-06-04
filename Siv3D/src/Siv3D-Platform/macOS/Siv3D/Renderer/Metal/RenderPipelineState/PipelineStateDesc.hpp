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
# include <Siv3D/Common.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>

namespace s3d
{
	struct PipelineStateDesc
	{
		VertexShader::IDType vs;

		PixelShader::IDType ps;

		uint16 pixelFormat;

		uint16 sampleCount;

		[[nodiscard]]
		friend constexpr bool operator ==(const PipelineStateDesc& lhs, const PipelineStateDesc& rhs) noexcept
		{
			return ((lhs.vs == rhs.vs)
				 && (lhs.ps == rhs.ps)
				 && (lhs.pixelFormat == rhs.pixelFormat)
				 && (lhs.sampleCount == rhs.sampleCount));
		}
	};
}

template <>
struct std::hash<s3d::PipelineStateDesc>
{
	[[nodiscard]]
	size_t operator ()(const s3d::PipelineStateDesc& value) const noexcept
	{
		return s3d::Hash(value);
	}
};
