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
# include <Siv3D/DepthStencilState.hpp>
# include <Siv3D/HashTable.hpp>
# include "../Device/D3D11Device.hpp"

namespace s3d
{
	class D3D11DepthStencilState
	{
	public:

		void init(const D3D11Device& device);

		void set(const DepthStencilState& state);

	private:

		using StateTable = HashTable<DepthStencilState, ComPtr<ID3D11DepthStencilState>>;

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		StateTable m_states;

		DepthStencilState m_currentState = DepthStencilState::Invalid();

		StateTable::iterator create(const DepthStencilState& state);
	};
}
