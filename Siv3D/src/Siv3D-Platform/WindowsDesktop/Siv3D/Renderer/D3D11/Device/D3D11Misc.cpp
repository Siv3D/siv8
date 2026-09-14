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
# include "D3D11AdapterCache.hpp"
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <span>

namespace s3d
{
	namespace
	{
		inline constexpr D3D_FEATURE_LEVEL MinimumD3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		static void SaveAdapterCache(const DXGI_ADAPTER_DESC& adapterDesc, const D3D_FEATURE_LEVEL featureLevel)
		{
			LOG_SCOPED_DEBUG("SaveAdapterCache()");

			const FilePath adapterCacheFilePath = (CacheDirectory::Engine() + U"gpu/adapter.cache");
			const D3D11AdapterCache cacheData =
			{
				.luid			= adapterDesc.AdapterLuid,
				.vendorId		= adapterDesc.VendorId,
				.deviceId		= adapterDesc.DeviceId,
				.subSysId		= adapterDesc.SubSysId,
				.revision		= adapterDesc.Revision,
				.featureLevel	= featureLevel,
			};

			// Preserve the cache lifetime when unchanged, but replace a different GPU's record.
			{
				BinaryFileReader reader{ adapterCacheFilePath };
				D3D11AdapterCache previous{};
				if (reader && reader.readExact(previous)
					&& (std::memcmp(&previous, &cacheData, sizeof(cacheData)) == 0))
				{
					LOG_INFO("ℹ️ Adapter cache is unchanged");
					return;
				}
			}

			BinaryFileWriter writer{ adapterCacheFilePath };
			const std::string adapterName = Unicode::ToUTF8(Unicode::FromWstring(adapterDesc.Description));
			if ((not writer) || (not writer.write(cacheData))
				|| (writer.write(adapterName.data(), adapterName.size()) != static_cast<int64>(adapterName.size())))
			{
				LOG_WARN("Failed to save the adapter cache");
				return;
			}

			LOG_INFO(fmt::format("ℹ️ Adapter cache saved ({})", adapterName));
		}

		[[nodiscard]]
		static bool Match(const DXGI_ADAPTER_DESC1& adapterDesc, const D3D11AdapterCache& cache) noexcept
		{
			return ((std::memcmp(&adapterDesc.AdapterLuid, &cache.luid, sizeof(LUID)) == 0)
				&& (adapterDesc.VendorId == cache.vendorId)
				&& (adapterDesc.DeviceId == cache.deviceId)
				&& (adapterDesc.SubSysId == cache.subSysId)
				&& (adapterDesc.Revision == cache.revision));
		}

		[[nodiscard]]
		static constexpr D3D11Adapter::Vendor ToVendor(const uint32 vendorId) noexcept
		{
			switch (vendorId)
			{
			case 0x10DE:
				return D3D11Adapter::Vendor::NVIDIA;
			case 0x1002:
				return D3D11Adapter::Vendor::AMD;
			case 0x8086:
				return D3D11Adapter::Vendor::Intel;
			case 0x1414:
				return D3D11Adapter::Vendor::Microsoft;
			default:
				return D3D11Adapter::Vendor::Unknown;
			}
		}

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
		static D3D_FEATURE_LEVEL CheckFeatureLevel(IDXGIAdapter1* pAdapter, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice)
		{
			LOG_SCOPED_DEBUG("CheckFeatureLevel()");

			static constexpr D3D_FEATURE_LEVEL FeatureLevels[] =
			{
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			ComPtr<ID3D11Device> pDevice;
			ComPtr<ID3D11DeviceContext> pDeviceContext;
			D3D_FEATURE_LEVEL maxLevel = D3D_FEATURE_LEVEL_9_1;

			LOG_TRACE("D3D11CreateDevice()");

			HRESULT hr = pD3D11CreateDevice(pAdapter,
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				0,
				FeatureLevels,
				static_cast<UINT>(std::size(FeatureLevels)),
				D3D11_SDK_VERSION,
				&pDevice,
				&maxLevel,
				&pDeviceContext);

			if (hr == E_INVALIDARG)
			{
				LOG_TRACE("D3D11CreateDevice()");

				// DirectX 11.1 ランタイムは D3D_FEATURE_LEVEL_12_0 以上を認識しないので、除外してやり直す
				hr = pD3D11CreateDevice(pAdapter,
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					0,
					&FeatureLevels[2],
					static_cast<UINT>(std::size(FeatureLevels) - 2),
					D3D11_SDK_VERSION,
					&pDevice,
					&maxLevel,
					&pDeviceContext);

				if (hr == E_INVALIDARG)
				{
					LOG_TRACE("D3D11CreateDevice()");

					// DirectX 11.0 ランタイムは D3D_FEATURE_LEVEL_11_1 以上を認識しないので、除外してやり直す
					hr = pD3D11CreateDevice(pAdapter,
						D3D_DRIVER_TYPE_UNKNOWN,
						nullptr,
						0,
						&FeatureLevels[3],
						static_cast<UINT>(std::size(FeatureLevels) - 3),
						D3D11_SDK_VERSION,
						&pDevice,
						&maxLevel,
						&pDeviceContext);
				}
			}

			if (FAILED(hr))
			{
				LOG_FAIL("❌ - D3D11CreateDevice() failed");
				return D3D_FEATURE_LEVEL_9_1;
			}

			return maxLevel;
		}

		[[nodiscard]]
		static D3D_FEATURE_LEVEL GetWARPFeatureLevel(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice)
		{
			LOG_SCOPED_DEBUG("GetWARPFeatureLevel()");

			D3D_FEATURE_LEVEL result = D3D_FEATURE_LEVEL_10_1;

			static constexpr D3D_FEATURE_LEVEL FeatureLevels[] =
			{
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1
			};

			LOG_TRACE("D3D11CreateDevice()");

			ComPtr<ID3D11Device> pDevice;
			HRESULT hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP,
				0, 0, FeatureLevels, static_cast<UINT>(std::size(FeatureLevels)),
				D3D11_SDK_VERSION, &pDevice, &result, nullptr);

			if (hr == E_INVALIDARG)
			{
				LOG_TRACE("D3D11CreateDevice()");

				// DirectX 11.1 ランタイムは D3D_FEATURE_LEVEL_12_0 以上を認識しないので、除外してやり直す
				hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP,
					0, 0, &FeatureLevels[2], static_cast<UINT>(std::size(FeatureLevels) - 2),
					D3D11_SDK_VERSION, &pDevice, &result, nullptr);

				if (hr == E_INVALIDARG)
				{
					LOG_TRACE("D3D11CreateDevice()");

					// DirectX 11.0 ランタイムは D3D_FEATURE_LEVEL_11_1 を認識しないので、除外してやり直す
					hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP,
						0, 0, &FeatureLevels[3], static_cast<UINT>(std::size(FeatureLevels) - 3),
						D3D11_SDK_VERSION, &pDevice, &result, nullptr);
				}
			}

			if (FAILED(hr))
			{
				return D3D_FEATURE_LEVEL_10_1;
			}
			else
			{
				return result;
			}
		}

		[[nodiscard]]
		static D3D_FEATURE_LEVEL GetReferenceFeatureLevel(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice)
		{
			LOG_SCOPED_DEBUG("GetReferenceFeatureLevel()");

			D3D_FEATURE_LEVEL result = D3D_FEATURE_LEVEL_10_1;

			static constexpr D3D_FEATURE_LEVEL FeatureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1
			};

			LOG_TRACE("D3D11CreateDevice()");

			ComPtr<ID3D11Device> pDevice;
			HRESULT hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_REFERENCE,
				0, 0, FeatureLevels, static_cast<UINT>(std::size(FeatureLevels)),
				D3D11_SDK_VERSION, &pDevice, &result, nullptr);

			if (hr == E_INVALIDARG)
			{
				LOG_TRACE("D3D11CreateDevice()");

				// DirectX 11.0 ランタイムは D3D_FEATURE_LEVEL_11_1 を認識しないので、除外してやり直す
				hr = pD3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_REFERENCE,
					0, 0, &FeatureLevels[1], static_cast<UINT>(std::size(FeatureLevels) - 1),
					D3D11_SDK_VERSION, &pDevice, &result, nullptr);
			}

			if (FAILED(hr))
			{
				LOG_FAIL("❌ Failed to create D3D11 device with Reference driver");
				return D3D_FEATURE_LEVEL_10_1;
			}
			else
			{
				return result;
			}
		}

		[[nodiscard]]
		static Optional<D3D11DeviceInfo> CreateHardwareDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			IDXGIAdapter1* pAdapter, const std::span<const D3D_FEATURE_LEVEL> featureLevels, const uint32 createDeviceFlag)
		{
			LOG_SCOPED_DEBUG("CreateHardwareDevice()");

			D3D11DeviceInfo deviceInfo;
			const D3D_DRIVER_TYPE driverType = (pAdapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE);

			LOG_TRACE("D3D11CreateDevice()");

			if (FAILED(pD3D11CreateDevice(pAdapter, driverType, nullptr, createDeviceFlag,
				featureLevels.data(), static_cast<UINT>(featureLevels.size()), D3D11_SDK_VERSION,
				&deviceInfo.device, &deviceInfo.featureLevel, &deviceInfo.context)))
			{
				return none;
			}

			deviceInfo.dxgiDevice = GetDXGIDevice1(deviceInfo.device.Get());
			deviceInfo.deviceType = D3D_DRIVER_TYPE_HARDWARE;
			return deviceInfo;
		}

		[[nodiscard]]
		static D3D11DeviceInfo FinishHardwareDevice(D3D11DeviceInfo deviceInfo, const Array<D3D11Adapter>& hardwareAdapters,
			const uint32 createDeviceFlag, const bool usedDefaultAdapter, const bool saveAdapterCache)
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

			if (saveAdapterCache)
			{
				SaveAdapterCache(desc, deviceInfo.featureLevel);
			}

			return deviceInfo;
		}

		[[nodiscard]]
		static Optional<D3D11DeviceInfo> TryHardwareAdapters(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			const Array<D3D11Adapter>& hardwareAdapters, const std::span<const uint32> attempts, const bool saveAdapterCache)
		{
			for (const uint32 flags : attempts)
			{
				for (const auto& adapter : hardwareAdapters)
				{
					if (auto deviceInfo = CreateHardwareDevice(pD3D11CreateDevice, adapter.pAdapter.Get(),
						{ &adapter.featureLevel, 1 }, flags))
					{
						return FinishHardwareDevice(std::move(*deviceInfo), hardwareAdapters, flags, false, saveAdapterCache);
					}
				}
			}

			return none;
		}

		[[nodiscard]]
		static Optional<D3D11DeviceInfo> CreateWARPDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const D3D_FEATURE_LEVEL targetFeatureLevel, const uint32 createDeviceFlag)
		{
			LOG_SCOPED_DEBUG("CreateWARPDevice()");

			D3D_FEATURE_LEVEL featureLevel;
			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> context;

			LOG_TRACE("D3D11CreateDevice()");

			if (SUCCEEDED(pD3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP,
				nullptr,
				createDeviceFlag,
				&targetFeatureLevel,
				1,
				D3D11_SDK_VERSION,
				&device,
				&featureLevel,
				&context)))
			{
				return D3D11DeviceInfo
				{
					.device					= device,
					.dxgiDevice				= GetDXGIDevice1(device.Get()),
					.context				= context,
					.adapterIndex			= none,
					.deviceType				= D3D_DRIVER_TYPE_WARP,
					.featureLevel			= featureLevel,
				};
			}

			return none;
		}

		[[nodiscard]]
		static Optional<D3D11DeviceInfo> CreateReferenceDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const D3D_FEATURE_LEVEL targetFeatureLevel, const uint32 createDeviceFlag)
		{
			LOG_SCOPED_DEBUG("CreateReferenceDevice()");

			D3D_FEATURE_LEVEL featureLevel;
			ComPtr<ID3D11Device> device;
			ComPtr<ID3D11DeviceContext> context;

			LOG_TRACE("D3D11CreateDevice()");

			if (SUCCEEDED(pD3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_REFERENCE,
				nullptr,
				createDeviceFlag,
				&targetFeatureLevel,
				1,
				D3D11_SDK_VERSION,
				&device,
				&featureLevel,
				&context)))
			{
				return D3D11DeviceInfo
				{
					.device					= device,
					.dxgiDevice				= GetDXGIDevice1(device.Get()),
					.context				= context,
					.adapterIndex			= none,
					.deviceType				= D3D_DRIVER_TYPE_REFERENCE,
					.featureLevel			= featureLevel,
				};
			}

			return none;
		}
	}

	namespace D3D11Misc
	{
		////////////////////////////////////////////////////////////////
		//
		//	LoadAdapterCache
		//
		////////////////////////////////////////////////////////////////

		Optional<D3D11AdapterCache> LoadAdapterCache()
		{
			LOG_SCOPED_DEBUG("LoadAdapterCache()");

			const FilePath adapterCacheFilePath = (CacheDirectory::Engine() + U"gpu/adapter.cache");

			if (not FileSystem::Exists(adapterCacheFilePath))
			{
				LOG_INFO("ℹ️ Adapter cache file not found");
				return none;
			}

			if (const auto lastWrite = FileSystem::WriteTime(adapterCacheFilePath))
			{
				const Days duration = DurationCast<Days>(DateTime::Now() - *lastWrite);

				if (Days{ 14 } <= duration)
				{
					// 2 週間以上前のキャッシュファイルの場合、再更新を行う
					LOG_INFO("ℹ️ 14 days or older cache file found");
					FileSystem::Remove(adapterCacheFilePath);

					return none;
				}
			}

			BinaryFileReader reader{ adapterCacheFilePath };

			if (not reader)
			{
				return none;
			}

			if (D3D11AdapterCache cache{}; reader.readExact(cache))
			{
				const size_t adapterNameLength = (reader.size() - reader.getPos());
				std::string adapterName(adapterNameLength, '\0');
				reader.read(adapterName.data(), adapterName.size());

				LOG_INFO(fmt::format("ℹ️ Adapter cache loaded ({})", adapterName));
				return cache;
			}

			return none;
		}

		////////////////////////////////////////////////////////////////
		//
		//	EnumHardwareAdapters
		//
		////////////////////////////////////////////////////////////////

		void EnumHardwareAdapters(HardwareAdapterList& result, IDXGIFactory6* pDXGIFactory6, IDXGIFactory2* pDXGIFactory2,
			PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const DXGI_GPU_PREFERENCE GPU_Preference, const Optional<D3D11AdapterCache>& cache)
		{
			LOG_SCOPED_DEBUG("EnumHardwareAdapters()");

			result.adapters.clear();
			result.usedCache = false;
			auto& adapters = result.adapters;

			for (uint32 adapterIndex = 0; ; ++adapterIndex)
			{
				ComPtr<IDXGIAdapter1> pAdapter;

				// リストの最後で DXGIERR_NOT_FOUND が返る
				{
					HRESULT hr = DXGI_ERROR_NOT_FOUND;

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
				}

				DXGI_ADAPTER_DESC1 adapterDesc;
				pAdapter->GetDesc1(&adapterDesc);

				// キャッシュされたアダプターと一致したらこれ以上調べない
				if (cache && Match(adapterDesc, *cache))
				{
					LOG_INFO("ℹ️ Found a cached hardware adapter");

					D3D11Adapter adapter =
					{
						.pAdapter		= pAdapter,
						.adapterIndex	= adapterIndex,
						.name			= Unicode::FromWstring(adapterDesc.Description),
						.featureLevel	= cache->featureLevel,
						.vendor			= ToVendor(adapterDesc.VendorId),
						.desc			= adapterDesc,
					};

					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {} (feature level: {})", adapterIndex, ToString(adapterDesc), ToString(adapter.featureLevel)));
					adapters.push_back(std::move(adapter));

					// 残りのアダプターをスキップする
					LOG_INFO("ℹ️ Skipped the remaining adapters");
					result.usedCache = true;
					return;
				}

				// Microsoft Basics Display Driver はスキップする
				if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: software adapter (skipped)", adapterIndex));
					continue;
				}

				D3D11Adapter adapter =
				{
					.pAdapter		= pAdapter,
					.adapterIndex	= adapterIndex,
					.name			= Unicode::FromWstring(adapterDesc.Description),
					.featureLevel	= CheckFeatureLevel(pAdapter.Get(), pD3D11CreateDevice),
					.vendor			= ToVendor(adapterDesc.VendorId),
					.desc			= adapterDesc,
				};

				if (adapter.featureLevel < MinimumD3DFeatureLevel)
				{
					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}] does not support D3D_FEATURE_LEVEL_{} (skipped)", adapterIndex, ToString(MinimumD3DFeatureLevel)));
					continue;
				}

				LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {} (feature level: {})", adapterIndex, ToString(adapterDesc), ToString(adapter.featureLevel)));
				adapters.push_back(std::move(adapter));
			}

			LOG_INFO(fmt::format("ℹ️ {} hardware adapter{} available", adapters.size(), ((adapters.size() == 1) ? "" : "s")));
		}

		////////////////////////////////////////////////////////////////
		//
		//	CreateDevice
		//
		////////////////////////////////////////////////////////////////

		D3D11DeviceInfo CreateDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			FunctionRef<void(HardwareAdapterList&, bool)> enumHardwareAdapters,
			EngineOption::D3D11Driver targetDriverType, const bool useDebugLayer)
		{
			LOG_SCOPED_DEBUG("CreateDevice()");

			constexpr uint32 BaseCreateDeviceFlag = 0;
			const bool saveAdapterCache = (targetDriverType == EngineOption::D3D11Driver::Hardware);

			// ハードウェアアダプタ
			if ((targetDriverType == EngineOption::D3D11Driver::Hardware)
				|| (targetDriverType == EngineOption::D3D11Driver::Hardware_FavorIntegrated))
			{
				constexpr uint32 CreateDeviceFlags[] = { (BaseCreateDeviceFlag | D3D11_CREATE_DEVICE_DEBUG), BaseCreateDeviceFlag };
				const auto attempts = std::span{ CreateDeviceFlags }.subspan(useDebugLayer ? 0 : 1);

				HardwareAdapterList hardwareAdapters;
				enumHardwareAdapters(hardwareAdapters, true);

				// Exhaust both debug modes before discarding a cached selection.
				if (auto deviceInfo = TryHardwareAdapters(pD3D11CreateDevice, hardwareAdapters.adapters, attempts, saveAdapterCache))
				{
					return std::move(*deviceInfo);
				}

				if (hardwareAdapters.usedCache)
				{
					LOG_WARN("ℹ️ Cached hardware candidates failed. Enumerating again without the adapter cache");
					enumHardwareAdapters(hardwareAdapters, false);

					// Retry once, including a fresh feature-level probe of the previously cached adapter.
					if (auto deviceInfo = TryHardwareAdapters(pD3D11CreateDevice, hardwareAdapters.adapters, attempts, saveAdapterCache))
					{
						return std::move(*deviceInfo);
					}
				}

				// Keep nullptr + HARDWARE for compatibility with drivers that may reject explicit selection.
				// Try it even without enumerated candidates, and negotiate its own feature level.
				static constexpr D3D_FEATURE_LEVEL DefaultFeatureLevels[] =
				{
					D3D_FEATURE_LEVEL_12_1,
					D3D_FEATURE_LEVEL_12_0,
					D3D_FEATURE_LEVEL_11_1,
					MinimumD3DFeatureLevel,
				};

				LOG_INFO("ℹ️ Explicit hardware adapters failed. Trying the default adapter (nullptr + HARDWARE)");
				for (const uint32 flags : attempts)
				{
					if (auto deviceInfo = CreateHardwareDevice(pD3D11CreateDevice, nullptr, DefaultFeatureLevels, flags))
					{
						return FinishHardwareDevice(std::move(*deviceInfo), hardwareAdapters.adapters, flags, true, saveAdapterCache);
					}
				}

				// ハードウェアアダプタでデバイスを作成できなかった場合、WARP ドライバにフォールバックする
				LOG_WARN("ℹ️ Failed to create D3D11 device with hardware adapters. Fallback to WARP driver");
				targetDriverType = EngineOption::D3D11Driver::WARP;
			}

			// WARP ドライバ
			if (targetDriverType == EngineOption::D3D11Driver::WARP)
			{
				// WARP ドライバの feature level を取得する
				const D3D_FEATURE_LEVEL warpFeatureLevel = GetWARPFeatureLevel(pD3D11CreateDevice);
				LOG_INFO(fmt::format("ℹ️ [D3D_DRIVER_TYPE_WARP] supports D3D_FEATURE_LEVEL_{}", ToString(warpFeatureLevel)));

				if (const auto deviceInfo = CreateWARPDevice(pD3D11CreateDevice, warpFeatureLevel, BaseCreateDeviceFlag))
				{
					LOG_INFO(fmt::format("✅ D3D11 device created. Driver type: WARP (feature level: {0})", ToString(deviceInfo->featureLevel)));
					return *deviceInfo;
				}

				// WARP ドライバでデバイスを作成できなかった場合、Reference ドライバにフォールバックする
				LOG_WARN("ℹ️ Failed to create D3D11 device with WARP driver. Fallback to Reference driver");
				targetDriverType = EngineOption::D3D11Driver::Reference;
			}

			// Reference ドライバ
			if (targetDriverType == EngineOption::D3D11Driver::Reference)
			{
				// Reference ドライバの feature level を取得する
				const D3D_FEATURE_LEVEL referenceFeatureLevel = GetReferenceFeatureLevel(pD3D11CreateDevice);
				LOG_INFO(fmt::format("ℹ️ [D3D_DRIVER_TYPE_REFERENCE] supports D3D_FEATURE_LEVEL_{}", ToString(referenceFeatureLevel)));

				if (const auto deviceInfo = CreateReferenceDevice(pD3D11CreateDevice, referenceFeatureLevel, BaseCreateDeviceFlag))
				{
					return *deviceInfo;
				}

				LOG_FAIL("❌ Failed to create D3D11 device with Reference driver");
			}

			throw InternalEngineError{ "D3D11Misc::CreateDevice() failed" };
		}
	}
}
