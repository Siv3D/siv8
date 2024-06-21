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

# include <Siv3D/Graphics2D.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
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
