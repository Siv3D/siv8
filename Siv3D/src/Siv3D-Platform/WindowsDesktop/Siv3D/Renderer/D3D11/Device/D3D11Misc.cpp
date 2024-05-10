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

# include <Siv3D/Optional.hpp>
# include "D3D11Misc.hpp"
# include "D3D11AdapterCache.hpp"
# include <Siv3D/CacheDirectory/CacheDirectory.hpp>
# include <Siv3D/FormatUtility.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Optional<D3D11AdapterCache> LoadAdapterCache()
		{
			const FilePath adapterCacheFile = (CacheDirectory::Engine() + U"gpu/adapter.cache");

			if (not FileSystem::Exists(adapterCacheFile))
			{
				LOG_INFO("ℹ️ Adapter cache file not found");
				return none;
			}

			if (const auto lastWrite = FileSystem::WriteTime(adapterCacheFile))
			{
				const Days duration = DurationCast<Days>(DateTime::Now() - *lastWrite);

				if (Days{ 14 } < duration)
				{
					// 2 週間以上前のキャッシュファイルの場合、再更新を行う
					LOG_INFO("ℹ️ 14 days or older cache file found");
					return none;
				}
			}

			BinaryReader reader{ adapterCacheFile };

			if (not reader)
			{
				return none;
			}

			if (D3D11AdapterCache cache{}; reader.read(cache))
			{
				return cache;
			}

			return none;
		}

		static void SaveAdapterCache(const D3D11AdapterCache& cache)
		{
			const FilePath adapterCacheFile = (CacheDirectory::Engine() + U"gpu/adapter.cache");
			
			BinaryWriter{ adapterCacheFile }.write(cache);
		}

		[[nodiscard]]
		static bool Match(const DXGI_ADAPTER_DESC& adapterDesc, const D3D11AdapterCache& cache) noexcept
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
		static std::string ToString(const DXGI_ADAPTER_DESC& desc)
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
				return "D3D_FEATURE_LEVEL_9_1";
			case D3D_FEATURE_LEVEL_9_2:
				return "D3D_FEATURE_LEVEL_9_2";
			case D3D_FEATURE_LEVEL_9_3:
				return "D3D_FEATURE_LEVEL_9_3";
			case D3D_FEATURE_LEVEL_10_0:
				return "D3D_FEATURE_LEVEL_10_0";
			case D3D_FEATURE_LEVEL_10_1:
				return "D3D_FEATURE_LEVEL_10_1";
			case D3D_FEATURE_LEVEL_11_0:
				return "D3D_FEATURE_LEVEL_11_0";
			case D3D_FEATURE_LEVEL_11_1:
				return "D3D_FEATURE_LEVEL_11_1";
			case D3D_FEATURE_LEVEL_12_0:
				return "D3D_FEATURE_LEVEL_12_0";
			case D3D_FEATURE_LEVEL_12_1:
				return "D3D_FEATURE_LEVEL_12_1";
			default:
				return "Unknown";
			}
		}

		struct AdapterDesc2
		{
			bool d3d11_1_runtimeSupport = false;
			bool isSoftware = false;
		};

		[[nodiscard]]
		static AdapterDesc2 GetAdapterDesc2(IDXGIAdapter* pAdapter)
		{
			AdapterDesc2 desc2;
			ComPtr<IDXGIAdapter2> pAdapter2;

			if (SUCCEEDED(pAdapter->QueryInterface(__uuidof(IDXGIAdapter2), &pAdapter2)))
			{
				DXGI_ADAPTER_DESC2 desc;

				if ((SUCCEEDED(pAdapter2->GetDesc2(&desc))
					&& (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)))
				{
					desc2.isSoftware = true;
				}

				desc2.d3d11_1_runtimeSupport = true;
			}

			return desc2;
		}

		[[nodiscard]]
		static bool CheckHardwareDevice(D3D11Adapter& adapter, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const D3D_FEATURE_LEVEL minimumFeatureLevel, const Optional<D3D_FEATURE_LEVEL>& requestedFeatureLevel)
		{
			static constexpr D3D_FEATURE_LEVEL FeatureLevels[] =
			{
			# if defined(SIV3D_USE_DIRECT3D11_3) || defined(SIV3D_USE_DIRECT3D11_4) 
				D3D_FEATURE_LEVEL_12_1,
				D3D_FEATURE_LEVEL_12_0,
			# endif
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0
			};

			ComPtr<ID3D11Device> pDevice;
			ComPtr<ID3D11DeviceContext> pDeviceContext;

			LOG_DEBUG("D3D11CreateDevice()");

			HRESULT hr = pD3D11CreateDevice(adapter.pAdapter.Get(),
				D3D_DRIVER_TYPE_UNKNOWN,
				nullptr,
				0,
				FeatureLevels,
				static_cast<UINT>(std::size(FeatureLevels)),
				D3D11_SDK_VERSION,
				&pDevice,
				&adapter.maxLevel,
				&pDeviceContext);

			if (hr == E_INVALIDARG)
			{
			# if defined(SIV3D_USE_DIRECT3D11_3) || defined(SIV3D_USE_DIRECT3D11_4) 

				LOG_DEBUG("D3D11CreateDevice()");

				// DirectX 11.1 ランタイムは D3D_FEATURE_LEVEL_12_x を認識しないので、除外してやり直す
				hr = pD3D11CreateDevice(adapter.pAdapter.Get(),
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					0,
					&FeatureLevels[2],
					static_cast<UINT>(std::size(FeatureLevels) - 2),
					D3D11_SDK_VERSION,
					&pDevice,
					&adapter.maxLevel,
					&pDeviceContext);

				if (hr == E_INVALIDARG)
				{
					LOG_DEBUG("D3D11CreateDevice()");

					// DirectX 11.0 ランタイムは D3D_FEATURE_LEVEL_11_1 を認識しないので、除外してやり直す
					hr = pD3D11CreateDevice(adapter.pAdapter.Get(),
						D3D_DRIVER_TYPE_UNKNOWN,
						nullptr,
						0,
						&FeatureLevels[3],
						static_cast<UINT>(std::size(FeatureLevels) - 3),
						D3D11_SDK_VERSION,
						&pDevice,
						&adapter.maxLevel,
						&pDeviceContext);
				}

			# else

				// DirectX 11.0 ランタイムは D3D_FEATURE_LEVEL_11_1 を認識しないので、除外してやり直す
				hr = pD3D11CreateDevice(adapter.pAdapter.Get(),
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					0,
					&FeatureLevels[1],
					static_cast<UINT>(std::size(FeatureLevels) - 1),
					D3D11_SDK_VERSION,
					&pDevice,
					&adapter.maxLevel,
					&pDeviceContext);

			# endif
			}

			if (FAILED(hr))
			{
				LOG_FAIL("❌ - D3D11CreateDevice() failed");
				return false;
			}
			else if (adapter.maxLevel < minimumFeatureLevel)
			{
				LOG_FAIL(fmt::format("❌ - adapter.maxLevel ({}) does not meet minimumFeatureLevel requirements ({})",
					ToString(adapter.maxLevel), ToString(minimumFeatureLevel)));
				return false;
			}

			if (!requestedFeatureLevel || (requestedFeatureLevel == adapter.maxLevel))
			{
				adapter.selectedLevel = adapter.maxLevel;
			}
			else if (requestedFeatureLevel > adapter.maxLevel)
			{
				LOG_FAIL(fmt::format("❌ - adapterInfo.maxLevel ({}) does not meet requestedFeatureLevel requirements ({})",
					ToString(adapter.maxLevel), ToString(*requestedFeatureLevel)));
				return false;
			}
			else
			{
				pDevice.Reset();
				pDeviceContext.Reset();

				LOG_DEBUG("D3D11CreateDevice()");

				D3D_FEATURE_LEVEL returnedFeatureLevel;
				hr = pD3D11CreateDevice(adapter.pAdapter.Get(),
					D3D_DRIVER_TYPE_UNKNOWN,
					nullptr,
					0,
					&requestedFeatureLevel.value(),
					1,
					D3D11_SDK_VERSION,
					&pDevice,
					&returnedFeatureLevel,
					&pDeviceContext);

				if (SUCCEEDED(hr) && (returnedFeatureLevel == requestedFeatureLevel))
				{
					adapter.selectedLevel = *requestedFeatureLevel;
				}
				else
				{
					LOG_FAIL(fmt::format("❌ - D3D11CreateDevice() failed with specified requestedFeatureLevel {}", ToString(*requestedFeatureLevel)));
					return false;
				}
			}

			if (D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS ho = {};
				SUCCEEDED(pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &ho, sizeof(ho))))
			{
				adapter.computeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x = ho.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x;
			}

			LOG_INFO(fmt::format("ℹ️ - [D3D_DRIVER_TYPE_HARDWARE] selected: {} (max: {})", ToString(adapter.selectedLevel), ToString(adapter.maxLevel)));
			return true;
		}

		[[nodiscard]]
		static Array<D3D11Adapter> GetAdapters_impl(IDXGIFactory2* pDXGIFactory2, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const D3D11AdapterCache& cache)
		{
			Array<D3D11Adapter> adapters;

			for (uint32 adapterIndex = 0; ; ++adapterIndex)
			{
				ComPtr<IDXGIAdapter> pAdapter;

				// リストの最後で DXGIERR_NOT_FOUND が返る
				if (FAILED(pDXGIFactory2->EnumAdapters(adapterIndex, &pAdapter)))
				{
					break;
				}

				DXGI_ADAPTER_DESC adapterDesc;
				pAdapter->GetDesc(&adapterDesc);

				// キャッシュされたアダプターと一致したらこれ以上調べない
				if (Match(adapterDesc, cache))
				{
					LOG_INFO("ℹ️ Found a cached adapter");

					D3D11Adapter adapter =
					{
						.pAdapter		= pAdapter,
						.name			= Unicode::FromWstring(adapterDesc.Description),
						.maxLevel		= cache.maxLevel,
						.selectedLevel	= cache.selectedLevel,
						.adapterIndex	= adapterIndex,
						.vendor			= ToVendor(adapterDesc.VendorId),
						.d3d11_1_runtimeSupport = cache.d3d11_1_runtimeSupport,
						.computeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x = cache.computeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x
					};

					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {} (supports Direct3D 11.{} runtime)",
						adapterIndex, ToString(adapterDesc), (adapter.d3d11_1_runtimeSupport ? 1 : 0)));

					adapters.push_back(adapter);
					break;
				}

				// Direct3D 11.1 ランタイムが使用可能かチェックする
				const auto [d3d11_1_runtimeSupport, isSoftware] = GetAdapterDesc2(pAdapter.Get());

				// Microsoft Basics Display Driver はスキップする
				if (isSoftware)
				{
					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}] is Microsoft Basics Display Driver (skipped)", adapterIndex));
					continue;
				}

				D3D11Adapter adapter =
				{
					.pAdapter		= pAdapter,
					.name			= Unicode::FromWstring(adapterDesc.Description),
					.adapterIndex	= adapterIndex,
					.vendor			= ToVendor(adapterDesc.VendorId),
					.d3d11_1_runtimeSupport = d3d11_1_runtimeSupport,
				};

				LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {} (supports Direct3D 11.{} runtime)",
					adapterIndex, ToString(adapterDesc), (d3d11_1_runtimeSupport ? 1 : 0)));

				if (not CheckHardwareDevice(adapter, pD3D11CreateDevice, D3D_FEATURE_LEVEL_10_0, unspecified))
				{
					LOG_FAIL("❌ - getHardwareDevice() failed");
					continue;
				}

				adapters.push_back(adapter);
			}

			return adapters;
		}

		[[nodiscard]]
		static Array<D3D11Adapter> GetAdapters_impl(IDXGIFactory2* pDXGIFactory2, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice)
		{
			Array<D3D11Adapter> adapters;

			for (uint32 adapterIndex = 0; ; ++adapterIndex)
			{
				ComPtr<IDXGIAdapter> pAdapter;

				// リストの最後で DXGIERR_NOT_FOUND が返る
				if (FAILED(pDXGIFactory2->EnumAdapters(adapterIndex, &pAdapter)))
				{
					break;
				}

				// Direct3D 11.1 ランタイムが使用可能かチェックする
				const auto [d3d11_1_runtimeSupport, isSoftware] = GetAdapterDesc2(pAdapter.Get());

				// Microsoft Basics Display Driver はスキップする
				if (isSoftware)
				{
					LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}] is Microsoft Basics Display Driver (skipped)", adapterIndex));
					continue;
				}

				DXGI_ADAPTER_DESC adapterDesc;
				pAdapter->GetDesc(&adapterDesc);

				D3D11Adapter adapter =
				{
					.pAdapter					= pAdapter,
					.name						= Unicode::FromWstring(adapterDesc.Description),
					.adapterIndex				= adapterIndex,
					.vendor						= ToVendor(adapterDesc.VendorId),
					.d3d11_1_runtimeSupport		= d3d11_1_runtimeSupport,
				};

				LOG_INFO(fmt::format("ℹ️ IDXGIAdapter [{}]: {} (supports Direct3D 11.{} runtime)",
					adapterIndex, ToString(adapterDesc), (d3d11_1_runtimeSupport ? 1 : 0)));

				if (not CheckHardwareDevice(adapter, pD3D11CreateDevice, D3D_FEATURE_LEVEL_10_0, unspecified))
				{
					LOG_FAIL("❌ - getHardwareDevice() failed");
					continue;
				}

				adapters.push_back(adapter);
			}

			return adapters;
		}
	}

	namespace D3D11Misc
	{
		Array<D3D11Adapter> GetAdapters(IDXGIFactory2* pDXGIFactory2, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const bool GPU_Preference_HighPerformance)
		{
			Optional<D3D11AdapterCache> cache;

			if (GPU_Preference_HighPerformance)
			{
				cache = LoadAdapterCache();
			}

			Array<D3D11Adapter> adapters;

			if (cache)
			{
				adapters = GetAdapters_impl(pDXGIFactory2, pD3D11CreateDevice, *cache);
			}
			else
			{
				adapters = GetAdapters_impl(pDXGIFactory2, pD3D11CreateDevice);
			}

			LOG_INFO(fmt::format("ℹ️ {} adapter{} available", adapters.size(), ((adapters.size() == 1) ? "" : "s")));

			return adapters;
		}
	}
}
