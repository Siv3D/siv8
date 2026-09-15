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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include "../D3D11.hpp"
# include "D3D11DisplayFrequency.hpp"

namespace s3d
{
	class D3D11SwapChain
	{
	public:

		~D3D11SwapChain();

		void init(HWND hWnd, IDXGIFactory2* factory, ID3D11Device* device, const Size& frameBufferSize);

		[[nodiscard]]
		bool present();

		void waitForFrame();

		[[nodiscard]]
		IDXGISwapChain1* getSwapChain1() const noexcept
		{
			return m_swapChain1.Get();
		}

		void setVSyncEnabled(const bool enabled) noexcept;

		[[nodiscard]]
		bool isVSyncEnabled() const noexcept;

	private:

		static constexpr uint32 DefaultMaximumFrameLatency = 1;
		
		uint32 m_nonVSyncPresentFlags = 0;

		ComPtr<IDXGISwapChain1> m_swapChain1;

		HANDLE m_waitableObject = nullptr;

		HWND m_hWnd = nullptr;

		D3D11DisplayFrequency m_displayFrequency;

		Rect m_previousWindowBounds		= Rect::Empty();

		bool m_vSyncEnabled				= true;

	};
}
