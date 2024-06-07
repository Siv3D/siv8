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

		void set(const RasterizerState& state);

		void setScissorRect(const Rect& scissorRect);
	
	private:

		using StateTable = HashTable<RasterizerState, ComPtr<ID3D11RasterizerState>>;

		ID3D11Device* m_device = nullptr;

		ID3D11DeviceContext* m_context = nullptr;

		StateTable m_states;

		RasterizerState m_currentState = RasterizerState::Invalid();

		StateTable::iterator create(const RasterizerState& state);
	};
}
