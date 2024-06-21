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
# include <Siv3D/Utility.hpp>

namespace s3d
{
	enum class MetalRenderer2DCommandType : uint32
	{
		Null,

		Draw,

		//DrawNull,

		ColorMul,

		ColorAdd,

		BlendState,

		RasterizerState,

		//VSSamplerState0,

		//VSSamplerState1,

		//VSSamplerState2,

		//VSSamplerState3,

		//VSSamplerState4,

		//VSSamplerState5,

		//VSSamplerState6,

		//VSSamplerState7,

		//PSSamplerState0,

		//PSSamplerState1,

		//PSSamplerState2,

		//PSSamplerState3,

		//PSSamplerState4,

		//PSSamplerState5,

		//PSSamplerState6,

		//PSSamplerState7,

		ScissorRect,

		Viewport,

		//SDFParams,

		//InternalPSConstants,

		//SetRT,

		SetVS,

		SetPS,

		//Transform,

		//SetConstantBuffer,

		//VSTexture0,

		//VSTexture1,

		//VSTexture2,

		//VSTexture3,

		//VSTexture4,

		//VSTexture5,

		//VSTexture6,

		//VSTexture7,

		//PSTexture0,

		//PSTexture1,

		//PSTexture2,

		//PSTexture3,

		//PSTexture4,

		//PSTexture5,

		//PSTexture6,

		//PSTexture7,

		SIZE_,
	};
	static_assert(FromEnum(MetalRenderer2DCommandType::SIZE_) < 64);

	struct MetalRenderer2DCommand
	{
		MetalRenderer2DCommandType type	: 8 = MetalRenderer2DCommandType::Null;

		uint32 index					: 24 = 0;

		constexpr MetalRenderer2DCommand() = default;

		constexpr MetalRenderer2DCommand(MetalRenderer2DCommandType _type, uint32 _index)
			: type{ _type }
			, index{ _index } {}
	};

	struct MetalDrawCommand
	{
		uint32 indexCount = 0;
	};
}
