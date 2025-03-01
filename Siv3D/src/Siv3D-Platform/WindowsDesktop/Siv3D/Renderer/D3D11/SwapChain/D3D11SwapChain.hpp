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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include "../Device/D3D11Device.hpp"

namespace s3d
{
	class D3D11SwapChain
	{
	public:

		void init(const HWND hWnd, const D3D11Device& device, const Size& frameBufferSize);

		[[nodiscard]]
		bool present();

		[[nodiscard]]
		IDXGISwapChain1* getSwapChain1() const noexcept
		{
			return m_swapChain1.Get();
		}

		void setVSyncEnabled(const bool enabled) noexcept;

		[[nodiscard]]
		bool isVSyncEnabled() const noexcept;

		void setLowLatencyMode(const bool enabled) noexcept;

		[[nodiscard]]
		bool isLowLatencyMode() const noexcept;

	private:

		static constexpr uint32 MaximumFrameLatency_Default = 2;

		static constexpr uint32 MaximumFrameLatency_LowLatency = 1;
		
		HWND m_hWnd						= nullptr;
		
		ID3D11Device* m_device			= nullptr;
		
		ID3D11DeviceContext* m_context	= nullptr;

		IDXGIDevice1* m_dxgiDevice1		= nullptr;

		bool m_tearingSupport			= false;

		ComPtr<IDXGISwapChain1> m_swapChain1;

		double m_displayFrequency		= 60.0;

		Rect m_previousWindowBounds		= Rect::Empty();

		uint32 m_maximumFrameLatency	= MaximumFrameLatency_Default;

		bool m_vSyncEnabled				= true;

		[[nodiscard]]
		bool presentVSync();

		[[nodiscard]]
		bool presentNonVSync();
	};
}
