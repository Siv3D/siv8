//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
//
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
//
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)

# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Renderer/D3D11/Device/D3D11Misc.hpp>

namespace
{
	struct DeviceCreationCall
	{
		IDXGIAdapter* adapter;
		D3D_DRIVER_TYPE driverType;
		UINT flags;
		Array<D3D_FEATURE_LEVEL> featureLevels;
	};

	class DeviceCreationScript
	{
	public:

		DeviceCreationScript()
		{
			const auto& backing = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice();
			m_device = backing.getDevice();
			m_context = backing.getContext();
			ComPtr<IDXGIAdapter> adapter;
			REQUIRE(SUCCEEDED(backing.getDXGIDevice1()->GetAdapter(&adapter)));
			ComPtr<IDXGIAdapter1> adapter1;
			REQUIRE(SUCCEEDED(adapter.As(&adapter1)));
			DXGI_ADAPTER_DESC1 desc{};
			REQUIRE(SUCCEEDED(adapter1->GetDesc1(&desc)));

			// Creation is scripted; real COM objects exercise the device's actual GPU identity queries.
			// Candidate metadata deliberately differs from the returned device for the first candidate.
			D3D11Adapter candidate;
			candidate.pAdapter = adapter1;
			candidate.desc = desc;
			candidate.desc.AdapterLuid.LowPart ^= 1;
			candidate.name = U"Unrelated candidate";
			candidate.adapterIndex = 3;
			candidate.featureLevel = D3D_FEATURE_LEVEL_12_1;
			candidates.push_back(candidate);
			candidate.desc = desc;
			candidate.name = Unicode::FromWstring(desc.Description);
			candidate.adapterIndex = 9;
			candidate.featureLevel = D3D_FEATURE_LEVEL_11_0;
			candidates.push_back(candidate);
			m_previous = s_active;
			s_active = this;
		}

		~DeviceCreationScript()
		{
			s_active = m_previous;
		}

		DeviceCreationScript(const DeviceCreationScript&) = delete;
		DeviceCreationScript& operator =(const DeviceCreationScript&) = delete;

		D3D11DeviceInfo run(const bool debug = false,
			const EngineOption::D3D11Driver driver = EngineOption::D3D11Driver::Hardware_FavorIntegrated)
		{
			// FavorIntegrated exercises hardware selection without touching the user's adapter cache.
			return D3D11Misc::CreateDevice(CreateDevice,
				[&](D3D11Misc::HardwareAdapterList& result, const bool useCache)
				{
					cacheRequests.push_back(useCache);
					creationCountsAtEnumeration.push_back(calls.size());
					result.adapters = (useCache ? candidates : refreshedCandidates);
					result.usedCache = (useCache && usedCache);
				}, driver, debug);
		}

		void enumerate(D3D11Misc::HardwareAdapterList& result, const Optional<D3D11AdapterCache>& cache)
		{
			const auto& backing = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice();
			D3D11Misc::EnumHardwareAdapters(result, nullptr, backing.getDXGIFactory2(), CreateDevice,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, cache);
		}

		D3D11AdapterCache firstAdapterCache()
		{
			const auto& backing = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice();
			ComPtr<IDXGIAdapter1> adapter;
			REQUIRE(SUCCEEDED(backing.getDXGIFactory2()->EnumAdapters1(0, &adapter)));
			DXGI_ADAPTER_DESC1 desc{};
			REQUIRE(SUCCEEDED(adapter->GetDesc1(&desc)));
			return D3D11AdapterCache{
				.luid = desc.AdapterLuid,
				.vendorId = desc.VendorId,
				.deviceId = desc.DeviceId,
				.subSysId = desc.SubSysId,
				.revision = desc.Revision,
				.featureLevel = D3D_FEATURE_LEVEL_12_1,
			};
		}

		Array<D3D11Adapter> candidates;
		Array<D3D11Adapter> refreshedCandidates;
		bool usedCache = false;
		Array<bool> cacheRequests;
		Array<size_t> creationCountsAtEnumeration;
		Array<HRESULT> results;
		HRESULT defaultResult = E_FAIL;
		Array<DeviceCreationCall> calls;

	private:

		static HRESULT WINAPI CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE driverType, HMODULE software,
			UINT flags, const D3D_FEATURE_LEVEL* featureLevels, UINT featureLevelCount, UINT sdkVersion,
			ID3D11Device** device, D3D_FEATURE_LEVEL* featureLevel, ID3D11DeviceContext** context)
		{
			auto& script = *s_active;
			CHECK(software == nullptr);
			CHECK(sdkVersion == D3D11_SDK_VERSION);
			script.calls.push_back({ adapter, driverType, flags,
				Array<D3D_FEATURE_LEVEL>{ featureLevels, (featureLevels + featureLevelCount) } });
			const size_t index = (script.calls.size() - 1);
			const HRESULT result = ((index < script.results.size()) ? script.results[index] : script.defaultResult);
			if (FAILED(result))
			{
				return result;
			}

			CHECK(script.calls.back().featureLevels.contains(D3D_FEATURE_LEVEL_11_0));
			if (device)
			{
				script.m_device.CopyTo(device);
			}
			if (context)
			{
				script.m_context.CopyTo(context);
			}
			if (featureLevel)
			{
				*featureLevel = D3D_FEATURE_LEVEL_11_0;
			}
			return result;
		}

		inline static thread_local DeviceCreationScript* s_active = nullptr;
		DeviceCreationScript* m_previous = nullptr;
		ComPtr<ID3D11Device> m_device;
		ComPtr<ID3D11DeviceContext> m_context;
	};
}

TEST_CASE("D3D11Device.first_candidate_with_debug_stops_selection")
{
	DeviceCreationScript script;
	script.candidates.erase(script.candidates.begin());
	script.results = { S_OK };
	const auto device = script.run(true);
	REQUIRE(script.calls.size() == 1);
	CHECK(script.calls[0].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(script.calls[0].flags == static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG));
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.explicit_candidates_precede_default_adapter")
{
	DeviceCreationScript script;
	script.results = { E_FAIL, S_OK };
	const auto device = script.run();
	REQUIRE(script.calls.size() == 2);
	CHECK(script.calls[0].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(script.calls[1].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(script.calls[0].featureLevels.front() == D3D_FEATURE_LEVEL_12_1);
	CHECK(script.calls[1].featureLevels.front() == D3D_FEATURE_LEVEL_11_0);
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.explicit_candidates_without_debug_precede_default_adapter")
{
	DeviceCreationScript script;
	script.results = { E_FAIL, E_FAIL, E_FAIL, S_OK };
	const auto device = script.run(true);
	REQUIRE(script.calls.size() == 4);
	for (size_t i = 0; i < script.calls.size(); ++i)
	{
		CHECK(script.calls[i].adapter != nullptr);
		CHECK(script.calls[i].driverType == D3D_DRIVER_TYPE_UNKNOWN);
		CHECK(script.calls[i].flags == ((i < 2) ? static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG) : 0u));
	}
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.default_adapter_resolves_actual_index_and_feature_level")
{
	DeviceCreationScript script;
	// Neither candidate's requested level is suitable for the default adapter in this scenario.
	script.candidates[1].featureLevel = D3D_FEATURE_LEVEL_11_1;
	script.results = { E_FAIL, E_FAIL, S_OK };
	const auto device = script.run();
	REQUIRE(script.calls.size() == 3);
	const auto& call = script.calls.back();
	CHECK(call.adapter == nullptr);
	CHECK(call.driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(call.featureLevels.front() == D3D_FEATURE_LEVEL_12_1);
	CHECK(call.featureLevels.back() == D3D_FEATURE_LEVEL_11_0);
	CHECK(device.adapterIndex == 9u);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(device.featureLevel == D3D_FEATURE_LEVEL_11_0);
}

TEST_CASE("D3D11Device.default_adapter_does_not_inherit_an_unrelated_index")
{
	DeviceCreationScript script;
	script.candidates.pop_back();
	script.results = { E_FAIL, S_OK };
	const auto device = script.run();
	CHECK(not device.adapterIndex);
	REQUIRE(script.calls.size() == 2);
	CHECK(script.calls.back().driverType == D3D_DRIVER_TYPE_HARDWARE);
}

TEST_CASE("D3D11Device.empty_candidates_still_try_default_adapter_with_debug_fallback")
{
	DeviceCreationScript script;
	script.candidates.clear();
	script.results = { DXGI_ERROR_SDK_COMPONENT_MISSING, S_OK };
	const auto device = script.run(true);
	REQUIRE(script.calls.size() == 2);
	for (const auto& call : script.calls)
	{
		CHECK(call.adapter == nullptr);
		CHECK(call.driverType == D3D_DRIVER_TYPE_HARDWARE);
	}
	CHECK(script.calls[0].flags == static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG));
	CHECK(script.calls[1].flags == 0u);
	CHECK(not device.adapterIndex);
}

TEST_CASE("D3D11Device.default_adapter_retries_without_debug_after_all_explicit_attempts")
{
	DeviceCreationScript script;
	script.results = { E_FAIL, E_FAIL, E_FAIL, E_FAIL, DXGI_ERROR_SDK_COMPONENT_MISSING, S_OK };
	const auto device = script.run(true);
	REQUIRE(script.calls.size() == 6);
	for (size_t i = 0; i < 4; ++i)
	{
		CHECK(script.calls[i].driverType == D3D_DRIVER_TYPE_UNKNOWN);
		CHECK(script.calls[i].adapter != nullptr);
	}
	for (size_t i = 4; i < 6; ++i)
	{
		CHECK(script.calls[i].driverType == D3D_DRIVER_TYPE_HARDWARE);
		CHECK(script.calls[i].adapter == nullptr);
	}
	CHECK(script.calls[4].flags == static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG));
	CHECK(script.calls[5].flags == 0u);
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.warp_follows_default_hardware_failure")
{
	DeviceCreationScript script;
	script.results = { E_FAIL, E_FAIL, E_FAIL, S_OK, S_OK };
	const auto device = script.run();
	REQUIRE(script.calls.size() == 5);
	CHECK(script.calls[2].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[4].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_WARP);
	CHECK(not device.adapterIndex);
	CHECK(script.cacheRequests == Array<bool>{ true });
}

TEST_CASE("D3D11Device.explicit_software_driver_bypasses_hardware_selection")
{
	for (const auto driver : { EngineOption::D3D11Driver::WARP, EngineOption::D3D11Driver::Reference })
	{
		DeviceCreationScript script;
		script.results = { S_OK, S_OK };
		const auto device = script.run(false, driver);
		const auto expected = ((driver == EngineOption::D3D11Driver::WARP) ? D3D_DRIVER_TYPE_WARP : D3D_DRIVER_TYPE_REFERENCE);
		REQUIRE(script.calls.size() == 2);
		CHECK(script.calls[0].driverType == expected);
		CHECK(script.calls[1].driverType == expected);
		CHECK(device.deviceType == expected);
		CHECK(script.cacheRequests.isEmpty());
	}
}

TEST_CASE("D3D11Device.all_creation_paths_fail")
{
	DeviceCreationScript script;
	CHECK_THROWS_AS(script.run(), InternalEngineError);
	REQUIRE(script.calls.size() == 7);
	CHECK(script.calls[2].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[5].driverType == D3D_DRIVER_TYPE_REFERENCE);
}

TEST_CASE("D3D11Device.cached_success_avoids_reenumeration")
{
	DeviceCreationScript script;
	script.candidates.erase(script.candidates.begin());
	script.usedCache = true;
	script.results = { S_OK };
	const auto device = script.run();
	CHECK(script.cacheRequests == Array<bool>{ true });
	CHECK(script.calls.size() == 1);
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.cached_success_without_debug_avoids_reenumeration")
{
	DeviceCreationScript script;
	script.candidates.erase(script.candidates.begin());
	script.usedCache = true;
	script.results = { DXGI_ERROR_SDK_COMPONENT_MISSING, S_OK };
	const auto device = script.run(true);
	CHECK(script.cacheRequests == Array<bool>{ true });
	CHECK(script.calls.size() == 2);
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.cache_failure_recovers_a_later_adapter")
{
	DeviceCreationScript script;
	script.refreshedCandidates = script.candidates;
	script.candidates.pop_back();
	script.usedCache = true;
	script.results = { E_FAIL, E_FAIL, S_OK };
	const auto device = script.run();
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0, 1 });
	REQUIRE(script.calls.size() == 3);
	for (const auto& call : script.calls)
	{
		CHECK(call.driverType == D3D_DRIVER_TYPE_UNKNOWN);
	}
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.stale_cached_feature_level_is_replaced_before_default_fallback")
{
	DeviceCreationScript script;
	script.candidates.erase(script.candidates.begin());
	script.refreshedCandidates = script.candidates;
	script.candidates.front().featureLevel = D3D_FEATURE_LEVEL_12_1;
	script.usedCache = true;
	script.results = { E_FAIL, S_OK };
	const auto device = script.run();
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	REQUIRE(script.calls.size() == 2);
	CHECK(script.calls[0].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_12_1 });
	CHECK(script.calls[1].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_0 });
	CHECK(script.calls[1].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(device.featureLevel == D3D_FEATURE_LEVEL_11_0);
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.cache_failure_exhausts_debug_modes_before_reenumeration")
{
	DeviceCreationScript script;
	script.candidates.erase(script.candidates.begin());
	script.refreshedCandidates = script.candidates;
	script.candidates.front().featureLevel = D3D_FEATURE_LEVEL_12_1;
	script.usedCache = true;
	script.results = { E_FAIL, E_FAIL, S_OK };
	const auto device = script.run(true);
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0, 2 });
	REQUIRE(script.calls.size() == 3);
	CHECK(script.calls[0].flags == static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG));
	CHECK(script.calls[1].flags == 0u);
	CHECK(script.calls[2].flags == static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG));
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.failed_cache_refresh_precedes_default_adapter")
{
	DeviceCreationScript script;
	script.refreshedCandidates = script.candidates;
	script.candidates.pop_back();
	script.usedCache = true;
	script.results = { E_FAIL, E_FAIL, E_FAIL, S_OK };
	const auto device = script.run();
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	REQUIRE(script.calls.size() == 4);
	CHECK(script.calls[1].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(script.calls[2].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[3].adapter == nullptr);
	// The refreshed list supplies the actual default GPU's index.
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.empty_cache_refresh_still_tries_default_adapter")
{
	DeviceCreationScript script;
	script.candidates.pop_back();
	script.usedCache = true;
	script.results = { E_FAIL, S_OK };
	const auto device = script.run();
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	REQUIRE(script.calls.size() == 2);
	CHECK(script.calls[1].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(not device.adapterIndex);
}

TEST_CASE("D3D11Device.failed_cache_refresh_and_default_adapter_fall_back_to_warp")
{
	DeviceCreationScript script;
	script.refreshedCandidates = script.candidates;
	script.candidates.pop_back();
	script.usedCache = true;
	script.results = { E_FAIL, E_FAIL, E_FAIL, E_FAIL, S_OK, S_OK };
	const auto device = script.run();
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	REQUIRE(script.calls.size() == 6);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[4].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[5].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_WARP);
}

TEST_CASE("D3D11Device.cache_refresh_is_bounded_when_every_creation_path_fails")
{
	DeviceCreationScript script;
	script.refreshedCandidates = script.candidates;
	script.candidates.pop_back();
	script.usedCache = true;
	CHECK_THROWS_AS(script.run(), InternalEngineError);
	CHECK(script.cacheRequests == Array<bool>{ true, false });
	REQUIRE(script.calls.size() == 8);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[4].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[6].driverType == D3D_DRIVER_TYPE_REFERENCE);
}

TEST_CASE("D3D11Device.enumeration_reports_cache_use_and_refreshes_feature_levels")
{
	DeviceCreationScript script;
	script.defaultResult = S_OK;
	D3D11Misc::HardwareAdapterList result;
	const auto cache = script.firstAdapterCache();
	result.adapters.reserve(64);
	script.enumerate(result, cache);
	REQUIRE(result.usedCache);
	REQUIRE(result.adapters.size() == 1);
	CHECK(result.adapters.front().featureLevel == D3D_FEATURE_LEVEL_12_1);
	CHECK(script.calls.isEmpty());
	const size_t capacity = result.adapters.capacity();

	// Use the real enumeration loop with successful feature probes, without creating extra devices.
	script.enumerate(result, none);
	CHECK(not result.usedCache);
	CHECK(result.adapters.capacity() >= capacity);
	CHECK(script.calls.size() == result.adapters.size());
	for (const auto& adapter : result.adapters)
	{
		CHECK(adapter.featureLevel == D3D_FEATURE_LEVEL_11_0);
	}
}

TEST_CASE("D3D11Device.unmatched_cache_does_not_mark_enumeration_as_cached")
{
	DeviceCreationScript script;
	script.defaultResult = S_OK;
	auto cache = script.firstAdapterCache();
	cache.vendorId = 0xFFFFFFFF;
	D3D11Misc::HardwareAdapterList result;
	result.usedCache = true;
	script.enumerate(result, cache);
	CHECK(not result.usedCache);
	CHECK(script.calls.size() == result.adapters.size());
}

# endif
