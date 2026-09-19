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

namespace s3d
{
	namespace Graphics2D
	{
		namespace Internal
		{
			void SetConstantBuffer(ShaderStage stage, uint32 slot, const void* data, size_t size);

			uint32 BeginConstantBufferScope(ShaderStage stage, uint32 slot, const void* data, size_t size);

			void EndConstantBufferScope(ShaderStage stage, uint32 slot, uint32 previous);

			void SetColorMul(const Float4& color);

			void SetColorAdd(const Float3& color);

			void SetBlendState(const BlendState& blendState);

			void SetRasterizerState(const RasterizerState& rasterizerState);

			void SetVSSamplerState(uint32 slot, const SamplerState& samplerState);

			void SetPSSamplerState(uint32 slot, const SamplerState& samplerState);

			void SetSamplerState(ShaderStage shaderStage, uint32 slot, const SamplerState& samplerState);

			void SetScissorRect(const Optional<Rect>& rect);

			void SetViewport(const Optional<Rect>& viewport);

			void SetSDFParameters(const std::array<Float4, 3>& params);

			void SetCustomVertexShader(const Optional<VertexShader>& vs);

			void SetCustomPixelShader(const Optional<PixelShader>& ps);

			void SetLocalTransform(const Mat3x2& transform);

			void SetCameraTransform(const Mat3x2& transform);
		}

	}
}
