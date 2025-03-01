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

# include "D3D11Device.hpp"
# include <Siv3D/DLL.hpp>
# include <Siv3D/EngineOption.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>
# include "D3D11Misc.hpp"

namespace s3d
{
	namespace
	{
		/// @brief d3d11.dll から D3D11CreateDevice() を取得します。
		/// @return D3D11CreateDevice() のアドレス
		[[nodiscard]]
		static PFN_D3D11_CREATE_DEVICE LoadD3D11CreateDevice()
		{
			const LibraryHandle d3d11 = DLL::LoadSystemLibrary(L"d3d11.dll");
			return DLL::GetFunction(d3d11, "D3D11CreateDevice");
		}

		/// @brief dxgi.dll から CreateDXGIFactory1() を取得します。
		/// @return CreateDXGIFactory1() のアドレス
		[[nodiscard]]
		static decltype(CreateDXGIFactory1)* LoadCreateDXGIFactory1()
		{
			const LibraryHandle dxgi = DLL::LoadSystemLibrary(L"dxgi.dll");
			return DLL::GetFunction(dxgi, "CreateDXGIFactory1");
		}

		/// @brief D3D11_CREATE_DEVICE_DEBUG が使えるかを調べます。
		/// @return D3D11_CREATE_DEVICE_DEBUG が使える場合 true, それ以外の場合 false
		[[nodiscard]]
		static bool HasDebugLayer()
		{
			return (DLL::LoadSystemLibraryNoThrow(L"D3D11_1SDKLayers.dll") != nullptr);
		}

		static void CreateDXGIFactories(decltype(CreateDXGIFactory1)* pCreateDXGIFactory1, ComPtr<IDXGIFactory2>& dxgiFactory2, ComPtr<IDXGIFactory6>& dxgiFactory6)
		{
			// DXGI 1.2
			if (FAILED(pCreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory2))))
			{
				throw InternalEngineError{ "CreateDXGIFactory1() failed" };
			}

			// DXGI 1.6
			if (SUCCEEDED(dxgiFactory2->QueryInterface(IID_PPV_ARGS(&dxgiFactory6))))
			{
				LOG_INFO("ℹ️ DXGI 1.6 is available");
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11Device::init()
	{
		LOG_SCOPED_DEBUG("D3D11Device::init()");

		m_pD3D11CreateDevice	= LoadD3D11CreateDevice();
		m_pCreateDXGIFactory1	= LoadCreateDXGIFactory1();

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			m_hasDebugLayer = HasDebugLayer();
			
			if (m_hasDebugLayer)
			{
				LOG_INFO("ℹ️ D3D11 debug layer is available");
			}
		}

		CreateDXGIFactories(m_pCreateDXGIFactory1, m_DXGIFactory2, m_DXGIFactory6);

		{
			const EngineOption::D3D11Driver targetDriverType = g_engineOptions.d3d11Driver;

			if (targetDriverType == EngineOption::D3D11Driver::Hardware)
			{
				m_hardwareAdapters = D3D11Misc::EnumHardwareAdapters(m_DXGIFactory6.Get(), m_DXGIFactory2.Get(), m_pD3D11CreateDevice, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);
			}
			else if (targetDriverType == EngineOption::D3D11Driver::Hardware_FavorIntegrated)
			{
				m_hardwareAdapters = D3D11Misc::EnumHardwareAdapters(m_DXGIFactory6.Get(), m_DXGIFactory2.Get(), m_pD3D11CreateDevice, DXGI_GPU_PREFERENCE_MINIMUM_POWER);
			}

			m_deviceInfo = D3D11Misc::CreateDevice(m_pD3D11CreateDevice, m_hardwareAdapters, targetDriverType, m_hasDebugLayer);
		}
	}
}
