//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/ScopedRenderTarget2D.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include <exception>
# include <utility>

namespace s3d
{
	ScopedRenderTarget2D::ScopedRenderTarget2D(const Optional<ColorRenderTarget>& renderTarget)
		: m_oldRenderTargets{ SIV3D_ENGINE(Renderer2D)->getRenderTargets() }
		, m_oldViewport{ SIV3D_ENGINE(Renderer2D)->getViewport() }
	{
		SIV3D_ENGINE(Renderer2D)->setRenderTarget(renderTarget);
		SIV3D_ENGINE(Renderer2D)->setViewport(none);
	}

	ScopedRenderTarget2D::ScopedRenderTarget2D(const std::span<const ColorRenderTarget> renderTargets)
		: m_oldRenderTargets{ SIV3D_ENGINE(Renderer2D)->getRenderTargets() }
		, m_oldViewport{ SIV3D_ENGINE(Renderer2D)->getViewport() }
	{
		SIV3D_ENGINE(Renderer2D)->setRenderTargets(renderTargets);
		SIV3D_ENGINE(Renderer2D)->setViewport(none);
	}

	ScopedRenderTarget2D::ScopedRenderTarget2D(const std::initializer_list<ColorRenderTarget> renderTargets)
		: ScopedRenderTarget2D{ std::span<const ColorRenderTarget>{ renderTargets.begin(), renderTargets.size() } } {}

	ScopedRenderTarget2D::ScopedRenderTarget2D(ScopedRenderTarget2D&& other) noexcept
		: m_oldRenderTargets{ std::exchange(other.m_oldRenderTargets, none) }
		, m_oldViewport{ other.m_oldViewport } {}

	ScopedRenderTarget2D::~ScopedRenderTarget2D() noexcept(false)
	{
		if (not m_oldRenderTargets)
		{
			return;
		}
		try
		{
			SIV3D_ENGINE(Renderer2D)->restoreRenderTargets(*m_oldRenderTargets, m_oldViewport);
		}
		catch (...)
		{
			if (std::uncaught_exceptions() == 0)
			{
				throw;
			}
			LOG_FAIL("Failed to flush 2D draws while unwinding ScopedRenderTarget2D");
		}
	}
}
