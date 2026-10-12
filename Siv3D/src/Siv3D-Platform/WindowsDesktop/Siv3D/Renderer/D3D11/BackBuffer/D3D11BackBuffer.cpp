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

# include "D3D11BackBuffer.hpp"
# include <Siv3D/Graphics.hpp>
# include <Siv3D/Window.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/SamplerState.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Shader/IShader.hpp>
# include <Siv3D/Scene/SceneUtility.hpp>
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/EngineShader/IEngineShader.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	void D3D11BackBuffer::configureScene(const SceneConfig& config)
	{
		auto state = m_sceneState;
		state.configure(config, Window::GetState());
		applySceneState(state);
	}

	SceneConfig D3D11BackBuffer::getSceneConfig() const noexcept
	{
		return m_sceneState.config;
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::init(const D3D11Device& device, IDXGISwapChain1* swapChain, CRenderer_D3D11* pRenderer)
	{
		LOG_SCOPED_DEBUG("D3D11BackBuffer::init()");
		LOG_INFO(fmt::format("ℹ️ Scene MSAA: x{}", m_sceneBuffers.sampleCount));

		m_device		= device.getDevice();
		m_context		= device.getContext();
		m_swapChain1	= swapChain;
		m_pRenderer		= pRenderer;

		m_backBuffer	= D3D11InternalTexture2D::CreateFromSwapChain(m_device, m_swapChain1);
		
		{
			m_sceneState.configure(SceneConfig{}, Window::GetState());
			const Size sceneSize = m_sceneState.size;

			if (m_sceneBuffers.sampleCount == 1)
			{
				m_sceneBuffers.nonMSAA	= D3D11InternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
			else
			{
				m_sceneBuffers.msaa		= D3D11InternalTexture2D::CreateMSRenderTexture(m_device, sceneSize, m_sceneBuffers.sampleCount);
			}
		}

		clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::clear(const D3D11ClearTarget clearTarget)
	{
		if (clearTarget & D3D11ClearTarget::Scene)
		{
			m_sceneBuffers.getSceneTexture().clear(m_context, m_sceneStyle.backgroundColor);
		}

		if (clearTarget & D3D11ClearTarget::BackBuffer)
		{
			m_backBuffer.clear(m_context, m_sceneStyle.letterboxColor);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneStyle
	//
	////////////////////////////////////////////////////////////////

	SceneStyle& D3D11BackBuffer::getSceneStyle() noexcept
	{
		return m_sceneStyle;
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::updateSceneBufferSize()
	{
		auto state = m_sceneState;
		state.updateWindow(Window::GetState());
		applySceneState(state);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderSceneToBackBuffer
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::renderSceneToBackBuffer()
	{
		unbindAllRenderTargets();

		const Size backBufferSize = m_backBuffer.size();

		if (m_sceneBuffers.sampleCount == 1)
		{
			const Size sceneBufferSize = m_sceneBuffers.nonMSAA.size();

			if ((backBufferSize == sceneBufferSize)
				&& (getLetterboxComposition().second == RectF{ backBufferSize }))
			{
				m_sceneBuffers.nonMSAA.copyTo(m_context, m_backBuffer);
			}
			else
			{
				bindRenderTarget(m_backBuffer.getRTV());
				bindTextureAsPSResource(m_sceneBuffers.nonMSAA.getSRV());
				drawFullScreenTriangle();
				unbindAllPSTextures();
				unbindAllRenderTargets();
			}
		}
		else
		{
			const Size sceneBufferSize = m_sceneBuffers.msaa.size();

			if ((backBufferSize == sceneBufferSize)
				&& (getLetterboxComposition().second == RectF{ backBufferSize }))
			{
				m_sceneBuffers.msaa.resolveTo(m_context, m_backBuffer);
			}
			else
			{
				if (m_sceneBuffers.nonMSAA.size() != sceneBufferSize)
				{
					m_sceneBuffers.nonMSAA = D3D11InternalTexture2D::CreateRenderTexture(m_device, sceneBufferSize);
				}
				m_sceneBuffers.msaa.resolveTo(m_context, m_sceneBuffers.nonMSAA);

				bindRenderTarget(m_backBuffer.getRTV());
				bindTextureAsPSResource(m_sceneBuffers.nonMSAA.getSRV());
				drawFullScreenTriangle();
				unbindAllPSTextures();
				unbindAllRenderTargets();
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeBackBuffer
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::resizeBackBuffer(const Size backBufferSize)
	{
		assert((0 < backBufferSize.x) && (0 < backBufferSize.y));

		LOG_DEBUG(fmt::format("D3D11BackBuffer::resizeBackBuffer({})", backBufferSize));

		unbindAllRenderTargets();

		m_backBuffer.reset();
		{
			DXGI_SWAP_CHAIN_DESC1 desc{};
			m_swapChain1->GetDesc1(&desc);
			
			if (FAILED(m_swapChain1->ResizeBuffers(desc.BufferCount, backBufferSize.x, backBufferSize.y, desc.Format, desc.Flags)))
			{
				throw InternalEngineError{ "IDXGISwapChain->ResizeBuffers() failed" };
			}

			m_backBuffer = D3D11InternalTexture2D::CreateFromSwapChain(m_device, m_swapChain1);
			
			clear(D3D11ClearTarget::BackBuffer);
		}

	}

	////////////////////////////////////////////////////////////////
	//
	//	getBackBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& D3D11BackBuffer::getBackBufferSize() const noexcept
	{
		return m_backBuffer.size();
	}


	const Size& D3D11BackBuffer::getSceneSize() const noexcept
	{
		return m_sceneState.size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSceneBufferSize
	//
	////////////////////////////////////////////////////////////////

	const Size& D3D11BackBuffer::getSceneBufferSize() const noexcept
	{
		return m_sceneBuffers.getSceneTexture().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLetterboxComposition
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, RectF> D3D11BackBuffer::getLetterboxComposition() const noexcept
	{
		return m_sceneState.composition();
	}

	////////////////////////////////////////////////////////////////
	//
	//	bindSceneTextureAsRenderTarget
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::bindSceneTextureAsRenderTarget()
	{
		bindRenderTarget(m_sceneBuffers.getSceneTexture().getRTV());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCaptureTexture
	//
	////////////////////////////////////////////////////////////////

	std::pair<ID3D11Texture2D*, Size> D3D11BackBuffer::getCaptureTexture()
	{
		unbindAllRenderTargets();

		if (m_sceneBuffers.sampleCount == 1)
		{
			return{ m_sceneBuffers.nonMSAA.getTexture(), m_sceneBuffers.nonMSAA.size() };
		}
		else
		{
			const Size sceneSize = m_sceneBuffers.msaa.size();

			if (m_sceneBuffers.nonMSAA.size() != sceneSize)
			{
				m_sceneBuffers.nonMSAA = D3D11InternalTexture2D::CreateRenderTexture(m_device, sceneSize);
			}
			m_sceneBuffers.msaa.resolveTo(m_context, m_sceneBuffers.nonMSAA);

			return{ m_sceneBuffers.nonMSAA.getTexture(), m_sceneBuffers.nonMSAA.size() };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void D3D11BackBuffer::bindRenderTarget(ID3D11RenderTargetView* const rtv)
	{
		ID3D11RenderTargetView* const rtvs[Graphics::RenderTargetCount] = { rtv };
		m_context->OMSetRenderTargets(Graphics::RenderTargetCount, rtvs, nullptr);
	}

	void D3D11BackBuffer::unbindAllRenderTargets()
	{
		constexpr ID3D11RenderTargetView* nullRTVs[Graphics::RenderTargetCount] = { nullptr };
		m_context->OMSetRenderTargets(Graphics::RenderTargetCount, nullRTVs, nullptr);
	}

	void D3D11BackBuffer::bindTextureAsPSResource(ID3D11ShaderResourceView* const srv)
	{
		m_context->PSSetShaderResources(0, 1, &srv);
	}

	void D3D11BackBuffer::unbindAllPSTextures()
	{
		constexpr ID3D11ShaderResourceView* nullSRVs[Graphics::TextureSlotCount] = { nullptr };
		m_context->PSSetShaderResources(0, Graphics::TextureSlotCount, nullSRVs);
	}

	void D3D11BackBuffer::drawFullScreenTriangle()
	{
		// view port
		{
			const auto [s, viewRect] = getLetterboxComposition();
			const D3D11_VIEWPORT viewport{
				.TopLeftX	= static_cast<float>(viewRect.x),
				.TopLeftY	= static_cast<float>(viewRect.y),
				.Width		= static_cast<float>(viewRect.w),
				.Height		= static_cast<float>(viewRect.h),
				.MinDepth	= 0.0f,
				.MaxDepth	= 1.0f,
			};
			m_context->RSSetViewports(1, &viewport);
		}

		// render states
		{
			const SamplerState samplerState = ((m_sceneState.config.scalingFilter == TextureFilter::Linear) ?
				SamplerState::ClampLinear : SamplerState::ClampNearest);
			m_pRenderer->getSamplerState().setPS(0, samplerState);
			m_pRenderer->getBlendState().set(BlendState::Opaque);
		//	pRenderer->getDepthStencilState().set(DepthStencilState::Default2D);
			m_pRenderer->getRasterizerState().set(RasterizerState::Default2D, false);
		}

		// shaders
		{
			const auto& vs = SIV3D_ENGINE(EngineShader)->getVS(EngineVS::FullScreenTriangle);
			const auto& ps = SIV3D_ENGINE(EngineShader)->getPS(EnginePS::FullScreenTriangle);

			SIV3D_ENGINE(Shader)->setVS(vs.id());
			SIV3D_ENGINE(Shader)->setPS(ps.id());
		}

		// draw null vertex buffer
		{
			m_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
			m_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
			m_context->IASetInputLayout(nullptr);
			m_context->Draw(3, 0);
		}
	}
	void D3D11BackBuffer::applySceneState(const SceneMisc::RenderState& state)
	{
		if (state == m_sceneState)
		{
			return;
		}
		SceneMisc::ValidateSceneSize(state.size);
		const Size size = state.renderSize();
		SceneMisc::ValidateSceneSize(size);
		const bool replaceBuffer = (size != getSceneBufferSize());
		const bool restartScene = (replaceBuffer || (state.size != m_sceneState.size));
		SceneBuffer buffers;
		if (replaceBuffer)
		{
			buffers.sampleCount = m_sceneBuffers.sampleCount;
			if (buffers.sampleCount == 1)
			{
				buffers.nonMSAA = D3D11InternalTexture2D::CreateRenderTexture(m_device, size);
			}
			else
			{
				buffers.msaa = D3D11InternalTexture2D::CreateMSRenderTexture(m_device, size, buffers.sampleCount);
			}
		}
		if (restartScene)
		{
			if (replaceBuffer)
			{
				unbindAllRenderTargets();
				m_sceneBuffers = std::move(buffers);
			}
			clear(D3D11ClearTarget::Scene | D3D11ClearTarget::BackBuffer);
			SIV3D_ENGINE(Renderer2D)->setSceneScale(Float2{ (float(size.x) / state.size.x), (float(size.y) / state.size.y) });
		}
		m_sceneState = state;
		SIV3D_ENGINE(Cursor)->updateBaseWindowTransform();
	}

}
