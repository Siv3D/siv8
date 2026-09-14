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
# include <Siv3D/Renderer/D3D11/SwapChain/D3D11SwapChain.hpp>
# include <wrl/implements.h>

namespace
{
	using namespace Microsoft::WRL;
	using SwapChainInterfaces = ChainInterfaces<IDXGISwapChain2, IDXGISwapChain1, IDXGISwapChain, IDXGIDeviceSubObject, IDXGIObject>;
	using FactoryInterfaces = ChainInterfaces<IDXGIFactory5, IDXGIFactory4, IDXGIFactory3, IDXGIFactory2, IDXGIFactory1, IDXGIFactory, IDXGIObject>;

	// These COM objects record the real initialization/Present calls without creating another window or GPU swap chain.
	class ScriptedSwapChain final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, SwapChainInterfaces>
	{
		using Base = RuntimeClass<RuntimeClassFlags<ClassicCom>, SwapChainInterfaces>;

	public:
		bool exposeSwapChain2 = true;
		HRESULT latencyResult = S_OK;
		bool provideWaitableHandle = true;
		UINT maximumFrameLatency = 0;
		uint32 latencyCalls = 0;
		uint32 waitableCalls = 0;
		HANDLE issuedWaitableHandle = nullptr;
		DXGI_SWAP_CHAIN_DESC1 desc{};
		Array<std::pair<UINT, UINT>> presents;

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override
		{
			if ((iid == __uuidof(IDXGISwapChain2)) && not exposeSwapChain2)
			{
				*object = nullptr;
				return E_NOINTERFACE;
			}
			return Base::QueryInterface(iid, object);
		}

		HRESULT STDMETHODCALLTYPE Present(UINT interval, UINT flags) override
		{
			presents.emplace_back(interval, flags);
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_SWAP_CHAIN_DESC1* result) override
		{
			*result = desc;
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** output) override
		{
			*output = nullptr;
			return DXGI_ERROR_NOT_FOUND;
		}

		HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT maximum) override
		{
			++latencyCalls;
			maximumFrameLatency = maximum;
			return latencyResult;
		}

		HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject() override
		{
			++waitableCalls;
			if (not provideWaitableHandle)
			{
				return nullptr;
			}
			// The production swap chain owns and closes the returned handle.
			issuedWaitableHandle = ::CreateEventW(nullptr, TRUE, TRUE, nullptr);
			REQUIRE(issuedWaitableHandle != nullptr);
			return issuedWaitableHandle;
		}

		HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID, UINT, const void*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID, const IUnknown*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID, UINT*, void*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetParent(REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetDevice(REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetBuffer(UINT, REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL, IDXGIOutput*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL*, IDXGIOutput**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT, UINT, UINT, DXGI_FORMAT, UINT) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetHwnd(HWND*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetCoreWindow(REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE Present1(UINT, UINT, const DXGI_PRESENT_PARAMETERS*) override { return E_NOTIMPL; }
		BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported() override { return FALSE; }
		HRESULT STDMETHODCALLTYPE GetRestrictToOutput(IDXGIOutput**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetBackgroundColor(const DXGI_RGBA*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetBackgroundColor(DXGI_RGBA*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetRotation(DXGI_MODE_ROTATION) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetRotation(DXGI_MODE_ROTATION*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetSourceSize(UINT, UINT) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetSourceSize(UINT*, UINT*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(UINT*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetMatrixTransform(const DXGI_MATRIX_3X2_F*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetMatrixTransform(DXGI_MATRIX_3X2_F*) override { return E_NOTIMPL; }
	};

	class ScriptedFactory final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, FactoryInterfaces>
	{
		using Base = RuntimeClass<RuntimeClassFlags<ClassicCom>, FactoryInterfaces>;

	public:
		HRESULT factory5Result = S_OK;
		HRESULT featureResult = S_OK;
		BOOL allowTearing = TRUE;
		HRESULT creationResult = S_OK;
		HRESULT associationResult = S_OK;
		uint32 factory5Queries = 0;
		uint32 factory6Queries = 0;
		uint32 featureQueries = 0;
		uint32 creationCalls = 0;
		uint32 associationCalls = 0;
		IUnknown* expectedDevice = nullptr;
		HWND expectedWindow = nullptr;
		ComPtr<ScriptedSwapChain> swapChain = Make<ScriptedSwapChain>();

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override
		{
			if (iid == __uuidof(IDXGIFactory5))
			{
				++factory5Queries;
				if (FAILED(factory5Result))
				{
					*object = nullptr;
					return factory5Result;
				}
			}
			if (iid == __uuidof(IDXGIFactory6))
			{
				++factory6Queries;
			}
			return Base::QueryInterface(iid, object);
		}

		HRESULT STDMETHODCALLTYPE CheckFeatureSupport(DXGI_FEATURE feature, void* data, UINT size) override
		{
			++featureQueries;
			CHECK(feature == DXGI_FEATURE_PRESENT_ALLOW_TEARING);
			REQUIRE(size == sizeof(BOOL));
			CHECK(*static_cast<BOOL*>(data) == FALSE);
			*static_cast<BOOL*>(data) = allowTearing;
			return featureResult;
		}

		HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(IUnknown* device, HWND window,
			const DXGI_SWAP_CHAIN_DESC1* desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* fullscreen,
			IDXGIOutput* output, IDXGISwapChain1** result) override
		{
			++creationCalls;
			CHECK(device == expectedDevice);
			CHECK(window == expectedWindow);
			CHECK(fullscreen == nullptr);
			CHECK(output == nullptr);
			swapChain->desc = *desc;
			*result = nullptr;
			if (FAILED(creationResult))
			{
				return creationResult;
			}
			return swapChain.CopyTo(result);
		}

		HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND window, UINT flags) override
		{
			++associationCalls;
			CHECK(window == expectedWindow);
			CHECK(flags == (DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER));
			return associationResult;
		}

		HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID, UINT, const void*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID, const IUnknown*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID, UINT*, void*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetParent(REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE EnumAdapters(UINT, IDXGIAdapter**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetWindowAssociation(HWND*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE CreateSwapChain(IUnknown*, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(HMODULE, IDXGIAdapter**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE EnumAdapters1(UINT, IDXGIAdapter1**) override { return E_NOTIMPL; }
		BOOL STDMETHODCALLTYPE IsCurrent() override { return FALSE; }
		BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled() override { return FALSE; }
		HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(IUnknown*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(HANDLE, LUID*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(HWND, UINT, DWORD*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(HANDLE, DWORD*) override { return E_NOTIMPL; }
		void STDMETHODCALLTYPE UnregisterStereoStatus(DWORD) override {  }
		HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(HWND, UINT, DWORD*) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(HANDLE, DWORD*) override { return E_NOTIMPL; }
		void STDMETHODCALLTYPE UnregisterOcclusionStatus(DWORD) override {  }
		HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**) override { return E_NOTIMPL; }
		UINT STDMETHODCALLTYPE GetCreationFlags() override { return 0; }
		HRESULT STDMETHODCALLTYPE EnumAdapterByLuid(LUID, REFIID, void**) override { return E_NOTIMPL; }
		HRESULT STDMETHODCALLTYPE EnumWarpAdapter(REFIID, void**) override { return E_NOTIMPL; }
	};

	struct SwapChainFixture
	{
		ComPtr<ScriptedFactory> factory = Make<ScriptedFactory>();
		D3D11SwapChain swapChain;

		void init()
		{
			auto* device = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getDevice().getDevice();
			// The fake factory and refresh-rate query use this HWND; the desktop window is never modified.
			const HWND window = ::GetDesktopWindow();
			factory->expectedDevice = device;
			factory->expectedWindow = window;
			swapChain.init(window, factory.Get(), device, Size{ 640, 480 });
		}

		void checkConfiguration(const bool tearing) const
		{
			const auto& desc = factory->swapChain->desc;
			CHECK(desc.Width == 640);
			CHECK(desc.Height == 480);
			CHECK(desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM);
			CHECK(desc.BufferCount == 3);
			CHECK(desc.SampleDesc.Count == 1);
			CHECK(desc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD);
			const UINT expectedFlags = (DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT
				| (tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0));
			CHECK(desc.Flags == expectedFlags);
			CHECK(factory->factory5Queries == 1);
			CHECK(factory->factory6Queries == 0);
			CHECK(factory->creationCalls == 1);
			CHECK(factory->associationCalls == 1);
			CHECK(factory->swapChain->latencyCalls == 1);
			CHECK(factory->swapChain->maximumFrameLatency == 1);
			CHECK(factory->swapChain->waitableCalls == 1);
			CHECK(swapChain.getSwapChain1() == static_cast<IDXGISwapChain1*>(factory->swapChain.Get()));
		}
	};
}

TEST_CASE("D3D11SwapChain.factory5_enables_tearing_without_factory6")
{
	SwapChainFixture fixture;
	fixture.init();
	fixture.checkConfiguration(true);
	CHECK(fixture.factory->featureQueries == 1);
	// Factory6 is genuinely unavailable on the scripted object.
	ComPtr<IDXGIFactory6> factory6;
	CHECK(fixture.factory.As(&factory6) == E_NOINTERFACE);
	fixture.swapChain.waitForFrame();
}

TEST_CASE("D3D11SwapChain.missing_factory5_preserves_flip_and_waitable_object")
{
	for (const HRESULT hr : { E_NOINTERFACE, E_FAIL })
	{
		SwapChainFixture fixture;
		fixture.factory->factory5Result = hr;
		fixture.init();
		fixture.checkConfiguration(false);
		CHECK(fixture.factory->featureQueries == 0);
		fixture.swapChain.setVSyncEnabled(false);
		REQUIRE(fixture.swapChain.present());
		CHECK(fixture.factory->swapChain->presents.back().second == 0);
	}
}

TEST_CASE("D3D11SwapChain.unsupported_tearing_preserves_flip_and_waitable_object")
{
	SwapChainFixture fixture;
	fixture.factory->allowTearing = FALSE;
	fixture.init();
	fixture.checkConfiguration(false);
	CHECK(fixture.factory->featureQueries == 1);
}

TEST_CASE("D3D11SwapChain.failed_feature_query_does_not_enable_tearing")
{
	for (const HRESULT hr : { E_FAIL, E_INVALIDARG })
	{
		SwapChainFixture fixture;
		fixture.factory->featureResult = hr;
		// Even a populated TRUE result must be ignored when the query fails.
		fixture.factory->allowTearing = TRUE;
		fixture.init();
		fixture.checkConfiguration(false);
		CHECK(fixture.factory->featureQueries == 1);
		fixture.swapChain.setVSyncEnabled(false);
		REQUIRE(fixture.swapChain.present());
		CHECK(fixture.factory->swapChain->presents.back().second == 0);
	}
}

TEST_CASE("D3D11SwapChain.present_flags_follow_creation_flags_and_vsync")
{
	for (const BOOL tearing : { FALSE, TRUE })
	{
		SwapChainFixture fixture;
		fixture.factory->allowTearing = tearing;
		fixture.init();
		for (const bool vsync : { true, false, true, false })
		{
			fixture.swapChain.setVSyncEnabled(vsync);
			CHECK(fixture.swapChain.isVSyncEnabled() == vsync);
			REQUIRE(fixture.swapChain.present());
			const auto [interval, flags] = fixture.factory->swapChain->presents.back();
			CHECK(interval == (vsync ? 1u : 0u));
			CHECK(flags == ((not vsync && tearing) ? DXGI_PRESENT_ALLOW_TEARING : 0u));
		}
		CHECK(fixture.factory->swapChain->presents.size() == 4);
		CHECK(fixture.factory->factory5Queries == 1);
		CHECK(fixture.factory->featureQueries == 1);
	}
}

TEST_CASE("D3D11SwapChain.creation_failure_stops_initialization")
{
	SwapChainFixture fixture;
	fixture.factory->creationResult = E_FAIL;
	CHECK_THROWS_AS(fixture.init(), InternalEngineError);
	CHECK(fixture.factory->creationCalls == 1);
	CHECK(fixture.factory->swapChain->latencyCalls == 0);
	CHECK(fixture.factory->swapChain->waitableCalls == 0);
	CHECK(fixture.factory->associationCalls == 0);
}

TEST_CASE("D3D11SwapChain.missing_swapchain2_fails_initialization")
{
	SwapChainFixture fixture;
	fixture.factory->swapChain->exposeSwapChain2 = false;
	CHECK_THROWS_AS(fixture.init(), InternalEngineError);
	CHECK(fixture.factory->swapChain->latencyCalls == 0);
	CHECK(fixture.factory->swapChain->waitableCalls == 0);
}

TEST_CASE("D3D11SwapChain.frame_latency_failure_stops_initialization")
{
	SwapChainFixture fixture;
	fixture.factory->swapChain->latencyResult = DXGI_ERROR_DEVICE_REMOVED;
	CHECK_THROWS_AS(fixture.init(), InternalEngineError);
	CHECK(fixture.factory->swapChain->latencyCalls == 1);
	CHECK(fixture.factory->swapChain->waitableCalls == 0);
	CHECK(fixture.factory->associationCalls == 0);
}

TEST_CASE("D3D11SwapChain.window_association_failure_precedes_handle_acquisition")
{
	SwapChainFixture fixture;
	fixture.factory->associationResult = E_FAIL;
	CHECK_THROWS_AS(fixture.init(), InternalEngineError);
	CHECK(fixture.factory->associationCalls == 1);
	CHECK(fixture.factory->swapChain->waitableCalls == 0);
}

TEST_CASE("D3D11SwapChain.missing_waitable_handle_fails_initialization")
{
	SwapChainFixture fixture;
	fixture.factory->swapChain->provideWaitableHandle = false;
	CHECK_THROWS_AS(fixture.init(), InternalEngineError);
	CHECK(fixture.factory->swapChain->waitableCalls == 1);
	CHECK(fixture.factory->swapChain->issuedWaitableHandle == nullptr);
}

TEST_CASE("D3D11SwapChain.closes_the_waitable_handle")
{
	HANDLE handle = nullptr;
	{
		SwapChainFixture fixture;
		fixture.init();
		handle = fixture.factory->swapChain->issuedWaitableHandle;
		DWORD flags = 0;
		REQUIRE(::GetHandleInformation(handle, &flags));
		fixture.swapChain.waitForFrame();
	}
	DWORD flags = 0;
	const BOOL handleStillOpen = ::GetHandleInformation(handle, &flags);
	const DWORD error = ::GetLastError();
	CHECK(handleStillOpen == FALSE);
	CHECK(error == ERROR_INVALID_HANDLE);
}

# endif
