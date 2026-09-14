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
# include <wrl/implements.h>

namespace
{
	// A distinct COM identity for the scripted candidate; no real D3D API receives this object.
	class AdapterToken final : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IDXGIAdapter1>
	{
	public:
		HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID, UINT, const void*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID, const IUnknown*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID, UINT*, void*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetParent(REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE EnumOutputs(UINT, IDXGIOutput**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetDesc(DXGI_ADAPTER_DESC*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(REFGUID, LARGE_INTEGER*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_ADAPTER_DESC1*) override { return E_NOTIMPL; }
	};

	const Array<D3D_FEATURE_LEVEL> HardwareLevels = {
		D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

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
			candidate.pAdapter = Microsoft::WRL::Make<AdapterToken>();
			candidate.desc = desc;
			candidate.desc.AdapterLuid.LowPart ^= 1;
			candidate.adapterIndex = 3;
			candidates.push_back(candidate);
			candidate.desc = desc;
			candidate.pAdapter = adapter1;
			candidate.adapterIndex = 9;
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
			const EngineOption::D3D11Driver driver = EngineOption::D3D11Driver::Hardware)
		{
			auto result = D3D11Misc::CreateDevice(CreateDevice,
				[&](Array<D3D11Adapter>& result)
				{
					creationCountsAtEnumeration.push_back(calls.size());
					result = candidates;
				}, driver, debug);
			CHECK(result.device.Get() == m_device.Get());
			CHECK(result.context.Get() == m_context.Get());
			CHECK(result.dxgiDevice != nullptr);
			CHECK(result.featureLevel == successFeatureLevel);
			return result;
		}

		Array<D3D11Adapter> candidates;
		Array<size_t> creationCountsAtEnumeration;
		Array<HRESULT> results;
		HRESULT defaultResult = E_FAIL;
		D3D_FEATURE_LEVEL successFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		Array<DeviceCreationCall> calls;

	private:

		static HRESULT WINAPI CreateDevice(IDXGIAdapter* adapter, D3D_DRIVER_TYPE driverType, HMODULE software,
			UINT flags, const D3D_FEATURE_LEVEL* featureLevels, UINT featureLevelCount, UINT sdkVersion,
			ID3D11Device** device, D3D_FEATURE_LEVEL* featureLevel, ID3D11DeviceContext** context)
		{
			auto& script = *s_active;
			CHECK(software == nullptr);
			CHECK(sdkVersion == D3D11_SDK_VERSION);
			CHECK(device != nullptr);
			CHECK(context != nullptr);
			CHECK(featureLevel != nullptr);
			script.calls.push_back({ adapter, driverType, flags,
				Array<D3D_FEATURE_LEVEL>{ featureLevels, (featureLevels + featureLevelCount) } });
			const size_t index = (script.calls.size() - 1);
			const HRESULT result = ((index < script.results.size()) ? script.results[index] : script.defaultResult);
			if (FAILED(result))
			{
				return result;
			}

			CHECK(script.calls.back().featureLevels.contains(script.successFeatureLevel));
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
				*featureLevel = script.successFeatureLevel;
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
	CHECK(script.calls[0].adapter == script.candidates[0].pAdapter.Get());
	CHECK(script.calls[1].adapter == script.candidates[1].pAdapter.Get());
	CHECK(script.calls[0].featureLevels == HardwareLevels);
	CHECK(script.calls[1].featureLevels == HardwareLevels);
	CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0 });
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
		CHECK(script.calls[i].adapter == script.candidates[i % 2].pAdapter.Get());
		CHECK(script.calls[i].driverType == D3D_DRIVER_TYPE_UNKNOWN);
		CHECK(script.calls[i].flags == ((i < 2) ? static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG) : 0u));
	}
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.default_adapter_resolves_actual_index_and_feature_level")
{
	DeviceCreationScript script;
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
		CHECK(script.calls[i].adapter == script.candidates[i % 2].pAdapter.Get());
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
	script.results = { E_FAIL, E_FAIL, E_FAIL, S_OK };
	const auto device = script.run();
	REQUIRE(script.calls.size() == 4);
	CHECK(script.calls[2].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_WARP);
	CHECK(not device.adapterIndex);
	CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0 });
}

TEST_CASE("D3D11Device.explicit_software_driver_bypasses_hardware_selection")
{
	for (const auto driver : { EngineOption::D3D11Driver::WARP, EngineOption::D3D11Driver::Reference })
	{
		DeviceCreationScript script;
		script.results = { S_OK };
		const auto device = script.run(false, driver);
		const auto expected = ((driver == EngineOption::D3D11Driver::WARP) ? D3D_DRIVER_TYPE_WARP : D3D_DRIVER_TYPE_REFERENCE);
		REQUIRE(script.calls.size() == 1);
		CHECK(script.calls[0].driverType == expected);
		CHECK(script.calls[0].adapter == nullptr);
		CHECK(script.calls[0].flags == 0u);
		CHECK(script.calls[0].featureLevels.front() == ((driver == EngineOption::D3D11Driver::WARP)
			? D3D_FEATURE_LEVEL_12_1 : D3D_FEATURE_LEVEL_11_1));
		CHECK(script.calls[0].featureLevels.back() == D3D_FEATURE_LEVEL_10_1);
		CHECK(device.deviceType == expected);
		CHECK(script.creationCountsAtEnumeration.isEmpty());
	}
}

TEST_CASE("D3D11Device.all_creation_paths_fail")
{
	DeviceCreationScript script;
	CHECK_THROWS_AS(script.run(), InternalEngineError);
	REQUIRE(script.calls.size() == 5);
	CHECK(script.calls[2].driverType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.calls[3].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[4].driverType == D3D_DRIVER_TYPE_REFERENCE);
}

TEST_CASE("D3D11Device.hardware_negotiates_feature_level_without_recreation")
{
	for (const auto level : HardwareLevels)
	{
		DeviceCreationScript script;
		script.successFeatureLevel = level;
		script.results = { S_OK };
		const auto device = script.run();
		REQUIRE(script.calls.size() == 1);
		CHECK(script.calls.front().featureLevels == HardwareLevels);
		CHECK(script.calls.front().adapter == script.candidates.front().pAdapter.Get());
		CHECK(device.featureLevel == level);
		CHECK(device.deviceType == D3D_DRIVER_TYPE_HARDWARE);
		CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0 });
	}
}

TEST_CASE("D3D11Device.integrated_preference_uses_the_same_creation_flow")
{
	DeviceCreationScript script;
	script.results = { S_OK };
	const auto device = script.run(false, EngineOption::D3D11Driver::Hardware_FavorIntegrated);
	REQUIRE(script.calls.size() == 1);
	CHECK(script.calls.front().featureLevels == HardwareLevels);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_HARDWARE);
	CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0 });
}

TEST_CASE("D3D11Device.hardware_retries_unrecognized_feature_levels_on_the_same_adapter")
{
	for (const size_t rejectedLists : { 1u, 2u })
	{
		DeviceCreationScript script;
		script.results.assign(rejectedLists, E_INVALIDARG);
		script.results.push_back(S_OK);
		const auto device = script.run(true);
		REQUIRE(script.calls.size() == (rejectedLists + 1));
		CHECK(script.calls[0].featureLevels == HardwareLevels);
		CHECK(script.calls[1].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 });
		if (rejectedLists == 2)
		{
			CHECK(script.calls[2].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_0 });
		}
		for (const auto& call : script.calls)
		{
			CHECK(call.adapter == script.candidates.front().pAdapter.Get());
			CHECK(call.driverType == D3D_DRIVER_TYPE_UNKNOWN);
			CHECK(call.flags == static_cast<UINT>(D3D11_CREATE_DEVICE_DEBUG));
		}
		CHECK(device.featureLevel == D3D_FEATURE_LEVEL_11_0);
	}
}

TEST_CASE("D3D11Device.feature_level_retries_restart_for_each_candidate")
{
	DeviceCreationScript script;
	script.results = { E_INVALIDARG, E_INVALIDARG, E_FAIL, S_OK };
	const auto device = script.run();
	REQUIRE(script.calls.size() == 4);
	CHECK(script.calls[2].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_0 });
	CHECK(script.calls[3].featureLevels == HardwareLevels);
	CHECK(script.calls[3].adapter == script.candidates[1].pAdapter.Get());
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.feature_level_retries_restart_without_debug")
{
	DeviceCreationScript script;
	script.candidates.pop_back();
	script.results = { E_INVALIDARG, DXGI_ERROR_SDK_COMPONENT_MISSING, S_OK };
	const auto device = script.run(true);
	REQUIRE(script.calls.size() == 3);
	CHECK(script.calls[1].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 });
	CHECK(script.calls[2].featureLevels == HardwareLevels);
	CHECK(script.calls[2].driverType == D3D_DRIVER_TYPE_UNKNOWN);
	CHECK(script.calls[2].flags == 0u);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_HARDWARE);
}

TEST_CASE("D3D11Device.default_adapter_negotiates_its_own_feature_levels")
{
	DeviceCreationScript script;
	script.results = { E_INVALIDARG, E_INVALIDARG, E_FAIL, E_FAIL, E_INVALIDARG, E_INVALIDARG, S_OK };
	const auto device = script.run();
	REQUIRE(script.calls.size() == 7);
	CHECK(script.calls[4].featureLevels == HardwareLevels);
	CHECK(script.calls[5].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 });
	CHECK(script.calls[6].featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_0 });
	for (size_t i = 4; i < 7; ++i)
	{
		CHECK(script.calls[i].adapter == nullptr);
		CHECK(script.calls[i].driverType == D3D_DRIVER_TYPE_HARDWARE);
	}
	CHECK(device.adapterIndex == 9u);
}

TEST_CASE("D3D11Device.software_drivers_keep_their_feature_level_ranges")
{
	for (const auto driver : { EngineOption::D3D11Driver::WARP, EngineOption::D3D11Driver::Reference })
	{
		DeviceCreationScript script;
		script.successFeatureLevel = D3D_FEATURE_LEVEL_10_1;
		script.results = { S_OK };
		const auto device = script.run(true, driver);
		REQUIRE(script.calls.size() == 1);
		CHECK(script.calls.front().flags == 0u);
		CHECK(device.featureLevel == D3D_FEATURE_LEVEL_10_1);
		CHECK(script.creationCountsAtEnumeration.isEmpty());
	}
}

TEST_CASE("D3D11Device.software_drivers_retry_unrecognized_feature_levels")
{
	for (const auto driver : { EngineOption::D3D11Driver::WARP, EngineOption::D3D11Driver::Reference })
	{
		DeviceCreationScript script;
		const size_t rejectedLists = ((driver == EngineOption::D3D11Driver::WARP) ? 2 : 1);
		script.results.assign(rejectedLists, E_INVALIDARG);
		script.results.push_back(S_OK);
		const auto device = script.run(false, driver);
		REQUIRE(script.calls.size() == (rejectedLists + 1));
		CHECK(script.calls.back().featureLevels == Array<D3D_FEATURE_LEVEL>{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 });
		for (const auto& call : script.calls)
		{
			CHECK(call.adapter == nullptr);
			CHECK(call.driverType == device.deviceType);
		}
	}
}

TEST_CASE("D3D11Device.reference_follows_warp_failure_without_recreation")
{
	DeviceCreationScript script;
	script.results = { E_FAIL, S_OK };
	const auto device = script.run(false, EngineOption::D3D11Driver::WARP);
	REQUIRE(script.calls.size() == 2);
	CHECK(script.calls[0].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[1].driverType == D3D_DRIVER_TYPE_REFERENCE);
	CHECK(device.deviceType == D3D_DRIVER_TYPE_REFERENCE);
	CHECK(not device.adapterIndex);
	CHECK(script.creationCountsAtEnumeration.isEmpty());
}

TEST_CASE("D3D11Device.invalid_argument_retries_are_bounded")
{
	DeviceCreationScript script;
	script.defaultResult = E_INVALIDARG;
	CHECK_THROWS_AS(script.run(true), InternalEngineError);
	// Two explicit candidates and the default adapter, each in two debug modes, plus WARP and Reference.
	REQUIRE(script.calls.size() == (2 * 2 * 3 + 2 * 3 + 3 + 2));
	CHECK(script.creationCountsAtEnumeration == Array<size_t>{ 0 });
	CHECK(script.calls[18].driverType == D3D_DRIVER_TYPE_WARP);
	CHECK(script.calls[21].driverType == D3D_DRIVER_TYPE_REFERENCE);
}

TEST_CASE("D3D11Device.enumeration_collects_metadata_and_reuses_storage")
{
	const auto& backing = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice();
	Array<D3D11Adapter> candidates;
	candidates.reserve(64);
	const size_t capacity = candidates.capacity();
	D3D11Misc::EnumHardwareAdapters(candidates, nullptr, backing.getDXGIFactory2(), DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);
	CHECK(candidates.capacity() >= capacity);

	// Compare against DXGI directly, including adapters that may not support the required feature level.
	size_t expectedCount = 0;
	for (uint32 index = 0; ; ++index)
	{
		ComPtr<IDXGIAdapter1> adapter;
		const HRESULT hr = backing.getDXGIFactory2()->EnumAdapters1(index, &adapter);
		if (hr == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		REQUIRE(SUCCEEDED(hr));
		DXGI_ADAPTER_DESC1 desc{};
		REQUIRE(SUCCEEDED(adapter->GetDesc1(&desc)));
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}
		REQUIRE(expectedCount < candidates.size());
		const auto& candidate = candidates[expectedCount++];
		CHECK(candidate.pAdapter != nullptr);
		CHECK(candidate.adapterIndex == index);
		CHECK(candidate.desc.AdapterLuid.LowPart == desc.AdapterLuid.LowPart);
		CHECK(candidate.desc.AdapterLuid.HighPart == desc.AdapterLuid.HighPart);
		CHECK(candidate.desc.Flags == desc.Flags);
	}
	CHECK(candidates.size() == expectedCount);
	D3D11Misc::EnumHardwareAdapters(candidates, nullptr, backing.getDXGIFactory2(), DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);
	CHECK(candidates.size() == expectedCount);
	CHECK(candidates.capacity() >= capacity);
}

# endif
