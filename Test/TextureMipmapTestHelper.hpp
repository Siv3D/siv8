//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Siv3DTest.hpp"

namespace s3d::Test
{
	inline void CheckMipColors(const Texture& texture, const std::span<const Color> colors)
	{
		REQUIRE(texture);
		REQUIRE(texture.mipLevels() == colors.size());
		CHECK(texture.hasMipmaps() == (1 < colors.size()));
		CHECK(texture.mipSize(texture.mipLevels()).isZero());
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedScissorRect2D scissor{ none };
		const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
		const ScopedColorAdd2D add{ ColorF{ 0.0, 0.0 }, ScopedColorAdd2D::Target::Set };
		const ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
		const ScopedRenderStates2D states{ BlendState::Opaque, SamplerState::ClampNearest };
		for (uint32 level = 0; level < texture.mipLevels(); ++level)
		{
			const Size size = texture.mipSize(level);
			const RenderTexture target{ size };
			{
				const ScopedRenderTarget2D renderTarget{ target };
				const ScopedViewport2D viewport{ Rect{ size } };
				texture.resized(size).draw();
			}
			Image actual;
			REQUIRE(target.readAsImage(actual));
			CHECK(actual == Image{ size, colors[level] });
		}
	}
}
