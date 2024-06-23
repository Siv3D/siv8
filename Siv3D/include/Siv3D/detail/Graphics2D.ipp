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

namespace s3d
{
	namespace Graphics2D
	{
		namespace Internal
		{
			void SetColorMul(const Float4& color);

			void SetColorAdd(const Float3& color);

			void SetBlendState(const BlendState& blendState);

			void SetRasterizerState(const RasterizerState& rasterizerState);

			void SetVSSamplerState(uint32 slot, const SamplerState& samplerState);

			void SetPSSamplerState(uint32 slot, const SamplerState& samplerState);

			void SetSamplerState(ShaderStage shaderStage, uint32 slot, const SamplerState& samplerState);

			void SetScissorRect(const Optional<Rect>& rect);

			void SetViewport(const Optional<Rect>& viewport);

			void SetLocalTransform(const Mat3x2& transform);

			void SetCameraTransform(const Mat3x2& transform);
		}
	}
}
