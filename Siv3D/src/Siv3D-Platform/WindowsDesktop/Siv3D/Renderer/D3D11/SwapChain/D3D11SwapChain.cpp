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

# include "D3D11SwapChain.hpp"
# include <Siv3D/Window.hpp>
# include <Siv3D/WindowState.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>
# include <dwmapi.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool CheckTearingSupport(const D3D11Device& device)
		{
			if (IDXGIFactory6* factory = device.getDXGIFactory6())
			{
				BOOL allowTearing = FALSE;
				
				const HRESULT hr = factory->CheckFeatureSupport(
					DXGI_FEATURE_PRESENT_ALLOW_TEARING,
					&allowTearing, sizeof(allowTearing));
				
				return (SUCCEEDED(hr) && (allowTearing == TRUE));
			}
			else
			{
				return false;
			}
		}

		[[nodiscard]]
		static double ToRefreshRateHz(const uint64 count)
		{
			::LARGE_INTEGER frequency;
			::QueryPerformanceFrequency(&frequency);
			return (static_cast<double>(frequency.QuadPart) / count);
		}

		[[nodiscard]]
		static double GetDisplayFrequency(IDXGISwapChain1* swapChain1)
		{
			ComPtr<IDXGIOutput> pOutput;

			LOG_TRACE("IDXGISwapChain::GetContainingOutput()");

			if (SUCCEEDED(swapChain1->GetContainingOutput(&pOutput)))
			{
				DXGI_OUTPUT_DESC desc;

				LOG_TRACE("IDXGIOutput::GetDesc()");

				if (SUCCEEDED(pOutput->GetDesc(&desc)))
				{
					LOG_TRACE("EnumDisplaySettingsW()");

					DEVMODE devMode{};
					devMode.dmSize = sizeof(DEVMODE);
					::EnumDisplaySettingsW(desc.DeviceName, ENUM_CURRENT_SETTINGS, &devMode);

					return devMode.dmDisplayFrequency;
				}
			}

			LOG_TRACE("DwmGetCompositionTimingInfo()");

			DWM_TIMING_INFO timingInfo{};
			timingInfo.cbSize = sizeof(DWM_TIMING_INFO);
			::DwmGetCompositionTimingInfo(nullptr, &timingInfo);

			return ToRefreshRateHz(timingInfo.qpcRefreshPeriod);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11SwapChain::init(const HWND hWnd, const D3D11Device& device, const Size& frameBufferSize)
	{
		LOG_SCOPED_DEBUG("D3D11SwapChain::init()");

		m_hWnd = hWnd;
		m_device = device.getDevice();
		m_context = device.getContext();
		m_dxgiDevice1 = device.getDXGIDevice1();
		m_tearingSupport = CheckTearingSupport(device);

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
			.SwapEffect			= (device.supportsDXGI1_4() ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD),
			.AlphaMode			= DXGI_ALPHA_MODE_IGNORE,
			.Flags				= static_cast<uint32>(m_tearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0),
		};

		// Swap chain を作成する
		{
			LOG_DEBUG("IDXGIFactory2::CreateSwapChainForHwnd()");

			if (FAILED(device.getDXGIFactory2()->CreateSwapChainForHwnd(
				m_device,
				hWnd,
				&desc,
				nullptr,
				nullptr,
				&m_swapChain1)))
			{
				throw InternalEngineError{ "IDXGIFactory2::CreateSwapChainForHwnd() failed" };
			}
		}

		{
			LOG_TRACE("IDXGIFactory::MakeWindowAssociation()");

			constexpr uint32 Flags = (DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

			if (FAILED(device.getDXGIFactory2()->MakeWindowAssociation(hWnd, Flags)))
			{
				throw InternalEngineError{ "IDXGIFactory::MakeWindowAssociation() failed" };
			}
		}

		{
			LOG_TRACE(fmt::format("IDXGIDevice1::SetMaximumFrameLatency({})", m_maximumFrameLatency));
			m_dxgiDevice1->SetMaximumFrameLatency(m_maximumFrameLatency);
		}

		m_displayFrequency = GetDisplayFrequency(m_swapChain1.Get());
		LOG_INFO(fmt::format("ℹ️ Display refresh rate: {:.1f} Hz", m_displayFrequency));

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
			m_displayFrequency		= GetDisplayFrequency(m_swapChain1.Get());
			m_previousWindowBounds	= windowBounds;
			
			LOG_INFO(fmt::format("ℹ️ Display refresh rate: {:.1f} Hz", m_displayFrequency));
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
	//	setLowLatencyMode
	//
	////////////////////////////////////////////////////////////////

	void D3D11SwapChain::setLowLatencyMode(const bool enabled) noexcept
	{
		const uint32 maximumFrameLatency = (enabled ? MaximumFrameLatency_LowLatency : MaximumFrameLatency_Default);

		if (m_maximumFrameLatency != maximumFrameLatency)
		{
			m_maximumFrameLatency = maximumFrameLatency;

			{
				LOG_TRACE(fmt::format("IDXGIDevice1::SetMaximumFrameLatency({})", m_maximumFrameLatency));
				m_dxgiDevice1->SetMaximumFrameLatency(m_maximumFrameLatency);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	isLowLatencyMode
	//
	////////////////////////////////////////////////////////////////

	bool D3D11SwapChain::isLowLatencyMode() const noexcept
	{
		return (m_maximumFrameLatency == MaximumFrameLatency_LowLatency);
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
			::Sleep(static_cast<int32>((1000 / m_displayFrequency) * 0.9));
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
		const UINT presentFlags = (m_tearingSupport ? DXGI_PRESENT_ALLOW_TEARING : 0);
		
		const HRESULT hr = m_swapChain1->Present(0, presentFlags);

		if (hr == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(static_cast<int32>((1000 / m_displayFrequency) * 0.9));
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
