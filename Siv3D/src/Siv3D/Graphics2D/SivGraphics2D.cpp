﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Graphics2D.hpp>
# include <Siv3D/Graphics.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[noreturn]]
		static void ThrowVSSamplerIndexOutOfRange()
		{
			throw Error{ U"VSSampler index out of range" };
		}

		[[noreturn]]
		static void ThrowPSSamplerIndexOutOfRange()
		{
			throw Error{ U"PSSampler index out of range" };
		}
	}

	namespace Graphics2D
	{
		Float4 GetColorMul()
		{
			return SIV3D_ENGINE(Renderer2D)->getColorMul();
		}

		Float3 GetColorAdd()
		{
			return SIV3D_ENGINE(Renderer2D)->getColorAdd();
		}

		BlendState GetBlendState()
		{
			return SIV3D_ENGINE(Renderer2D)->getBlendState();
		}

		RasterizerState GetRasterizerState()
		{
			return SIV3D_ENGINE(Renderer2D)->getRasterizerState();
		}

		SamplerState GetVSSamplerState(const uint32 slot)
		{
			if (Graphics::TextureSlotCount <= slot)
			{
				ThrowVSSamplerIndexOutOfRange();
			}

			return SIV3D_ENGINE(Renderer2D)->getVSSamplerState(slot);
		}

		SamplerState GetPSSamplerState(const uint32 slot)
		{
			if (Graphics::TextureSlotCount <= slot)
			{
				ThrowPSSamplerIndexOutOfRange();
			}

			return SIV3D_ENGINE(Renderer2D)->getPSSamplerState(slot);
		}

		SamplerState GetSamplerState(const ShaderStage shaderStage, const uint32 slot)
		{
			if (shaderStage == ShaderStage::Vertex)
			{
				return GetVSSamplerState(slot);
			}
			else
			{
				return GetPSSamplerState(slot);
			}
		}

		Optional<Rect> GetScissorRect()
		{
			return SIV3D_ENGINE(Renderer2D)->getScissorRect();
		}

		Optional<Rect> GetViewport()
		{
			return SIV3D_ENGINE(Renderer2D)->getViewport();
		}

		namespace Internal
		{
			void SetColorMul(const Float4& color)
			{
				SIV3D_ENGINE(Renderer2D)->setColorMul(color);
			}

			void SetColorAdd(const Float3& color)
			{
				SIV3D_ENGINE(Renderer2D)->setColorAdd(color);
			}

			void SetBlendState(const BlendState& blendState)
			{
				SIV3D_ENGINE(Renderer2D)->setBlendState(blendState);
			}

			void SetRasterizerState(const RasterizerState& rasterizerState)
			{
				SIV3D_ENGINE(Renderer2D)->setRasterizerState(rasterizerState);
			}

			void SetVSSamplerState(const uint32 slot, const SamplerState& samplerState)
			{
				if (Graphics::TextureSlotCount <= slot)
				{
					ThrowVSSamplerIndexOutOfRange();
				}

				SIV3D_ENGINE(Renderer2D)->setVSSamplerState(slot, samplerState);
			}

			void SetPSSamplerState(const uint32 slot, const SamplerState& samplerState)
			{
				if (Graphics::TextureSlotCount <= slot)
				{
					ThrowPSSamplerIndexOutOfRange();
				}

				SIV3D_ENGINE(Renderer2D)->setPSSamplerState(slot, samplerState);
			}

			void SetSamplerState(const ShaderStage shaderStage, const uint32 slot, const SamplerState& samplerState)
			{
				if (shaderStage == ShaderStage::Vertex)
				{
					SetVSSamplerState(slot, samplerState);
				}
				else
				{
					SetPSSamplerState(slot, samplerState);
				}
			}

			void SetScissorRect(const Optional<Rect>& rect)
			{
				SIV3D_ENGINE(Renderer2D)->setScissorRect(rect);
			}

			void SetViewport(const Optional<Rect>& viewport)
			{
				SIV3D_ENGINE(Renderer2D)->setViewport(viewport);
			}
		}
	}
}