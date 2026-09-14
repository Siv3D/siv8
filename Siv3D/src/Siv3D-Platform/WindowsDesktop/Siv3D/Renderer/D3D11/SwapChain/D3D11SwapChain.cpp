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

# include "D3D11SwapChain.hpp"
# include <Siv3D/Window.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool CheckTearingSupport(IDXGIFactory2* factory)
		{
			ComPtr<IDXGIFactory5> factory5;
			if (FAILED(factory->QueryInterface(IID_PPV_ARGS(&factory5))))
			{
				return false;
			}

			BOOL allowTearing = FALSE;
			const HRESULT hr = factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
			return (SUCCEEDED(hr) && (allowTearing == TRUE));
		}
	}


	D3D11SwapChain::~D3D11SwapChain()
	{
		if (m_waitableObject)
		{
			::CloseHandle(m_waitableObject);
			m_waitableObject = nullptr;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11SwapChain::init(const HWND hWnd, IDXGIFactory2* factory, ID3D11Device* device, const Size& frameBufferSize)
	{
		LOG_SCOPED_DEBUG("D3D11SwapChain::init()");

		m_hWnd = hWnd;

		const bool allowTearing = CheckTearingSupport(factory);

		// Windows 10 and later support flip discard and frame-latency waitable objects.
		const DXGI_SWAP_CHAIN_DESC1 desc =
		{
			.Width				= static_cast<uint32>(frameBufferSize.x),
			.Height				= static_cast<uint32>(frameBufferSize.y),
			.Format				= DXGI_FORMAT_R8G8B8A8_UNORM,
			.Stereo				= false,
			.SampleDesc			= { .Count = 1, .Quality = 0 },
			.BufferUsage		= DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount		= 3,
			.Scaling			= DXGI_SCALING_STRETCH,
			.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode			= DXGI_ALPHA_MODE_IGNORE,
			.Flags				= static_cast<uint32>(
									 (allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0) |
									 DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)
		};

		// Swap chain を作成する
		{
			LOG_DEBUG("IDXGIFactory2::CreateSwapChainForHwnd()");

			if (FAILED(factory->CreateSwapChainForHwnd(
				device,
				hWnd,
				&desc,
				nullptr,
				nullptr,
				&m_swapChain1)))
			{
				throw InternalEngineError{ "IDXGIFactory2::CreateSwapChainForHwnd() failed" };
			}
		}

		// Present must use the flags enabled for this swap chain, not just the factory's capabilities.
		m_nonVSyncPresentFlags = ((desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) ? DXGI_PRESENT_ALLOW_TEARING : 0);

		ComPtr<IDXGISwapChain2> swapChain2;
		if (FAILED(m_swapChain1.As(&swapChain2)))
		{
			throw InternalEngineError{ "IDXGISwapChain1::QueryInterface(IDXGISwapChain2) failed" };
		}

		LOG_TRACE(fmt::format("IDXGISwapChain2::SetMaximumFrameLatency({})", DefaultMaximumFrameLatency));
		if (FAILED(swapChain2->SetMaximumFrameLatency(DefaultMaximumFrameLatency)))
		{
			throw InternalEngineError{ "IDXGISwapChain2::SetMaximumFrameLatency() failed" };
		}

		{
			LOG_TRACE("IDXGIFactory::MakeWindowAssociation()");

			constexpr uint32 Flags = (DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

			if (FAILED(factory->MakeWindowAssociation(hWnd, Flags)))
			{
				throw InternalEngineError{ "IDXGIFactory::MakeWindowAssociation() failed" };
			}
		}

		LOG_TRACE("IDXGISwapChain2::GetFrameLatencyWaitableObject()");
		m_waitableObject = swapChain2->GetFrameLatencyWaitableObject();
		if (not m_waitableObject)
		{
			throw InternalEngineError{ "IDXGISwapChain2::GetFrameLatencyWaitableObject() failed" };
		}

		m_displayFrequency.update(m_hWnd);

		m_previousWindowBounds = Window::GetState().bounds;
	}

	////////////////////////////////////////////////////////////////
	//
	//	present
	//
	////////////////////////////////////////////////////////////////

	bool D3D11SwapChain::present()
	{
		if (const Rect windowBounds = Window::GetState().bounds;
			windowBounds != m_previousWindowBounds)
		{
			m_displayFrequency.update(m_hWnd);
			m_previousWindowBounds = windowBounds;
		}

		if (m_vSyncEnabled)
		{
			return presentVSync();
		}
		else
		{
			return presentNonVSync();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	waitForFrame
	//
	////////////////////////////////////////////////////////////////

	void D3D11SwapChain::waitForFrame()
	{
		if (m_waitableObject)
		{
			::WaitForSingleObjectEx(m_waitableObject, 1000, true);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVSyncEnabled
	//
	////////////////////////////////////////////////////////////////

	void D3D11SwapChain::setVSyncEnabled(const bool enabled) noexcept
	{
		m_vSyncEnabled = enabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isVSyncEnabled
	//
	////////////////////////////////////////////////////////////////

	bool D3D11SwapChain::isVSyncEnabled() const noexcept
	{
		return m_vSyncEnabled;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	bool D3D11SwapChain::presentVSync()
	{
		const HRESULT hr = m_swapChain1->Present(1, 0);

		if (hr == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(m_displayFrequency.getOccludedSleepMillisec());
		}
		else if (hr == DXGI_ERROR_DEVICE_RESET)
		{
			LOG_FAIL("❌ IDXGISwapChain::Present() failed (DXGI_ERROR_DEVICE_RESET)");
			return false;
		}
		else if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			LOG_FAIL("❌ IDXGISwapChain::Present() failed (DXGI_ERROR_DEVICE_REMOVED)");
			return false;
		}

		return true;
	}

	bool D3D11SwapChain::presentNonVSync()
	{
		const HRESULT hr = m_swapChain1->Present(0, m_nonVSyncPresentFlags);

		if (hr == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(m_displayFrequency.getOccludedSleepMillisec());
		}
		else if (hr == DXGI_ERROR_DEVICE_RESET)
		{
			LOG_FAIL("❌ IDXGISwapChain::Present() failed (DXGI_ERROR_DEVICE_RESET)");
			return false;
		}
		else if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			LOG_FAIL("❌ IDXGISwapChain::Present() failed (DXGI_ERROR_DEVICE_REMOVED)");
			return false;
		}

		return true;
	}
}
