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
			return D3D11Misc::CreateDevice(CreateDevice, candidates, driver, debug);
		}

		Array<D3D11Adapter> candidates;
		Array<HRESULT> results;
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
			const HRESULT result = ((index < script.results.size()) ? script.results[index] : E_FAIL);
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

# endif
