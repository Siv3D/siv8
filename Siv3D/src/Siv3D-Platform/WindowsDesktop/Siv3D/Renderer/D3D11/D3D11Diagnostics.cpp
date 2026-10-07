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

# include "D3D11Diagnostics.hpp"
# include <Siv3D/FormatLiteral.hpp>

namespace s3d::D3D11Diagnostics
{
	std::string FormatHRESULT(const HRESULT hr)
	{
		std::string_view name;
		switch (hr)
		{
		case S_OK:
			name = "S_OK"; break;
		case S_FALSE:
			name = "S_FALSE"; break;
		case E_INVALIDARG:
			name = "E_INVALIDARG"; break;
		case E_OUTOFMEMORY:
			name = "E_OUTOFMEMORY"; break;
		case E_FAIL:
			name = "E_FAIL"; break;
		case E_NOINTERFACE:
			name = "E_NOINTERFACE"; break;
		case E_NOTIMPL:
			name = "E_NOTIMPL"; break;
		case E_ACCESSDENIED:
			name = "E_ACCESSDENIED"; break;
		case DXGI_ERROR_INVALID_CALL:
			name = "DXGI_ERROR_INVALID_CALL"; break;
		case DXGI_ERROR_NOT_FOUND:
			name = "DXGI_ERROR_NOT_FOUND"; break;
		case DXGI_ERROR_UNSUPPORTED:
			name = "DXGI_ERROR_UNSUPPORTED"; break;
		case DXGI_ERROR_DEVICE_REMOVED:
			name = "DXGI_ERROR_DEVICE_REMOVED"; break;
		case DXGI_ERROR_DEVICE_HUNG:
			name = "DXGI_ERROR_DEVICE_HUNG"; break;
		case DXGI_ERROR_DEVICE_RESET:
			name = "DXGI_ERROR_DEVICE_RESET"; break;
		case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
			name = "DXGI_ERROR_DRIVER_INTERNAL_ERROR"; break;
		case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
			name = "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE"; break;
		case DXGI_ERROR_SDK_COMPONENT_MISSING:
			name = "DXGI_ERROR_SDK_COMPONENT_MISSING"; break;
		default:
			break;
		}

		if (name.empty())
		{
			return fmt::format("0x{:08X}", static_cast<uint32>(hr));
		}
		return fmt::format("0x{:08X} ({})", static_cast<uint32>(hr), name);
	}

	std::string GetPresentFailureMessage(IDXGISwapChain* swapChain, const HRESULT presentResult,
		const uint32 syncInterval, const uint32 flags)
	{
		std::string message = fmt::format(
			"IDXGISwapChain::Present failed: SyncInterval={}, Flags=0x{:08X}, HRESULT={}",
			syncInterval, flags, FormatHRESULT(presentResult));

		ComPtr<ID3D11Device> device;
		if (const HRESULT hr = swapChain->GetDevice(IID_PPV_ARGS(&device)); FAILED(hr))
		{
			message += fmt::format("; GetDevice(ID3D11Device) failed: HRESULT={}", FormatHRESULT(hr));
		}
		else
		{
			message += fmt::format("; DeviceRemovedReason={}", FormatHRESULT(device->GetDeviceRemovedReason()));
		}

		return message;
	}
}
