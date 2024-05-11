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
		bool present(bool vSync);

		[[nodiscard]]
		IDXGISwapChain1* getSwapChain1() const noexcept
		{
			return m_swapChain1.Get();
		}

	private:
		
		HWND m_hWnd						= nullptr;
		
		ID3D11Device* m_device			= nullptr;
		
		ID3D11DeviceContext* m_context	= nullptr;

		bool m_tearingSupport			= false;

		ComPtr<IDXGISwapChain1> m_swapChain1;

		double m_displayFrequency		= 60.0;

		Rect m_previousWindowBounds		= Rect::Empty();

		[[nodiscard]]
		bool presentVSync();

		[[nodiscard]]
		bool presentNonVSync();
	};
}
