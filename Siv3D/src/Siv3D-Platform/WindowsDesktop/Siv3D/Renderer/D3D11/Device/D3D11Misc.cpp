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

# include <Siv3D/Optional.hpp>
# include "D3D11Misc.hpp"
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <span>

namespace s3d
{
	namespace
	{
		inline constexpr D3D_FEATURE_LEVEL FeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
		};

		// Hardware requires 11_0; keep the existing software-driver feature-level ranges.
		inline constexpr auto HardwareFeatureLevels = std::span{ FeatureLevels }.first<4>();
		inline constexpr auto WARPFeatureLevels = std::span{ FeatureLevels };
		inline constexpr auto ReferenceFeatureLevels = std::span{ FeatureLevels }.subspan<2>();

		[[nodiscard]]
		static std::string ToString(const DXGI_ADAPTER_DESC1& desc)
		{
			return fmt::format("{} (Dedicated GPU memory: {}, System: {}, Shared: {})",
				Unicode::FromWstring(desc.Description),
				FormatDataSize(desc.DedicatedVideoMemory),
				FormatDataSize(desc.DedicatedSystemMemory),
				FormatDataSize(desc.SharedSystemMemory));
		}

		[[nodiscard]]
		static constexpr std::string_view ToString(const D3D_FEATURE_LEVEL featureLevel) noexcept
		{
			switch (featureLevel)
			{
			case D3D_FEATURE_LEVEL_9_1:
				return "9_1";
			case D3D_FEATURE_LEVEL_9_2:
				return "9_2";
			case D3D_FEATURE_LEVEL_9_3:
				return "9_3";
			case D3D_FEATURE_LEVEL_10_0:
				return "10_0";
			case D3D_FEATURE_LEVEL_10_1:
				return "10_1";
			case D3D_FEATURE_LEVEL_11_0:
				return "11_0";
			case D3D_FEATURE_LEVEL_11_1:
				return "11_1";
			case D3D_FEATURE_LEVEL_12_0:
				return "12_0";
			case D3D_FEATURE_LEVEL_12_1:
				return "12_1";
			default:
				return "Unknown";
			}
		}

		[[nodiscard]]
		static ComPtr<IDXGIDevice1> GetDXGIDevice1(ID3D11Device* pDevice)
		{
			ComPtr<IDXGIDevice1> dxgiDevice;
			if (FAILED(pDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice))))
			{
				throw InternalEngineError{ "ID3D11Device::QueryInterface(IDXGIDevice1) failed" };
			}
			return dxgiDevice;
		}

		[[nodiscard]]
		static Optional<D3D11DeviceInfo> TryCreateDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			IDXGIAdapter1* pAdapter, const D3D_DRIVER_TYPE driverType,
			std::span<const D3D_FEATURE_LEVEL> featureLevels, const uint32 createDeviceFlag)
		{
			for (;;)
			{
				D3D11DeviceInfo deviceInfo;
				LOG_TRACE("D3D11CreateDevice()");
				const HRESULT hr = pD3D11CreateDevice(pAdapter, driverType, nullptr, createDeviceFlag,
					featureLevels.data(), static_cast<UINT>(featureLevels.size()), D3D11_SDK_VERSION,
					&deviceInfo.device, &deviceInfo.featureLevel, &deviceInfo.context);

				if (SUCCEEDED(hr))
				{
					deviceInfo.dxgiDevice = GetDXGIDevice1(deviceInfo.device.Get());
					deviceInfo.deviceType = (pAdapter ? D3D_DRIVER_TYPE_HARDWARE : driverType);
					return deviceInfo;
				}

				if (hr != E_INVALIDARG)
				{
					return none;
				}

				// Older runtimes reject unrecognized feature levels instead of trying the next one.
				if (D3D_FEATURE_LEVEL_12_0 <= featureLevels.front())
				{
					while (D3D_FEATURE_LEVEL_11_1 < featureLevels.front())
					{
						featureLevels = featureLevels.subspan(1);
					}
				}
				else if (featureLevels.front() == D3D_FEATURE_LEVEL_11_1)
				{
					featureLevels = featureLevels.subspan(1);
				}
				else
				{
					return none;
				}
			}
		}

		[[nodiscard]]
		static D3D11DeviceInfo FinishHardwareDevice(D3D11DeviceInfo deviceInfo, const Array<D3D11Adapter>& hardwareAdapters,
			const uint32 createDeviceFlag, const bool usedDefaultAdapter)
		{
			// Query the created device: the default adapter need not be one of the enumerated candidates.
			ComPtr<IDXGIAdapter> adapter;
			if (FAILED(deviceInfo.dxgiDevice->GetAdapter(&adapter)))
			{
				throw InternalEngineError{ "IDXGIDevice::GetAdapter() failed" };
			}

			DXGI_ADAPTER_DESC desc{};
			if (FAILED(adapter->GetDesc(&desc)))
			{
				throw InternalEngineError{ "IDXGIAdapter::GetDesc() failed" };
			}

			for (const auto& candidate : hardwareAdapters)
			{
				if ((candidate.desc.AdapterLuid.LowPart == desc.AdapterLuid.LowPart)
					&& (candidate.desc.AdapterLuid.HighPart == desc.AdapterLuid.HighPart))
				{
					deviceInfo.adapterIndex = candidate.adapterIndex;
					break;
				}
			}

			LOG_INFO(fmt::format("✅ D3D11 device{} created. Driver type: Hardware ({}) (feature level: {}){}",
				((createDeviceFlag & D3D11_CREATE_DEVICE_DEBUG) ? " with debug layer" : ""),
				Unicode::FromWstring(desc.Description), ToString(deviceInfo.featureLevel),
				(usedDefaultAdapter ? " (default adapter fallback)" : "")));

			return deviceInfo;
		}
	}

	namespace D3D11Misc
	{
		////////////////////////////////////////////////////////////////
		//
		//	EnumHardwareAdapters
		//
		////////////////////////////////////////////////////////////////

		void EnumHardwareAdapters(Array<D3D11Adapter>& result, IDXGIFactory6* pDXGIFactory6, IDXGIFactory2* pDXGIFactory2,
			const DXGI_GPU_PREFERENCE GPU_Preference)
		{
			LOG_SCOPED_DEBUG("EnumHardwareAdapters()");
			result.clear();

			for (uint32 adapterIndex = 0; ; ++adapterIndex)
			{
				ComPtr<IDXGIAdapter1> pAdapter;
				HRESULT hr;

				if (pDXGIFactory6)
				{
					LOG_TRACE("IDXGIFactory6::EnumAdapterByGpuPreference()");
					hr = pDXGIFactory6->EnumAdapterByGpuPreference(adapterIndex, GPU_Preference, IID_PPV_ARGS(&pAdapter));
				}
				else
				{
					LOG_TRACE("IDXGIFactory2::EnumAdapters1()");
					hr = pDXGIFactory2->EnumAdapters1(adapterIndex, &pAdapter);
				}

				if (hr == DXGI_ERROR_NOT_FOUND)
				{
					LOG_TRACE("-> DXGI_ERROR_NOT_FOUND");
					break;
				}

				if (FAILED(hr))
				{
					LOG_WARN("Failed to enumerate a DXGI adapter");
					break;
				}

				DXGI_ADAPTER_DESC1 adapterDesc{};
				if (FAILED(pAdapter->GetDesc1(&adapterDesc)))
				{
					LOG_WARN("IDXGIAdapter1::GetDesc1() failed");
					continue;
				}

				// Microsoft Basic Render Driver is handled by the WARP fallback.
				if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {} (skipped)", adapterIndex, ToString(adapterDesc)));
					continue;
				}

				LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {}", adapterIndex, ToString(adapterDesc)));
				result.push_back(D3D11Adapter
				{
					.pAdapter = std::move(pAdapter),
					.adapterIndex = adapterIndex,
					.desc = adapterDesc,
				});
			}

			LOG_INFO(fmt::format("ℹ️ {} hardware adapter candidate{}", result.size(), ((result.size() == 1) ? "" : "s")));
		}

		////////////////////////////////////////////////////////////////
		//
		//	CreateDevice
		//
		////////////////////////////////////////////////////////////////

		D3D11DeviceInfo CreateDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			FunctionRef<void(Array<D3D11Adapter>&)> enumHardwareAdapters,
			EngineOption::D3D11Driver targetDriverType, const bool useDebugLayer)
		{
			LOG_SCOPED_DEBUG("CreateDevice()");
			constexpr uint32 BaseCreateDeviceFlag = 0;

			if ((targetDriverType == EngineOption::D3D11Driver::Hardware)
				|| (targetDriverType == EngineOption::D3D11Driver::Hardware_FavorIntegrated))
			{
				constexpr uint32 CreateDeviceFlags[] = { (BaseCreateDeviceFlag | D3D11_CREATE_DEVICE_DEBUG), BaseCreateDeviceFlag };
				const auto attempts = std::span{ CreateDeviceFlags }.subspan(useDebugLayer ? 0 : 1);

				Array<D3D11Adapter> hardwareAdapters;
				enumHardwareAdapters(hardwareAdapters);

				// Try every explicit candidate in both debug modes before the default adapter.
				for (const uint32 flags : attempts)
				{
					for (const auto& adapter : hardwareAdapters)
					{
						if (auto deviceInfo = TryCreateDevice(pD3D11CreateDevice, adapter.pAdapter.Get(),
							D3D_DRIVER_TYPE_UNKNOWN, HardwareFeatureLevels, flags))
						{
							return FinishHardwareDevice(std::move(*deviceInfo), hardwareAdapters, flags, false);
						}
					}
				}

				// Keep nullptr + HARDWARE for drivers that may reject explicit selection, even with no candidates.
				LOG_INFO("ℹ️ Explicit hardware adapters failed. Trying the default adapter (nullptr + HARDWARE)");
				for (const uint32 flags : attempts)
				{
					if (auto deviceInfo = TryCreateDevice(pD3D11CreateDevice, nullptr, D3D_DRIVER_TYPE_HARDWARE, HardwareFeatureLevels, flags))
					{
						return FinishHardwareDevice(std::move(*deviceInfo), hardwareAdapters, flags, true);
					}
				}

				LOG_WARN("ℹ️ Failed to create D3D11 device with hardware adapters. Fallback to WARP driver");
				targetDriverType = EngineOption::D3D11Driver::WARP;
			}

			if (targetDriverType == EngineOption::D3D11Driver::WARP)
			{
				if (auto deviceInfo = TryCreateDevice(pD3D11CreateDevice, nullptr, D3D_DRIVER_TYPE_WARP, WARPFeatureLevels, BaseCreateDeviceFlag))
				{
					LOG_INFO(fmt::format("✅ D3D11 device created. Driver type: WARP (feature level: {})", ToString(deviceInfo->featureLevel)));
					return std::move(*deviceInfo);
				}

				LOG_WARN("ℹ️ Failed to create D3D11 device with WARP driver. Fallback to Reference driver");
				targetDriverType = EngineOption::D3D11Driver::Reference;
			}

			if (targetDriverType == EngineOption::D3D11Driver::Reference)
			{
				if (auto deviceInfo = TryCreateDevice(pD3D11CreateDevice, nullptr, D3D_DRIVER_TYPE_REFERENCE, ReferenceFeatureLevels, BaseCreateDeviceFlag))
				{
					LOG_INFO(fmt::format("✅ D3D11 device created. Driver type: Reference (feature level: {})", ToString(deviceInfo->featureLevel)));
					return std::move(*deviceInfo);
				}

				LOG_FAIL("❌ Failed to create D3D11 device with Reference driver");
			}

			throw InternalEngineError{ "D3D11Misc::CreateDevice() failed" };
		}
	}
}
