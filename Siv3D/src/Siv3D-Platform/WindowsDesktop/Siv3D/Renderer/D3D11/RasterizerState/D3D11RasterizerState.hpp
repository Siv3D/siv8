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
# include <Siv3D/RasterizerState.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/HashTable.hpp>
# include "../Device/D3D11Device.hpp"

namespace s3d
{
	class D3D11RasterizerState
	{
	public:

		void init(const D3D11Device& device);

		void set(const RasterizerState& state, bool scissorEnabled);

		void setScissorRect(const Rect& scissorRect);
	
	private:

		using StateTable = HashTable<std::pair<RasterizerState, bool>, ComPtr<ID3D11RasterizerState>>;

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		StateTable m_states;

		std::pair<RasterizerState, bool> m_currentState = { RasterizerState::Invalid(), false };

		StateTable::iterator create(const std::pair<RasterizerState, bool>& state);
	};
}
