//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	enum class D3D11Renderer2DCommandType : uint32
	{
		Null,

		SetBuffers,

		UpdateBuffers,

		Draw,

		//DrawNull,

		ColorMul,

		ColorAdd,

		QuadWarpParameters,

		PatternParameters,

		BlendState,

		RasterizerState,

		VSSamplerState0,

		VSSamplerState1,

		VSSamplerState2,

		VSSamplerState3,

		VSSamplerState4,

		VSSamplerState5,

		VSSamplerState6,

		VSSamplerState7,


		PSSamplerState0,

		PSSamplerState1,

		PSSamplerState2,

		PSSamplerState3,

		PSSamplerState4,

		PSSamplerState5,

		PSSamplerState6,

		PSSamplerState7,


		ScissorRect,

		Viewport,

		//SDFParams,

		//InternalPSConstants,

		//SetRT,

		SetVS,

		SetPS,

		Transform,

		//SetConstantBuffer,

		VSTexture0,

		VSTexture1,

		VSTexture2,

		VSTexture3,

		VSTexture4,

		VSTexture5,

		VSTexture6,

		VSTexture7,

		PSTexture0,

		PSTexture1,

		PSTexture2,

		PSTexture3,

		PSTexture4,

		PSTexture5,

		PSTexture6,

		PSTexture7,

		SIZE_,
	};
	static_assert(FromEnum(D3D11Renderer2DCommandType::SIZE_) < 64);

	struct D3D11Renderer2DCommand
	{
		D3D11Renderer2DCommandType type	: 8 = D3D11Renderer2DCommandType::Null;

		uint32 index					: 24 = 0;
	};

	struct D3D11DrawCommand
	{
		uint32 indexCount = 0;
	};
}
