//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <array>
# include <Siv3D/Common.hpp>
# include <Siv3D/Hash.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/BlendState.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>

namespace s3d
{
	struct PipelineStateDesc
	{
		VertexShader::IDType vs;

		PixelShader::IDType ps;

		std::array<uint16, 8> pixelFormats{};

		uint16 sampleCount = 1;

		uint8 colorCount = 1;

		uint8 colorWriteMask = 1;
		
		BlendState blendState;

		[[nodiscard]]
		friend bool operator ==(const PipelineStateDesc&, const PipelineStateDesc&) noexcept = default;

	};
}

template <>
struct std::hash<s3d::PipelineStateDesc>
{
	[[nodiscard]]
	size_t operator ()(const s3d::PipelineStateDesc& value) const noexcept
	{
		// Hash explicit fields, never structure padding.
		const std::array<s3d::uint32, 8> key{
			value.vs.value(), value.ps.value(), value.blendState.asValue(),
			s3d::uint32(value.sampleCount) | (s3d::uint32(value.colorCount) << 16) | (s3d::uint32(value.colorWriteMask) << 24),
			s3d::uint32(value.pixelFormats[0]) | (s3d::uint32(value.pixelFormats[1]) << 16),
			s3d::uint32(value.pixelFormats[2]) | (s3d::uint32(value.pixelFormats[3]) << 16),
			s3d::uint32(value.pixelFormats[4]) | (s3d::uint32(value.pixelFormats[5]) << 16),
			s3d::uint32(value.pixelFormats[6]) | (s3d::uint32(value.pixelFormats[7]) << 16),
		};
		return s3d::Hash(key);
	}
};
